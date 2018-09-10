#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "portaudio.h"
#include <math.h>
#include <complex.h>
#include <fftw3.h>
#include <string.h>

#define DITHER_FLAG     (0) 
#define WRITE_TO_FILE   (0)	
#define PA_SAMPLE_TYPE  paInt16
#define SAMPLE_SILENCE  (0)

typedef short SAMPLE;
typedef struct
{ 
	int          frameIndex;  /* Index into sample array. */
	int          maxFrameIndex;
	SAMPLE      *recordedSamples;
	int	     longitud;
}
paTestData;

//DEFINICION DE FUNCIONES
paTestData config_buffer();
PaStreamParameters config_device();
static int recordCallback(const void *inputBuffer,void *outputBuffer,unsigned long framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,void *userData );                          
void grabar_senal(struct PaStreamParameters  inputParameters, paTestData data, double signal []);
void fft(double senal_in[], double senal_out[], int n);
void ponderar(double lp_ter[], double lpA_ter[], double lpC_ter[],double calibr[],double *Lp, double *LpA, double *LpC, int n, double fsenal[]);
void ganancia(int gain);

//VARIABLES GLOBALES
double txmedida=0.120;  
double fmuestreo = 44000.0;
int ncanales = 1;
int framesxbuffer = 512;
//double fcalib = 0.145695; //CALIBRACION A 0dB
double fcalib =1.399183; //CALIBRACION A 20dB
//double fcalib = 0.0962; //CALIBRACION A -10dB
int tend;
int tequiv;
double trigger;

//CUERPO DEL PROGRAMA
int main (void)
{
	//VARIABLES DE MEDIDA DE TIEMPO
	clock_t t_s_segundo,t_segundo;	
	clock_t inicio, fin,funcioninicio,funcionfin;

	//CONFIGURACION DE BUFFER 
	paTestData buf_audio;
	buf_audio=config_buffer();

	//CONFIGURACION DISPOSITIVO DE ENTRADA
	PaStreamParameters  device;
	device=config_device();

	//FIJAR LA GANANCIA
	ganancia(20);

	//VARIABLES DE MEDIDA
	double senal[buf_audio.longitud],fsenal[buf_audio.longitud/2];	
	int i,contador_segundo,contador_minuto,contador_equivalente;
	double lp_ter_inst[25], lpA_ter_inst[25], lpC_ter_inst[25], Lp_inst, LpA_inst, LpC_inst;
	double suma1s, sumaA1s, sumaC1s,suma1s_ter[25];
	double Leq1s, LeqA1s, LeqC1s, Leq1s_ter[25];
	double suma, sumaA, sumaC, suma_ter[25];
	double Leq1min, LeqA1min,LeqC1min,Leq1min_ter[25];
	double sumaeq, sumaAeq, sumaCeq, sumaeq_ter[25];
	double Leq, LeqA,LeqC,Leq_ter[25];
	char hora[17];
	char nombre[17];
 
	double max, min;
	time_t rawtime;
	struct tm * timeinfo;
	FILE *fi;
	FILE *fs;
	FILE *fpp;
	fi=fopen("/home/pi/Documents/data/instantaneo", "w");
	fs= fopen("/home/pi/Documents/data/espectro", "w");
	char fichero[50];
	char archivo[22];
	strcpy(fichero,"/home/pi/Documents/registros/"); 
	
	//LECTURA FICHERO DE CALIBRACION DEL MICROFONO
	FILE *fh;
	fh=fopen("/home/pi/Documents/data/micro", "r");
	double numero, calibr[25];
	for(i=0;i<25;i++)
	{
		fscanf(fh, "%lf", &numero);
		calibr[i]=numero;
	}
	fclose(fh);

	//INICIALIZACION DE LOS VALORES VOLCADOS AL FICHERO
	fprintf(fi,"00.0\n");
	fprintf(fi,"00.0\n");
	fprintf(fi,"00.0\n");  
	fprintf(fi,"00.0\n");
	fprintf(fi,"00.0\n");
	time( &rawtime);
	timeinfo = localtime(&rawtime);
	strcpy(hora, "---");
	fprintf(fi,"%s\n",hora);

	//ESPERAR A INICIO MINUTO 
	while(strcmp(nombre, "00") != 0)
	{
		t_segundo=t_s_segundo=clock();
		contador_segundo=0;
		suma1s=0;
		sumaA1s=0;
		sumaC1s=0;
		for(i=0;i<25;i++)
		{
			suma1s_ter[i]=0;
		}

		//COMIENZO SEGUNDO
		while(((t_segundo-t_s_segundo)/CLOCKS_PER_SEC)<0.9)
		{
			grabar_senal(device,buf_audio,senal);

			//PASAR LA SENAL EN bits A SENAL EN Pa
			for(i=0;i<buf_audio.longitud;i++)
			{
				senal[i] = (senal[i]*5.0/(65536.0))/fcalib;
			}			
			fft(senal, fsenal,buf_audio.longitud);
			ponderar(lp_ter_inst, lpA_ter_inst, lpC_ter_inst,calibr, &Lp_inst, &LpA_inst, &LpC_inst, buf_audio.longitud, fsenal);

			//SUMAR LOGARITMICAMENTE
			suma1s=suma1s+pow(10,Lp_inst*0.1);
			sumaA1s=sumaA1s+pow(10,LpA_inst*0.1);
			sumaC1s=sumaC1s+pow(10,LpC_inst*0.1);
				
			for(i=0;i<25;i++)
			{
				suma1s_ter[i]=suma1s_ter[i]+pow(10,lp_ter_inst[i]*0.1);
			}	
			contador_segundo=contador_segundo+1;
			t_segundo=clock();
		}

		//EVALUAR EQUIVALENTES 1s 
		Leq1s=(10*log10(suma1s/contador_segundo));
		LeqA1s=(10*log10(sumaA1s/contador_segundo));
		LeqC1s=10*log10(sumaC1s/contador_segundo);
		for(i=0;i<25;i++)
		{
			Leq1s_ter[i]=10*log10(suma1s_ter[i]/contador_segundo);
		}
		
		//SALIDA SEGUNDO
		rewind(fi);
		fflush(fi);
		rewind(fs);
		for(i=0;i<25;i++)
		{
			fprintf(fs, "%.0f\n", Leq1s_ter[i]);
			fflush(fs);
		}		
		time( &rawtime);
		timeinfo = localtime(&rawtime);
		strftime(nombre, 22, "%S", timeinfo); 
	}

	time( &rawtime);
	timeinfo = localtime(&rawtime);
	strftime(archivo, 22, "%Y-%m-%d_%H-%M-00", timeinfo); 
	strcat(fichero, archivo);
	fpp= fopen(fichero, "w");

	//INICIO REGISTRO
	while(1)
	{
		sumaeq=0;
		sumaAeq=0;
		sumaCeq=0;
		for(i=0;i<25;i++)
		{
			sumaeq_ter[i]=0;
		}

		//SESION DE REGISTRO
		for(contador_equivalente=0;contador_equivalente<5;contador_equivalente++)
		{	 
			inicio=fin=clock();
			system("date");
			max=0;
			min=9999;
			suma=0;
			sumaA=0;
			sumaC=0;
			for(i=0;i<25;i++)
			{
				suma_ter[i]=0;
			}

			//COMIENZO MINUTO
			//while(((fin - inicio) / CLOCKS_PER_SEC)<57.2)
			for(contador_minuto=0;contador_minuto<40;contador_minuto++)
			{
					t_segundo=t_s_segundo=clock();
					contador_segundo=0;
					suma1s=0;
					sumaA1s=0;
					sumaC1s=0;
					for(i=0;i<25;i++)
					{
						suma1s_ter[i]=0;
					}

					//COMIENZO SEGUNDO
					while(((t_segundo-t_s_segundo)/CLOCKS_PER_SEC)<0.9)
					{
						grabar_senal(device,buf_audio,senal);

						//PASAR LA SENAL EN bits A SENAL EN Pa
						for(i=0;i<buf_audio.longitud;i++)
						{
							senal[i] = (senal[i]*5.0/(65536.0))/fcalib;
						}			
						fft(senal, fsenal,buf_audio.longitud);
						ponderar(lp_ter_inst, lpA_ter_inst, lpC_ter_inst,calibr, &Lp_inst, &LpA_inst, &LpC_inst, buf_audio.longitud, fsenal);

						//SUMAR LOGARITMICAMENTE
						suma1s=suma1s+pow(10,Lp_inst*0.1);
						sumaA1s=sumaA1s+pow(10,LpA_inst*0.1);
						sumaC1s=sumaC1s+pow(10,LpC_inst*0.1);
				
						for(i=0;i<25;i++)
						{
							suma1s_ter[i]=suma1s_ter[i]+pow(10,lp_ter_inst[i]*0.1);
						}	
						contador_segundo=contador_segundo+1;
						t_segundo=clock();
					}

					//EVALUAR EQUIVALENTES 1s 
					Leq1s=(10*log10(suma1s/contador_segundo));
					LeqA1s=(10*log10(sumaA1s/contador_segundo));
					LeqC1s=10*log10(sumaC1s/contador_segundo);
					for(i=0;i<25;i++)
					{
						Leq1s_ter[i]=10*log10(suma1s_ter[i]/contador_segundo);
					}
				
					//SALIDA SEGUNDO
					rewind(fi);
					fprintf(fi,"%.1f ", LeqA1s);    //instantaneo 
					fflush(fi);
					rewind(fs);
					for(i=0;i<25;i++)
					{
						fprintf(fs, "%.0f\n", Leq1s_ter[i]);
						fflush(fs);
					}
					//MAXIMO Y MINIMO ALCANZADOS
					if(LeqA1s>max)
					{
						max=LeqA1s;
					}
					if(LeqA1s<min)
					{
						min=LeqA1s;
					}

					//SUMAR LOGARITMICAMENTE
					suma=suma+pow(10,Leq1s*0.1);
					sumaA=sumaA+pow(10,LeqA1s*0.1);
					sumaC=sumaC+pow(10,LeqC1s*0.1);
					for(i=0;i<25;i++)
					{
						suma_ter[i]=suma_ter[i]+pow(10,Leq1s_ter[i]*0.1);
					}
					fin=clock();
			}
			printf("******\n");
			system("date");
			printf("******\n");
			//EVALUAR EQUIVALENTES 1min 
			Leq1min=10*log10(suma/contador_minuto);
			LeqA1min=10*log10(sumaA/contador_minuto);
			LeqC1min=10*log10(sumaC/contador_minuto);
			for(i=0;i<25;i++)
			{
				Leq1min_ter[i]=10*log10(suma_ter[i]/contador_minuto);
			}	

			//SALIDA MINUTO
			fprintf(fi,"%.1f ",LeqA1min);
			fprintf(fi,"%.1f ",LeqC1min);  
			fprintf(fi,"%.1f ",max);
			fprintf(fi,"%.1f ",min);
			time( &rawtime);
			timeinfo = localtime(&rawtime);
			strftime(hora, 22, "%H-%M-00", timeinfo); 
			fprintf(fi,"%s\n",hora);
			
			//SUMAR LOGARITMICAMENTE
			sumaeq=sumaeq+pow(10,Leq1min*0.1);
			sumaAeq=sumaAeq+pow(10,LeqA1min*0.1);
			sumaCeq=sumaCeq+pow(10,LeqC1min*0.1);
			for(i=0;i<25;i++)
			{
				sumaeq_ter[i]=sumaeq_ter[i]+pow(10,Leq1min_ter[i]*0.1);
			}

			time( &rawtime);
			timeinfo = localtime(&rawtime);
			strftime(nombre, 22, "%S", timeinfo); 
			printf("%f\n",(float)((fin - inicio) / CLOCKS_PER_SEC));
			while(strcmp(nombre, "00") != 0)
			{
				time( &rawtime);
				timeinfo = localtime(&rawtime);
				strftime(nombre, 22, "%S", timeinfo); 
			}
				
		}

		//EVALUAR EQUIVALENTES
		Leq=10*log10(sumaeq/contador_equivalente);
		LeqA=10*log10(sumaAeq/contador_equivalente);
		LeqC=10*log10(sumaCeq/contador_equivalente);
		for(i=0;i<25;i++)
		{
			Leq_ter[i]=10*log10(sumaeq_ter[i]/contador_equivalente);
		}	

		//SALIDA EQUIVALENTES
		fprintf(fpp,"%s      %.1f %.1f  %.1f  %.1f %.1f %.1f\n", hora,  Leq, LeqA, LeqC, LeqC-LeqA,max,min);
		fflush(fpp);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//CONFIGURACION DEL DISPOSITIVO DE ENTRADA Y DEL BUFFER DE AUDIO
/* VOID(VOID) ambas */
paTestData config_buffer()
{
	int                 i;
	int                 totalFrames;
	int                 numSamples;
	int                 numBytes;
	SAMPLE              max, val;
	paTestData 	    data;

	data.maxFrameIndex = totalFrames = (txmedida*fmuestreo); /* Record for a few seconds. */
	data.frameIndex = 0;
	numSamples = totalFrames * ncanales;
	numBytes = numSamples * sizeof(SAMPLE);
	data.recordedSamples = (SAMPLE *) malloc( numBytes ); /* From now on, recordedSamples is initialised. */	
	data.longitud = numSamples;

	for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

   	freopen("/dev/null","w",stderr);
	Pa_Initialize();
	freopen("/dev/tty","w",stderr);
   	
	//system("clear");
	return data;	
}

PaStreamParameters config_device()
{
	PaStreamParameters  inputParameters;
	PaStream*           stream;
		
	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
   	
	inputParameters.channelCount = ncanales;                    
	inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	return inputParameters;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//FUNCION INTERNA PARA LA GRABACION DEL AUDIO

static int recordCallback( const void *inputBuffer, void *outputBuffer,
	                           unsigned long framesPerBuffer,
	                           const PaStreamCallbackTimeInfo* timeInfo,
	                           PaStreamCallbackFlags statusFlags,
	                           void *userData )
	{	
		paTestData *data = (paTestData*)userData;
		const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
		SAMPLE *wptr = &data->recordedSamples[data->frameIndex * ncanales];
		long framesToCalc;
		long i;
		int finished;
		unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;
		   
		(void) outputBuffer; /* Prevent unused variable warnings. */
		(void) timeInfo;
		(void) statusFlags;
		(void) userData;
		   
		if( framesLeft < framesPerBuffer )
		{
			framesToCalc = framesLeft;
			finished = paComplete;
		}
		else
		{
			framesToCalc = framesPerBuffer;
			finished = paContinue;
		}
		   
		if( inputBuffer == NULL )
		{
			for( i=0; i<framesToCalc; i++ )
			{
				*wptr++ = SAMPLE_SILENCE;  /* left */
				if( ncanales == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
			}
		}
		else
		{
			for( i=0; i<framesToCalc; i++ )
			{
				*wptr++ = *rptr++;  /* left */
				if( ncanales == 2 ) *wptr++ = *rptr++;  /* right */
			}
		}
		data->frameIndex += framesToCalc;
		return finished;
	}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//GRABAR SENAL
/* VOID(configuracion dispositivo, buffer de datos, senal de doubles) */

void grabar_senal(struct PaStreamParameters  inputParameters, paTestData data, double signal [])
{
	PaStream*           stream;
	PaError             err = paNoError;
	int i;
	SAMPLE val;

	clock_t start, end;

	Pa_OpenStream(
		&stream,
                &inputParameters,
                NULL,                  /* &outputParameters, */
                fmuestreo,
               framesxbuffer,
                paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                recordCallback,
                &data ); 

	//grabacion de la senal
	Pa_StartStream( stream );

	while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
	{   
	}
	Pa_CloseStream( stream );
	
	for( i=0; i<data.longitud; i++ )
        {
            val = data.recordedSamples[i];
	    signal[i]=(double)val;
        }

	return;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//FFT
/* VOID (vector senal en tiempo, vector senal en frecuencia, longitud senal) */
void fft(double senal_in[], double senal_out[], int n)
{
	int largo = n; //se copia la longitud para que no se modifique
	int nmedios = round(n/2.0);
	int i;

	fftw_complex senal_f[nmedios];

	fftw_plan w = fftw_plan_dft_r2c_1d(largo,senal_in,senal_f,0);	

	fftw_execute(w);
	fftw_destroy_plan(w);

	for(i=0;i<nmedios;i++)
	{
		
		senal_out[i]=cabs(senal_f[i]);	
	}

	return;
}
	
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//PONDERAR
/* VOID (vector para almacenar Lp ponderado por tercios, valores por referencia de Lp ponderado, vectr rms por tercio) */
void ponderar(double lp_ter[], double lpA_ter[], double lpC_ter[],double calibr[],double *Lp, double *LpA, double *LpC, int n, double fsenal[])
{
	double correccionC[25], correccionA[25], rms[25];
	int k,i;
	double LpAaux=0.0;
	double LpCaux=0.0;
	double Lpaux=0.0; 
	double finf[25], fsup[25], fcentro[25], deltaf,ms;	
	deltaf=fmuestreo/n;
	
	//frecuencias inferior, superior y central de cada tercio
	finf[0]=20.0;
	finf[1]=56.2;
	finf[2]=70.8;
	finf[3]=89.1;
	finf[4]=112;
	finf[5]=141.0;
	finf[6]=178.0;
	finf[7]=224.0;
	finf[8]=282.0;
	finf[9]=355.0;
	finf[10]=447.0;
	finf[11]=562.0;
	finf[12]=708.0;
	finf[13]=891.0;
	finf[14]=1122.0;
	finf[15]=1413.0;
	finf[16]=1778.0;
	finf[17]=2239.0;
	finf[18]=2818.0;
	finf[19]=3548.0;
	finf[20]=4467.0;
	finf[21]=5623.0;
	finf[22]=7079.0;
	finf[23]=8913.0;
	finf[24]=11220.0;

	fcentro[0]=0.0;
	fcentro[1]=63.0;
	fcentro[2]=80.0;
	fcentro[3]=100.0;
	fcentro[4]=125.0;
	fcentro[5]=160.0;
	fcentro[6]=200.0;
	fcentro[7]=250.0;
	fcentro[8]=315.0;
	fcentro[9]=400.0;
	fcentro[10]=500.0;
	fcentro[11]=630.0;
	fcentro[12]=800.0;
	fcentro[13]=1000.0;
	fcentro[14]=1250.0;
	fcentro[15]=1600.0;
	fcentro[16]=2000.0;
	fcentro[17]=2500.0;
	fcentro[18]=3150.0;
	fcentro[19]=4000.0;
	fcentro[20]=5000.0;
	fcentro[21]=6300.0;
	fcentro[22]=8000.0;
	fcentro[23]=10000.0;
	fcentro[24]=12500.0;

	fsup[0]=56.2;
	fsup[1]=70.8;
	fsup[2]=89.1;
	fsup[3]=112;
	fsup[4]=141.0;
	fsup[5]=178.0;
	fsup[6]=224.0;
	fsup[7]=282.0;
	fsup[8]=355.0;
	fsup[9]=447.0;
	fsup[10]=562.0;
	fsup[11]=708.0;
	fsup[12]=891.0;
	fsup[13]=1122.0;
	fsup[14]=1413.0;
	fsup[15]=1778.0;
	fsup[16]=2239.0;
	fsup[17]=2818.0;
	fsup[18]=3548.0;
	fsup[19]=4467.0;
	fsup[20]=5623.0;
	fsup[21]=7079.0;
	fsup[22]=8913.0;
	fsup[23]=11220.0;
	fsup[24]=14103.0;

	for(k=0;k<25;k++)
	{
		ms=0.0;
		//pasar a indice la frecuencia
		int comienzo=(int)round(finf[k]/deltaf);
		int fin=(int)round(fsup[k]/deltaf);
		
		for(i=comienzo;i<(fin+1);i++)
		{
			ms=ms+cabs(fsenal[i])*cabs(fsenal[i]);	
		}

		//aqui se aplica que la senal de entrada es una fft compleja 
		//calculada aplicando hermiticidad
		rms[k]=sqrt((ms/(n/2))/n);
	}

	correccionA[0]=-28.35;
	correccionA[1]=-26.2;
	correccionA[2]=-22.5;
	correccionA[3]=-19.1;
	correccionA[4]=-16.1;
	correccionA[5]=-13.4;
	correccionA[6]=-10.9;
	correccionA[7]=-8.6;
	correccionA[8]=-6.6;
	correccionA[9]=-4.8;
	correccionA[10]=-3.2;
	correccionA[11]=-1.9;
	correccionA[12]=-0.8;
	correccionA[13]=0;
	correccionA[14]=0.6;
	correccionA[15]=1.0;
	correccionA[16]=1.2;
	correccionA[17]=1.3;
	correccionA[18]=1.2;
	correccionA[19]=1.0;
	correccionA[20]=0.5;
	correccionA[21]=-0.1;
	correccionA[22]=-1.1;
	correccionA[23]=-2.5;
	correccionA[24]=-4.3;

	correccionC[0]=-4.0;
	correccionC[1]=-0.8;
	correccionC[2]=-0.5;
	correccionC[3]=-0.3;
	correccionC[4]=-0.2;
	correccionC[5]=-0.1;
	correccionC[6]=0.0;
	correccionC[7]=0.0;
	correccionC[8]=0.0;
	correccionC[9]=0.0;
	correccionC[10]=0.0;
	correccionC[11]=0.0;
	correccionC[12]=0.0;
	correccionC[13]=0.0;
	correccionC[14]=0.0;
	correccionC[15]=-0.1;
	correccionC[16]=-0.2;
	correccionC[17]=-0.3;
	correccionC[18]=-0.5;
	correccionC[19]=-0.8;
	correccionC[20]=-1.3;
	correccionC[21]=-2.0;
	correccionC[22]=-3.0;
	correccionC[23]=-4.4;
	correccionC[24]=-6.2;

	for(k=0;k<25;k++)
	{
		lp_ter[k]=20*log10(rms[k]/2e-5)+calibr[k];
		Lpaux=Lpaux+pow(10,0.1*lp_ter[k]);
		
		lpA_ter[k]=lp_ter[k]+correccionA[k];
		LpAaux=LpAaux+pow(10,0.1*lpA_ter[k]);

		lpC_ter[k]=lp_ter[k]+correccionC[k];
		LpCaux=LpCaux+pow(10,0.1*lpC_ter[k]);
	}

	*LpA=10*log10(LpAaux);
	*LpC=10*log10(LpCaux);
	*Lp=10*log10(Lpaux);

	return;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/////MODIFICAR GANANCIA

void ganancia(int gain)
{
	char a[]="amixer -c 1 sset Mic ";
	char g[32];

	if(gain>-1)
	{
		sprintf(g, "%d", abs(gain));
		strcat(strcat(a, g), "dB");	
		system(a);
	}else 
	{
		sprintf(g, "%d", abs(gain));
		strcat(strcat(a, g), "dB-");	
		system(a);
	}
	
	return;
}	

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/////LECTURA DE LOS VALORES DE CONFIGRACION

void fijarvalores ()
{
	int relleno;
	double doblerelleno;
	char rell[20];
	FILE *fc;
	fc = fopen("/home/pi/Desktop/configuracion", "r");
	//fscanf(fc, "%i" , &haytrig);
	
	//if(haytrig==99)
	//{
	//	tequiv=60;
	//	fscanf(fc, "%lf" , &trigger);
	//	fscanf(fc, "%lf" , &txmedida);
	//	fscanf(fc, "%i" , &relleno);
	//	fscanf(fc, "%i" , &relleno);
	//}
	//if(haytrig!=99)
	//{
		fscanf(fc, "%s", &doblerelleno);
		fscanf(fc, "%s", &doblerelleno);
		fscanf(fc, "%s", &doblerelleno);
		fscanf(fc, "%lf" , &trigger);
		fscanf(fc, "%lf" , &txmedida);
		//txmedida=0.125;
		fscanf(fc, "%i" , &tequiv);
		tequiv=tequiv;
		fscanf(fc, "%i" , &tend);
		tend=tend*60;
		printf("%lf, %lf, %i, %i\n" , trigger, txmedida, tequiv, tend);
	//}
	
	fclose(fc);
	return;
}


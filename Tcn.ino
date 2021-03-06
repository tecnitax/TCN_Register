
#include "Nextion.h"

// Variables necesarias de la pantalla

NexText t0 = NexText(0, 1, "t3");
NexText t28 = NexText(2,2,"t28"); // ubicacion
NexText t38 = NexText(2,3,"t38"); // fecha de instalacion
NexText t48 = NexText(2,4,"t48"); // tecnico que instala
NexText t58 = NexText(2,14,"t58");  // tiempo promediado 
NexText t83 = NexText(2,21,"t83");  // decibelios a los que despierta el registrador
NexText t208 = NexText(2,22,"t208"); // Ponderacion temporal
NexText t209 = NexText(2,23,"t209"); // Promediado
NexText t210 = NexText(2,24,"t210"); 
NexText t500 = NexText(2,22,"t500");
NexText tcod = NexText(2,1,"t0");
NexWaveform s0 = NexWaveform(0,3,"s0"); 

NexButton b0 =  NexButton(2, 7, "b1");


NexButton b12 = NexButton(11,3,"b33");

//AJUSTES
NexButton b10 =  NexButton(0, 32, "b1");

//Descarga
NexButton b9  =  NexButton(12, 3, "b1");

//Actualizar lista dispositivos conectados 
NexButton b20  =  NexButton(12, 17, "b2989");

//Boton desconectar dispositivo seleccionado
NexButton b21 = NexButton(12, 24, "b35");
NexText tsel = NexText(12,25,"tsel");


// FECHA
NexButton b11 = NexButton(8,28,"b0");
NexText tini = NexText(8,30,"t12");
NexText tfin = NexText(8,31,"t13");

NexPage page7 = NexPage(7,0,"page6");
NexPage page8 = NexPage(12,0,"page8");
time_t t = time(NULL);
struct tm tm  = *localtime(&t);

char oldname1[100] = "configuracion";
char newname1[100] = "configuracion_OLD_";
string str;
char oldname2[100] = "setup";
char newname2[100] = "setup_OLD";

// variables del archivo de Configuracion
char ubicacion[100],finstalacion[100],instalador[100];


char buffer[100],ponderacion[100],promediado[100],tiempopromedio[5],trigger[5],triggernum[5],ponderaciontemp[6],duracion[6],codigo[8];




FILE *fichero; // Instantaneo
FILE *fichero2; // Acceso al registrador
FILE *fichero3; // Configuracion (codigo de dispositivo, ubicacion, fecha, y tecnico) 
FILE *fichero4; // SETUP
FILE *fichero5;
FILE *fichero6;

int i;
char nombre[100];
char str19[100];

char *dest;
char *destmedida;
char *dest2;
char *dest3;
char str1[10], str2[25], str3[25], str4[25], str5[10], str6[10], str7[10], str8[10];
char str11[10], str12[25], str13[25], str14[25], str15[10], str16[10], str17[10], str18[10],strcode[13];
char medida[10]={0};

NexTouch *nex_listen_list[] = 
{
    &b0,
    &b9,	
    &b11,
    &b10,
    &b12,
    &b20,
    &b21,
    NULL
};

void strreverse(char* begin, char* end)
{
        char aux;
        while(end>begin)
               aux=*end, *end--=*begin, *begin++=aux;
}



void itoa(int value, char* str, int base)
{
        static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        char* wstr=str;
        int sign;
        // Validate base
        if (base<2 || base>35){ *wstr='\0'; return; }
        // Take care of sign
        if ((sign=value) < 0) value = -value;
        // Conversion. Number is reversed.
        do *wstr++ = num[value%base]; while(value/=base);
        if(sign<0) *wstr++='-';
        *wstr='\0';
	

        // Reverse string
        strreverse(str,wstr-1);
}

void fechasenmedio(char fechauno[10], char fechados[10])
{
    char mesuno[]="  ";
    char mesdos[]="  ";
    char diauno[]="  ";
    char diados[]="  ";
    printf("%s\n","Al entrar en la funcion fechasenmedio ");

	printf("%s\n",fechauno);
	printf("%s\n",fechados);
	system("rm /home/pi/Desktop/registrador14/registros/* ");
    char cadenadia[2];
    char cadenadiaaux[2];
    char cadenames[2];
    char cadenamesaux[2];
    char cadenaano[4]={0};
    char cad[4]={0};
    char *p;

    FILE *fi;
	 FILE * file1 = fopen("/home/pi/Desktop/registrador14/prueba.sh", "w+");
	 fprintf(file1,"cd /home/pi/Documents/registros");
	 fprintf(file1,"\n");
	 fprintf(file1,"%s","scp -r ");

	printf("%s\n"," despues de empezar a añadir al archivo de copia 1");
	fflush(file1);

    int numeroanouno, numeroanodos, numeromesuno, numeromesdos, numerodiauno, numerodiados, i;
    int mes, dia, ano, diafin,diainicio,mesinicio,mesfin;

        //OBTENER ANOS EN ENTEROS
    numeroanouno=0;
    numeroanodos=0;
    for(i=0; i<4; i++)
    {
        numeroanodos=numeroanodos*10+(fechados[i]-'0');
        numeroanouno=numeroanouno*10+(fechauno[i]-'0');
    }
	printf("%s\n", "antes de convertirlo.... "); 
	printf("%s\n", cad);
	sprintf(cad,"%i",numeroanouno);
	
	printf("%s\n", "despues... ... ");
	printf("%s\n",cad );



    //OBTENER MESE EN ENTEROS
    strncpy(mesdos,&fechados[5],2);
    strncpy(mesuno,&fechauno[5],2);
    numeromesuno=0;
    numeromesdos=0;
    for(i=0; i<2; i++)
    {
        numeromesdos=numeromesdos*10+(mesdos[i]-'0');
        numeromesuno=numeromesuno*10+(mesuno[i]-'0');
    }

    //OBTENER DIAS EN ENTEROS
    strncpy(diados,&fechados[8],2);
    strncpy(diauno,&fechauno[8],2);
    numerodiauno=0;
    numerodiados=0;
    for(i=0; i<2; i++)
    {
        numerodiados=numerodiados*10+(diados[i]-'0');
        numerodiauno=numerodiauno*10+(diauno[i]-'0');
    }
                
//FALTA SI ES ANO BISIESTO
    int diames[12];
    diames[1]=31;
    diames[2]=28;
    diames[3]=31;
    diames[4]=30;
    diames[5]=31;
    diames[6]=30;
    diames[7]=31;
    diames[8]=31;
    diames[9]=30;
    diames[10]=31;
    diames[11]=30;
    diames[12]=31;

    if(numeroanouno!=numeroanodos)
    {
        for(ano=numeroanouno;ano<numeroanodos+1;ano++)
        {
            if(ano%4==0&&ano%100!=0||ano%400==0)
            {
                diames[2]=29;
            }
            mesinicio=1;
            mesfin=12;
            if(ano==numeroanouno)
            {
                mesinicio=numeromesuno;
                mesfin=12;
            }
            if(ano==numeroanodos)
            {
                mesinicio=1;
                mesfin=numeromesdos;
            }

            for(mes=mesinicio; mes<mesfin+1; mes++)
		{
		 diafin=diames[mes];
                diainicio=1;
                if(mes==numeromesdos)
                {
                    diafin=numerodiados;
                }

                if(mes==numeromesuno)
                {
                    diainicio=numerodiauno;
                }
		for(dia=diainicio; dia<diafin+1; dia++)
                {
                    memset(cadenadia, 0,2);
                    memset(cadenames, 0,2);
                    memset(cadenaano, 0,4);

                    itoa(ano,cadenaano,10);
                    if(dia<10)
                    {
                        strncpy(cadenadia,"0",1);
                    }
                    itoa(dia,cadenadiaaux,10);
                    strcat(cadenadia,cadenadiaaux);
                    if(mes<10)
                    {
                        strncpy(cadenames,"0",1);
                    }
                    itoa(mes,cadenamesaux,10);
                    strcat(cadenames,cadenamesaux);
                    fprintf(file1,"%s-%s-%s* ",cadenaano,cadenames,cadenadia);
                }
            }
            diames[2]=28;
        }
    }
	
    if(numeroanouno==numeroanodos)
    {
        ano=numeroanouno;
        printf("%s\n", "despues... ... al entrar... ");
        printf("%i\n", numeroanouno );
	char prueba[4];
	sprintf(prueba,"%i",numeroanouno);
	printf("%s\n", "despues... ... al entrar... 2");
	printf("%s\n", prueba);
        for(mes=numeromesuno; mes<numeromesdos+1; mes++)
        {

           diafin=diames[mes];
            diainicio=1;
            if(mes==numeromesdos)
            {
                diafin=numerodiados;
            }

            if(mes==numeromesuno)
            {
                diainicio=numerodiauno;
            }
            for(dia=diainicio; dia<diafin+1; dia++)
            if(mes==numeromesuno)
            {
                diainicio=numerodiauno;
            }
            for(dia=diainicio; dia<diafin+1; dia++)
            {
                memset(cadenadia, 0,2);
                memset(cadenames, 0,2);
                memset(cadenaano, 0,4);
		
		itoa(ano,cadenaano,10);
                if(dia<10)
                {
                    strncpy(cadenadia,"0",1);
                }
                itoa(dia,cadenadiaaux,10);
                strcat(cadenadia,cadenadiaaux);
                if(mes<10)
                {
                    strncpy(cadenames,"0",1);
                }
                itoa(mes,cadenamesaux,10);

                strcat(cadenames,cadenamesaux);



		fprintf(file1,"%s-%s-%s* ",prueba,cadenames,cadenadia);
            }
        }
    }

	// Cambiar por la carpeta...

    fprintf(file1,"%s\n","/home/pi/Desktop/registrador14/registros");
    fflush(file1);



// hasta aqui ok
	
	printf("%s\n","Hasta aqui--- OK3 ");
	system("sh prueba.sh");
	
		
}
void b10PopCallback(void *ptr)
{
	

	printf("%s\n" , "Boton ajustes presionado ");

	

	fichero6 = fopen("/home/pi/Documents/historicoconfig/configuracion" , "r");
	char *destino;
	char ubi[100];
        char *pch;        
	fgets(buffer, 99, fichero6);
                printf("%s\n", buffer);
        
	// LEEMOS LA UBICACION DEL DISPOSITIVO: 
	fgets(buffer, 99, fichero6);
                printf("%s\n", buffer);

	printf ("Splitting string \"%s\" into tokens:\n", buffer);

        pch = strtok (buffer," ");
        int i = 0;
	char * ubicacion;

        //Bucle de espacio.
        while (pch != NULL)
        {
	        if(i==1)
                {
			ubicacion = pch;        
                }
                pch = strtok (NULL, " ");
                i=i+1;
        }

	ubicacion = strtok (ubicacion, "\n");
	printf("UBICACION DESDE ARCHIVO : %s\n", ubicacion);


	Serial.print("t28.txt=");
                Serial.write(0x22);
                Serial.print(ubicacion);
                Serial.write(0x22);
                Serial.write(0xff);
                Serial.write(0xff);
                Serial.write(0xff);


	char * instalador;
	// LEEMOS EL TECNICO INSTALADOR DEL DISPOSITIVO:
        fgets(buffer, 99, fichero6);
        printf("%s\n", buffer);

        printf ("Splitting string \"%s\" into tokens:\n", buffer);

        pch = strtok (buffer," ");
        i = 0;
        

        //Bucle de espacio.
        while (pch != NULL)
        {
                if(i==2)
                {
                        instalador = pch;
                }
                pch = strtok (NULL, " ");
                i=i+1;
        }

	printf("INSTALADOR DESDE ARCHIVO : %s\n", instalador);
	instalador  = strtok (instalador , "\n");
            
                Serial.print("t48.txt=");
                Serial.write(0x22);
                Serial.print(instalador);
                Serial.write(0x22);
                Serial.write(0xff);
                Serial.write(0xff);
                Serial.write(0xff);


	char * fechainstalacion ;
        // LEEMOS LA FECHA DE INSTALACION DEL DISPOSITIVO:
        fgets(buffer, 99, fichero6);
        printf("%s\n", buffer);

        printf ("Splitting string \"%s\" into tokens:\n", buffer);

        pch = strtok (buffer," ");
        i = 0;


        //Bucle de espacio.
        while (pch != NULL)
        {
                if(i==2)
                {
                       fechainstalacion  = pch;
                }
                pch = strtok (NULL, " ");
                i=i+1;
        }

	printf("FECHA DE INSTALACION  DESDE ARCHIVO : %s\n", fechainstalacion);
        fechainstalacion  = strtok (fechainstalacion , "\n");

                Serial.print("t38.txt=");
                Serial.write(0x22);
                Serial.print(fechainstalacion);
                Serial.write(0x22);
                Serial.write(0xff);
                Serial.write(0xff);
                Serial.write(0xff);

	fgets(buffer, 99, fichero6);

	pch = strtok(buffer, " ");
	char * tregistros ;
	
	i=0;
	//Bucle de espacio.
        while (pch != NULL)
        {
                if(i==3)
                {
                      tregistros  = pch;
                }
                pch = strtok (NULL, " ");
                i=i+1;
        }
	 printf("TIEMPO DE REGISTROS DESDE ARCHIVO : %s\n", tregistros);
		
	int j = 0;
	j=atoi(tregistros);


		Serial.print("n58.val=");
                Serial.print(j);
                Serial.write(0xff);
                Serial.write(0xff);
                Serial.write(0xff);

	
	
	fclose(fichero6);
	
}


void b12PopCallback(void *ptr)
{
	printf("%s\n", "Enviando la fecha 24");
 
	system("rm dispositivos.txt");
	system("rm numeroconectados.txt");
	system("rm conectados.txt");
	
	char f[20];
        char g[20];
	string h;
	char *destday;
	char *destmonth;

	NexText t1 = NexText(11,2,"t94");
	NexText t2 = NexText(11,4,"t95");

        memset(f, 0, sizeof(f));
        t1.getText(f, sizeof(f));
	
	memset(g, 0, sizeof(g));
        t2.getText(g, sizeof(g));
	
        printf("%s\n" , "Fecha inicio:: ");
        printf("%s\n" , f);
        printf("%s\n" , "Fecha fin::  ");
        printf("%s\n" , g);
	printf("%s\n" , "Cambiando de pagina... ... .. .. .  ");	


	char fechaconsi[10]={0};
	char * diainicio;
	char * mesinicio;
	char * anyoinicio;
	char * pch;
	printf ("Splitting string \"%s\" into tokens:\n", f);

	pch = strtok (f,"/");
	int i = 0;
	//Bucle de espacio. 	
	while (pch != NULL)
	{
		if(i==0)
		{
			diainicio = pch;
		}
		else if(i==1)
		{
			 mesinicio = pch;
		}
		else if(i==2)
		{
                         anyoinicio = pch;
		}
		pch = strtok (NULL, "/");
		i=i+1;
	}
	
	strcat(fechaconsi,anyoinicio);
        strcat(fechaconsi,"-");
        strcat(fechaconsi,mesinicio);
        strcat(fechaconsi,"-");
        strcat(fechaconsi,diainicio);
	printf("%s\n", "al salir del while inicio. ");
	printf("%s\n",fechaconsi);



        char fechaconsf[10]={0};
        char * diafin;
        char * mesfin;
        char * anyofin;
        char * pch1;
        pch1 = strtok (g,"/");

	int j = 0;

	while (pch1 != NULL)
        {
                if(j==0)
                {
                        diafin = pch1;
                }
                else if(j==1)
                {
                         mesfin = pch1;
		}
                else if(j==2)
                {
                         anyofin = pch1;
                }
                pch1 = strtok (NULL, "/");
                j=j+1;
		
        }
	strcat(fechaconsf,anyofin);
	strcat(fechaconsf,"-");
	strcat(fechaconsf,mesfin);
	strcat(fechaconsf,"-");
	strcat(fechaconsf,diafin);
	printf("%s\n","Al salir del while de final");
	printf("%s\n", fechaconsf);

	
	fechasenmedio(fechaconsi,fechaconsf);

system("rm /home/pi/Desktop/registrador14/listadearchivos.txt");
system("rm /home/pi/Desktop/registrador14/numerodearchivoss.txt");

       
        delay(3000);
        page8.show();

	system("sh getDevices.sh");
	FILE *deviceslist;
	deviceslist = fopen("/home/pi/Desktop/registrador14/dispositivos.txt", "r");
	
	
	system("ls /media/pi >> /home/pi/Desktop/registrador14/conectados.txt");
	
        system("wc -l <  /home/pi/Desktop/registrador14/conectados.txt  >> /home/pi/Desktop/registrador14/numeroconectados.txt ");


        system("ls /home/pi/Desktop/registrador14/registros >> listadearchivos.txt");
         system("wc -l <  /home/pi/Desktop/registrador14/listadearchivos.txt  >> /home/pi/Desktop/registrador14/numerodearchivoss.txt ");


	FILE * fichero10 = fopen("/home/pi/Desktop/registrador14/numerodearchivoss.txt", "r");
        char numero1[15];
        char texto[]="Se van a descargar : ";
        fscanf(fichero10,"%s",numero1);

        printf("%s\n",  numero1 );

        strcat(texto, numero1);
	strcat(texto, " archivos ");

        Serial.print("terr3.txt=");
                        Serial.write(0x22);
                        Serial.print(texto);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);



	FILE * fileconn = fopen("/home/pi/Desktop/registrador14/numeroconectados.txt","r");

	char numero[2];	
	fscanf(fileconn,"%s", numero);
	
	 printf("%s\n", "Hay ...");
	 printf("%s", numero);
	 printf("%s", "dispositivo...");
	char * n2 = strtok(numero, "\n");
	char key[] = "1";	
	char key1[] = "2";
	char key2[] = "3";
	//Contamos los archivos antes de copiarlos al pendrive
	system("ls /home/pi/Desktop/registrador14/registros >> listadearchivos.txt");
        system("wc -l <  /home/pi/Desktop/registrador14/listadearchivos.txt  >> /home/pi/Desktop/registrador14/numerodearchivoss.txt ");




	// LISTA ORIGINAL
	if(strcmp(numero,key)==0)
	{
		printf("%s\n", "Hay uno...");
		char str[100];
		fgets (str, 99, deviceslist );
		char * r = strtok(str,"\n");
		Serial.print("d0.txt=");
                        Serial.write(0x22);
                        Serial.print(r);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);
	}
	
	if(strcmp(numero,key1)==0)
        {

		char p[100];
                fgets (p, 99, deviceslist );
                char * s = strtok(p,"\n");
                Serial.print("d0.txt=");
                        Serial.write(0x22);
                        Serial.print(s);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

		

                printf("%s\n", "Hay dos...");
                char str2[100];
                fgets (str2, 99, deviceslist );
		printf("%s\n", str2);
                char * r2 = strtok(str2,"\n");
                Serial.print("d1.txt=");
                        Serial.write(0x22);
                        Serial.print(r2);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);
        }



	if(strcmp(numero,key2)==0)
        {
                printf("%s\n", "Hay tres...");
                char str3[100];
                fgets (str3, 99, deviceslist );
                printf("%s\n", str3);
                char * r3 = strtok(str3,"\n");
                Serial.print("d2.txt=");
                        Serial.write(0x22);
                        Serial.print(r3);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);
        }


	

	printf("%s\n", "Hasta aqui ok 7 ");
}


void b20PopCallback(void *ptr)
{
 // BOTON ACTUALIZAR LISTA ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        system("rm dispositivos.txt");
        system("rm numeroconectados.txt");
        system("rm conectados.txt");


         printf("%s\n", "Actualizando lista de dispositivos conectados.... .....  .. . . ");
        system("sh getDevices.sh");
        FILE *deviceslist;
        deviceslist = fopen("/home/pi/Desktop/registrador14/dispositivos.txt", "r");


        system("ls /media/pi >> /home/pi/Desktop/registrador14/conectados.txt");

        system("wc -l <  /home/pi/Desktop/registrador14/dispositivos.txt  >> /home/pi/Desktop/registrador14/numeroconectados.txt ");

        FILE * fileconn = fopen("/home/pi/Desktop/registrador14/numeroconectados.txt","r");
	printf("%s\n", "Actualizando lista de dispositivos conectados.... .....  .. . . 2 ");


        char key1[] = "1";
        char key2[] = "2";
        char key3[] = "3";
	char key4[] = "4";
	char p[100];
	char str2[100];
        char numero[2];
        fscanf(fileconn,"%s", numero);
	
	if(strcmp(numero,key1)==0)
	{
		printf("%s\n", " despues de actualizar hay uno  ");
		fgets (p, 99, deviceslist );
                char * s = strtok(p,"\n");
                Serial.print("d6.txt=");
                        Serial.write(0x22);
                        Serial.print(s);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);


		Serial.print("d7.txt=");
                        Serial.write(0x22);
                        Serial.print("");
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);
	}

	if(strcmp(numero,key2)==0)
        {
                printf("%s\n", " despues de actualizar hay dos  ");
                fgets (p, 99, deviceslist );
                char * s = strtok(p,"\n");
                Serial.print("d6.txt=");
                        Serial.write(0x22);
                        Serial.print(s);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);




                fgets (str2, 99, deviceslist );
                printf("%s\n", str2);
                char * r2 = strtok(str2,"\n");
                Serial.print("d7.txt=");
                        Serial.write(0x22);
                        Serial.print(r2);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);



        }

}

void b21PopCallback(void *ptr)
{
	system("rm /home/pi/Desktop/registrador14/desmontado.txt");
///////////////////////////////////////BOTON DESCONECTAR///////////////////////////////////////////////////////////////////////////////////////////
	printf("%s\n", "Desconectando dispositivo... .. .. ");
	char codigo[100];
	char comando[200]="sudo umount /media/pi/";
	memset(codigo,0, sizeof(codigo));

	tsel.getText(codigo, sizeof(codigo));	
	printf("%s\n", "Desconectando dispositivo... .. .. 2");
	printf("%s\n", codigo);
	
	// Para cuando el nombre del PENDRIVE tiene espacios... 
	char vare[22];
	memset(vare,0,sizeof(vare));
	strcat(vare,"'");
	
	printf("Despues A: %s\n", vare);
	//sprintf(vare,"%c","'");
	printf("Despues B1: %s\n", vare);
	strcat(vare,codigo);

	strcat(vare,"'");
	printf("Despues B2: %s\n", vare);

	strcat(comando, vare);
	printf("Despues B3: %s\n", comando);
        printf("%s\n", "Desconectando dispositivo... .. .. 3");
        printf("%s\n", comando);
	strcat(comando, " 2> desmontado.txt " );
	system(comando);

	FILE * desfile = fopen("/home/pi/Desktop/registrador14/desmontado.txt", "r");
	char linea[100];	
	fgets(linea, 99, desfile);

	printf("%s\n", linea);

	printf("El archivo contiene %u \n",  (unsigned)strlen(linea));

	if( (unsigned)strlen(linea)<12)
	{
		//  printf("%s\n", "Puede retirar el dispositivo de almacenamiento con seguridad.");
		Serial.print("terr3.txt=");
                        Serial.write(0x22);
                        Serial.print( "Puede retirar el dispositivo de almacenamiento con seguridad." );
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

	}
	else
	{
		//printf("%s\n", "Ha ocurrido un error, y no se ha podido extraer, asegurese de que se trata del dispositivo correcto.");
		Serial.print("terr3.txt=");
                        Serial.write(0x22);
                        Serial.print("Ha ocurrido un error, y no se ha podido extraer, asegurese de que se trata del dispositivo correcto.");
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

	}	
}


void b11PopCallback(void *ptr)
{
	printf("%s", "boton descarga pulsado 8 . \n");
	char fini[20];
	char ffin[20];

	memset(ffin, 0, sizeof(ffin));
        tfin.getText(ffin, sizeof(ffin));

	memset(fini, 0, sizeof(fini));
	tini.getText(fini, sizeof(fini));

	printf("%s\n" , "Fecha inicio:: ");
	printf("%s\n" , fini);
	printf("%s\n" , "Fecha fin::  ");
	printf("%s\n" , ffin );
       
}


void b9PopCallback(void *ptr)
{

///////////////////////////////////////////////////////BOTON DESCARGA EN EL ASISTENTE////////////////////////////////////////////////////////////////
	printf("%s\n",  "ASISTENTE DE DESCARGA" );
	        
        char codigo[100];
        char comando[100]="mkdir /media/pi/";
        char strcode[12];
	char nombre[50];	
	char carpeta1[60]="descarga";	

	NexText terr3 = NexText(12,26,"terr3");

system("rm /home/pi/Desktop/registrador14/listadearchivos.txt");
system("rm /home/pi/Desktop/registrador14/numerodearchivoss.txt");

	char cadena[300]="scp -r /home/pi/Desktop/registrador14/registros/ /media/pi/";

	printf("Despues de inicializar:  %s\n", cadena);

	int year = tm.tm_year+1900;
        int month = tm.tm_mon+1;
        char anyo[4];
        char mes[3];
        char dia[2];
        char hora[2];
        char minu[2];
        char secu[2];
	FILE * rutadescarga = fopen("/home/pi/Desktop/registrador14/rutadescarga.txt", "w");
	FILE * fechadescarga = fopen("/home/pi/Desktop/registrador14/fechadescarga.txt", "w");



	fputs(cadena,rutadescarga);
	fflush(rutadescarga);
	fclose(rutadescarga);

        sprintf(anyo, "%i_", year);  
	fputs(anyo,fechadescarga);
	fflush(fechadescarga);
        sprintf(mes, "%i_",tm.tm_mon+1);
fputs(mes,fechadescarga);
 fflush(fechadescarga);
        sprintf(dia, "%i_",tm.tm_mday);
fputs(dia,fechadescarga);
 fflush(fechadescarga);

	sprintf(hora,"%i_",tm.tm_hour);
fputs(hora,fechadescarga);
 fflush(fechadescarga);

        sprintf(minu,"%i_",tm.tm_min);
fputs(minu,fechadescarga);
 fflush(fechadescarga);

        sprintf(secu,"%i",tm.tm_sec);
fputs(secu,fechadescarga);
 fflush(fechadescarga);
fclose(fechadescarga);

	memset(codigo,0, sizeof(codigo));

	fichero5 = fopen("/home/pi/Desktop/registrador12/deviceCode","r");
        tsel.getText(codigo, sizeof(codigo));
	
	
        char fechadedescarga[100];
	char rutadedescarga[100];
	fscanf(fichero5,"%s", strcode);

	FILE * fichero8 = fopen("/home/pi/Desktop/registrador14/fechadescarga.txt","r");
	FILE * fichero9 = fopen("/home/pi/Desktop/registrador14/rutadescarga.txt","r");


	fscanf(fichero8,"%s",fechadedescarga);
	fgets(rutadedescarga, 99, fichero9);
	//fscanf(fichero9,"%s",rutadedescarga); // aqui almacenamos la ruta donde se va a descargar, si no se sobreescribe la variable.
	
	

 printf("Despues de inicializar 3 :  %s\n", rutadedescarga);
	//strcat(rutadedescarga,codigo);
	//strcat(comando, codigo);	

	char sinespacios[22];
	strcat(sinespacios, "'");		
	strcat(sinespacios, codigo);
	strcat(sinespacios, "'");
	 printf("Despues de inicializar 3b :  %s\n", sinespacios);
        strcat(rutadedescarga,sinespacios);
        strcat(comando, sinespacios);




 printf("Despues de inicializar 4 :  %s\n", rutadedescarga);
	strcat(comando, "/descarga_");
	 strcat(rutadedescarga,"/descarga_");
printf("Despues de inicializar 5 :  %s\n", rutadedescarga);	
	strcat(comando,strcode);
	 strcat(rutadedescarga,strcode);
printf("Despues de inicializar 6 :  %s\n", rutadedescarga);
	strcat(comando,"_");
	strcat(rutadedescarga,"_");
	

	strcat(comando, fechadedescarga);
	strcat(rutadedescarga,fechadedescarga);
	printf("%s\n", comando);
	strcat(rutadedescarga, " 2> resultadocopia.txt");

printf("Despues de inicializar 7 :  %s\n", rutadedescarga);
printf("Despues de inicializar 8 :  %s\n", comando);



	system(comando);
	system(rutadedescarga);

	system("ls /home/pi/Desktop/registrador14/registros >> listadearchivos.txt");
	 system("wc -l <  /home/pi/Desktop/registrador14/listadearchivos.txt  >> /home/pi/Desktop/registrador14/numerodearchivoss.txt ");
 
	printf("%s\n",  "ASISTENTE DE DESCARGA 3b" );
	
	printf("%s\n", fechadedescarga);	
	
	printf("%s\n",  "ASISTENTE DE DESCARGA 4" );

	FILE * fichero10 = fopen("/home/pi/Desktop/registrador14/numerodearchivoss.txt", "r");
	char numero[15];
	char texto[]="Archivos descargados: ";		
	fscanf(fichero10,"%s",numero);

	printf("%s\n",  numero );

	strcat(texto, numero);

	FILE * rescp = fopen("/home/pi/Desktop/registrador14/resultadocopia.txt", "r");
        char linea[100];
        fgets(linea, 99, rescp);

        printf("%s\n", linea);

        printf("El archivo contiene %u \n",  (unsigned)strlen(linea));

        if( (unsigned)strlen(linea)>12)
	{
		Serial.print("terr3.txt=");
                        Serial.write(0x22);
                        Serial.print("Ha ocurrido un error al copiar los registros, seleccione el dispositivo correcto. ");
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

	}
	else
	{


	Serial.print("terr3.txt=");
                        Serial.write(0x22);
                        Serial.print(texto);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);
	}

	
}

void b0PopCallback(void *ptr)
{
        int year = tm.tm_year+1900;
        int month = tm.tm_mon+1;
        char anyo[4];
        char mes[3];
        char dia[2];
        char hora[2];
        char minu[2];
        char secu[2];
        char fecha2[20];
        sprintf(anyo, "%i_", year);
        sprintf(mes, "%i_",tm.tm_mon+1);
        sprintf(dia, "%i_",tm.tm_mday);

        sprintf(hora,"%i_",tm.tm_hour);
        sprintf(minu,"%i_",tm.tm_min);
        sprintf(secu,"%i",tm.tm_sec);
        strcat(newname1,anyo);
        strcat(newname1,mes);
        strcat(newname1,dia);
        strcat(newname1,hora);
        strcat(newname1,minu);
        strcat(newname1,secu);

	char fechamod[20];
        strcat(fechamod,anyo);
        strcat(fechamod,mes);
        strcat(fechamod,dia);
        strcat(fechamod,hora);
        strcat(fechamod,minu);
        strcat(fechamod,secu);

        rename(oldname1,newname1);
        

        printf("%s\n", "Persiguiendo segmentation fault");


	char comandocopia[200]="cp /home/pi/Documents/historicoconfig/configuracion /home/pi/Documents/historicoconfig/";
	//	system("cp /home/pi/Documents/historicoconfig/configuracion /home/pi/Documents/historicoconfig/%s ",newname1);
	
	strcat(comandocopia,newname1);

	//	 printf("%s\n", comandocopia);

	system(comandocopia);

	FILE * pFile = fopen ("/home/pi/Documents/historicoconfig/configuracion","w+");
	char fec[]="FECHA MODIFICACION: ";
	strcat(fec,fechamod);

	fputs (fec ,pFile);
	fputs ("\n" ,pFile);
	char ubicaciondisp[100];
	memset(ubicaciondisp, 0, sizeof(ubicaciondisp));
	t28.getText(ubicaciondisp, sizeof(ubicaciondisp));
	
	char ubicaciondis1[]="UBICACION: ";
	strcat(ubicaciondis1, ubicaciondisp);

	fputs(ubicaciondis1, pFile);
	fputs("\n",pFile);

	char tcoinstalador[100];
	char tcoinstalador1[]="TECNICO INSTALADOR: ";
        memset(tcoinstalador, 0, sizeof(tcoinstalador));
        t48.getText(tcoinstalador,sizeof(tcoinstalador));
	strcat(tcoinstalador1, tcoinstalador);
	fputs(tcoinstalador1,pFile);
	fputs("\n",pFile);

//	fputs ("TECNICO INSTALADOR: JAOMIX\n" ,pFile);

//	printf("%s\n ", "Dichoso segmentation fault");
	
	char fechainstalacion[100];
        char fechainstalacion1[]="FECHA INSTALACION: ";
        memset(fechainstalacion, 0, sizeof(fechainstalacion));
        t38.getText(fechainstalacion,sizeof(fechainstalacion));
        strcat(fechainstalacion1, fechainstalacion);
        fputs(fechainstalacion1,pFile);
        fputs("\n",pFile);

	char tiempopromedio[100];
	char tiempopromedio1[]="TIEMPO DE REGISTROS: ";
	memset(tiempopromedio, 0, sizeof(tiempopromedio));
	t58.getText(tiempopromedio, sizeof(tiempopromedio));	
	strcat(tiempopromedio1,tiempopromedio);

	printf("%s\n " , tiempopromedio1);

	if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 01")==0 )
	{
		//		 tiempopromedio1= "TIEMPO DE REGISTROS: 1";
		fputs( "TIEMPO DE REGISTROS: 1" ,pFile);
		fputs("\n",pFile);	 
	}
	else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 02")==0 )
	{
		fputs( "TIEMPO DE REGISTROS: 2" ,pFile);
                fputs("\n",pFile);
	}else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 03")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 3" ,pFile);
                fputs("\n",pFile);
        }else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 04")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 4" ,pFile);
                fputs("\n",pFile);
        }else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 05")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 5" ,pFile);
                fputs("\n",pFile);
        }else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 06")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 6" ,pFile);
                fputs("\n",pFile);
        }else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 07")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 7" ,pFile);
                fputs("\n",pFile);
        }else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 08")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 8" ,pFile);
                fputs("\n",pFile);
        }else if( strcmp(tiempopromedio1, "TIEMPO DE REGISTROS: 09")==0 )
        {
                fputs( "TIEMPO DE REGISTROS: 9" ,pFile);
                fputs("\n",pFile);
        }
	else 
	{
	       fputs( tiempopromedio1 ,pFile);
                fputs("\n",pFile);
        }
	fflush(pFile);
	
//	system("killall tcn");	
	system("./tcn");


}


void setup()
{
	nexInit();
//	system("./hora");
	b0.attachPop(b0PopCallback, &b0);
	b9.attachPop(b9PopCallback, &b9);   
	b10.attachPop(b10PopCallback, &b10);     
        b11.attachPop(b11PopCallback, &b11);
	b12.attachPop(b12PopCallback, &b12);
	b20.attachPop(b20PopCallback, &b20);
	b21.attachPop(b21PopCallback, &b21);
	b11.setText("10");
}


void loop()
{ 

	//   tcod.setText("TCNPRUEBA");

	// Para que este pendiente de los botones.
	nexLoop(nex_listen_list);
	fichero5 = fopen("/home/pi/Desktop/registrador12/deviceCode","r");
	fichero = fopen("/home/pi/Documents/data/instantaneo","r");
	while(!feof(fichero))
	{
		// recogemos los valores del archivo de instantaneo
		// el primero varia cada segundo es el LAeq1s
		// el segundo valor es el LAeq1min 
		// el tercer valor es el LCeq1min
		// el cuarto es el LAmax
		// el quinto es el LAmin
		// el sexto es la hora a la que se produce


		fscanf(fichero5,"%s",strcode);
		fscanf(fichero, "%s %s %s %s %s %s\n", medida, str12, str13, str14, str15, str16);
		
		Serial.print("t0.txt=");
                        Serial.write(0x22);
                        Serial.print(strcode);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);
		

                Serial.print("t890.txt=");
                        Serial.write(0x22);
                        Serial.print(strcode);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

		// Comprobamos...
		//	printf("%s\n", "para el t3. ");
		//	printf("%s\n", medida);
			
		// Mostramos el LAeq1s
		   //En la pantalla de texto
			Serial.print("t3.txt=");
			Serial.write(0x22);
			Serial.print(medida);
			Serial.write(0x22);	
			Serial.write(0xff);
			Serial.write(0xff);
			Serial.write(0xff);

		 //    y en el vumetro y la grafica
			
			destmedida  = strtok(medida,".");
                       // printf("%s\n", "para el vumetro. ");
                       // printf("%s\n", destmedida);

					
			i = atoi(destmedida);
			printf("%i\n", i);
			s0.addValue(0,i);

			Serial.print("j0.val=");
			Serial.print(i);
			Serial.write(0xff);
			Serial.write(0xff);
			Serial.write(0xff);

		
			//fgets(str12,5,fichero);
                        // LeqA 1min
                        Serial.print("t2.txt=");
                        Serial.write(0x22);
                        Serial.print(str12);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

			//fgets(str13,5,fichero);

		
			// LeqC 1min
			Serial.print("t109.txt=");
			Serial.write(0x22);
			Serial.print(str13);
			Serial.write(0x22);
			Serial.write(0xff);
			Serial.write(0xff);
			Serial.write(0xff);

			//fgets(str14,5,fichero);
			// LAmin
			Serial.print("t4.txt=");
			Serial.write(0x22);
			Serial.print(str14);
			Serial.write(0x22);
			Serial.write(0xff);
			Serial.write(0xff);
			Serial.write(0xff);
                  
			//fgets(str15,5,fichero);
			// LAmax
			Serial.print("t110.txt=");
			Serial.write(0x22);
			Serial.print(str15);
			Serial.write(0x22);
			Serial.write(0xff);
			Serial.write(0xff);
			Serial.write(0xff);

                  // Mostramos la hora del valor minuto a minuto
                        //hora
			//fgets(str16,5,fichero);

                        Serial.print("t1.txt=");
                        Serial.write(0x22);
                        Serial.print(str16);
                        Serial.write(0x22);
                        Serial.write(0xff);
                        Serial.write(0xff);
                        Serial.write(0xff);

		
	}delay(1000);
}

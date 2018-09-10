#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void fechasenmedio(char fechauno[10], char fechados[10]);

int main (void)
{
	FILE *proceso;
	char inicio[]="2018-03-11";
	char fin[]="2019-03-20";
	fechasenmedio(inicio,fin);
	proceso = popen("sh /home/galadriel/Escritorio/copiar.sh", "w");
	return 0;
}

	
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

    char cadenadia[2];
    char cadenadiaaux[2];
    char cadenames[2];
    char cadenamesaux[2];
    char cadenaano[4];

    FILE *fi;
    fi=fopen("/home/pi/data/copiar.sh","w");
    fprintf(fi,"%s\n", "cd /home/pi/Documents/registros");
    fprintf(fi,"%s","scp -r ");

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
                    fprintf(fi,"%s-%s-%s ",cadenaano,cadenames,cadenadia);
                }
            }
            diames[2]=28;
        }
    }

    if(numeroanouno==numeroanodos)
    {
        ano=numeroanouno;
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
                fprintf(fi,"%s-%s-%s ",cadenaano,cadenames,cadenadia);
            }
        }
    }

    fprintf(fi,"%s\n","/home/galadriel/Escritorio");
    return;
}
		 		
		

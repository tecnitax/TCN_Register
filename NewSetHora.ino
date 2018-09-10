
#include "Nextion.h"
#include <sys/time.h>
#include <stdio.h>

 
NexText t1 = NexText(0, 14, "tday");
NexText t2 = NexText(0, 16, "tmonth");

NexText t3 = NexText(0, 17, "tyear");
NexText t6 = NexText(0, 30, "t6");
NexText t7 = NexText(0, 18, "thour");
NexText t8 = NexText(0, 19, "tminute");
NexText t9 = NexText(0, 20, "tsecond");
NexText t10 = NexText(0, 31, "t7");

char buffer[5] = {0};
char buffer2[2];
char buffer3[5] ={0};
char buffer4[5],buffer5[5],buffer6[5],buffer7[5];

struct tm mytime;
struct timeval garfield;
time_t new_time;

char  cadena[50] = "sudo date -s '12 JAN 2016 20:37:00'";

char cadena2[90] = "sudo date -s ";

FILE *salida;

char buffer12[100];

FILE *fichero;


void setup(void)
{
	nexInit();
	t1.setText("11");
	salida = fopen("/home/pi/Desktop/aitesco/output.txt","w+");

	// Dia del año de la pantalla
        memset(buffer,0,sizeof(buffer));
        t6.getText(buffer,sizeof(buffer));
        fputs("\n", salida);
        fputs(buffer,salida);
        fputs("\n", salida);

	// Mes del año de la pantalla
        memset(buffer2,0,sizeof(buffer2));
        t2.getText(buffer2,sizeof(buffer2));
        fputs("\n", salida);
        fputs(buffer2,salida);
        fputs("\n", salida);

	// año de la pantalla
	memset(buffer3,0, sizeof(buffer3));
	t3.getText(buffer3,sizeof(buffer3));
        fputs("\n", salida);
        fputs(buffer3,salida);
        fputs("\n", salida);

	// hora de la pantalla
	memset(buffer4, 0, sizeof(buffer3));
	t7.getText(buffer4, sizeof(buffer4));
        fputs("\n", salida);
        fputs(buffer4 ,salida);
        fputs("\n", salida);

        // minutos  de la pantalla
        memset(buffer5, 0, sizeof(buffer5));
        t8.getText(buffer5, sizeof(buffer5));
        fputs("\n", salida);
        fputs(buffer5,salida);
        fputs("\n", salida);


        // segundos de la pantalla
        memset(buffer6, 0, sizeof(buffer6));
        t9.getText(buffer6, sizeof(buffer6));
        fputs("\n", salida);
        fputs(buffer6 , salida);
        fputs("\n", salida);


	// mes en letra que desde ino no se convierte
        memset(buffer7, 0, sizeof(buffer7));
        t10.getText(buffer7, sizeof(buffer7));
        fputs("\n", salida);
        fputs(buffer7 , salida);
        fputs("\n", salida);

	strcat(cadena2,"'");
	strcat(cadena2,buffer);	
	strcat(cadena2," ");
	strcat(cadena2,buffer7);
	
	strcat(cadena2," ");
	strcat(cadena2,buffer3);
	strcat(cadena2," ");
	strcat(cadena2,buffer4);
	strcat(cadena2,":");
	strcat(cadena2,buffer5);
	strcat(cadena2,":");
	strcat(cadena2,buffer6);
	strcat(cadena2,"'");

	fputs(cadena2 , salida);
        fputs("\n", salida);
	printf("%s\n", cadena2);
	system(cadena2);
	fclose(salida);

}

void loop(void)
{

	exit(0);

}

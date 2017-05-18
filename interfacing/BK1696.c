/*


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "kenBoard.h"
#include "BK1696.h"



int initializeBK1696(int address){
	char chardata[64];
	char command[16];
	char add[5];
	int j;

	strncpy(command,"SESS",sizeof(command));
	initialize_rs485(9600,25);
	// convert address to a two character string
		j=address/10;
		add[0]=48+j;
		j=address % 10;
		add[1]=48+j;
		add[2]=13;//the B&K expects a CR at the end of each command
		add[3]='\0';
	strcat(command,add);
	write_rs485ASCII(command,chardata);
	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr(chardata,"OK") != NULL){ // found "OK"
		j=0;
	}
return j;
}

int setOutputBK1696(int address,int status){
	char chardata[64];
	char command[16];
	char add[5];
	int j;

	strncpy(command,"SOUT",sizeof(command));
	initialize_rs485(9600,25);
	// convert address to a two character string
		j=address/10;
		add[0]=48+j;
		j=address % 10;
		add[1]=48+j;
	
	if (status==0) {
	add[2]=48;
	} else {
	add[2]=49;
	}
		add[3]=13;//the B&K expects a CR at the end of each command
		add[4]='\0';
	strcat(command,add);
	write_rs485ASCII(command,chardata);
	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr(chardata,"OK") != NULL){ // found "OK"
		j=0;
	}
return j;
}



int getVoltsAmpsBK1696(int address, float* volts, float* amps){
	char chardata[64];
	char command[16]="GETD";
	char add[5];
	int j,i;
	float tempv;

	initialize_rs485(9600,25);

	// convert address to a two character string
		j=address/10;
		add[0]=48+j;
		j=address % 10;
		add[1]=48+j;
		add[2]=13;
		add[3]='\0';

	strcat(command,add);

	write_rs485ASCII(command,chardata);
	j=-1;
	if ((strstr(chardata,"OK") != NULL)&(strlen(chardata)==11)){
		for (i=0;i<4;i++){
			command[i]=chardata[i];
		}
		command[i]='\0';
		tempv = atof(command);
		*volts = tempv/100.0;


		for (i=0;i<4;i++){
			command[i]=chardata[i+4];
		}
		command[i]='\0';
		tempv = atof(command);
		*amps = tempv/1000.0;
	 j=0;
	}

return j;	
}


int setVoltsBK1696(int address, float volts){
	char chardata[64];
	char command[16]="VOLT";
	char s[5];
	int j,i;

if (volts>20.0) volts=20.0;
if (volts<0.0) volts=0.0;

	initialize_rs485(9600,25);

	// convert address to a two character string
		j=address/10;
		s[0]=48+j;
		j=address % 10;
		s[1]=48+j;
		//s[2]=13;
		s[2]='\0';

	strcat(command,s);

	j = (int)lroundf(volts*10.0);
//printf("j= %d\t",j);
	i = j/100;
//printf("i= %d\t",i);
	s[0] = 48+i;
	j = j-(i*100);
	i = j/10;
//printf("i= %d\t",i);
	s[1] = 48+i;
	i = j%10;
//printf("i= %d\n",i);
	s[2] = 48+i;

	s[3] = 13;
	s[4] = '\0';
	

	strcat(command,s);

//	printf("%s\n",command);


	write_rs485ASCII(command,chardata);
	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr(chardata,"OK") != NULL){ // found "OK"
		j=0;
	}else{
	printf("BK1696 error\n");
	printf("Requested volts %f\n",volts);
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);

	}

return j;


}


int setAmpsBK1696(int address, float amps){
	char chardata[64];
	char command[16]="CURR";
	char s[5];
	int j,i;

if (amps>10.0) amps=10.0;
if (amps<0.0) amps=0.0;

	initialize_rs485(9600,25);

	// convert address to a two character string
		j=address/10;
		s[0]=48+j;
		j=address % 10;
		s[1]=48+j;
		//s[2]=13;
		s[2]='\0';

	strcat(command,s);

	j = (int)lroundf(amps*100.0);
printf("j= %d\t",j);
	i = j/100;
printf("i= %d\t",i);
	s[0] = 48+i;
	j = j-(i*100);
	i = j/10;
printf("i= %d\t",i);
	s[1] = 48+i;
	i = j%10;
printf("i= %d\n",i);
	s[2] = 48+i;

	s[3] = 13;
	s[4] = '\0';
	

	strcat(command,s);

	printf("%s\n",command);


	write_rs485ASCII(command,chardata);
	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr(chardata,"OK") != NULL){ // found "OK"
		j=0;
	}else{
	printf("BK1696 error\n");
	printf("Requested amps %f\n",amps);
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);

	}

return j;




}

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>

#include "Jsensors.h"


int main (void){

	short acc_raw[3];

	short comp_raw[3];

	short gyro_raw[3];

	if(TestSensors()){

		SetupSensors();

		for(;;){

			FetchAcc(acc_raw);
			FetchComp(comp_raw);
			FetchGyro(gyro_raw);

			printf("\n%4.2f    %4.2f    %4.2f           %4.2f    %4.2f    %4.2f              %4.2f    %4.2f    %4.2f", 
			acc_raw[0]/256.0, acc_raw[1]/256.0, acc_raw[2]/256.0, comp_raw[0]/256.0, comp_raw[1]/256.0, comp_raw[2]/256.0, 
			gyro_raw[0]/256.0, gyro_raw[1]/256.0, gyro_raw[2]/256.0);


			delay(20);
		}

	}else{

		printf("\n Sensor Error!\n");

	}

	return 0;
}





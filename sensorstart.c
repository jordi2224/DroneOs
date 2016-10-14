#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

	char ACC_ADD = 0x53;
		char ACC_X0 = 0x32;
		char ACC_X1 = 0x33;
		char ACC_Y0 = 0x34;
		char ACC_Y1 = 0x35;
		char ACC_Z0 = 0x36;
		char ACC_Z1 = 0x37;
	char GYRO_ADD = 0x68;
		char GYRO_X0 = 0x1D;
		char GYRO_X1 = 0x1E;
		char GYRO_Y0 = 0x1F;
		char GYRO_Y1 = 0x20;
		char GYRO_Z0 = 0x21;
		char GYRO_Z1 = 0x22;
		char GYRO_T0 = 0x1B;
		char GYRO_T1 = 0x1C;
	char COMP_ADD = 0x1E;
		char COMP_X0 = 0x03;
		char COMP_X1 = 0x04;
		char COMP_Y0 = 0x05;
		char COMP_Y1 = 0x06;
		char COMP_Z0 = 0x07;
		char COMP_Z1 = 0x08;

	int gyrox_offset = 0;
	int gyroy_offset = 0;
	int gyroz_offset = 0;


void FetchAcc(short acc_raw[]);	//Fetches raw data from ADXL345 accelerometer

void FetchComp(short comp_raw[]);	//Fetches raw data from HMC5883L magnetometer

void FetchGyro(short gyro_raw[]);	//Fetches raw dara from ITG3200

void CaliGyro();	//Samples and calibrates the gyroscope by modifying offsets

void SetupSensors();	//Sets sensors for measurments

int TestSensors();		//Checks sensors are working

char *shm, *s;

int main(){

	key_t key;
	int shmid;
	int size;
	
	key = 2000;

	char *shm, *s;
	size = 42;

	short acc_raw[3];
	short comp_raw[3];
	short gyro_raw[3];


	if((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0){
		perror("shmget");
	}

	if((shm = shmat(shmid, NULL, 0)) == (char *) -1){
		perror("shmat");
	}

	s = shm;
	*s = 0;

	if(TestSensors()){

		printf("\nDONE!\n");

		SetupSensors();

		for(;;){

			FetchAcc(acc_raw);
			FetchComp(comp_raw);
			FetchGyro(gyro_raw);

			delay(2);
		}

	}else{
		printf("\n FATAL ERROR");

	}

	return 0;
}



void FetchAcc(short acc_raw[]){

	int fd = wiringPiI2CSetup(ACC_ADD);
	s=shm+1;
	*s = wiringPiI2CReadReg8(fd, ACC_X0);
	s=shm+2;
	*s = wiringPiI2CReadReg8(fd, ACC_X1);
	s=shm+3;
	*s = wiringPiI2CReadReg8(fd, ACC_Y0);
	s=shm+4;
	*s = wiringPiI2CReadReg8(fd, ACC_Y1);
	s=shm+5;
	*s = wiringPiI2CReadReg8(fd, ACC_Z0);
	s=shm+6;
	*s = wiringPiI2CReadReg8(fd, ACC_Z1);

}


void FetchComp(short comp_raw[]){

	int fd = wiringPiI2CSetup(COMP_ADD);
	s=shm+13;
	*s = wiringPiI2CReadReg8(fd, COMP_X0);
	s=shm+14;
	*s = wiringPiI2CReadReg8(fd, COMP_X1);
	s=shm+15;
	*s = wiringPiI2CReadReg8(fd, COMP_Y0);
	s=shm+16;
	*s = wiringPiI2CReadReg8(fd, COMP_Y1);
	s=shm+17;
	*s = wiringPiI2CReadReg8(fd, COMP_Z0);
	s=shm+18;
	*s = wiringPiI2CReadReg8(fd, COMP_Z1);

}


void SetupSensors(){


	//Sensor setup:

		//Acc:
		int fd2 = wiringPiI2CSetup(ACC_ADD);
		wiringPiI2CWriteReg8(fd2, 0x2D, 0x08);	//Sets accelerometer to 2g range

		//Gyro:
		fd2 = wiringPiI2CSetup(GYRO_ADD);
		wiringPiI2CWriteReg8(fd2, 0x16, 0x18);	//Sets gyro to 8kHz Sample Rate
		CaliGyro();	//Calibrates gyro

		//Comp
		int fd3 = wiringPiI2CSetup(COMP_ADD);
		wiringPiI2CWriteReg8(fd3, 02, 0x00);	//Sets comp to constant measurement


}

int TestSensors(){

	int id_1;
	int id_3;
	int res;

	int fd1 = wiringPiI2CSetup(ACC_ADD);
	int fd3 = wiringPiI2CSetup(COMP_ADD);

	id_1 = wiringPiI2CReadReg8(fd1, 0x00);
	id_3 = wiringPiI2CReadReg8(fd3, 0x00);

	
	if( id_1>=0 && id_3>=0){
		res=1;
	}else{
		res=0;
	}
	
	return res;
}

void FetchGyro(short gyro_raw[]){

	int fd = wiringPiI2CSetup(GYRO_ADD);
	int meas;
	int i;
	int n=10;


	meas=0;
	for(i=0; i<n; i++){
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_X0);
	}
	gyro_raw[0] = (meas/n) + gyrox_offset;
	
	s=shm+7;		
	*s = (unsigned char)(gyro_raw[0]>>8);
	s=shm+8;
	*s = (unsigned char) gyro_raw[0] & 0xf;

	meas=0;
	for(i=0; i<n; i++){
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Y0);
	}
	gyro_raw[1] = (meas/n) + gyroy_offset;

	s=shm+9;		
	*s = (unsigned char)(gyro_raw[1]>>8);
	s=shm+10;
	*s = (unsigned char) gyro_raw[1] & 0xf;

	meas=0;
	for(i=0; i<n; i++){
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Z0);
	}
	gyro_raw[2] = (meas/n) + gyroz_offset;

	s=shm+11;		
	*s = (unsigned char)(gyro_raw[2]>>8);
	s=shm+12;
	*s = (unsigned char) gyro_raw[2] & 0xff;


}

void CaliGyro(){

	int i;
	int n=1000;
	int meas;

	int fd = wiringPiI2CSetup(GYRO_ADD);

	meas=0;
	for(i=0; i<n; i++){
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_X0);
	}
	gyrox_offset = -(meas/n);

	meas=0;
	for(i=0; i<n; i++){
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Y0);
	}
	gyroy_offset = -(meas/n);

	meas=0;
	for(i=0; i<n; i++){
		meas= meas + wiringPiI2CReadReg16(fd, GYRO_Z0);
	}
	gyroz_offset = -(meas/n);

}


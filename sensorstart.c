#include <wiringPiI2C.h>
#include <wiringPi.h>

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

	char MEM_ADD = 0x50;
		
	int gyrox_offset = 0;
	int gyroy_offset = 0;
	int gyroz_offset = 0;

	
void FetchAcc(short acc_raw[]);	//Fetches raw data from ADXL345 accelerometer

void FetchComp(short comp_raw[]);	//Fetches raw data from HMC5883L magnetometer

void SetupSensors();	//Sets sensors for measurments

int TestSensors();		//Checks sensors are working


int main(){

	short acc_raw[3];
	short comp_raw[3];
	int fd1, fd2;

	if(TestSensors()){

		SetupSensors();
		
		fd1 = wiringPiI2CSetup(MEM_ADD);
		wiringPiI2CWriteReg8(fd1, 0x00, 1);
		
		for(;;){

			FetchAcc(acc_raw);
			FetchComp(comp_raw);

			fd2 = wiringPiI2CSetup(MEM_ADD+1);
			wiringPiI2CWriteReg16(fd2, 0x00, acc_raw[0]);
			wiringPiI2CWriteReg16(fd2, 0x02, acc_raw[1]);
			wiringPiI2CWriteReg16(fd2, 0x04, acc_raw[2]);
			
			//TODO write bearing to EEPROM

			delay(2);
		}

	}else{
		
		fd1 = wiringPiI2CSetup(MEM_ADD);
		wiringPiI2CWriteReg8(fd1, 0x00, 0);

	}
	
	
	return 0;
}



void FetchAcc(short acc_raw[]){
	int fd = wiringPiI2CSetup(ACC_ADD);
	acc_raw[0] = wiringPiI2CReadReg16(fd, ACC_X0);
	acc_raw[1] = wiringPiI2CReadReg16(fd, ACC_Y0);
	acc_raw[2] = wiringPiI2CReadReg16(fd, ACC_Z0);

}


void FetchComp(short comp_raw[]){

	int fd = wiringPiI2CSetup(COMP_ADD);
	comp_raw[0] = wiringPiI2CReadReg16(fd, COMP_X0);
	comp_raw[1] = wiringPiI2CReadReg16(fd, COMP_Y0);
	comp_raw[2] = wiringPiI2CReadReg16(fd, COMP_Z0);

}


void SetupSensors(){


	//Sensor setup:

		//Acc:
		int fd1 = wiringPiI2CSetup(ACC_ADD);
		wiringPiI2CWriteReg8(fd1, 0x2D, 0x08);	//Sets accelerometer to 2g range

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

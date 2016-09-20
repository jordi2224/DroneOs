#include <wiringPi.h>
#include <wiringPiI2C.h>

int main(){

	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);

	digitalWrite(0, HIGH); //Turns on CPU status LED

	for(;;){
		if(/* TODO read EEPROM battery status */){
			digitalWrite(1, HIGH);
		}else{
			digitalWrite(1, LOW);
		}

		if(/* TODO read EEPROM sensor status */){
			digitalWrite(1, HIGH);
		}else{
			digitalWrite(1, LOW);
		}

		if(/* TODO read EEPROM comms status */){
			digitalWrite(1, HIGH);
		}else{
			digitalWrite(1, LOW);
		}

		if(/* TODO read EEPROM motors status */){
			digitalWrite(1, HIGH);
		}else{
			digitalWrite(1, LOW);
		}


		delay(100);	//Avoids saturating the I2C channel

	}


	return 0;
}

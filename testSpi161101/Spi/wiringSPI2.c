#include <stdint.h>
#include <stdio.h>
#include <wiringPiSPI.h>
#include <unistd.h>

void main(void){
	int chan = 1;
	int speed = 50000;
	uint8_t buff[3];
	
	if(wiringPiSPISetup(chan, speed) == -1){
		printf("could not initialize SPI\n");
		return;
	}
	

	while(1){
		buff[0] = 1;
		buff[1] = 2;
		buff[2] = 3;
		wiringPiSPIDataRW(chan, buff, 3);
		
		sleep(1);
	}
}

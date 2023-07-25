#include "sht30.h"
#include <bcm2835.h>
#define ADDR_SHT30 0x44

void sht30_init(){
    printf("Configurando sensor SHT30 ... \n");

    	if (!bcm2835_init())
    	{
      		printf("bcm2835_init failed. Are you running as root??\n");
      		exit();
    	}

      	if (!bcm2835_i2c_begin())
      	{
        	printf("bcm2835_i2c_begin failed. Are you running as root??\n");
			exit();
      	}
        bcm2835_i2c_setSlaveAddress( ADDR_SHT30 );
	    bcm2835_i2c_set_baudrate(100000);
}

Data sht30_getdata(){
    Data sensorData;
    float tempC, humR;
    char comando[2], sensor[6];
	unsigned short int temp, hum;
    register i;
    comando[0] = 0x2C;
	comando[1] = 0x0D;
	bcm2835_i2c_write(comando, 2);
    sleep(1);
    bcm2835_i2c_read(sensor, 6);
	temp = sensor[0] << 8 | sensor[1];
	sensorData.temperature = -45 + 175*(temp / 65535.0);
	hum = sensor[3] << 8 | sensor[4];
	sensorData.humidity = 100*(hum / 65535.0);
    return sensorData;
}
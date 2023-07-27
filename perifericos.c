/*******************************************************************************
* perifericos.c - iniIIC_SHT30();
* Realiza la inicialización de IIC y del sensor SHT30.
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bcm2835.h>
#include <syslog.h>
#include "perifericos.h"
// ZONA DE CÓDIGOS ********************************************************************

void iniIIC_SHT30()
{
        syslog( LOG_INFO, "Configurando sensor SHT30 ... \n\n" );

        if ( !bcm2835_init() )
        {
                syslog( LOG_INFO, "bcm2835_init error, ¿eres superusuario?\n" );
                exit( 1 );
        }

        if ( !bcm2835_i2c_begin() )
        {
                syslog( LOG_INFO, "bcm2835_init error, ¿eres superusuario?\n" );
                exit( 1 );
        }

        if ( signal( SIGINT, ISRsw ) == SIG_ERR )
        {
                syslog( LOG_ERR, "Error al asignar la señal\n" );
                exit( EXIT_FAILURE );
        }

        bcm2835_i2c_setSlaveAddress( ADDR_SHT30 );
        bcm2835_i2c_set_baudrate( BAUDIOS );

}

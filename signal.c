/*******************************************************************************
* signal.c - ISRsw();
* Realiza la terminación del servidor y del hilo.
********************************************************************************/
#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>

// ZONA DE CÓDIGOS ********************************************************************

void ISRsw( int sig )
{
        int estado;
        pid_t pid;

        if( sig == SIGCHLD )
        {
                syslog(LOG_INFO, "Recibiendo señal del proceso hijo.....\n" );
                pid = wait( &estado );
                syslog(LOG_INFO, "Proceso hijo con pid %d, terminado! \n\n\n", pid );
        }
        else if( sig == SIGINT )
        {
                syslog(LOG_INFO, "Señal SIGINT recibida\n" );
                adq = 0;
                syslog( LOG_INFO, "Concluimos la ejecución de la aplicacion Servidor \n" );
                close( sockfd );

        // This I2C end is done after a transfer if specified
                bcm2835_i2c_end();
                bcm2835_close();

                exit( 0 );
        }

}

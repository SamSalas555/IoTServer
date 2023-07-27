/*******************************************************************************
* signal.c - ISRsw();
* Realiza la terminación del servidor y del hilo.
********************************************************************************/

// ZONA DE DATOS   ********************************************************************
// DECLARACIÓN DE VARIABLES, ESTRUCTURAS Y FUNCIONES

extern int adq, sockfd;

void ISRsw( int sig );

// ZONA DE CÓDIGOS ********************************************************************

void ISRsw( int sig )
{
        int estado;
        pid_t pid;

        if( sig == SIGCHLD )
        {
                syslog( LOG_INFO, "Recibiendo señal del proceso hijo.....\n" );
                pid = wait( &estado );
                syslog( LOG_INFO, "Proceso hijo con pid %d, terminado! \n\n\n", pid );
        }
        else if( sig == SIGINT )
        {
                syslog( LOG_INFO, "Señal SIGINT recibida\n" );
                adq = 0;
                syslog( LOG_INFO, "Concluimos la ejecución de la aplicacion Servidor \n" );
                close( sockfd );

        // This I2C end is done after a transfer if specified
                bcm2835_i2c_end();
                bcm2835_close();

                exit( 0 );
        }

}

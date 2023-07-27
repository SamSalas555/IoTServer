/*******************************************************************************
* hilos.c - thread_sht30();
* Realiza las lecturas del sensor.
********************************************************************************/

// ZONA DE DATOS   ********************************************************************
// DECLARACIÓN DE VARIABLES, ESTRUCTURAS Y FUNCIONES
#include "hilos.h"
#include <syslog.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

extern int cliente_sockfd, pid,adq,sockfd;
// ZONA DE CÓDIGOS ********************************************************************
datos Tx;
void *thread_sht30( void *arg  )
{
        char comando[2], sensor[6];
        unsigned short int temp, hum;
        register int i;

        while( adq == 1 )
        {

                syslog( LOG_INFO, "En espera de peticiones de conexión ...\n\n" );
                if( ( cliente_sockfd = accept( sockfd, NULL, NULL ) ) < 0 )
                {
                        syslog( LOG_ERR, "Ocurrió algún problema al atender a un cliente" );
                        exit( EXIT_FAILURE );
                }
                syslog( LOG_INFO, "Se aceptó un cliente, atendiendolo... \n" );
                pid = fork();

                if( pid == -1 )
                {
                        syslog( LOG_ERR, "Error al crear el proceso hijo \n" );
                        exit( EXIT_FAILURE );
                }
                else if( pid == 0 ) //PROCESO HIJO
                {

 //      Inicia la transferencia de datos entre cliente y servidor

                comando[0] = 0x2C;
                comando[1] = 0x0D;
                bcm2835_i2c_write( comando, 2 );

                //sleep(1);

                bcm2835_i2c_read( sensor, 6 );


//      for( i = 0; i < 6; i++ )
//              printf( "Sensor %d : %d \n", i, sensor[i] );

                temp = sensor[0] << 8 | sensor[1];
                Tx.temp = -45 + 175 * ( temp / 65535.0 );

                syslog( LOG_INFO, "Temp: %d, Temperatura: %0.2f ºC \n", temp, Tx.temp );

                hum = sensor[3] << 8 | sensor[4];
                Tx.hume = 100*(hum / 65535.0);

                syslog( LOG_INFO, "Hum: %d, Humedad: %0.2f%% \n", hum, Tx.hume );


                atiendeCliente( cliente_sockfd );
                }
        }


        pthread_exit ( NULL );

}


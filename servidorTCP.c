/*******************************************************************************
* servidorTCP.c
* gcc servidorTCP -o servidorTCP.c -l bcm2835 -l pthread
********************************************************************************/

// ZONA DE BIBLIOTECAS ****************************************************************

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
#include <bcm2835.h>
#include <syslog.h>
#include "perifericos.h"
#include "hilos.h"
#include "procesos.h"
#include "signal.h"

// ZONA DE DATOS   ********************************************************************

#define PUERTO 		5000		//Número de puerto asignado al servidor
#define COLA_CLIENTES 	5 		//Tamaño de la cola de espera para clientes
#define EVER 		1

int adq;

int sockfd;

pid_t pid;
int cliente_sockfd;

// ZONA DE CÓDIGOS ********************************************************************

int main()
{
	pthread_t tid_sht30;

        struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que alma

	system ( "clear" );

//;---------------------------------------------------------------

 	if( signal( SIGCHLD, ISRsw ) == SIG_ERR )
        {
                perror( "Error en el enlace del manejador" );
                exit( EXIT_FAILURE );
        }
 	if( signal( SIGINT, ISRsw ) == SIG_ERR )
        {
                perror( "Error en el enlace del manejador" );
                exit( EXIT_FAILURE );
        }

	iniDemonio();

// Se abre un archivo log en modo de escritura.
    	openlog( "ServidorTCP-d", LOG_NDELAY | LOG_PID, LOG_LOCAL0 );

/*
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 *  INADDR_ANY - Se elige cualquier interfaz de entrada
 */
   	memset (&direccion_servidor, 0, sizeof (direccion_servidor));	//se limpia la estructura con ceros
   	direccion_servidor.sin_family 		= AF_INET;
   	direccion_servidor.sin_port 		= htons(PUERTO);
   	direccion_servidor.sin_addr.s_addr 	= INADDR_ANY;
/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
   	syslog( LOG_INFO, "\nCreando Socket ....\n");

	//sleep ( 1 );

	if( ( sockfd = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		syslog( LOG_ERR, "Ocurrió un problema en la creación del socket" );
		exit( EXIT_FAILURE );
   	}
/*
 *  bind - Se utiliza para unir un socket con una dirección de red determinada
 */
   	printf( "Configurando socket ...\n" );

	//sleep ( 1 );

   	if( bind( sockfd, ( struct sockaddr * ) &direccion_servidor, sizeof( direccion_servidor ) ) < 0 )
	{
                syslog( LOG_ERR, "Ocurrió un problema al configurar el socket." );
                exit( EXIT_FAILURE );
   	}
/*
 *  listen - Marca al socket indicado por sockfd como un socket pasivo, esto es, como un socket
 *  que será usado para aceptar solicitudes de conexiones de entrada usando "accept"
 *  Habilita una cola asociada la socket para alojar peticiones de conector procedentes
 *  de los procesos clientes
 */
   	syslog( LOG_INFO, "Estableciendo la aceptacion de clientes...\n" );

	//sleep( 1 );

	if( listen( sockfd, COLA_CLIENTES ) < 0 )
	{
                syslog(LOG_ERR, "Ocurrió un problema al crear la cola de aceptar peticiones de los clientes" );
        	exit(EXIT_FAILURE);

   	}

        iniIIC_SHT30();

/*
 *  accept - Aceptación de una conexión
 *  Selecciona un cliente de la cola de conexiones establecidas
 *  se crea un nuevo descriptor de socket para el manejo
 *  de la nueva conexion. Apartir de este punto se debe
 *  utilizar el nuevo descriptor de socket
 *  accept - ES BLOQUEANTE
 */

        adq = 1;
        pthread_create( &tid_sht30, NULL, thread_sht30, NULL);

        pause();

        pthread_join(tid_sht30, NULL);

        //return 0;
}

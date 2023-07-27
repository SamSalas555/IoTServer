// ZONA DE BIBLIOTECAS ****************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ZONA DE DATOS   ********************************************************************

#define  PUERTO     19015
#define  TAM_BUFFER 100
#define  DIR_IP     "3.131.207.170"

// DECLARACIÓN DE FUNCIONES, VARIABLES Y ESTRUCTURAS

typedef struct datos_Rx
{
        float temp;
        float hume;
} datos;

datos Rx;

// ZONA DE CÓDIGOS ********************************************************************

int main()
{
        int    tamano_direccion, sockfd;
        struct sockaddr_in direccion_servidor;

	system( "clear" );

/*
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 */
	memset ( &direccion_servidor, 0, sizeof ( direccion_servidor ) );
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons( PUERTO );
/*
 *	inet_pton - Convierte direcciones de texto IPv4 en forma binaria
 */
	if( inet_pton( AF_INET, DIR_IP, &direccion_servidor.sin_addr ) <= 0 )
	{
		perror( "Ocurrió un error al momento de asignar la dirección IP" );
		exit(1);
	}
/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
	printf( "Creando Socket ....\n" );
	if( ( sockfd = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror( "Ocurrió un problema en la creación del socket" );
		exit( 1 );
	}
/*
 *	Inicia el establecimiento de una conexion mediante una apertura
 *	activa con el servidor
 * 	connect - ES BLOQUEANTE
 */
	printf ( "Estableciendo conexión ...\n" );
	if( connect( sockfd, ( struct sockaddr * )&direccion_servidor, sizeof( direccion_servidor ) ) < 0 ) 
	{
		perror( "Ocurrió un problema al establecer la conexión" );
		exit( 1 );
	}

/*
 *	Inicia la recepción de datos para mostrar temperatura y humedad.
 */

	printf ( "Recibiendo contestación del servidor ...\n\n" );

	if ( read ( sockfd, &Rx, sizeof( Rx ) ) < 0 )
	{
		perror ( "Ocurrió algún problema al recibir datos del cliente" );
		exit( 1 );
	}

	printf ( "El servidor envió el siguiente mensaje: \n" );
	printf ( "Temperatura: %0.2f ºC\n", Rx.temp );
	printf ( "Humedad: %0.2f%%\n", Rx.hume );

	printf ( "\nCerrando la aplicación cliente\n" );
/*
 *	Cierre de la conexion
 */
	close( sockfd );

	exit ( 0 );
}

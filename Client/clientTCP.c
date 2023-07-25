#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./../lib/sht30.h"

//#define PUERTO 14412
#define PUERTO 5000
#define TAM_BUFFER 100
//#define DIR_IP "3.133.207.110"
#define DIR_IP "192.168.100.220"


struct datos
{
        float tempe;
        float hume;
};

struct datos ad;

int main(int argc, char **argv)
{
	system( "clear" );

	float prueba;
	int tamano_direccion, sockfd;
	struct sockaddr_in direccion_servidor;
	char leer_mensaje[TAM_BUFFER];
/*
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 */
	memset (&direccion_servidor, 0, sizeof (direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(PUERTO);
/*
 *	inet_pton - Convierte direcciones de texto IPv4 en forma binaria
 */
	if( inet_pton(AF_INET, DIR_IP, &direccion_servidor.sin_addr) <= 0 )
	{
		perror("Ocurrió un error al momento de asignar la dirección IP");
		exit(1);
	}
/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
	printf("Creando Socket ....\n");
	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrió un problema en la creación del socket");
		exit(1);
	}
/*
 *	Inicia el establecimiento de una conexion mediante una apertura
 *	activa con el servidor
 *  connect - ES BLOQUEANTE
 */
	printf ("Estableciendo conexión ...\n");
	if( connect(sockfd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor) ) < 0) 
	{
		perror ("Ocurrió un problema al establecer la conexión");
		exit(1);
	}
/*
 *	Inicia la transferencia de datos entre cliente y servidor
 */
	printf ("Enviando mensaje al servidor ...\n");
	if( write(sockfd, "Hola, soy Alan", 15) < 0 )
	{
		perror("Ocurrió un problema en el envío de un mensaje al cliente");
		exit(1);
	}

	printf ("Recibiendo contestación del servidor ...\n\n");
	if (read (sockfd, &ad/*leer_mensaje*/, sizeof(ad)/*TAM_BUFFER*/) < 0)
	{
		perror ("Ocurrió algún problema al recibir datos del cliente");
		exit(1);
	}

	printf ( "El servidor envió el siguiente mensaje: \n" );
	printf ( "Temperatura: %0.2f ºC\n", ad.tempe );
	printf ( "Humedad: %0.2f%%\n", ad.hume );

	printf ( "\nCerrando la aplicación cliente\n" );
/*
 *	Cierre de la conexion
 */
	close( sockfd );

	return 0;
}

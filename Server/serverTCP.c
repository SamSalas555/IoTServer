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
#include "./../lib/sht30.h"

#define PUERTO          5000    //Número de puerto asignado al servidor
#define COLA_CLIENTES   5               //Tamaño de la cola de espera para clientes
#define TAM_BUFFER      100
#define EVER            1
#define LONG_MEN        19

#define ADDR_SHT30      0x44
#define BAUDIOS         100000



void *thread_sht30 ( void *arg );

void manejador( int sig );
void atiendeCliente( int cliente_sockfd );

float tempC, humR;
int adq;

int sockfd;
float prueba;

pid_t pid;
int cliente_sockfd;


Data data;

int main(int argc, char **argv)
{
        system ("clear");

        pthread_t tid_sht30;
        int *res_sht30;

        struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que almacena direccion

//;---------------------------------------------------------------

        if( signal( SIGCHLD, manejador ) == SIG_ERR )
        {
                perror("Error en el enlace del manejador");
                exit(EXIT_FAILURE);
        }
        if( signal( SIGINT, manejador ) == SIG_ERR )
        {
                perror("Error en el enlace del manejador");
                exit(EXIT_FAILURE);
        }
/*
 *      AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 *  INADDR_ANY - Se elige cualquier interfaz de entrada
 */
        memset (&direccion_servidor, 0, sizeof (direccion_servidor));   //se limpia la estructura con ceros
        direccion_servidor.sin_family           = AF_INET;
        direccion_servidor.sin_port             = htons(PUERTO);
        direccion_servidor.sin_addr.s_addr      = INADDR_ANY;
/*
 *      Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
        printf("\nCreando Socket ....\n");

        sleep ( 1 );

        if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
        {
                perror("Ocurrio un problema en la creacion del socket");
                exit( 1 );
        }
/*
 *  bind - Se utiliza para unir un socket con una dirección de red determinada
 */
        printf("Configurando socket ...\n");

        sleep ( 1 );

        if( bind( sockfd, (struct sockaddr * ) &direccion_servidor, sizeof( direccion_servidor ) ) < 0 )
        {
                perror ( "Ocurrio un problema al configurar el socket" );
                exit( 1 );
        }
/*
 *  listen - Marca al socket indicado por sockfd como un socket pasivo, esto es, como un socket
 *  que será usado para aceptar solicitudes de conexiones de entrada usando "accept"
 *  Habilita una cola asociada la socket para alojar peticiones de conector procedentes
 *  de los procesos clientes
 */
        printf ( "Estableciendo la aceptacion de clientes...\n" );

        sleep( 1 );

        if( listen(sockfd, COLA_CLIENTES) < 0 )
        {
                perror( "Ocurrio un problema al crear la cola de aceptar peticiones de los clientes" );
                exit( 1 );
        }
        
        sht30_init();

        if ( signal( SIGINT, manejador ) == SIG_ERR )
        {
                perror( "Error al asignar la señal\n" );
                exit( EXIT_FAILURE );
        }


        adq = 1;
        pthread_create( &tid_sht30, NULL, thread_sht30, NULL  );

        pause();
        pthread_join( tid_sht30, (void **)&res_sht30  );

        printf ("Programa terminado :D\n");


    // This I2C end is done after a transfer if specified
        bcm2835_i2c_end();
        bcm2835_close();

        return 0;
/*
 *  accept - Aceptación de una conexión
 *  Selecciona un cliente de la cola de conexiones establecidas
 *  se crea un nuevo descriptor de socket para el manejo
 *  de la nueva conexion. Apartir de este punto se debe
 *  utilizar el nuevo descriptor de socket
 *  accept - ES BLOQUEANTE
 */

/*
        for(;EVER;)
        {
                printf ("En espera de peticiones de conexión ...\n\n");
                if( (cliente_sockfd = accept(sockfd, NULL, NULL)) < 0 )
                {
                        perror("Ocurrio algun problema al atender a un cliente");
                        exit(1);
                }
                printf("Se aceptó un cliente, atendiendolo... \n");
                pid = fork();

                if( pid == -1 )
                {
                        perror("Error al crear el proceso hijo \n");
                        exit(EXIT_FAILURE);
                }else if( pid == 0 ) //PROCESO HIJO
                {

 //     Inicia la transferencia de datos entre cliente y servidor
                        atiendeCliente( cliente_sockfd );
                }
        }
*/
}

void atiendeCliente( int cliente_sockfd )
{
        char leer_mensaje[ TAM_BUFFER ];

        if( read ( cliente_sockfd, leer_mensaje, TAM_BUFFER ) < 0 )
        {
                perror ("Ocurrio algun problema al recibir datos del cliente");
                exit(EXIT_FAILURE);
        }

        printf ("El cliente nos envio el siguiente mensaje: \n %s \n", leer_mensaje);


        data.temperature = tempC;
        data.humidity = humR;


        if( write (cliente_sockfd, &data, sizeof(data)/*LONG_MEN*/) < 0 )
        {
                perror("Ocurrio un problema en el envio de un mensaje al cliente");
                exit(EXIT_FAILURE);
        }
        //kill( getppid(), SIGUSR1 );
        //kill( getppid(), SIGCHLD );
        close (cliente_sockfd);
        exit(0);
}



void *thread_sht30( void *arg  )
{
        char comando[2], sensor[6];
        unsigned short int temp, hum;
        register int i;
        //pid_t pid;
        //int cliente_sockfd;


        while( adq == 1 )
        {
                printf ("En espera de peticiones de conexión ...\n\n");
                if( (cliente_sockfd = accept(sockfd, NULL, NULL)) < 0 )
                {
                        perror("Ocurrio algun problema al atender a un cliente");
                        exit(1);
                }
                printf("Se aceptó un cliente, atendiendolo... \n");
                pid = fork();

                if( pid == -1 )
                {
                        perror("Error al crear el proceso hijo \n");
                        exit(EXIT_FAILURE);
                }else if( pid == 0 ) //PROCESO HIJO
                {

 //      Inicia la transferencia de datos entre cliente y servidor


                comando[0] = 0x2C;
                comando[1] = 0x0D;
                bcm2835_i2c_write( comando, 2 );

                sleep(1);

                bcm2835_i2c_read( sensor, 6 );


//      for( i = 0; i < 6; i++ )
//              printf( "Sensor %d : %d \n", i, sensor[i] );

                temp = sensor[0] << 8 | sensor[1]; 
                tempC = -45 + 175 * ( temp / 65535.0 );

                printf( "Temp: %d, TempC: %f \n", temp, tempC );

                hum = sensor[3] << 8 | sensor[4];
                humR = 100*(hum / 65535.0);

                printf( "Hum: %d, HumR: %f \n", hum, humR );


                atiendeCliente( cliente_sockfd );
                }
        }


        pthread_exit ( NULL );

}


void manejador( int sig )
{
        int estado;
        pid_t pid;

        if( sig == SIGCHLD )
        {
                printf("Recibiendo señal del proceso hijo.....\n");
                pid = wait( &estado );
                printf("Proceso hijo con pid %d, terminado! \n\n\n", pid);
        } else if( sig == SIGINT )
        {
                printf("Señal SIGINT recibida\n");
                adq = 0;
                printf("Concluimos la ejecución de la aplicacion Servidor \n");
                close(sockfd);
                exit(0);
        }
}

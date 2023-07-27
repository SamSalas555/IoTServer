/*******************************************************************************
* procesos.c - atiendeCliente(); iniDemonio();
* Envía las variables de temperatura y humedad.
* Realiza la configuración del servidor como demonio.
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <pthread.h>
#include <stdint.h>
#include "procesos.h"
#include "hilos.h"

// ZONA DE CÓDIGOS ********************************************************************
extern datos Tx;
void atiendeCliente( int cliente_sockfd )
{
        if( write ( cliente_sockfd, &Tx, sizeof( Tx ) ) < 0 )
        {
                syslog( LOG_ERR, "Ocurrió un problema en el envío de un mensaje al cliente" );
                exit( EXIT_FAILURE );

        }

        //kill( getppid(), SIGUSR1 );
        //kill( getppid(), SIGCHLD );
        close ( cliente_sockfd );
        exit( 0 );
}

void iniDemonio()
{
    FILE *apArch;

    pid_t pid = 0;
    pid_t sid = 0;

// Se crea el proceso hijo
    pid = fork();
    if( pid == -1 )
    {
                perror("Error al crear el primer proceso hijo\n");
                exit(EXIT_FAILURE);
    }
/*
 * Se termina Proceso padre.
 * Al finalizar el proceso padre el proceso hijo es adoptado por init. 
 * El resultado es que la shell piensa que el comando terminó con éxito, 
 * permitiendo que el proceso hijo se ejecute de manera independiente en segundo plano.
 */
    if( pid )
    {
                printf("Se termina proceso padre, PID del proceso hijo %d \n", pid);
                exit(0);
    }
/* Se restablece el modo de archivo
 * Todos los procesos tiene una máscara que indica que permisos no deben establecerse al cre>
 * Así cuando se utilizan llamadas al sistema como open() los permisos especificados se comp>
 * desactivando de manera efectiva los que en ella se indiquen.
 * La máscara —denominada umask()— es heredada de padres a hijos por los procesos, por lo qu>
 * será el mismo que el que tenía configurada la shell que lanzó el demonio. Esto significa >
 * que permisos acabarán tenido los archivos que intente crear. Para evitarlo simplemente po>
 * los permisos.
 */
    umask(0);
/*
 * se inicia una nueva sesion
 * Cada proceso es miembro de un grupo y estos a su vez se reúnen en sesiones. En cada una d>
 * que hace las veces de líder, de tal forma que si muere todos los procesos de la sesión re>
 * La idea es que el líder muere cuando se quiere dar la sesión por terminada, por lo que me>
 * notifica al resto de procesos esta circunstancia para que puedan terminar ordenadamente.
 * Obviamente no estamos interesados en que el demonio termine cuando la sesión desde la que>
 * por lo que necesitamos crear nuestra propia sesión de la que dicho demonio será el líder.
 */
    sid = setsid();
    if( sid < 0 )
    {
                perror("Error al iniciar sesion");
                exit(EXIT_FAILURE);
    }
// Se realiza un segundo fork para separarnos completamente de la sesion del padre
    pid = fork( );
    if( pid == -1 )
    {
                perror("Error al crear el segundo proceso hijo\n");
                exit(EXIT_FAILURE);
    }
    if( pid )
    {
                printf("PID del segundo proceso hijo %d \n", pid);
                apArch = fopen("/home/alan/servidorTCP.pid", "w");
                fprintf(apArch, "%d", pid);
                fclose(apArch);

                exit(0);
    }
/*
 * Se cambia el directorio actual por root.
 * Hasta el momento el directorio de trabajo del proceso es el mismo que el de la shell en e>
 * que se ejecutó el comando. Este podría estar dentro de un punto de montaje cualquiera del>
* que no tenemos garantías de que vaya a seguir estando disponible durante la ejecución del>
 * Por eso es probable que prefiramos cambiar el directorio de trabajo al directorio raíz, y>
 * estar seguros de que siempre existirá
 */
    chdir("/");
/*
 * Se cierran los flujos de entrada y salida: stdin, stdout, stderr
 * Puesto que un demonio se ejecuta en segundo plano no debe estar conectado a ninguna termi>
 * Sin embargo esto plantea la cuestión de cómo indicar condiciones de error, advertencias u>
 * tipo de sucesos del programa. Algunos demonios almacenan estos mensajes en archivos espec>
 * o en su propia base de datos de sucesos. Sin embargo en muchos sistemas existe un servici>
 * para registrar estos eventos. En lo sistemas basados en UNIX este servicio lo ofrece el d>
 * al que otros procesos pueden enviar mensajes a través de la función syslog()
 */
    close( STDIN_FILENO  );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );
}

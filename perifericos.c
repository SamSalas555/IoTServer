/*******************************************************************************
* perifericos.c - iniIIC_SHT30();
* Realiza la inicialización de IIC y del sensor SHT30.
********************************************************************************/

// ZONA DE DATOS   ********************************************************************

#define ADDR_SHT30      0x44
#define BAUDIOS         100000

// DECLARACIÓN DE VARIABLES, ESTRUCTURAS Y FUNCIONES

void iniIIC_SHT30();
void ISRsw( int sig );

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

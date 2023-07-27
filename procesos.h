// ZONA DE DATOS   ********************************************************************
// DECLARACIÓN DE VARIABLES, ESTRUCTURAS Y FUNCIONES

void atiendeCliente( int cliente_sockfd );

void iniDemonio();
void umask();

typedef struct datos_Tx
{
      float temp;
      float hume;
} datos;

datos Tx;
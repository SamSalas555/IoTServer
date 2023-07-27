typedef struct datos_Tx
{
      float temp;
      float hume;
} datos;

datos Tx;

void *thread_sht30 ( void *arg );
void atiendeCliente( int cliente_sockfd );

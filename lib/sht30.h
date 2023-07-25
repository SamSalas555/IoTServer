typedef struct {
    float temperature;
    float humidity;
} Data;

void  sht30_init(void);
Data sht30_getdata();
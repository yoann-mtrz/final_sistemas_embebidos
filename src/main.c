#include <stdio.h>
#include "driver/uart.h"

static spi_device_handle_t spi_dev;
static float Vref=3.3;
static uint8_t config_classic = ;


//a)
void spi_bus_init (void){

    spi_bus_initialize(MCP4921, &buscfg,SPI_DMA_DISABLED); 
    spi_bus_add_device(MCP4921, &devcfg, spi_dev); // en el devcfg debemos pojner .mode=0, tambien funciona en mode=3 pero usamos el 0
}

//b)
uint16_t mcp4921_write (int N, uint8_t config){ //uint8_t config es como A/B , BUF, GA, SHDN
    uint8_t buf[2];
    buf[0]=config;
    buf[1]=decimal_to_bcd(N); //supongamos que ya la tenemos por falta de tiempo 

    spi_transaction_t t={
        .length=16;
        .tx_buffer=buf
    };
    spi_device_transmit (display, &t);

}

//c)

float mcp4921_set_threshold(float Vu){
    float Vdac=Vu/(1+22/10); //los valores de R2 y R1
    int N=Vdac*4096/Vref;
    if (N>4097 |N<0){
        printf("error N afuera de rango")
    }
    else(){
        mcp4921_write(N,config_classic);
    }
    
    float Vu_real=N*Vref*(1+22/10)/4096
    return Vu_real

}

//4. a)

void uart_report_threshold (float N, float Vu_real){

char data_buffer [64];
sprintf(data_buffer,"Umbral actualizado: N= %d\n",N , "| Vumbral = %d\n", Vu_real);
uart_write_bytes(UART_NUM_0, data_buffer,strlen(data_buffer));

}

//b)

void mcp4921_shutdown(int N){
    mcp4921_write(N, 0x1); //con un 1 SHDN se pone en output power down
}

//5

void app_main(void){
    uart_config_t uart_config={
        .baudrate=115200,
        .data_bits=UART_DATA_8_BITS,
        .parity=UART_PARITY_DISABLE,
        .stop_bits=UART_STOP_BITS_1,
        .flow_ctrl =UART_HW_FLOWCTRL_DISABLE,
        .source_clk=UART_SCLK_DEFAULT
    };

    uart_param_config(UART_NUM_0, &uart_config);    
    uart_driver_install(UART_NUM_0,4096,4096,0,NULL,0);

    float Vu_deseado;
    uart_read_bytes(UART_NUM_0, &Vu_deseado,1, portMAX_DELAY);

    int N=4096*Vu_deseado/(Vref*(1+22/10)); //porque uart_report_threshold usa N como parametros

    float Vu_real=mcp4921_set_threshold(Vu_deseado);
    
    uart_report_threshold(N,Vu_real);

}
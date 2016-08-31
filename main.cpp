#include "mbed.h"
#include "rtos.h"
#include "NanostackInterface.h"
#include "NanostackRfPhyMcr20a.h"
#include "NanostackRfPhyAtmel.h"

#if MBED_CONF_APP_TRACE
#include "mbed-trace/mbed_trace.h"

void trace_printer(const char* str) {
    printf("%s\r\n", str);
}
#endif // MBED_CONF_APP_TRACE

#if defined(MCR20)
NanostackRfPhyMcr20a phy(MCR20A_SPI_MOSI, MCR20A_SPI_MISO, MCR20A_SPI_SCLK,
                         MCR20A_SPI_CS, MCR20A_SPI_RST, MCR20A_SPI_IRQ);
#else
NanostackRfPhyAtmel phy(ATMEL_SPI_MOSI, ATMEL_SPI_MISO, ATMEL_SPI_SCLK,
                        ATMEL_SPI_CS, ATMEL_SPI_RST, ATMEL_SPI_SLP,
                        ATMEL_SPI_IRQ, ATMEL_I2C_SDA, ATMEL_I2C_SCL);
#endif

LoWPANNDInterface mesh(&phy);
//ThreadInterface mesh;
Serial output(USBTX, USBRX);

int main()
{

#if MBED_CONF_APP_TRACE
	mbed_trace_init();
    mbed_trace_print_function_set(trace_printer);
#endif
    output.baud(115200);
    printf("\r\n\r\nConnecting...\r\n");

    if (mesh.connect()) {
        printf("Connection failed!\r\n");
        return -1;
    }

    printf("connected. IP = %s\r\n", mesh.get_ip_address());
}

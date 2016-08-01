#include "mbed.h"
#include "rtos.h"
#include "NanostackInterface.h"

#if MBED_CONF_APP_TRACE
#include "mbed-trace/mbed_trace.h"

void trace_printer(const char* str) {
    printf("%s\r\n", str);
}
#endif // MBED_CONF_APP_TRACE

LoWPANNDInterface mesh;
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

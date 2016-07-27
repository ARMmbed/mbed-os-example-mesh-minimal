#include "mbed.h"
#include "rtos.h"
#include "NanostackInterface.h"

LoWPANNDInterface mesh;
Serial output(USBTX, USBRX);

int main()
{
    output.baud(115200);
    printf("\r\n\r\nConnecting...\r\n");

    if (mesh.connect()) {
        printf("Connection failed!\r\n");
        return -1;
    }

    printf("connected. IP = %s\r\n", mesh.get_ip_address());
}

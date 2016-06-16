#include "mbed.h"
#include "rtos.h"
#include "NanostackInterface.h"

LoWPANNDInterface mesh;
Serial output(USBTX, USBRX);

int main()
{
    output.baud(115200);
    output.printf("\r\n\r\nConnecting...\r\n");

    if (mesh.connect()) {
        output.printf("Connection failed!\r\n");
        return -1;
    }

    output.printf("connected. IP = %s\r\n", mesh.get_ip_address());
}
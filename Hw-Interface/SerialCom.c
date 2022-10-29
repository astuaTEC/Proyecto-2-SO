
#include <stdio.h>

#include "arduino-serial/arduino-serial-lib.h"
//#include "include/tools.h"

int main() {
    int fd, rc;
    char *serialport = "/dev/ttyACM0";
    int baudrate = 9600;

    fd = serialport_init(serialport, baudrate);

    if (fd < 0){
        //printError("serial port initialization failed");
        return fd;
    }

    printf("successfully opened serialport %s @ %d bps\n", serialport, baudrate);
    serialport_flush(fd);

    struct mg_context *ctx;

    uint8_t message_byte = 1 ;   //Estoy haciendo pruebas a ver si se imprime en el Arduino
    rc = serialport_write(fd, "1230000000456");  //Vamos a ver XD

    //revise whatsapp porfa jajaja


    if(rc < 0){
        //printError("Writing to serial port failed");
        return rc;
    }

    serialport_close(fd);

    return 0;
}
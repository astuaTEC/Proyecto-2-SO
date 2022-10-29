#include <string.h>
#include <errno.h>

#include "include/tools.h"

void printError(char *message){
    printf("%s: %s\n", message, strerror(errno));
}
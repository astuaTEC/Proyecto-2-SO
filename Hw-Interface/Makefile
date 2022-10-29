# Crear ejecutable para programa.c
# Compilador
CC = gcc
# Opciones de compilacion
CFLAGS = -Wall -I/usr/local/include

# Opciones de compilacion
CFLAGS2 = -L/usr/local/lib

# Nombre del ejecutable
TARGET = Arduino

# Archivos fuente de las librerias
SRCS = ./arduino-serial/arduino-serial-lib.c

# Objetos
OBJS = $(SRCS:.c=.o)

# Construir el ejecutable
$(TARGET): link Serial
		$(CC) $(CFLAGS2) arduino-serial-lib.o  SerialCom.o  -o SerialCom.out

# Dependencias
link: $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

Serial: SerialCom.c
	$(CC) -Wall -c SerialCom.c 

# Eliminar archivos generados
limpiar:
	rm -f $(TARGET) *.o
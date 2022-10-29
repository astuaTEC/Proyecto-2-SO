# Crear ejecutable para programa.c
# Compilador
CC = gcc
# Opciones de compilacion
CFLAGS = -Wall -I/usr/local/include

# Opciones de compilacion
CFLAGS2 = -L/usr/local/lib

# Nombre del ejecutable
TARGET = arduino

# Archivos fuente de las librerias
SRCS = ../arduino-serial/arduino-serial-lib.c

# Objetos
OBJS = $(SRCS:.c=.o)

# Construir el ejecutable
$(TARGET): link shipProgram
		$(CC) $(CFLAGS2) arduino-serial-lib.o  program.o -o program.out

# Dependencias
link: $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

shipProgram: program.c
	$(CC) -Wall -c program.c -lpthread

# Eliminar archivos generados
limpiar:
	rm -f $(TARGET) *.o

test: program.c
	gcc program.c -o program.out -lpthread
set(SOURCES arduino-serial-lib.c)

set(LIBRARY_NAME Arduino-serial)

add_library(${LIBRARY_NAME} ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC "./mongoose")
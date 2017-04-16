CC=gcc
HW1_0540112_Ser :server.o ser.o
	$(CC) -o HW1_0540112_Ser server.o ser.o

server.o :server.cpp ser.h ser.cpp
	$(CC) -c server.cpp ser.cpp
clean:
	rm server.o HW1_0540112_Ser ser.o

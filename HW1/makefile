CC=gcc
HW1_0540112_Ser :server.o
	$(CC) -o HW1_0540112_Ser server.o

server.o :server.cpp
	$(CC) -c server.cpp
subsystem:
	cd cli; make
clean:
	rm server.o HW1_0540112_Ser

CCFLAGS = `pkg-config libccgnu2 --cflags`
LDFLAGS = `pkg-config libccgnu2 --libs`

all: jaus_test jaus_receiver send_packet

JAUS_helpers.o: src/JAUS_helpers.h src/JAUS_helpers.cpp
	g++ -c src/JAUS_helpers.cpp -o JAUS_helpers.o $(CCFLAGS)

JAUS_Message.o: src/JAUS_Message.cpp src/JAUS_helpers.h
	g++ -c src/JAUS_Message.cpp -o JAUS_Message.o $(CCFLAGS)

JAUS_test.o: src/JAUS_helpers.h src/JAUS_Message.h src/JAUS_test.cpp
	g++ -c src/JAUS_test.cpp -o main.o $(CCFLAGS)

jaus_test: JAUS_Message.o JAUS_helpers.o JAUS_test.o
	g++ JAUS_Message.o JAUS_helpers.o main.o -o jaus_test $(LDFLAGS)

JAUS_receiver.o: src/JAUS_receiver.cpp
	g++ -c src/JAUS_receiver.cpp -o JAUS_receiver.o

jaus_receiver: JAUS_Message.o JAUS_helpers.o JAUS_receiver.o
	g++ JAUS_Message.o JAUS_helpers.o JAUS_receiver.o -o jaus_receiver $(LDFLAGS)

send_packet: src/send_packet.c
	gcc src/send_packet.c -o send_packet

clean:
	rm -rf *.o jaus_test

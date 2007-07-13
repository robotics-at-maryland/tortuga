all: jaus_test jaus_receiver send_packet jaus_active_demo

JAUS_helpers.o: src/JAUS_helpers.h src/JAUS_helpers.cpp
	g++ -c src/JAUS_helpers.cpp -o JAUS_helpers.o

JAUS_Message.o: src/JAUS_Message.cpp src/JAUS_helpers.h
	g++ -c src/JAUS_Message.cpp -o JAUS_Message.o

JAUS_test.o: src/JAUS_helpers.h src/JAUS_Message.h src/JAUS_test.cpp
	g++ -c src/JAUS_test.cpp -o main.o

jaus_test: JAUS_Message.o JAUS_helpers.o JAUS_test.o
	g++ JAUS_Message.o JAUS_helpers.o main.o -o jaus_test

JAUS_receiver.o: src/JAUS_receiver.cpp
	g++ -c src/JAUS_receiver.cpp -o JAUS_receiver.o

JAUS_active_demo.o: src/JAUS_active_demo.cpp
	g++ -c src/JAUS_active_demo.cpp -o JAUS_active_demo.o

jaus_receiver: JAUS_Message.o JAUS_helpers.o JAUS_receiver.o
	g++ JAUS_Message.o JAUS_helpers.o JAUS_receiver.o -o jaus_receiver

send_packet: src/send_packet.c
	gcc src/send_packet.c -o send_packet

jaus_active_demo: JAUS_active_demo.o
	g++ JAUS_Message.o JAUS_helpers.o JAUS_active_demo.o -o jaus_active_demo

clean:
	rm -rf *.o jaus_test

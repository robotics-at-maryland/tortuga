
all: jaus_test

JAUS_helpers.o: src/JAUS_helpers.h
	g++ -c src/JAUS_helpers.cpp -o JAUS_helpers.o

JAUS_Message.o: src/JAUS_Message.cpp src/JAUS_helpers.h
	g++ -c src/JAUS_Message.cpp -o JAUS_Message.o

main.o: src/JAUS_helpers.h src/JAUS_Message.h src/main.cpp
	g++ -c src/main.cpp -o main.o

jaus_test: JAUS_Message.o JAUS_helpers.o main.o
	g++ JAUS_Message.o JAUS_helpers.o main.o -o jaus_test

clean:
	rm -rf *.o jaus_test

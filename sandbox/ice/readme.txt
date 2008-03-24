Simple ICE demonstration
Leo Singer

Includes an ICE server in C++ and a client in Python

How to build:
First, make sure that you have ICE and scons installed on your system.
Then, inside this directory, type "scons".

How to run:
Fire up a shell in this directory and type "build/testserver".  This will launch
a C++ application that will provide a service.

Then fire up another shell in this directory and type "build/python/testclient".
This will send 1000 pairs of setSpeed/getSpeed commands to the server.  In both
terminals you should see some messages that occur when the methods get called.

If you are in an adventurous mood, type "./toomanyclients.sh 5000".

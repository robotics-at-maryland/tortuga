Simple ICE demonstration
Leo Singer

Includes an ICE server in C++ and a client in Python

How to build:
First, make sure that you have ICE and scons installed on your system.
Then, inside this directory, type "scons".

How to run:
Fire up a shell in this directory and type "build/mockserver".  This will launch
a C++ application that will provide a factory service that creates mock vehicles.

Then fire up another shell in this directory and type "build/python/mockclient.py".
This will provide you with a Python shell from which you can create and manipulate
vehicles served by "build/mockserver".

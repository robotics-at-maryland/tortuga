Based off the Clock demo in the ICE sample code, downloaded from:
http://www.zeroc.com/download/Ice/3.3/Ice-3.3.0-demos.tar.gz




To run the demo, start the IceStorm service:

$ icebox --Ice.Config=config.icebox

This configuration assumes there is a subdirectory named db in the
current working directory.

In a separate window:

$ server

In another window:

$ client

While the server continues to run, "pingReceived" messages should be
displayed in the subscriber window.  You may start as many clients in as many windows as you like, and they should all get the "pingReceived" messages at about the same time.

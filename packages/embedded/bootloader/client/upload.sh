echo U >> /dev/ttyUSB0
sleep 2
./hcloader /dev/ttyUSB0 ../../v1/handyCam.hex

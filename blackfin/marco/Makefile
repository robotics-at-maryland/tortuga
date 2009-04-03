ICETARGET=/opt/IceE-1.3.0-bfin
ICEHOST=/opt/IceE-1.3.0-bfin
SLICE2CPPE=${ICEHOST}/bin/slice2cppe
#CXX=/opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-g++ -ftemplate-depth-128 -Wl,--enable-new-dtags -Wl,-rpath,/opt/IceE-1.3.0-bfin/lib -Wall -D_REENTRANT -I/usr/include/nptl -lpthread -I. -I${ICETARGET}/include -L${ICETARGET}/lib -lIceE -lrt
CXXCOMMON=/opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-g++
CXX=${CXXCOMMON} -ftemplate-depth-128 -I. -I${ICETARGET}/include -ftemplate-depth-128 -Wall -D_REENTRANT -I/usr/include/nptl -g
LD=${CXXCOMMON} -Wl,--enable-new-dtags -Wl,-rpath,/opt/IceE-1.3.0-bfin/lib -ftemplate-depth-128 -Wall -D_REENTRANT -I/usr/include/nptl -g -L${ICETARGET}/lib -lIceE -L/usr/lib/nptl -lpthread -lrt
#CXX=g++ -I. -I${ICETARGET}/include -L${ICETARGET}/lib -lpthread -lIceE -lrt

all : marco

marco : main.o
	${LD} -o $@ $^

marcopolo.cpp marcopolo.h : ../../common/marcopolo.ice
	${SLICE2CPPE} -I. --ice $^

clean :
	rm -f marcopolo.cpp marcopolo.h marco *.o

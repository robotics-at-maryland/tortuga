ICETARGET=/opt/IceE-1.3.0-bfin
ICEHOST=/opt/IceE-1.3.0
SLICE2CPPE=${ICEHOST}/bin/slice2cppe
CXX=/opt/uClinux/bfin-linux-uclibc/bin/bfin-linux-uclibc-g++ -I. -I${ICETARGET}/include -L${ICETARGET}/lib -lIceE -lIceEC -lpthread

all : marco

marco : marcopolo.cpp main.cpp
	${CXX} $^ -o $@

marcopolo.cpp marcopolo.h : ../../common/marcopolo.ice
	${SLICE2CPPE} $^

clean :
	rm -f marcopolo.cpp marcopolo.h marco

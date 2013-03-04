CXX ?= g++
CC ?= gcc
CFLAGS ?= -Wall -Werror -g -O3 -fPIC
LIBS ?=  -lboost_thread-mt

all: 

all: udpproxy udpsend udprecv
udpproxy: udp_priv.o udp_proxy.o config.o main.o
	$(CXX) $(CFLAGS) -o udpproxy udp_priv.o udp_proxy.o config.o main.o $(LIBS)
udpsend: udp_priv.o udp_send.o
	$(CXX) $(CFLAGS) -o udpsend udp_priv.o udp_send.o $(LIBS)
udprecv: udp_priv.o udp_recv.o
	$(CXX) $(CFLAGS) -o udprecv udp_priv.o udp_recv.o $(LIBS)

clean:
	rm -f *~ *.o udpproxy udpsend udprecv

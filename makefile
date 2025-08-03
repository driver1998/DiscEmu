
ifeq (,$(TOOLCHAIN_PREFIX))
$(error TOOLCHAIN_PREFIX is not set)
endif

ifeq (,$(U8G2_PREFIX))
$(error U8G2_PREFIX is not set)
endif

ifeq (,$(CFLAGS))
$(error CFLAGS is not set)
endif

ifeq (,$(LDFLAGS))
$(error LDFLAGS is not set)
endif

CXXFLAGS+=-I${U8G2_PREFIX}/include -std=c++17
CFLAGS+= -I${U8G2_PREFIX}/include
LDFLAGS+= -L${U8G2_PREFIX}/lib -static
LDLIBS+=-l:libu8g2arm.a

CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++

default: disc-emu

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

disc-emu: main.o menu.o input.o usb.o network.o
	$(CXX) $(LDFLAGS)  main.o menu.o input.o usb.o network.o $(LDLIBS) -o disc-emu

clean:
	rm *.o disc-emu
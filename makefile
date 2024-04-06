
ifeq (,$(TOOLCHAIN_PREFIX))
$(error TOOLCHAIN_PREFIX is not set)
endif

ifeq (,$(CFLAGS))
$(error CFLAGS is not set)
endif

ifeq (,$(LDFLAGS))
$(error LDFLAGS is not set)
endif

INCLUDE=-I$(SYSROOT)/include -I$(SYSROOT)/usr/include
LDFLAGS+=-L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib -static
LDLIBS+=-l:libu8g2arm.a

CXXFLAGS=-std=c++17

CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++

default: disc-emu

%.o: %.cpp
	$(CXX) $(INCLUDE) $(CXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -o $@ -c $<

disc-emu: main.o menu.o input.o usb.o network.o
	$(CXX) $(LDFLAGS)  main.o menu.o input.o usb.o network.o $(LDLIBS) -o disc-emu

clean:
	rm *.o disc-emu
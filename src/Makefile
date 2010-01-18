CC = gcc.exe
CPP = g++.exe

BIN = ../bin/freekick.exe
C_SRCS = 
CPP_SRCS = freekick.cpp random.cpp

C_OBJS = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
OBJS = $(C_OBJS) $(CPP_OBJS)

LIBS =  -L"../lib" -lcxcore -lcv -lhighgui -lblobslib -lvideoInput -ldxguid -lstrmbase -lstrmiids -luuid -lole32 -loleaut32
INCS =  -I"../include"

CFLAGS = -O2 -Wall -Wextra -enable-auto-import

all: $(BIN)

$(BIN): $(OBJS)
	@echo "  [LD]    $@"
	@$(CPP) $(CFLAGS) -o $(BIN) $(OBJS) $(LIBS)

%.o: %.c
	@echo "  [CC]    $@"
	@$(CC) $(CFLAGS) $(INCS) -c $<

%.o: %.cpp
	@echo "  [CPP]   $@"
	@$(CPP) $(CFLAGS) $(INCS) -c $<

clean:
	rm -rf $(BIN) $(C_OBJS) $(CPP_OBJS) $(DEPS)


SOURCE  := $(wildcard *.cpp)
INCLUDE := 
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

TARGET  := WebServer
CC      := g++
LIBS    := -lpthread
CFLAGS  := -std=c++11 -g3 -ggdb3 -Wall -O0 $(INCLUDE)
CXXFLAGS:= $(CFLAGS)

all : $(TARGET)

objs : $(OBJS)

rebuild : clean all

clean : 
	rm -rf *.o
	rm -rf $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
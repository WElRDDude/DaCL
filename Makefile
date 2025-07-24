CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 $(shell pkg-config --cflags opencv4)
LDFLAGS = -lpthread -lwiringPi $(shell pkg-config --libs opencv4)

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

all: dacl

dacl: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f src/*.o dacl
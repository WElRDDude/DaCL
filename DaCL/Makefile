# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O2 -I/usr/include/opencv4 -I/usr/include/
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lgpiod -lpthread

# Target executable
TARGET = lab_car_video_system

# Source files
SRCS = main.cpp CircularBuffer.cpp VideoCapture.cpp CANBusListener.cpp ManualTrigger.cpp VideoOverlay.cpp StorageManager.cpp

# Object files (corresponding to source files)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each source file into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean

# Compiler
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I.

# Boost libraries
LIBS = -lboost_system -lpthread

# Source files for robot and led
ROBOT_SRC = robot.cpp robot_pc.cpp
LED_SRC = led.cpp

# Object files
ROBOT_OBJS = $(ROBOT_SRC:.cpp=.o)
LED_OBJS = $(LED_SRC:.cpp=.o)

# Executables
ROBOT_EXEC = robot
LED_EXEC = led

# Default target
all: $(ROBOT_EXEC) $(LED_EXEC)

# Robot executable
$(ROBOT_EXEC): $(ROBOT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Led executable
$(LED_EXEC): $(LED_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Object file compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(ROBOT_OBJS) $(LED_OBJS) $(ROBOT_EXEC) $(LED_EXEC)

.PHONY: all clean


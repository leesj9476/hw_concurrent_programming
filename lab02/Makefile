# Compiler and Compile options.
CC = g++
CXXFLAGS = -g -O2 -Wall -std=c++11

# Macros specifying path for compile.
SRCS := prac_mutex.cpp
OBJS := $(SRCS:.cpp=.o)
INC = ./include/
LIB = ./lib/ -pthread

# Pre-Processor.
CPPFLAGS += -I$(INC)

# Compile command.
TARGET = prac_mutex
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJS) -L$(LIB)

# Delete binary & object files.
clean:
	rm $(TARGET) $(OBJS)

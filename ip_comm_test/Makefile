CXX      = g++
CC       = gcc
CXXFLAGS = -Wall -Wextra -O2 -g -I./lib -pthread
CFLAGS   = -Wall -Wextra -O2 -g -I./lib -pthread

TARGET   = ip_comm_test
LIBDIR   = lib

# Find all .c and .cpp files in lib/
C_SOURCES   = $(wildcard $(LIBDIR)/*.c)
CPP_SOURCES = $(wildcard $(LIBDIR)/*.cpp)

# Main source (ip_comm_test.cpp)
MAIN_SRC = $(TARGET).cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

# All object files
OBJS = $(MAIN_OBJ) \
       $(C_SOURCES:.c=.o) \
       $(CPP_SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
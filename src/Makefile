# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++23 -O3 -Wall -Wextra

BLDFLAGS = -O3

DBGFLAGS = -O0 -D__DBG

# Target executable
TARGET = Sudoku

# Source files
SRCS = State.cpp Sudoku.cpp 

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS) $(BLDFLAGS)

debug: $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS) $(DBGFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
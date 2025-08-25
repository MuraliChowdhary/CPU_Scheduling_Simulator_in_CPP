# Makefile for CPU Scheduling Simulator

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = cpu_scheduler
SOURCE = cpu_scheduler.cpp

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Install dependencies (if needed)
install:
	@echo "No external dependencies required for this C++ program"

.PHONY: all clean run install
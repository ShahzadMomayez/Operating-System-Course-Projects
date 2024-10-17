# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread
TARGET = main
OBJECTS =  main.o #filter.o

# Build the main target
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile filter.cpp to filter.o
#filter.o: filter.cpp filter.hpp
#	$(CXX) $(CXXFLAGS) -c filter.cpp

# Compile bmp.cpp to bmp.o
bmp.o: main.cpp main.hpp 
	$(CXX) $(CXXFLAGS) -c main.cpp

# Clean the build directory
clean:
	rm -f $(TARGET) $(OBJECTS)

# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = bmp
OBJECTS =  bmp.o #filter.o

# Build the main target
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile filter.cpp to filter.o
#filter.o: filter.cpp filter.hpp
#	$(CXX) $(CXXFLAGS) -c filter.cpp

# Compile bmp.cpp to bmp.o
bmp.o: bmp.cpp bmp.hpp 
	$(CXX) $(CXXFLAGS) -c bmp.cpp

# Clean the build directory
clean:
	rm -f $(TARGET) $(OBJECTS)

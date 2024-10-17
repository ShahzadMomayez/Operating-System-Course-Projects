CC = g++
BUILD_DIR = build
INCLUDE_DIR = includes
CFLAGS = -std=c++17 -lstdc++fs

MAIN_EXECUTABLE_FILE = main.out
COUNTRY_EXECUTABLE_FILE = buildings.out
CLUB_EXECUTABLE_FILE = resources.out
REDUCE_EXECUTABLE_FILE = biller.out

MAIN_OBJECTS = \
	$(BUILD_DIR)/main.o

COUNTRY_OBJECTS = \
	$(BUILD_DIR)/Building.o

CLUB_OBJECTS = \
	$(BUILD_DIR)/resource.o

REDUCE_OBJECTS = \
	$(BUILD_DIR)/Biller.o


MainSensitivityList = \
	main.cpp

ReduceSensitivityList = \
	Biller.cpp

CountrySensitivityList = \
	Building.cpp

ClubsSensitivityList = \
	resource.cpp

all: $(BUILD_DIR) $(MAIN_EXECUTABLE_FILE) $(COUNTRY_EXECUTABLE_FILE) $(CLUB_EXECUTABLE_FILE) $(REDUCE_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MAIN_EXECUTABLE_FILE): $(MAIN_OBJECTS)
	$(CC) $(MAIN_OBJECTS) $(CFLAGS) -o $(MAIN_EXECUTABLE_FILE)

$(COUNTRY_EXECUTABLE_FILE): $(COUNTRY_OBJECTS)
	$(CC) $(COUNTRY_OBJECTS) $(CFLAGS) -o $(COUNTRY_EXECUTABLE_FILE)

$(CLUB_EXECUTABLE_FILE): $(CLUB_OBJECTS)
	$(CC) $(CLUB_OBJECTS) $(CFLAGS) -o $(CLUB_EXECUTABLE_FILE)

$(REDUCE_EXECUTABLE_FILE): $(REDUCE_OBJECTS)
	$(CC) $(REDUCE_OBJECTS) $(CFLAGS) -o $(REDUCE_EXECUTABLE_FILE)

$(BUILD_DIR)/main.o: $(MainSensitivityList)
	$(CC) -c main.cpp $(CFLAGS) -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/Biller.o: $(ReduceSensitivityList)
	$(CC) -c Biller.cpp $(CFLAGS) -o $(BUILD_DIR)/Biller.o

$(BUILD_DIR)/Building.o: $(CountrySensitivityList)
	$(CC) -c Building.cpp $(CFLAGS) -o $(BUILD_DIR)/Building.o

$(BUILD_DIR)/resource.o: $(ClubsSensitivityList)
	$(CC) -c resource.cpp $(CFLAGS) -o $(BUILD_DIR)/resource.o


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out \
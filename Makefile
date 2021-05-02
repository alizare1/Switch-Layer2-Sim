CC = g++
BUILD_DIR = build
CFLAGS = -std=c++11 -Wall -Werror

SWITCH_EXECUTABLE_FILE = switch.out
SYSTEM_EXECUTABLE_FILE = system.out
SIM_EXECUTABLE_FILE = sim.out

all: $(SERVER_EXECUTABLE_FILE) \
	$(CLIENT_EXECUTABLE_FILE) \
	$(SIM_EXECUTABLE_FILE)


InterfaceSensitivityList = \
	interface/Interface.cpp \
	interface/Interface.hpp 

SystemSensitivityList = \
	system/System.cpp \
	system/System.hpp 

SwitchSensitivityList = \
	switch/Switch.cpp \
	switch/Switch.hpp


$(SYSTEM_EXECUTABLE_FILE): $(SystemSensitivityList)
	$(CC) $(CFLAGS) system/System.cpp -o $(SYSTEM_EXECUTABLE_FILE)

$(SWITCH_EXECUTABLE_FILE): $(SwitchSensitivityList)
	$(CC) $(CFLAGS) switch/Switch.cpp -o $(SWITCH_EXECUTABLE_FILE)

$(SIM_EXECUTABLE_FILE): $(InterfaceSensitivityList) $(SWITCH_EXECUTABLE_FILE) $(SYSTEM_EXECUTABLE_FILE)
	$(CC) $(CFLAGS) interface/Interface.cpp -o $(SIM_EXECUTABLE_FILE)



.PHONY: clean
clean:
	rm *.out
	rm -r fifos/
CC		:= g++
C_FLAGS := -std=c++17 -w -g -fopenmp -Ofast

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib
SO_FLAG	:= ldl
COMMAND_TRACEGEN := tracegen
COMMAND_MAIN := main
THREADS := 2
CSVOUTPUT := 1
DISPLAY_MAP :=0

LIBRARIES	:= src/ragelGenerator.cpp src/tracePattern.cpp

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
EXECUTABLE_TRACE	:= generateTrace.exe
else
EXECUTABLE	:= main
EXECUTABLE_TRACE	:= generateTrace
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)
	$(RM) $(BIN)/$(EXECUTABLE_TRACE)

tracegen:
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $(SRC)/$(EXECUTABLE).cpp -o $(BIN)/$(EXECUTABLE_TRACE) $(LIBRARIES) -$(SO_FLAG)
	./$(BIN)/$(EXECUTABLE_TRACE) $(COMMAND_TRACEGEN)

main:
	d=$$(date +%s)\
	; $(CC) $(C_FLAGS) -D THREADS=$(THREADS) -D CSVOUTPUT=$(CSVOUTPUT) -D DISPLAY_MAP=$(DISPLAY_MAP) -I$(INCLUDE) -L$(LIB) $(SRC)/$(EXECUTABLE).cpp -$(SO_FLAG) -o $(BIN)/$(EXECUTABLE) $(LIBRARIES) -$(SO_FLAG)	\
	&& echo "Build took $$(($$(date +%s)-d)) seconds"
	./$(BIN)/$(EXECUTABLE) $(COMMAND_MAIN)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -$(SO_FLAG) -o $@ $(LIBRARIES)
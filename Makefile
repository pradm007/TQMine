CC		:= g++
C_FLAGS := -std=c++14 -w -g -lm -fopenmp -Ofast

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
TRACE_EM_PATH := "traceBin/mag_EM"
TRACE_TIME_PATH := "./traceBin/mag_Time"
OUTPUT_FILE_PATH := "./output/mine-map-timed_mag.csv"

TRACE_EM_PATH := "traceBin/serverlog_EM_10000"
TRACE_TIME_PATH := "./traceBin/serverlog_Time_10000"
OUTPUT_FILE_PATH := "./output/mine-map-timed_server.csv"

DO_AGGREGATE := true
# DO_AGGREGATE := false
# OUTPUT_FILE_PATH_AGG := "./output/mine-map-timed_mag_aggregated_server.csv"
OUTPUT_FILE_PATH_AGG := "./output/mine-map-timed_mag_aggregated.csv"

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
	;$(CC) $(C_FLAGS) -D THREADS=$(THREADS) -D CSVOUTPUT=$(CSVOUTPUT) -D DISPLAY_MAP=$(DISPLAY_MAP) -I$(INCLUDE) -L$(LIB) $(SRC)/$(EXECUTABLE).cpp -$(SO_FLAG) -o $(BIN)/$(EXECUTABLE) $(LIBRARIES) -$(SO_FLAG)	\
	&& echo "Build took $$(($$(date +%s)-d)) seconds"
	./$(BIN)/$(EXECUTABLE) $(COMMAND_MAIN)
	
	# d=$$(date +%s)\
	; $(CC) $(C_FLAGS) -D TRACE_EM_PATH=$(TRACE_EM_PATH) -D TRACE_TIME_PATH=$(TRACE_TIME_PATH) -D OUTPUT_FILE_PATH=$(OUTPUT_FILE_PATH) -D DO_AGGREGATE=$(DO_AGGREGATE) -D OUTPUT_FILE_PATH_AGG=$(OUTPUT_FILE_PATH_AGG) -D THREADS=$(THREADS) -D CSVOUTPUT=$(CSVOUTPUT) -D DISPLAY_MAP=$(DISPLAY_MAP) -I$(INCLUDE) -L$(LIB) $(SRC)/$(EXECUTABLE).cpp -$(SO_FLAG) -o $(BIN)/$(EXECUTABLE) $(LIBRARIES) -$(SO_FLAG)	\
	&& echo "Build took $$(($$(date +%s)-d)) seconds"
	# ./$(BIN)/$(EXECUTABLE) $(COMMAND_MAIN)
	# -D TRACE_EM_PATH=$(TRACE_EM_PATH) -D TRACE_TIME_PATH=$(TRACE_TIME_PATH) -D OUTPUT_FILE_PATH=$(OUTPUT_FILE_PATH) -D DO_AGGREGATE=$(DO_AGGREGATE) -D OUTPUT_FILE_PATH_AGG=$(OUTPUT_FILE_PATH_AGG) 

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -$(SO_FLAG) -o $@ $(LIBRARIES)
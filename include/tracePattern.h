#ifndef TRACE_PATTERN
#define TRACE_PATTERN

#include "../include/commonHeader.h"
#include "../include/writeToFile.h"
#include <dlfcn.h>

class TracePattern {
    string shareObject_FileName = "./bin/fsm.so";

    public:
    void loadAndTrace();
    vector<double> loopAndPresentData(string &patternKey, vector<vector<string> > &numberList, string &regexPattern, bool aggregate);
    
};
#endif

#ifndef TRACE_EM_PATH
#define TRACE_EM_PATH "./traceBin/mag_EM"
#endif

#ifndef TRACE_TIME_PATH
#define TRACE_TIME_PATH "./traceBin/mag_Time"
#endif

#ifndef OUTPUT_FILE_PATH
#define OUTPUT_FILE_PATH "./output/mine-map-timed_mag.csv"
#endif

#ifndef OUTPUT_FILE_PATH_AGG
#define OUTPUT_FILE_PATH_AGG "./output/mine-map-timed_mag_aggregated.csv"
#endif

#ifndef DO_AGGREGATE
#define DO_AGGREGATE true
#endif
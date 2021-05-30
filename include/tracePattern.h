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
// #define TRACE_EM_PATH "./traceBin/mag_EM"
// #define TRACE_EM_PATH "./traceBin/serverlogWindowCount_EM"
// #define TRACE_EM_PATH "./traceBin/serverlog_EM_5000"
// #define TRACE_EM_PATH "./traceBin/serverlog_EM_10000"
// #define TRACE_EM_PATH "./traceBin/androidlog_EM_fixedWindow"
#define TRACE_EM_PATH "./traceBin/androidlog_EM_eachAppUsageWindow"
#endif

#ifndef TRACE_TIME_PATH
// #define TRACE_TIME_PATH "./traceBin/mag_Time"
// #define TRACE_TIME_PATH "./traceBin/serverlogWindowCount_Time"
// #define TRACE_TIME_PATH "./traceBin/serverlog_Time_5000"
// #define TRACE_TIME_PATH "./traceBin/serverlog_Time_10000"
// #define TRACE_TIME_PATH "./traceBin/androidlog_Time_fixedWindow"
#define TRACE_TIME_PATH "./traceBin/androidlog_Time_eachAppUsageWindow"
#endif

#ifndef OUTPUT_FILE_PATH
// #define OUTPUT_FILE_PATH "./output/mine-map-timed_mag.csv"
// #define OUTPUT_FILE_PATH "./output/mine-map-timed_server.csv"
// #define OUTPUT_FILE_PATH "./output/mine-map-timed_windowcountserver.csv"
#define OUTPUT_FILE_PATH "./output/mine-map-android_fixed_usage.csv"
#endif

#ifndef OUTPUT_FILE_PATH_AGG
// #define OUTPUT_FILE_PATH_AGG "./output/mine-map-timed_mag_aggregated.csv"
// #define OUTPUT_FILE_PATH_AGG "./output/mine-map-timed_aggregated_windowcountserver.csv"
// #define OUTPUT_FILE_PATH_AGG "./output/mine-map-timed_aggregated_server.csv"
#define OUTPUT_FILE_PATH_AGG "./output/mine-map-timed_aggregated_android_fixed.csv"
#endif

#ifndef DO_AGGREGATE
#define DO_AGGREGATE true
#endif
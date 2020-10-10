#ifndef MINE_TRACE
#define MINE_TRACE

#include "../include/commonHeader.h"
#include "../include/ragelGenerator.h"
#include "../include/tracePattern.h"
#include <stdio.h>

string pattern = "0M1M2";
int alphabetLength = 3;

void _main_mineTrace() {

    // Take the input
    cout << "Enter Regex pattern: " << endl << "Sample Format: " << endl 
         << "<0>[0,0]M[1201,6000]<1>[0,0] : Denotes Event 0 in time bound [0,0] unit followed by" << endl
         << "                               quantifiable value in [1201, 6000] unit followed by  " << endl
         << "                               Event 1 in time bound [0,0]" << endl;
    pattern = "<0>[0,0]M[1201,6000]<1>[0,0]";
    cin >> pattern;

    cout << "Enter event length (recommended max limit 100): ";
    alphabetLength = 10;
    cin >> alphabetLength;

    // Generate ragel file
    RagelGenerator rg;
    rg.generateRagelFile(pattern, alphabetLength);

    // Compile ragel file
    if (Util::fexists("./bin/fsm.rl")) {
        double t = omp_get_wtime();
        
        string exec = "ragel -C -T0 ./bin/fsm.rl -o ./bin/fsm.cpp && g++ -std=c++11 -fopenmp -Ofast -fpic -w -g -shared -D THREADS=" + to_string(THREADS) + " -D DISPLAY_MAP=" + to_string(DISPLAY_MAP) + " -o ./bin/fsm.so -ldl ./bin/fsm.cpp -ldl";

        // cout << "exec " << exec << endl;
        system(exec.c_str());

        printf("State machine created successfully [Elapsed time: %.6f ms]\n", (1000 * (omp_get_wtime() - t)));
    } else {
        perror("Ragel file not found !!");
        return;
    }

    // // Perform mining
    TracePattern tp; 
    tp.loadAndTrace();
}

#endif
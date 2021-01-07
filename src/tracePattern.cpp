#include "../include/tracePattern.h"

/**
 * Function to display the mean and variance of the dataset
 */
vector<double> TracePattern::loopAndPresentData(string &patternKey, vector<vector<string> > &numberList, string &regexPattern, bool aggregate) {
	vector<string> patternKeyMap;
	int currentIndex = 0, bracketStart = 0;
	vector<double> aggregateMetric;

	if (DEBUG) {
		cout << " patternKey " << patternKey << endl;
	}

	// //Remove dot from patternKey i.e from the user's input
	// string newPatternKey = "";
	// for (int i=0;i<patternKey.size();i++) {
	// 	if (patternKey[i] != '.') {
	// 		newPatternKey += patternKey[i];
	// 	}
	// }
	// patternKey = newPatternKey;

	// This breaks the pattern key into sub elements
	while (patternKey[currentIndex] != '\0') {
		if (patternKey[currentIndex] != '[' && bracketStart == 0) {
			if (patternKeyMap.empty()) {
				string element(1, patternKey.at(currentIndex));
				patternKeyMap.push_back(element);
			} else {
				int size = patternKeyMap.size();
				patternKeyMap[size - 1] += (char)patternKey.at(currentIndex);
			}
		} else {
			// everything after the bracket for number mining starts
			if (patternKey[currentIndex] == '[') {
				bracketStart = 1;
				patternKeyMap.push_back("");
			} else if (patternKey[currentIndex] >= 97 &&
						patternKey[currentIndex] <= 122) {
				bracketStart = 0;
				currentIndex--;
			}
		}
		currentIndex++;
	}

	if (DEBUG) {
		cout << "Partition done " << endl;
		for (int i = 0; i < patternKeyMap.size(); i++) {
			cout << patternKeyMap[i] << " ";
		}
		cout << endl;
	}

	try {
	vector<vector<double>> numberListInDouble;
	for (int i = 0; i < numberList.size(); i++) {
		if (!aggregate) {
		printf("\tList %d : \n", i + 1);
		}
		int start = 0;
		string fullMatchedPattern;
		//  = patternKeyMap[start++];

		for (int j = 0; j < numberList[i].size(); j++) {
		// cout << "\tNumber in this list " << numberList[i][j] << endl;
		string numberIs = Util::removeComma(numberList[i][j]);

		if (numberIs.size() > 0) {
			if (!aggregate) {
			cout << "\t\t\t---" << numberIs << endl;
			}
			if (start < patternKeyMap.size()) {
			fullMatchedPattern += patternKeyMap[start++];
			}
			fullMatchedPattern += numberIs + ".";
		}
		}

		if (start < patternKeyMap.size()) {  // flush the remainning, there should
											// be only 1 pattern left at max
		fullMatchedPattern += patternKeyMap[start++];
		}
		// cout << "fullMatchedPattern " << fullMatchedPattern << endl;
		// cout << "regexPattern " << regexPattern << endl;

		if (regex_match(fullMatchedPattern, regex(regexPattern))) {
		if (DEBUG) {
			cout << "Matched with " << regexPattern << endl;
		}
		vector<double> *innerList = new vector<double>;
		for (int j = 0; j < numberList[i].size(); j++) {
			string innerNumber = Util::removeComma(numberList[i][j]);
			if (innerNumber.size() > 0) {
			innerList->push_back(stod(innerNumber));
			}
		}
		numberListInDouble.push_back(*innerList);
		} else {
		if (DEBUG) {
			cout << "Not matched with " << regexPattern << endl;
		}
		}
	}

	if (!aggregate) {
		cout << "Inner pattern match completed. Total matches found : "
			<< numberListInDouble.size() << endl;
	}
	if (numberListInDouble.size() > 0) {
		int innerSize = numberListInDouble[0].size();
		for (int j = 0; j < innerSize; j++) {
		long double MU = 0;
		long double VAR = 0;
		long _min = LONG_MAX;
		long _max = 0;
		for (int i = 0; i < numberListInDouble.size(); i++) {
			MU += (numberListInDouble[i][j] / numberListInDouble.size());
			_min = min(_min, (long)numberListInDouble[i][j]);
			_max = max(_max, (long)numberListInDouble[i][j]);
		}

		for (int i = 0; i < numberListInDouble.size(); i++) {
			VAR += (pow((MU - numberListInDouble[i][j]), 2) /
					numberListInDouble.size());
		}
		VAR = sqrt(VAR);
		if (!aggregate) {
			cout << "For placeholder at " << j + 1 << " : " << endl;
			cout << "\t Mean " << MU << endl;
			cout << "\t Variance " << VAR << endl;
			cout << "\t Min " << _min << endl;
			cout << "\t Max " << _max << endl;
			cout << endl;
		} else {
			aggregateMetric.push_back((roundf(MU * 100) / 100));
			aggregateMetric.push_back((roundf(VAR * 100) / 100));
			aggregateMetric.push_back((double)_min);
			aggregateMetric.push_back((double)_max);
			return aggregateMetric;
		}
		}
	}

	return aggregateMetric;

	} catch (exception &err) {
		cout << " a standard exception was caught, with message '" << err.what()
			<< "'\n";
	}
}

/**
 * Function to load the trace and perform mining
 */
void TracePattern::loadAndTrace() {
  void *lib = dlopen(shareObject_FileName.c_str(), RTLD_NOW);
  if (!lib) {
    printf("dlopen failed: %s\n", dlerror());
    return;
  }
  cout << "Dynamic Shared Object :" << endl;
  cout << "\t\t File Name : " << shareObject_FileName << endl;
  cout << "\t\t Status : Success" << endl;

  //Prepare readstream for traceEM and traceTime
  ifstream myfileTraceEM(TRACE_EM_PATH);
  ifstream myfileTraceTime(TRACE_TIME_PATH);
  // COMMENT_FLAG_0
  
  
  //Read the trace files into string
  string inputEM_string, inputTime_string;
  if (myfileTraceEM.is_open() && myfileTraceTime.is_open()) {
    inputEM_string.assign( (istreambuf_iterator<char>(myfileTraceEM) ), (istreambuf_iterator<char>()) );
    myfileTraceEM.close();
	
	inputTime_string.assign( (istreambuf_iterator<char>(myfileTraceTime) ), (istreambuf_iterator<char>()) );
    myfileTraceTime.close();
  } else {
    cout << "Something went wrong while opening file (either traceEM or traceTime)" << endl;
  }

  if (!inputEM_string.empty() && !inputTime_string.empty()) {
    double t = omp_get_wtime();
    unordered_map<string, vector<vector<string> > >* (*f)(string, string);
    f = (unordered_map<string, vector<vector<string> > >* (*)(string, string))dlsym(lib, "mine_pattern_parallelExecution");
    if (f) {
      printf("Dynamic Linker loaded successfully\n");
      unordered_map<string, vector<vector<string> > >* patternMap = f(inputEM_string, inputTime_string);

      if (CSVOUTPUT && inputEM_string.size() < 10000000) { //10 MB
        if (Util::writeToCSV(OUTPUT_FILE_PATH, *patternMap) != 0) {
          cout << "Output file generated successfully" << endl;
        } else {
          cout << "Something went wrong while generating output file" << endl;
        }
      }

	  /** Do the aggreagation and insert to file **/
	  string csvFileContent  = "Pattern,Count,Mean,Variance,Min,Max\n";
	  for (auto itr = patternMap->begin(); itr != patternMap->end(); itr++) {
		string pattern = (string) itr->first;

		vector<vector<string> > numberList = itr->second;	
		long count = numberList.size();
          
		vector<double> agg = loopAndPresentData(pattern, numberList, pattern, DO_AGGREGATE);
		
		double mean = agg[0];
		double variance = agg[1];
		double _min = agg[2];
		double _max = agg[3];
		
		csvFileContent += pattern + "," + to_string(count) + "," + to_string(mean) + "," + to_string(variance) + "," + to_string(_min) + "," + to_string(_max) + "\n";
	  }
	  
	  if (Util::writeToCSV_aggregate(OUTPUT_FILE_PATH_AGG, csvFileContent) != 0) {
		cout << "Output file (aggregate) generated successfully" << endl;
	  } else {
		cout << "Something went wrong while generating output file" << endl;
	  }
	  
      // COMMENT_FLAG_1
		
    } else {
      printf("Dynamic Linker loaded failed\n");
      printf("dlsym for function grab failed: %s\n", dlerror());
    }
    
	  printf("Full trace completed [Elapsed time: %.6f ms]\n", (1000 * (omp_get_wtime() - t)));
  } else {
    cout << "File might be empty. Size detected (traceEM) -> " << inputEM_string.size() << " and (traceTime) -> " << inputTime_string.size() << endl;
  }

  dlclose(lib);
}


/**
 * DO NOT REMOVE 
 * **/

// COMMENT_FLAG_0
//   ifstream myfileTraceEM("./traceBin/traceEM");
//   ifstream myfileTraceEM("./traceBin/data-01_cleaned_EM");
//   ifstream myfileTraceEM("./traceBin/data-all_cleaned_EM");
//   ifstream myfileTraceEM("./traceBin/mag_EM");
//   ifstream myfileTraceEM("./traceBin/traceEM_ori");
//   ifstream myfileTraceTime("./traceBin/traceTime");
//   ifstream myfileTraceTime("./traceBin/data-01_cleaned_Time");
//   ifstream myfileTraceTime("./traceBin/data-all_cleaned_Time");
//   ifstream myfileTraceTime("./traceBin/mag_Time");
//   ifstream myfileTraceTime("./traceBin/traceTime_ori");

// COMMENT_FLAG_1
/* 
	  // Uncomment for interactive mode
	  // Work on this
       int quit = 0;
       string inputString = "";
	   while(!quit) {
        cout << "Enter pattern that you are interested with specific event symbol (Enter N to quit) : ";
        inputString = "a.M.b";
        cin >> inputString;
        if (Util::willingToQuit(inputString)) {
          cout << "Exiting..." << endl;
          break;
        }
        auto itr = patternMap->find(inputString);
		
		// vector<vector<string> > numberList;
		// cout << "Checking input regex " << inputString << endl;
		
		// for(unordered_map<string,vector<vector<string> >>::iterator iter = patternMap->begin();
		// 	iter != patternMap->end(); ++iter){
			
		// 	string eachKey_s = iter->first;
			
		// 	cout << "Checking with " << eachKey_s << endl;
		// 	if (regex_match(eachKey_s, regex(inputString))) {
		// 		cout << "matched, now inserting all its elements" << endl;
		// 		numberList.insert(numberList.end(), iter->second.begin(), iter->second.end());
		// 	}
		// }
        // const bool is_in = numberList.size() > 0;
		
		const bool is_in = itr != patternMap->end();
        if (is_in) {
          string inputString2 = "a.M.b";
          cout << "Enter interested regex (Enter N to quit) : " ;
          cin >> inputString2;
          if (Util::willingToQuit(inputString2)) {
            cout << "Exiting..." << endl;
            break;
          }

          vector<vector<string> > numberList = itr->second;
          loopAndPresentData(inputString, numberList, inputString2);

          cout << "Enter N to quit, anything else to continue : ";
          cin >> inputString2;
          if (Util::willingToQuit(inputString2)) {
            cout << "Exiting..." << endl;
            break;
          }
        } else {
          cout << "Pattern not found. Try again " << endl;
        }
      } */

#include "../include/ragelGenerator.h"
#define LOCAL_DEBUG 0

void RagelGenerator::prepareAlphabetArr(int alphabetLength) {
    alphabetArr.clear();
    alphabetVisitied.clear();
    overallPattern.clear();


    if (alphabetLength <= MAX_EVENT_REPRESENTATION) {
        for (int i = 0; i < alphabetLength; i++) {
            string repsentationMarker(1,(char)(97 + i));
            alphabetArr.push_back(repsentationMarker);
            alphabetVisitied.push_back(0);
        }
        eventRepresentationLength = 1;
    } else {
        //If alphabet length exceeds alphabetical representation then use numerical suffix as A1,A2,...,B1,B2...

        int bucketSize = alphabetLength/MAX_EVENT_REPRESENTATION;
        int bucketMoreOver = alphabetLength % MAX_EVENT_REPRESENTATION;
        int maxBucketSize = bucketSize + (bucketMoreOver > 0 ? 1 : 0);

        eventIdentityCount = to_string(maxBucketSize-1).size();
        eventRepresentationLength = 1 + to_string(maxBucketSize-1).size(); //Minus 1 is because we are starting from 0 index as A0 -> A9 is 10 elements
        
        for (int j=0;j<MAX_EVENT_REPRESENTATION;j++) {
            for (int i = 0; i < bucketSize; i++) {
                string repsentationMarker(1,(char)(97 + j));
                
                //TODO: For now works only for less than 100 events. Increase this scope
                if (bucketSize > 9 && i < 10){
                    repsentationMarker += '0' + to_string(i); // takes care of A01,A02,A03...
                } else {
                    repsentationMarker += to_string(i);
                }

                alphabetArr.push_back(repsentationMarker);
                alphabetVisitied.push_back(0);
            }

            if (bucketMoreOver > 0) {
                //Insert the bucketMoreOver once horiztally
                string repsentationMarker(1,(char)(97 + j));
                string currentNumber = to_string(bucketSize);
                string prefixer( (maxBucketSize - 1 - currentNumber.size() ) , '0'); //All of this to just make it uniform as A01, A02...A11...A99

                repsentationMarker += prefixer + currentNumber;

                alphabetArr.push_back(repsentationMarker);
                alphabetVisitied.push_back(0);
                bucketMoreOver--;
            }
        }
    }

    if (LOCAL_DEBUG) {
        cout << "Events are : ";
        for (int _i=0; _i < alphabetArr.size(); _i++) {
            cout << alphabetArr[_i] << " ";
        }
        cout << endl;

        cout << "eventRepresentationLength : " << eventRepresentationLength << endl;

    }
}

/**
 * @param dynamicRegexExpression Either in format <0>[]M[]<1>[] or
 * @param alphabetLength Number of event
 */
string RagelGenerator::getRagelExpression(string &dynamicRegexExpression) {
  string overallPattern = "";
  string eventSec = "eventSection+";
  string quantSec = "numberSection+";

  int eventBracket = -1; // 1-< and 0->
                         //   int timeBracket = -1;         // 1-[ and 0-]
  int eventSectionInserted = 0; // 0-false, 1 true
  int quantSectionInserted = 0; // 0-false, 1 true
  //   int timeShouldStart = 0;      // 0-true, 1-false
  int TREcount = 0;
  int commaFaced = 0; // 0-did not face 1-faced

  // Count the TREs and prepare the ragel expression
  for (int i = 0; i < dynamicRegexExpression.size(); i++) {
    // cout << " Evaluating " << dynamicRegexExpression[i] << endl;
    if (dynamicRegexExpression[i] == '<') {
      eventBracket = 1;
    } else if (dynamicRegexExpression[i] == '>') {
      eventBracket = 0;
    } else if (dynamicRegexExpression[i] == 'M') {
      if (quantSectionInserted == 0) {
        overallPattern += quantSec + " ";
        quantSectionInserted = 1;
      }
    } else if ((dynamicRegexExpression[i] - '0') >= 0 &&
               (dynamicRegexExpression[i] - '0') <= 9) {

      if (eventBracket == 1) {
        // This is going through event section now
        if (eventSectionInserted == 0) {
          overallPattern += eventSec + " ";
          eventSectionInserted = 1;
        }
      } else if (eventSectionInserted) {
        // This is for time bound of event section
        int val = dynamicRegexExpression[i] - '0';
        if (commaFaced == 0) {
          // For entry bound

          int _val = eventTimeBound[eventTimeBound.size() - 1][commaFaced];

          if (_val == -1)
            _val = 0; // For first time
          _val = _val * 10 + val;

          eventTimeBound[eventTimeBound.size() - 1][commaFaced] = _val;
        } else {
          // For exit bound
          int _val = eventTimeBound[eventTimeBound.size() - 1][commaFaced];

          if (_val == -1)
            _val = 0; // For first time
          _val = _val * 10 + val;

          eventTimeBound[eventTimeBound.size() - 1][commaFaced] = _val;
        }
      } else if (quantSectionInserted) {
        // This is for time bound of quant section
        int val = dynamicRegexExpression[i] - '0';
        if (commaFaced == 0) {
          // For entry bound

          int _val = quantTimeBound[quantTimeBound.size() - 1][commaFaced];

          if (_val == -1)
            _val = 0; // For first time
          _val = _val * 10 + val;

          quantTimeBound[quantTimeBound.size() - 1][commaFaced] = _val;
        } else {
          // For exit bound
          int _val = quantTimeBound[quantTimeBound.size() - 1][commaFaced];

          if (_val == -1)
            _val = 0; // For first time
          _val = _val * 10 + val;

          quantTimeBound[quantTimeBound.size() - 1][commaFaced] = _val;
        }
      }
    } else if (dynamicRegexExpression[i] == '[') {
      //   cout << "eventSectionInserted " << eventSectionInserted << endl;
      //   cout << "quantSectionInserted " << quantSectionInserted << endl;
      if (quantSectionInserted == 1) {
        vector<int> _quantTimeBound;
        _quantTimeBound.push_back(-1);
        _quantTimeBound.push_back(-1);

        quantTimeBound.push_back(_quantTimeBound);
      } else if (eventSectionInserted == 1) {
        vector<int> _eventTimeBound;
        _eventTimeBound.push_back(-1);
        _eventTimeBound.push_back(-1);

        eventTimeBound.push_back(_eventTimeBound);
      }
    } else if (dynamicRegexExpression[i] == ']') {
      quantSectionInserted = 0;
      eventSectionInserted = 0;
      commaFaced = 0;
    } else if (dynamicRegexExpression[i] == ',') {
      commaFaced = 1;
    }
  }

  TREcount = quantTimeBound.size();

  return "((" + overallPattern + "' |') $to(CHUNK) %to(ACCEPT) $lerr(ERR));";
}

void RagelGenerator::getDynamicEventTimeDefinition() {
  dynamicEventTimeDefinition = "//Input Event Time\n";
  dynamicEventTimeDefinition += "int eventTRE_perInstance = " + to_string(eventTimeBound.size()) + ";\n";
  dynamicEventTimeDefinition += "long inputEventTime[" + to_string(eventTimeBound.size()) + "][2] = {";
  
  for (int i = 0; i < eventTimeBound.size(); i++) {
    dynamicEventTimeDefinition += "{";
    for (int j = 0; j < eventTimeBound[i].size(); j++) {
      dynamicEventTimeDefinition += to_string(eventTimeBound[i][j]);
      if (j == 0)
        dynamicEventTimeDefinition += ",";
    }
    dynamicEventTimeDefinition += "}";
    
    if (i < eventTimeBound.size()-1)
        dynamicEventTimeDefinition += ",";
  }

  dynamicEventTimeDefinition += "};\n";
  dynamicEventTimeDefinition += "long inputQuantTime[" + to_string(eventTimeBound.size() - 1) + "][2] = {";
  
  for (int i = 0; i < quantTimeBound.size(); i++) {
    dynamicEventTimeDefinition += "{";
    for (int j = 0; j < quantTimeBound[i].size(); j++) {
      dynamicEventTimeDefinition += to_string(quantTimeBound[i][j]);
      if (j == 0)
        dynamicEventTimeDefinition += ",";
    }
    dynamicEventTimeDefinition += "}";
    
    if (i < quantTimeBound.size()-1)
        dynamicEventTimeDefinition += ",";
        
  }
  dynamicEventTimeDefinition += "};\n\n";

  dynamicEventTimeDefinition += "long processingEventTime[" + to_string(eventTimeBound.size()) + "][2] = {";
  
  for (int i = 0; i < eventTimeBound.size(); i++) {
    dynamicEventTimeDefinition += "{";
    for (int j = 0; j < eventTimeBound[i].size(); j++) {
      dynamicEventTimeDefinition += "0";
      if (j == 0)
        dynamicEventTimeDefinition += ",";
    }
    dynamicEventTimeDefinition += "}";
    
    if (i < eventTimeBound.size()-1)
        dynamicEventTimeDefinition += ",";
  }
  dynamicEventTimeDefinition += "}; // This stores entry and exit for each TRE event scope\n";
}

void RagelGenerator::printArray(vector<vector<int>> &vec, string label = "array") {
  cout << "Output for " << label << endl;
  for (int i = 0; i < vec.size(); i++) {
    for (int j = 0; j < vec[i].size(); j++) {
      cout << vec[i][j] << " ";
    }
    cout << endl;
  }
}

string RagelGenerator::getFullRagelContent(string &fullRagelExpression) {
    string content = R"(
    #include <bits/stdc++.h>
    #include <omp.h>
    using namespace std;
    #ifndef DEBUG
    #define DEBUG 0
    #endif

    #ifndef MINIMAL
    #define MINIMAL 1
    #endif

    #ifndef MINIMAL_2
    #define MINIMAL_2 1
    #endif

     #ifndef DISPLAY_MAP
     #define DISPLAY_MAP 1
     #endif

    const string NUM_REGEX = "[0-9]+";
    const string FLOAT_REGEX = "[0-9]*\.?[0-9]+";

    int is_faulty=0;
    vector<string> faultReason;
    int THREAD_COUNT = THREADS;
    
    )"+ dynamicEventTimeDefinition + R"(

    vector<string> inputEM_array, inputTime_array;
    int tokenCounter = 0;
    unordered_map<string, vector<vector<string> > > patternMap; //Global shared map between threads
    int g_reserveSize = 1e+7; //Reserve space for new global unordered Map

    int checkForFullAcceptance() {
        //Check time bounds within TREs
        for (int i=0; i<eventTRE_perInstance; i++) {
            for (int j=0; j<2; j++) {
                long time_diff = processingEventTime[i][1] - processingEventTime[i][0];
                if (!(inputEventTime[i][0] == 0 && inputEventTime[i][1] == 0) &&
				    ! (inputEventTime[i][0] <= time_diff && time_diff <= inputEventTime[i][1]) ){
            	    if (DEBUG) {
                        cout << "Time diff did not match for TRE"<<i<<endl;
                    }
                    return 0; // ERROR IN SCOPE
                }
            }
        }
        
        //Check time bounds among TREs
        for (int i=0; i<eventTRE_perInstance-1; i++) {
            for (int j=0; j<2; j++) {
                long time_diff = processingEventTime[i+1][0] - processingEventTime[i][1];
                if (!(inputQuantTime[i][0] == 0 && inputQuantTime[i][1] == 0) &&
				    ! (inputQuantTime[i][0] <= time_diff && time_diff <= inputQuantTime[i][1]) ) {
				    if (DEBUG) {
                        cout << "Time diff did not match among TRE"<<i<< " and TRE"<<(i+1)<<endl;
                    }
                    return 0; // ERROR IN SCOPE
                }
            }
        }
        return 1;
    }

    int getEventTime(int tokenCounter) {
        /* cout << "tokenCounter " << tokenCounter << endl;
        cout << "inputTime_array.size " << inputTime_array.size() << endl;
        cout << "inputTime_array[tokenCounter] " << inputTime_array[tokenCounter] << endl;
        
        for (int i=0;i<inputTime_array.size();i++) {
            cout << "--"<<inputTime_array[i] << endl;
        } */
        
        return stol(inputTime_array[tokenCounter]);
    }

    void insertIntoPatternList(unordered_map<string, vector < vector<string > > > &patternMap, string  &fullPattern, vector<string > *numberList) {

        auto itr = patternMap.find(fullPattern);
        const bool is_in = itr != patternMap.end();
        if (is_in) {
            if (!MINIMAL) {
                cout << "Found " << fullPattern << endl;
            }
            vector<vector<string > > *oldNumberList = &itr->second;
            oldNumberList->push_back(*numberList);
        } else {
            if (!MINIMAL) {
                cout << "Did not find " << fullPattern << " thus inserting new " << endl;
            }
            vector<vector<string> > *newNumberList = new vector<vector<string> >;

            // newNumberList->reserve(10000);
            newNumberList->push_back(*numberList);
            patternMap.emplace(fullPattern, *newNumberList);
        }

    }

    void displayPatternList_Internal(vector<vector<string> > &numberList) {
        for (int i =0; i < numberList.size(); i++) {
                printf("\tList %d : \n\t\t\t", i+1);
                for (int j=0;j<numberList[i].size(); j++) {
                    cout << numberList[i][j];
                }
                printf("\n");
            }
    }

    void displayPatternList(unordered_map<string, vector<vector<string> > > &patternMap) {
        for (auto itr = patternMap.begin(); itr != patternMap.end(); itr++) {
            
            string pattern = "" + (string) itr->first;
            cout << pattern << " :\n";
            vector<vector<string> > numberList = itr->second;	
            displayPatternList_Internal(numberList);
            cout << " " << endl;
        }
    }

    void mergeList(unordered_map<string, vector<vector<string> > > &patternMapInternal) {

        for (auto itr = patternMapInternal.begin(); itr != patternMapInternal.end(); itr++) {
            auto itr_global = patternMap.find((string) itr->first);
            const bool is_in = itr_global != patternMap.end();

            vector<vector<string> > pMapInternalValue = (vector<vector<string> >) itr->second;

            if (is_in) { //Existing pattern
                vector<vector<string> >  *oldNumberList = &itr_global->second;
                
                int oldSize = pMapInternalValue.size();

                for (int i=0; i<pMapInternalValue.size();i++) {
                    oldNumberList->push_back(pMapInternalValue[i]);
                }

            } else { //Insert new pattern

                int reserveSize = g_reserveSize > pMapInternalValue.size() ? g_reserveSize : pMapInternalValue.size();
                vector<vector<string> >  *newNumberList = new vector<vector<string> >;
                newNumberList->reserve(reserveSize);

                // #pragma omp parallel for
                for (int i=0; i<pMapInternalValue.size(); i++) {
                    newNumberList->push_back( pMapInternalValue.at(i) );
                }

                patternMap.emplace((string) itr->first,  *newNumberList);
            }

            // patternMapInternal.clear();
            // releaseMemory(pMapInternalValue);
            // free(pMapInternalValue);
        }
    }

    /**
    * @param input Input string to be splited
    * @param isTimeOrEM Int value represent 1 for EM, 2 for Time , rest - Error
    */
    void stringToStringArray(char *char_input, int isTimeOrEM=0) {
        string input(char_input);
        stringstream ssin(input);
        
        while(ssin.good()) {
            string _token;
            ssin >> _token;
            if (isTimeOrEM == 1) {
                inputEM_array.push_back(_token);
            } else if (isTimeOrEM == 2) {
                inputTime_array.push_back(_token);
            }
        }
    }

    void printTimeEvent() {
        if (DEBUG) {
            cout << "Processing Event Time --------" << endl;
            for (int i=0; i<eventTRE_perInstance; i++) {
                for (int j=0;j<2;j++) {
                    cout << processingEventTime[i][j] << " ";
                }
                cout << endl;
            }
        }
    }

    %% machine foo;
    %% write data;

    void mine_pattern(char *inputEM, char *inputTime) {
        int cs, matched = 0;
        char* p = inputEM;
        
        int totalLength = 0, currentLength = 0;
        vector<string > *numberList = new vector<string>;	
        string tempPatternList;
        tempPatternList.reserve(100);
        unordered_map<string, vector<vector<string> > > patternMapInternal;
        
        cs = foo_start;
        totalLength = strlen(inputEM);
        
        char *eof;
        if (!MINIMAL) {
            printf("Input EM %s \n",inputEM);
            printf("Input Time %s \n",inputTime);
        }
        
        stringToStringArray(inputEM, 1);
        stringToStringArray(inputTime, 2);
        
        if (DEBUG) {
            printf("cs is %d and foo_start is %d\n", cs, foo_start);
        }
        
        long previousEventTime=0;
        long _eventTime = 0;
        int _quantValue = 0;
        // string _quantValue = "";
        int currentTRECount = 0;
        int _has_event_entered = 0; // Determines if we enter an event stage in TRE. Gets reset immediately at M stage
        int _event_OR_quant_stage = 0; //Flips between 0 - unknown, 1 - Event, 2 - Quant
        int _event_OR_quant = 0;// Flips between 1 - Event, 2 - Quant
        

        %%{
            
            action CHUNK {
                //Invoked for each event
                // cout << "Element -> " << (char) fc << endl;
                currentLength++;
            }

            action EVENT {
                //Invoked for every event-character match
                if (DEBUG) {
                    printf("\tEvent =%c \n",fc);
                }
                // tokenCounter++; //Increase everytime there is an E/M
                _event_OR_quant_stage = 1;
                
                //Get Event Time
                _eventTime = getEventTime(tokenCounter);
                
                //Calculate and insertEventTime for this event
                if (_has_event_entered == 0) {
                    processingEventTime[currentTRECount][0] = _eventTime;
                    _has_event_entered++;
                }
                previousEventTime = _eventTime;
                
                _eventTime = 0;
                
                //Insert into pattern list
                tempPatternList += (char) fc;
            }
            
            action EVENT_EXIT {
                _event_OR_quant_stage = 1;
                
                //Get Event Time
                _eventTime = getEventTime(tokenCounter);
                
                //Can only be understood if it enters a quantitative state BUT problem with last TRE instance as there would not be any quantitative state following
                //Calculate and insertEventTime for this event
                if (_eventTime == 0) { // 
                    processingEventTime[currentTRECount][1] = previousEventTime;
                } else {
                    //This state should never actually occur.
                    processingEventTime[currentTRECount][1] = _eventTime;
                    previousEventTime = _eventTime;
                }
                
                _eventTime = 0;
                
            }
            
            action EVENT_IDENTITY {
                //Will only be invoked in case of events such as a01 or b02 where 01,02 would be the event identity
                if (DEBUG) {
                    printf("\tEvent Identity =%c \n",fc);
                }
                _event_OR_quant_stage = 1;
                
                
                //Insert into pattern list
                tempPatternList += (char) fc;
            }
            
            
            action DELIMITERS {
                //Invoked for every delimiter-character match
                if (DEBUG) {
                    printf("\tDelimiter-Character =%c \n",fc);
                }
                tokenCounter++; //Increase everytime there is an E/M.. now is decided based on space
                
                if (_event_OR_quant_stage == 2) {
                    //This would mean, the previous encounter was that of a Quant
                    numberList->push_back("");
                    numberList->push_back(to_string(_quantValue));
                    // numberList->push_back(_quantValue);
                    _quantValue = 0;
                    // _quantValue = "";
                }
                
                if (_event_OR_quant_stage == 1) {
                    //Insert into pattern list
                    tempPatternList += '.';
                }
                
                if (_event_OR_quant_stage != 0) {
                    _event_OR_quant = _event_OR_quant_stage;
                    _event_OR_quant_stage = 0;
                }
            }
            
            action NUM {
                if (fc >= 48 && fc <= 57) {
                    if (DEBUG) {
                        printf("\tNum =%c \n",fc);
                    }
                    // tokenCounter++; //Increase everytime there is an E/M
                    _event_OR_quant_stage = 2;
                    
                    _quantValue = _quantValue*10 + (fc - '0');
                    // _quantValue += fc;
                    // _quantValue++;
                    
                    if (_has_event_entered != 0) {
                        currentTRECount++; //Increment TRE instance counter only if _has_event_entered was true. This takes care of multiple quant fields
                        _has_event_entered = 0; //Reset
                    }
                    _eventTime = 0;
                    
                    if (_event_OR_quant != 2) {
                        //Insert into pattern list
                        tempPatternList += "[0-9]+.";
                        _event_OR_quant = 2; //Do only once
                    }
                } else {
                    is_faulty |= 1;
                    faultReason.push_back("INVLD_NMBR");
                }
            }
            
            action ACCEPT {
                //Accepting state
                if (!MINIMAL) {
                    printf("Partially accepted\n");
                }
                matched++;
                
                //Should have hit exit state [NOTE: NOT A GOOD PLACE TO KEEP EVENT EXIT]
                //Calculate and insertEventTime for this event
                if (DEBUG) {
                    cout << "_eventTime " << _eventTime << " previousEventTime " << previousEventTime << endl;
                }
                if (_eventTime == 0) { // 
                    processingEventTime[currentTRECount][1] = previousEventTime;
                } else {
                    //This state should never actually occur.
                    processingEventTime[currentTRECount][1] = _eventTime;
                    previousEventTime = _eventTime;
                }
                
                printTimeEvent();
                
                //Reset all counters
                previousEventTime=0;
                _eventTime = 0;
                currentTRECount = 0;
                _has_event_entered = 0; // Determines if we enter an event stage in TRE. Gets reset immediately at M stage
                
                int isFullyAccepted = checkForFullAcceptance();
                if (isFullyAccepted) {
                    if (!MINIMAL) {
                        printf("Finally accepted\n");
                    }
                    //Prune the previous M.
                    tempPatternList = tempPatternList.substr(0, tempPatternList.size()-1);
                    
                    // Insert into the pattern List
                    insertIntoPatternList(patternMapInternal, tempPatternList, numberList);
                }

                //Reset more counters
                tempPatternList.clear();
                numberList = new vector<string>;
                
                cs = foo_start; // Move to start state
            }
            
            action ERR {
                if (DEBUG) {
                    printf("Error happened.\n");
                    printf("\tcurrent processing character =%c \n",fc);
                }
                
                //Reset all counters
                previousEventTime=0;
                _eventTime = 0;
                currentTRECount = 0;
                _has_event_entered = 0; // Determines if we enter an event stage in TRE. Gets reset immediately at M stage
                
                cs = foo_start; // Move to start state
                if (DEBUG) {
                    printf("cs is %d and foo_start is %d\n", cs, foo_start);
                    printf("Resetting state (and probably would check with first state)\n");
                    // printf("Resetting state (and move forward)\n");
                }
                // p++;

                //Reset more counters
                tempPatternList.clear();
                numberList = new vector<string>;
                
                if (currentLength >= totalLength) {
                    // Force break... very bad practice
                    if (DEBUG) {
                        cout << "Trying to break "<< endl;
                    }
                    fbreak;
                }
            }
        
            FLOAT_REGEX = /[0-9]*\.?[0-9]+/;
            NUM = [0-9]+;
            EVENTREP = [a-z]+;
            delimiters = space <to(DELIMITERS);
            subeventSection = ((delimiters)((lower) <to(EVENT) )([0-9]{0} <to(EVENT_IDENTITY)));
            
            eventSection = (subeventSection %from(EVENT_EXIT));
            numberSection = ((delimiters)(NUM <to(NUM)));
            
            
            main := )" + fullRagelExpression + R"(

            write init nocs;
            write exec noend;
        }%%

        if (!MINIMAL_2)	{
            cout << "Finished processing \n\n";
        }
        
        if (DEBUG) {
            cout << "Displaying internal pattern map per thread" << endl;
            #pragma omp critical
            displayPatternList(patternMapInternal);
        }
        
        if (!MINIMAL_2) {
            printf("For Thread %d \t", omp_get_thread_num());
            printf("Pattern matched %d times\n", matched);
        }
        
        #pragma omp critical
        {
            if (DEBUG) {
                cout << "Merging internal and external list" << endl;
            }
            mergeList(patternMapInternal);
            if (DEBUG) {
                cout << "Merge finished for Thread " << omp_get_thread_num() << endl;
            }
        }

    }

    int isDigit(string digitChar_str) {
        
        char digitChar = digitChar_str[0];
        int val = digitChar - '0';
        return val >= 0 && val <= 9;
    }

    int isSpace(string spaceChar_str) {
        char spaceChar = spaceChar_str[0];
        return spaceChar == 32;
    }

    int isCharacter(string alphabetChar_str) {
        
        char alphabetChar = alphabetChar_str[0];
        return alphabetChar >= 'a' && alphabetChar <= 'z';
    }

    vector<string> inputEM_Stream_per_thread, inputTime_Stream_per_thread;
    void chunkDivider(string inputEM, string inputTime) {
        int currentThreadIndex = 0;
        int leftPointer = 0, rightPointer = 0;
        inputEM_Stream_per_thread.push_back("");
        inputTime_Stream_per_thread.push_back("");
        
        int is_event_quant_stage = 0; // Flag : 0-space, 1-Event, 2-Quant
        
        int quantCount_perInstance = eventTRE_perInstance; // Taking 1 extra M just to be safe. This would could TRE M TRE as with 2 M so that we cut at TRE M TRE M
        
        int currentQuantCount_forInstance = 0;
        
        vector<string> _private_inputEM_array, _private_inputTime_array;
        stringstream ssinEM(inputEM);
        
        while(ssinEM.good()) {
            string _token;
            ssinEM >> _token;
            _private_inputEM_array.push_back(_token);
        }
        
        stringstream ssinTime(inputTime);
        
        while(ssinTime.good()) {
            string _token;
            ssinTime >> _token;
            _private_inputTime_array.push_back(_token);
        }
        
        //cout << "_private_inputEM_array.size() " << _private_inputEM_array.size() << endl;
        //cout << "_private_inputTime_array.size() " << _private_inputTime_array.size() << endl;
        
        while(rightPointer < _private_inputEM_array.size()) {
            
            // cout << _private_inputEM_array[rightPointer] << " rightPointer " << rightPointer << endl;
            
            if (isCharacter(_private_inputEM_array[rightPointer])) {
                is_event_quant_stage = 1;
            }
            
            if (rightPointer == (_private_inputEM_array.size()-2)) {
                //Flush the current state
                //We need to substring this chunk and add it to thread pool
                string inputEM_chunk = " " ,inputTime_chunk = " ";
                
                int _lfP = leftPointer;
                while(_lfP <= rightPointer) {
                    inputEM_chunk += _private_inputEM_array[_lfP] + " ";
                    inputTime_chunk += _private_inputTime_array[_lfP] + " ";
                    _lfP++;
                }
                inputEM_chunk += "| "; //To append final delimiter
                inputTime_chunk += "0 "; //To append final delimiter
                
                inputEM_Stream_per_thread.push_back(inputEM_chunk);
                inputTime_Stream_per_thread.push_back(inputTime_chunk);
                
                cout << "Must break now " << endl;
                break;
            }
            
            if (isCharacter(_private_inputEM_array[rightPointer]) && isDigit(_private_inputEM_array[rightPointer + 1])) {
                if (is_event_quant_stage != 2) {
                    currentQuantCount_forInstance++;
                    is_event_quant_stage = 2;
                }
            }
            
            //Chunk this for a thread
            if (currentQuantCount_forInstance == quantCount_perInstance) {
                //We need to substring this chunk and add it to thread pool
                string inputEM_chunk = " " ,inputTime_chunk = " ";
                
                int _lfP = leftPointer;
                while(_lfP <= rightPointer) {
                    inputEM_chunk += _private_inputEM_array[_lfP] + " ";
                    inputTime_chunk += _private_inputTime_array[_lfP] + " ";
                    _lfP++;
                }
                inputEM_chunk += "| "; //To append final delimiter
                inputTime_chunk += "0 "; //To append final delimiter
                
                inputEM_Stream_per_thread.push_back(inputEM_chunk);
                inputTime_Stream_per_thread.push_back(inputTime_chunk);
                
                //Move the left pointer right-size to the next quant value
                // int _lfP = leftPointer;

                while(leftPointer <= rightPointer) {
                    if (isDigit(_private_inputEM_array[leftPointer]) && isCharacter(_private_inputEM_array[leftPointer + 1])) {
                        leftPointer = leftPointer + 1; // At space
                        currentQuantCount_forInstance--; // Decrement a quantCount
                        break;
                    }
                    leftPointer++;
                }
            }
            
            rightPointer++;
        }
    }

    void showChunks() {
        for (int i=0;i<inputEM_Stream_per_thread.size();i++) {
            cout << "Chunk " << (i+1) << " ~~~ " << endl;
            cout << "\t\t>>" << inputEM_Stream_per_thread[i] << "<<" << endl;
            cout << "\t\t>>" << inputTime_Stream_per_thread[i] << "<<" << endl;
        }
    }

        /**
         * Function for parallel Execution
         **/
        extern "C" {unordered_map<string, vector<vector<string> > >* mine_pattern_parallelExecution(string inputEM_string, string inputTime_string);}
        unordered_map<string, vector<vector<string> > >* mine_pattern_parallelExecution(string inputEM_string, string inputTime_string) {
            
            double t;

            if (DEBUG) {
                cout << "Initiating chunk division" << endl;
            }
            t = omp_get_wtime();
            chunkDivider(inputEM_string, inputTime_string);

            if (DEBUG) {
                printf("Finished chunk division in %.6f ms. \n", (1000 * (omp_get_wtime() - t)));
            }

            t = omp_get_wtime();
            if (DEBUG) {
                showChunks();
            }

            #pragma omp parallel for num_threads(1) shared(patternMap, inputEM_Stream_per_thread, inputTime_Stream_per_thread) firstprivate(g_reserveSize, inputEM_array, inputTime_array, tokenCounter)
            for (int i=0;i<inputEM_Stream_per_thread.size();i++) {
                
                string _inputEM_string = inputEM_Stream_per_thread[i];
                string _inputTime_string = inputTime_Stream_per_thread[i];
                
                char *_inputEM_char = (char *)malloc(_inputEM_string.size()+1 * sizeof(char *));
                char *_inputTime_char = (char *)malloc(_inputTime_string.size()+1 * sizeof(char *));
                
                strcpy(_inputEM_char, _inputEM_string.c_str());
                strcpy(_inputTime_char, _inputTime_string.c_str());
                
                //cout << _inputEM_char << endl;
                //cout << _inputTime_char << endl;
                //Send char* to minePattern per thread
                mine_pattern(_inputEM_char, _inputTime_char);
            }

            cout << "Size of pattern Map " << patternMap.size() << endl;
            if (DISPLAY_MAP) {
                displayPatternList(patternMap);
            }

            /* calculate and print processing time*/
            t = 1000 * (omp_get_wtime() - t);
            printf("Finished in %.6f ms using %d threads. \n", t, THREAD_COUNT);

            return &patternMap;
        }

    )";

    return content;
}

void RagelGenerator::generateRagelFile(string &dynamicRegexExpression, int alphabetLength) {

    if (LOCAL_DEBUG) {
        prepareAlphabetArr(alphabetLength);
        string fullRagelExpression = getRagelExpression(dynamicRegexExpression);
        getDynamicEventTimeDefinition();
        
        cout << "Dynamic Regex Expression " << dynamicRegexExpression << endl;
        cout << "Expression is " << fullRagelExpression << endl;
        printArray(eventTimeBound, "eventTimeBound");
        printArray(quantTimeBound, "quantTimeBound");
        return;
    }

    double t = omp_get_wtime();
    
    string fullRagelExpression = getRagelExpression(dynamicRegexExpression);
    getDynamicEventTimeDefinition();
    string ragelContent = getFullRagelContent(fullRagelExpression);

    if (Util::writeToFileFunc(fileName, ragelContent) != 0) {
    cout << "Ragel FSM file generated successfully" << endl;
    } else {
    cout << "Something went wrong" << endl;
    }

    printf("[Elapsed time:  %.6f ms]\n", (1000 * (omp_get_wtime() - t)));
}
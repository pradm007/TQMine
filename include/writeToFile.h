#ifndef WRITE_TO_FILE
#define WRITE_TO_FILE

#include "../include/commonHeader.h"
#include <stdio.h>

class Util {
  public:

  static bool fexists(const string filename) {
    ifstream ifile(filename.c_str());
    return (bool) ifile;
  }

  static int deleteFile(const string fileName) {
    cout << "Searching for file : " << fileName << endl;
    if (!fexists(fileName.c_str())){
      puts("\tNo old file found. ");
      return true;
    } else if (remove(fileName.c_str()) != 0) {
      perror("\tError deleting file");
      return false;
    } else {
      puts("Old file successfully deleted");
      return true;
    }
  }

  static int writeToFileFunc(const string fileName, const string fileContent) {

    ofstream myfile;
    myfile.flush();

    if (!deleteFile(fileName))
      return false;
    

    myfile.open(fileName);
    myfile << fileContent << std::flush;
    myfile.close();

    return true;
  }

  static int writeToCSV(const string fileName, const unordered_map<string, vector<vector<string> > > &patternMap) {
    ofstream csvFile;
    csvFile.open(fileName);
    csvFile << "Pattern,Count,Numbers\n";
    for (auto itr = patternMap.begin(); itr != patternMap.end(); itr++) {
      string pattern = "" + (string) itr->first;


      vector<vector<string> > numberList = itr->second;	
      csvFile<<pattern<<","<<numberList.size()<<endl;
      for (int i =0; i < numberList.size(); i++) {
        string tracedNumber=",,";
        for (int j=0;j<numberList[i].size(); j++) {
          if (numberList[i][j].size() > 0) {
            tracedNumber += numberList[i][j] + ",";
          }
        }
        if (!tracedNumber.empty()) {
          tracedNumber += "\n";
          csvFile << tracedNumber;
        }
      }
    }
  }
  
  static int writeToCSV_aggregate(const string fileName, const string content) {
    ofstream csvFile;
    csvFile.open(fileName);
    csvFile << content;
    
    return 1;
  }

  static int willingToQuit(string &input) {
	  return input.length() == 1 && ((char) input[0] == 'N' || (char) input[0] == 'n');
  }

  static string removeComma(string &input) {
    string output;
    for(int i=0;i<input.size();i++) {
      if (input[i] != ',') {
        output += input[i];
      }
    }

    return output;
  }

};

#endif
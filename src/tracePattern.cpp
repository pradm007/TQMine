#include "../include/tracePattern.h"

void TracePattern::loadAndTrace() {
  void *lib = dlopen(shareObject_FileName.c_str(), RTLD_NOW);
  if (!lib) {
    printf("dlopen failed: %s\n", dlerror());
    return;
  }
  cout << "Dynamic Shared Object :" << endl;
  cout << "\t\t File Name : " << shareObject_FileName << endl;
  cout << "\t\t Status : Success" << endl;


  char *input = "";
  ifstream myfile("./traceBin/trace");
  string inp;
  if (myfile.is_open()) {
    while (getline(myfile, inp)) {
    }
    myfile.close();
  }
  input = (char *)malloc((inp.size() + 1) * sizeof(char *));
  strcpy(input, inp.c_str());

  if (input != NULL && !string(input).empty()) {
    clock_t start = clock();
    void (*f)(char *);
    f = (void (*)(char *))dlsym(lib, "mine_pattern");
    if (f) {
      printf("Dynamic Linker loaded successfully\n");
      f(input);
    } else {
      printf("Dynamic Linker loaded failed\n");
      printf("dlsym for function grab failed: %s\n", dlerror());
    }
    clock_t end = clock();
    clock_t tot_Time = end - start;
    printf("Trace completed in %lf sec\n", ((float)tot_Time) / CLOCKS_PER_SEC);
  }

  dlclose(lib);
}
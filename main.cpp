#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include<cmath>
#include<bitset>

using namespace std;

/*int resetFile(ifstream infile){
  //Resetting file to the beginning before the next predictor
  infile.clear();
  infile.seekg(0);
  return 0;
}*/

int main(int argc, char *argv[]) {

  // Temporary variables
  unsigned long long addr;
  string behavior;

  // Open file for reading
  ifstream infile(argv[1]);

  // Always taken predictor
  int numBranches = 0, correctPred = 0;
  while(infile >> std::hex >> addr >> behavior) {
    if(behavior == "T") {
      correctPred++;
    }
    numBranches++;
  }
  cout << correctPred << "," << numBranches << ";" << endl;
  infile.clear();
  infile.seekg(0);

  // Always not taken predictor
  numBranches = 0; 
  correctPred = 0;
  while(infile >> std::hex >> addr >> behavior) {
    if(behavior == "NT") {
      correctPred++;
    }
    numBranches++;
  }
  cout << correctPred << "," << numBranches << ";" << endl;
  infile.clear();
  infile.seekg(0);

  // Bimodal Predictor with a single bit of history
  numBranches = 0; 
  correctPred = 0;
  // array that has the different sizes the table needs to be
  int difTableSizes[7] = {4, 8, 32, 64, 256, 1024, 4096};
  // Running a simulation for each table size
  int i = 0;
  while(i <= 6){
    int tableSize = difTableSizes[i];
    // creating the predictor table and initializing all the values to non-taken (1)
    int predictorTable[tableSize];
    fill(predictorTable, predictorTable + tableSize, 1);
    int logTableSize = log2(tableSize);
    unsigned int bitmask = (1 << logTableSize) - 1; //the log base 2 of the table size is used to determine how many bits the index will be
    while(infile >> std::hex >> addr >> behavior) {
      int index = addr & bitmask;
      if(predictorTable[index] == 1){
        if(behavior == "NT"){ //correct prediction, since counter is 1 bit no change is needed
          correctPred++;
        }else{ //incorrect prediction, counter needs to be changed
          predictorTable[index]--;
        }
      }else if(predictorTable[index] == 0){
        if(behavior == "T"){
          correctPred++;
        }else{
          predictorTable[index]++;
        }
      }
      numBranches++;
    }
    cout << correctPred << "," << numBranches << "; ";
    infile.clear();
    infile.seekg(0);
    numBranches = 0; 
    correctPred = 0;
    i++;
  }
  cout << endl;

  // Bimodal Predictor with a two bit counter


  i = 0;
  while(i <= 6){
    int tableSize = difTableSizes[i];
    // creating the predictor table and initializing all the values to non-taken (3)
    int predictorTable[tableSize];
    fill(predictorTable, predictorTable + tableSize, 3);
    int logTableSize = log2(tableSize);
    unsigned int bitmask = (1 << logTableSize) - 1;
    while(infile >> std::hex >> addr >> behavior) {
      numBranches++;
      int index = addr & bitmask;
      if(predictorTable[index] >= 2){
        if(behavior == "NT"){
          correctPred++;
          if(predictorTable[index] != 3){ //New if statement needed, since the value may need to be incremented even if the prediction is correct
            predictorTable[index]++;
          }
        }else{
          predictorTable[index]--;
        }
      }else if(predictorTable[index] <= 1){
        if(behavior == "T"){
          correctPred++;
          if(predictorTable[index] != 0){
            predictorTable[index]--;
          }
        }else{
          predictorTable[index]++;
        }
      }
    }
    cout << correctPred << "," << numBranches << "; ";
    numBranches = 0; 
    correctPred = 0;
    infile.clear();
    infile.seekg(0);
    i++;
  }
  cout << endl;

  // Bimodal Predictor with a three bit counter

  i = 0;
  while(i <= 6){
    int tableSize = difTableSizes[i];
    // creating the predictor table and initializing all the values to very strongly-taken (0)
    int predictorTable[tableSize];
    fill(predictorTable, predictorTable + tableSize, 0);
    int logTableSize = log2(tableSize);
    unsigned int bitmask = (1 << logTableSize) - 1;
    while(infile >> std::hex >> addr >> behavior) {
      numBranches++;
      int index = addr & bitmask;
      if(predictorTable[index] >= 3){
        if(behavior == "NT"){
          correctPred++;	
          if(predictorTable[index] != 5){
            predictorTable[index]++;
          }
        }else{
          predictorTable[index]--;
        }
      }else if(predictorTable[index] == 1){ //this else if statement is a special case for strongly taken, since the counter needs to be 
        if(behavior == "T"){                //incremented twice, according to the document attached to the project document
          correctPred++;
          predictorTable[index]--;
  	}else{
        predictorTable[index] = predictorTable[index] + 2;
        }
      }else if(predictorTable[index] <= 2){
        if(behavior == "T"){
          correctPred++;
          if(predictorTable[index] != 0){
            predictorTable[index]--;
          }
        }else{
          predictorTable[index]++;
        }
      }
    }
    cout << correctPred << "," << numBranches << "; ";
    numBranches = 0; 
    correctPred = 0;
    infile.clear();
    infile.seekg(0);
    i++;
  }
  cout << endl;

  // Gshare predictor
  int numBits = 2/*should be2*/; //represents the number of bits of the global history register
  while(numBits <= 12/*should be12*/){
    int ghr = 1; //GHR is initiallized to 1
    unsigned int bitmask = (1 << numBits) - 1;
    int predictorTable[4096];
    fill(predictorTable, predictorTable + 4096, 3); //predictor table is initiallized to strongly not taken (3)
    while(infile >> std::hex >> addr >> behavior) {
      numBranches++;
      int index = (addr & 4095) ^ ghr;
      if(predictorTable[index] >= 2){
        if(behavior == "NT"){
          correctPred++;
          if(predictorTable[index] != 3){
            predictorTable[index]++;
          }
        }else{
          predictorTable[index]--;
        }
      }else if(predictorTable[index] <= 1){
        if(behavior == "T"){
          correctPred++;
          if(predictorTable[index] != 0){
            predictorTable[index]--;
          }
        }else{
          predictorTable[index]++;
        }
      }

      //update ghr with a left shift 1 bit, with the incoming bit being a 0 if not taken and 1 if taken
      ghr = ghr << 1;
      if(behavior == "T"){
        ghr++;
      }
      ghr = ghr & bitmask;  //making sure the most significant bit is dropped
    }
    cout << correctPred << "," << numBranches << "; ";
    numBranches = 0; 
    correctPred = 0;
    infile.clear();
    infile.seekg(0);
    numBits++;
  }
  cout << endl;

  //Tournament Predictor
  int ghr = 0;
  // Creating and initializing entries in the three necessary tables
  int gsharePredTable[4096];
  int bimodPredTable[4096];
  int selectorTable[4096];
  fill(gsharePredTable, gsharePredTable + 4096, 3);
  fill(bimodPredTable, bimodPredTable + 4096, 5);
  fill(selectorTable, selectorTable + 4096, 3);
  unsigned int bitmask = (1 << 12) - 1;
  cout << "bitmask: " << bitmask << endl;

  while(infile >> std::hex >> addr >> behavior) {
    numBranches++;
    bool gshareCorrect = false;
    bool bimodCorrect = false;
    int gshareIndex, bimodIndex;

    //TOURNAMENT GSHARE

    gshareIndex = (addr & bitmask) ^ ghr;
    if(gsharePredTable[gshareIndex] >= 2){
      if(behavior == "NT"){
        gshareCorrect = true;
        if(gsharePredTable[gshareIndex] != 3){
          gsharePredTable[gshareIndex]++;
        }
      }else{
        gsharePredTable[gshareIndex]--;
      }
    }else if(gsharePredTable[gshareIndex] <= 1){
      if(behavior == "T"){
        gshareCorrect = true;
        if(gsharePredTable[gshareIndex] != 0){
          gsharePredTable[gshareIndex]--;
        }
      }else{
        gsharePredTable[gshareIndex]++;
      }
    }
    ghr = ghr << 1;
    if(behavior == "T"){
      ghr++;
    }
    ghr = ghr & bitmask;

    //TOURNAMENT BIMODAL
    bimodIndex = addr & bitmask;
    if(bimodPredTable[bimodIndex] >= 3){
      if(behavior == "NT"){
        bimodCorrect = true;
        if(bimodPredTable[bimodIndex] != 5){
          bimodPredTable[bimodIndex]++;
        }
      }else{
        bimodPredTable[bimodIndex]--;
      }
    }else if(bimodPredTable[bimodIndex] == 1){  
      if(behavior == "T"){              
        bimodCorrect = true;
        if(bimodPredTable[bimodIndex] != 0){
          bimodPredTable[bimodIndex]--;
      }else{
        bimodPredTable[bimodIndex] = bimodPredTable[bimodIndex] + 2;
      }
      }
    }else if(bimodPredTable[bimodIndex] <= 2){
      if(behavior == "T"){
        bimodCorrect = true;
        if(bimodPredTable[bimodIndex] != 0){
          bimodPredTable[bimodIndex]--;
        }
      }else{
        bimodPredTable[bimodIndex]++;
      }
    }

    // Selector table
    if(selectorTable[bimodIndex] >= 2 && bimodCorrect){ //favoring bimodal
      correctPred++;
      if(!gshareCorrect && selectorTable[bimodIndex] != 3){
        selectorTable[bimodIndex]++;
      }
    }else if(selectorTable[bimodIndex] <= 1 && gshareCorrect){
      correctPred++;
      if(!bimodCorrect && selectorTable[bimodIndex] != 0){
        selectorTable[bimodIndex]--;
      }
    }
  }
  cout << correctPred << "," << numBranches << "; " << endl;
  infile.clear();
  infile.seekg(0);

  return 0;
}

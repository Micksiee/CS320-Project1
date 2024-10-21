#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include<cmath>

using namespace std;


int main(int argc, char *argv[]) {

  // Temporary variables
  unsigned long long addr;
  string behavior;

  // Open file for reading

  ifstream infile(argv[argument]);

  // Always taken predictor
  int numBranches = 0, correctPred = 0;
  while(infile >> std::hex >> addr >> behavior) {
    if(behavior == "T") {
      correctPred++;
    }
    numBranches++;
  }
  cout << correctPred << "," << numBranches << ";" << endl;
  resetFile(infile);

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
  resetFile(infile);

  // Bimodal Predictor with a single bit of history
  numBranches = 0; 
  correctPred = 0;
  // array that has the different sizes the table needs to be
  int difTableSizes[7] = {4, 8, 32, 64, 256, 1024, 4096};
  int tableSize = difTableSizes[0];
  // Running a simulation for each table size
  int i = 0;
  while(i <= 6){
    int tableSize = difTableSizes[i];
    // creating the predictor table and initializing all the values to non-taken (1)
    int predictorTable[tableSize];
    fill(predictorTable, end(predictorTable), 1);
    unsigned int bitmask = (1 << log2(tableSize)) - 1; //the log base 2 of the table size is used to determine how many bits the index will be
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
    resetFile(infile);
    numBranches = 0; 
    correctPred = 0;
    i++;
  }
  cout << endl;

  // Bimodal Predictor with a two bit counter
  int tableSize = difTableSizes[0]; //initializing table size again

  int i = 0;
  while(i <= 6){
    int tableSize = difTableSizes[i];
    // creating the predictor table and initializing all the values to non-taken (3)
    int predictorTable[tableSize];
    fill(predictorTable, end(predictorTable), 3);
    unsigned int bitmask = (1 << log2(tableSize)) - 1;
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
    resetFile(infile);
    i++;
  }
  cout << endl;

  // Bimodal Predictor with a three bit counter
  int tableSize = difTableSizes[0]; //initializing table size again

  int i = 0;
  while(i <= 6){
    int tableSize = difTableSizes[i];
    // creating the predictor table and initializing all the values to very strongly-taken (0)
    int predictorTable[tableSize];
    fill(predictorTable, end(predictorTable), 0);
    unsigned int bitmask = (1 << log2(tableSize)) - 1;
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
      }else if(predictorTable[index] == 2){ //this else if statement is a special case for strongly taken, since the counter needs to be 
        if(behavior == "T"){                //incremented twice, according to the document attached to the project document
          correctPred++;
          if(predictorTable[index] != 0){
            predictorTable[index]--;
        }else{
          predictorTable[index] = predictorTable[index] + 2;
        }
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
    resetFile(infile);
    i++;
  }
  cout << endl;

  // Gshare predictor
  int numBits = 2; //represents the number of bits of the global history register
  while(numBits <= 12){
    int ghr = 1; //GHR is initiallized to 1
    unsigned int bitmask = (1 << numBits) - 1;
    int predictorTable[4096];
    fill(predictorTable, end(predictorTable), 3); //predictor table is initiallized to strongly not taken (3)
    while(infile >> std::hex >> addr >> behavior) {
      numBranches++;
      int index = (addr & bitmask) ^ ghr;
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
    resetFile(infile);
    numBits++;
  }
  cout << endl;

  //Tournament Predictor
  int ghr = 0;
  // Creating and initializing entries in the three necessary tables
  int gsharePredTable[4096];
  int bimodPredTable[4096];
  int selectorTable[4096];
  fill(gsharePredTable, end(gsharePredTable), 3);
  fill(bimodPredTable, end(bimodPredTable), 5);
  fill(selectorTable, end(selectorTable), 3);
  unsigned int bitmask = (1 << 12) - 1;

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
    }else if(bimodPredTable[bimodIndex] == 2){  
      if(behavior == "T"){              
        bimodCorrect = true;
        if(bimodPredTable[bimodIndex] != 0){
          bimodPredTable[bimodIndex]--;
      }else{
        bimodPredTable[bimodIndex] = bimodPredTable[bimodIndex] + 2;
      }
      }
    }else if(bimodPredTable[bimodIndex] <= 1){
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
  cout << correctPred << "," << numBranches << "; " endl;
  resetFile(infile);

  return 0;
}

int resetFile(ifstream infile){
  //Resetting file to the beginning before the next predictor
  infile.clear();
  infile.seekg(0);
  return 0;
}

// Helper function for neatening up the code for filling the array,
// found in an answer at https://stackoverflow.com/questions/2890598/how-to-initialize-all-elements-in-an-array-to-the-same-number-in-c
template <typename T, size_t N>
T* end(T (&pX)[N])
{
    return pX + N;
}
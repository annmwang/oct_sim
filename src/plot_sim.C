// c and c++ dependencies
#include <iostream>
#include <numeric>

// root dependencies
#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>

#include "../include/SimNtupleData.hh"

int plot_sim(const std::string inFileName, // Input file
			       const std::string outFileName = "outFileName.root", // Output file
			       const std::string treeName = "gingko", // Tree name for simulation data
             const std::string treeArgsName = "sim_args" // Tree name for simulation args
			     )
{
  // ROOT Global settings
  TH1::SetDefaultSumw2();

  TFile *inFile = new TFile(inFileName.c_str(), "READ");
  TTree *t = (TTree*)inFile->Get(treeName.c_str());
  TTree *t_args = (TTree*)inFile->Get(treeArgsName.c_str());
  SimNtupleData SN;
  SN.SetAddressReadData(t);
  SN.SetAddressReadArgs(t_args);

  static const unsigned int nevent = 10; //(int)t->GetEntries();

  for( unsigned int i = 0; i < nevent; i++){
    // Print the event number
    if( i % 100 == 0 ) std::cout<<"Event: "<<i<<"/"<<nevent<<std::endl;

    // Get the next entry
    t->GetEntry(i);
    if( i == 0){
      t_args->GetEntry(i);
      //std::cout<< SN.bkgrate << std::endl;
    }
    std::cout<< SN.real_x_muon << std::endl;
  }

  // Create the output file and write histograms
  TFile *outFile = new TFile(outFileName.c_str(), "recreate");
  outFile->Close();

  // Cleanup
  delete outFile;
  delete t;
  delete inFile;

  return 1;
}


int main(int argc, char*argv[]){

  int retVal = 0;
  if(argc == 2) { 
    std::cout<<__LINE__<<std::endl;
    retVal += plot_sim(argv[1]); 
    std::cout<<__LINE__<<std::endl;
  }
  else if(argc == 3) { retVal += plot_sim(argv[1], argv[2]); }

  std::cout<<__LINE__<<std::endl;
  return retVal;
}
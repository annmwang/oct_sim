// c and c++ dependencies
#include <iostream>
#include <numeric>

// Root dependencies
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>



int sim_hist_plotter(const std::string inFileName, // input file 
		     const std::string outFileName, // output file
		     const std::string treeName = "gingko" // tree name
		     ){
  
  // Root global settings
  TH1::SetDefaultSumw2();

  // Print settings
  std::cout<<"Input file: "<<inFileName<<std::endl;
  std::cout<<"Output file: "<<outFileName<<std::endl;


  // Setup the variables for the branches
  int EventNum;
  int NEvent;
  int Ntriggers;
  double real_x_muon;
  double real_y_muon;
  std::vector<int> * iRoad_x = 0;
  std::vector<int> * iRoad_u = 0;
  std::vector<int> * iRoad_v = 0;
  std::vector< std::vector<int> > * Hit_strips = 0;
  std::vector< std::vector<int> > * Hit_planes = 0;
  std::vector< std::vector<int> > * Hit_ages = 0;
  std::vector<int> * trigger_BC = 0;
  std::vector<int> * N_muon = 0;
  std::vector<int> * N_xmuon = 0;
  std::vector<double> * trig_x = 0;
  std::vector<double> * trig_y = 0;
  std::vector<double> * dtheta = 0;

  // Load the input file
  TFile *f = TFile::Open(inFileName.c_str(), "READ");
  TTree *t = (TTree*)f->Get(treeName.c_str());
  
  // Set addresses
  t->SetBranchAddress("EventNum", &EventNum);
  t->SetBranchAddress("NEvent", &NEvent);
  t->SetBranchAddress("Ntriggers", &Ntriggers);
  t->SetBranchAddress("real_x_muon", &real_x_muon);
  t->SetBranchAddress("real_y_muon", &real_y_muon);
  t->SetBranchAddress("iRoad_x", &iRoad_x);
  t->SetBranchAddress("iRoad_u", &iRoad_u);
  t->SetBranchAddress("iRoad_v", &iRoad_v);
  t->SetBranchAddress("Hit_strips", &Hit_strips);
  t->SetBranchAddress("Hit_planes", &Hit_planes);
  t->SetBranchAddress("Hit_ages", &Hit_ages);
  t->SetBranchAddress("trigger_BC", &trigger_BC);
  t->SetBranchAddress("N_muon", &N_muon);
  t->SetBranchAddress("N_xmuon", &N_xmuon);
  t->SetBranchAddress("trig_x", &trig_x);
  t->SetBranchAddress("trig_y", &trig_y);
  t->SetBranchAddress("dtheta", &dtheta);

  // Setup histograms

  static const int nbins_coords = 10000;
  static const float xlow_coords = -1000;
  static const float xhigh_coords = 1000;

  TH1D *h_trig_x_minus_real_x = new TH1D("h_trig_x_minus_real_x","h_trig_x_minus_real_x",nbins_coords, xlow_coords, xhigh_coords);
  TH1D *h_trig_y_minus_real_y = new TH1D("h_trig_y_minus_real_y","h_trig_y_minus_real_y",nbins_coords, xlow_coords, xhigh_coords);

  static const unsigned int nevent = (int) t->GetEntries();
  for( unsigned int i = 0; i < nevent; i++){
    if( i % 100 == 0) std::cout << "Event: "<<i<<"/"<<nevent<<std::endl;

    // Get the next entry
    t->GetEntry(i);

    // Fill the difference between triggered x/y and real x/y 
    for( unsigned int trig = 0; trig < trig_x->size(); trig++ ){
      h_trig_x_minus_real_x->Fill(trig_x->at(trig) - real_x_muon);
      h_trig_y_minus_real_y->Fill(trig_y->at(trig) - real_y_muon);
    }

  }

  // Create the output file 
  TFile *outFile = new TFile(outFileName.c_str(), "recreate");

  // Write the histograms
  h_trig_x_minus_real_x->Write();
  h_trig_y_minus_real_y->Write();

  outFile->Close();
  
  // Cleanup
  delete outFile;
  
  delete h_trig_x_minus_real_x;
  delete h_trig_y_minus_real_y;
  
  delete t;
  delete f;
  
  return 0;
}

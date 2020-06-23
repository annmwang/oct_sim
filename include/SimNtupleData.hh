/************** 

SIMULATION DATA TREES

Made by: Anthony Badea (June 2020)

**************/

// cpp dependencies
#include <vector>
#include <string>
#include <map>

// ROOT dependencies
#include <TTree.h>
#include <TVector3.h>
#include <TH1D.h>

// Personally compiled dictionaries
// #include "VectorDict.cxx"


// DataFormat
class SimNtupleData{

public:

    static const int nMaxPart = 10000;

    // Variables to write out to the tree
    int EventNum = 0;
    int Ntriggers = 0; // number of triggers on the roads

    double real_x_muon = 0; // x-location of a real muon
    double real_y_muon = 0; // y-location of a real muon

    std::vector<int> * iRoad_x = 0; // ??
    std::vector<int> * iRoad_u = 0; // ??
    std::vector<int> * iRoad_v = 0; // ??

    std::vector<std::vector<int>> * Hit_strips = 0; // ??
    std::vector<std::vector<int>> * Hit_planes = 0; // ??
    std::vector<std::vector<int>> * Hit_ages = 0; // ??
    std::vector<int> * trigger_BC = 0; // ??
    std::vector<int> * N_muon = 0; // ??
    std::vector<int> * N_xmuon = 0; // ??

    std::vector<double> * trig_x = 0; // x-locations of all triggers
    std::vector<double> * trig_y = 0; // y-locations of all triggers

    std::vector<double> * dtheta = 0; // ??

    // simulation parameters
    int NEvent = 0; // number of events to be generated
    int bkgrate = -999.; // Hz per strip
    int m_xroad = -999.; // size of x road in strips
    int m_NSTRIPS = -999.; // number of x strips
    int m_bcwind = -999.; // fixed time window (in bunch crossings) during which the algorithm collects ART hits
    int m_sig_art = -999.; // art time resolution (in nanoseconds)
    int killran = -999.; // bool if you want to kill one plane randomly
    int killxran = -999.; // bool if you want to kill one X plane randomly 
    int killuvran = -999.; // bool if you want to kill one U or V plane randomly 
    int m_sig_art_x = -999.; // ART position resolution (in strips). used to smear ART position
    std::vector<double> * mm_eff = 0; // efficiency of each PCB Left/Right of the MM
    int m_xthr = -999.; // required total number of hits on all x channels combined required for a trigger. Used in create_roads function which uses Road.hh in include folder
    int m_uvthr = -999.; // required total number of hits on all u and v channels combined required for a trigger. Used in create_roads function which uses Road.hh in include folder
    bool bkgflag; // decides if background should be generated
    bool pltflag; // decides if event displays should be plotted
    bool uvrflag; // decides if ??? used in set_chamber
    bool trapflag; // decides if ??? used in create_roads
    bool ideal_tp  ; // decides if ??? used in Road.hh
    bool ideal_vmm ; // decides if ??? used in finder function
    bool ideal_addc; // decides if ??? used in finder function
    bool write_tree; // decides if an output TTree is produced
    bool bkgonly; // decides if only the background should be produced
    bool smear_art; // decides if the arrival time of the ART hits due to muon tracks is smeared with a gaussian with a σ of 32 ns to emulate the ART time distribution
    bool funcsmear_art; // ONLY used if smear_art is false. Uses a custom smearing function rather than a gaussian. 
    int chamber = -999.; // Chamber value
    bool legacy; // Enable legacy mode
    int seed; // Random number generator seed

    // Identifier so that there is not histograms do not overwrite other versions in simultaneous SimNtuple declarations 
    int ID = 0; 

    // Histograms for simulation data
    TH1D *h_EventNum;
    TH1D *h_NEvent;
    TH1D *h_Ntriggers;
    TH1D *h_real_x_muon;
    TH1D *h_real_y_muon;
    TH1D *h_iRoad_x;
    TH1D *h_iRoad_u;
    TH1D *h_iRoad_v;
    TH1D *h_Hit_strips;
    TH1D *h_Hit_planes;
    TH1D *h_Hit_ages;
    TH1D *h_trigger_BC;
    TH1D *h_N_muon;
    TH1D *h_N_xmuon;
    TH1D *h_trig_x; 
    TH1D *h_trig_y; 
    TH1D *h_dtheta; 

    // Histograms for simulation parameters 
    TH1D *h_bkgrate;
    TH1D *h_m_xroad; 
    TH1D *h_m_NSTRIPS;
    TH1D *h_m_bcwind;
    TH1D *h_m_sig_art;
    TH1D *h_killran;
    TH1D *h_killxran;
    TH1D *h_killuvran;
    TH1D *h_m_sig_art_x;
    TH1D *h_mm_eff;
    TH1D *h_m_xthr;
    TH1D *h_m_uvthr;
    TH1D *h_bkgflag;
    TH1D *h_pltflag;
    TH1D *h_uvrflag;
    TH1D *h_trapflag;
    TH1D *h_ideal_tp;
    TH1D *h_ideal_vmm;
    TH1D *h_ideal_addc;
    TH1D *h_write_tree;
    TH1D *h_bkgonly;
    TH1D *h_smear_art;
    TH1D *h_funcsmear_art;
    TH1D *h_chamber;
    TH1D *h_legacy;
    TH1D *h_seed;

    // Map for all of the histograms
    std::map<std::string, TH1D*> h_map = {};

    SimNtupleData();
    SimNtupleData(int id);

    // Initialize functions
    void SetBranchData(TTree *t);
    void SetBranchArgs(TTree *t);
    void SetAddressReadData(TTree *t);
    void SetAddressReadArgs(TTree *t);

    void InitializeHistsMap_ALL();
    void InitializeHistsMap_DATA(); // for comparing to legacy data files

    void doFillArr(TH1D *hist, std::vector<int> *val);
    void doFillArr(TH1D *hist, std::vector<double> *val);
    void doFillArr(TH1D *hist, std::vector< std::vector<int> > *val);

    void InitializeHistsData(std::vector< std::string > hists);
    void FillHistsData(std::vector< std::string > hists);
    void InitializeHistsData_ALL();
    void FillHistsData_ALL();

    void InitializeHistsArgs(std::vector< std::string > hists);
    void FillHistsArgs(std::vector< std::string > hists);
    void InitializeHistsArgs_ALL();
    void FillHistsArgs_ALL();

    void DeleteAllHists();

private:
     
};

SimNtupleData::SimNtupleData(){}

SimNtupleData::SimNtupleData(int id){
  ID = id;
}

void SimNtupleData::InitializeHistsMap_ALL(){
  h_map["EventNum"] = h_EventNum;
  h_map["NEvent"] = h_NEvent;
  h_map["Ntriggers"] = h_Ntriggers;
  h_map["real_x_muon"] = h_real_x_muon;
  h_map["real_y_muon"] = h_real_y_muon;
  h_map["iRoad_x"] = h_iRoad_x;
  h_map["iRoad_u"] = h_iRoad_u;
  h_map["iRoad_v"] = h_iRoad_v;
  h_map["Hit_strips"] = h_Hit_strips;
  h_map["Hit_planes"] = h_Hit_planes;
  h_map["Hit_ages"] = h_Hit_ages;
  h_map["trigger_BC"] = h_trigger_BC;
  h_map["N_muon"] = h_N_muon;
  h_map["N_xmuon"] = h_N_xmuon;
  h_map["trig_x"] = h_trig_x;
  h_map["trig_y"] = h_trig_y;
  h_map["dtheta"] = h_dtheta;
  h_map["bkgrate"] = h_bkgrate;
  h_map["m_xroad"] = h_m_xroad;
  h_map["m_NSTRIPS"] = h_m_NSTRIPS;
  h_map["m_bcwind"] = h_m_bcwind;
  h_map["m_sig_art"] = h_m_sig_art;
  h_map["killran"] = h_killran;
  h_map["killxran"] = h_killxran;
  h_map["m_sig_art_x"] = h_m_sig_art_x;
  h_map["mm_eff"] = h_mm_eff;
  h_map["m_xthr"] = h_m_xthr;
  h_map["m_uvthr"] = h_m_uvthr;
  h_map["bkgflag"] = h_bkgflag;
  h_map["pltflag"] = h_pltflag;
  h_map["uvrflag"] = h_uvrflag;
  h_map["trapflag"] = h_trapflag;
  h_map["ideal_tp"] = h_ideal_tp;
  h_map["ideal_vmm"] = h_ideal_vmm;
  h_map["ideal_addc"] = h_ideal_addc;
  h_map["write_tree"] = h_write_tree;
  h_map["bkgonly"] = h_bkgonly;
  h_map["smear_art"] = h_smear_art;
  h_map["funcsmear_art"] = h_funcsmear_art;
  h_map["chamber"] = h_chamber;
  h_map["legacy"] = h_legacy;
  h_map["seed"] = h_seed;
} 

void SimNtupleData::InitializeHistsMap_DATA(){
  h_map["EventNum"] = h_EventNum;
  h_map["Ntriggers"] = h_Ntriggers;
  h_map["real_x_muon"] = h_real_x_muon;
  h_map["real_y_muon"] = h_real_y_muon;
  h_map["iRoad_x"] = h_iRoad_x;
  h_map["iRoad_u"] = h_iRoad_u;
  h_map["iRoad_v"] = h_iRoad_v;
  h_map["Hit_strips"] = h_Hit_strips;
  h_map["Hit_planes"] = h_Hit_planes;
  h_map["Hit_ages"] = h_Hit_ages;
  h_map["trigger_BC"] = h_trigger_BC;
  h_map["N_muon"] = h_N_muon;
  h_map["N_xmuon"] = h_N_xmuon;
  h_map["trig_x"] = h_trig_x;
  h_map["trig_y"] = h_trig_y;
  h_map["dtheta"] = h_dtheta;
} 


// Input: TTree
// Output: Writeable tree
void SimNtupleData::SetBranchData(TTree *t) {
  t->Branch("EventNum", &EventNum);
  t->Branch("Ntriggers", &Ntriggers);
  t->Branch("real_x_muon", &real_x_muon);
  t->Branch("real_y_muon", &real_y_muon);
  t->Branch("iRoad_x", &iRoad_x);
  t->Branch("iRoad_u", &iRoad_u);
  t->Branch("iRoad_v", &iRoad_v);
  t->Branch("Hit_strips", &Hit_strips);
  t->Branch("Hit_planes", &Hit_planes);
  t->Branch("Hit_ages", &Hit_ages);
  t->Branch("trigger_BC", &trigger_BC);
  t->Branch("N_muon", &N_muon);
  t->Branch("N_xmuon", &N_xmuon);
  t->Branch("trig_x", &trig_x);
  t->Branch("trig_y", &trig_y);
  t->Branch("dtheta", &dtheta);
}   



// Input: TTree
// Output: Writeable tree
void SimNtupleData::SetBranchArgs(TTree *t){
    t->Branch("NEvent", &NEvent);
    t->Branch("bkgrate", &bkgrate);
    t->Branch("m_xroad", &m_xroad);
    t->Branch("m_NSTRIPS", &m_NSTRIPS);
    t->Branch("m_bcwind", &m_bcwind);
    t->Branch("m_sig_art", &m_sig_art);
    t->Branch("killran", &killran);
    t->Branch("killxran", &killxran);
    t->Branch("killuvran", &killuvran);
    t->Branch("m_sig_art_x", &m_sig_art_x);
    t->Branch("mm_eff", &mm_eff);
    t->Branch("m_xthr", &m_xthr);
    t->Branch("m_uvthr", &m_uvthr);
    t->Branch("bkgflag", &bkgflag);
    t->Branch("pltflag", &pltflag);
    t->Branch("uvrflag", &uvrflag);
    t->Branch("trapflag", &trapflag);
    t->Branch("ideal_tp", &ideal_tp);
    t->Branch("ideal_vmm", &ideal_vmm);
    t->Branch("ideal_addc", &ideal_addc);
    t->Branch("write_tree", &write_tree);
    t->Branch("bkgonly", &bkgonly);
    t->Branch("smear_art", &smear_art);
    t->Branch("funcsmear_art", &funcsmear_art);
    t->Branch("chamber", &chamber);
    t->Branch("legacy", &legacy);
    t->Branch("seed", &seed);
}



// Input: TTree
// Output: Readable tree
void SimNtupleData::SetAddressReadData(TTree *t) {
  t->SetBranchAddress("EventNum", &EventNum);
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
}   



// Input: TTree
// Output: Readable tree
void SimNtupleData::SetAddressReadArgs(TTree *t){
    t->SetBranchAddress("NEvent", &NEvent);
    t->SetBranchAddress("bkgrate", &bkgrate);
    t->SetBranchAddress("m_xroad", &m_xroad);
    t->SetBranchAddress("m_NSTRIPS", &m_NSTRIPS);
    t->SetBranchAddress("m_bcwind", &m_bcwind);
    t->SetBranchAddress("m_sig_art", &m_sig_art);
    t->SetBranchAddress("killran", &killran);
    t->SetBranchAddress("killxran", &killxran);
    t->SetBranchAddress("killuvran", &killuvran);
    t->SetBranchAddress("m_sig_art_x", &m_sig_art_x);
    t->SetBranchAddress("mm_eff", &mm_eff);
    t->SetBranchAddress("m_xthr", &m_xthr);
    t->SetBranchAddress("m_uvthr", &m_uvthr);
    t->SetBranchAddress("bkgflag", &bkgflag);
    t->SetBranchAddress("pltflag", &pltflag);
    t->SetBranchAddress("uvrflag", &uvrflag);
    t->SetBranchAddress("trapflag", &trapflag);
    t->SetBranchAddress("ideal_tp", &ideal_tp);
    t->SetBranchAddress("ideal_vmm", &ideal_vmm);
    t->SetBranchAddress("ideal_addc", &ideal_addc);
    t->SetBranchAddress("write_tree", &write_tree);
    t->SetBranchAddress("bkgonly", &bkgonly);
    t->SetBranchAddress("smear_art", &smear_art);
    t->SetBranchAddress("funcsmear_art", &funcsmear_art);
    t->SetBranchAddress("chamber", &chamber);
    t->SetBranchAddress("legacy", &legacy);
    t->SetBranchAddress("seed", &seed);
}

void SimNtupleData::doFillArr(TH1D *hist, std::vector<int> *val){
  for( auto v : *val){
    hist->Fill(v);
  }
}

void SimNtupleData::doFillArr(TH1D *hist, std::vector<double> *val){
  for( auto v : *val){
    hist->Fill(v);
  }
}

void SimNtupleData::doFillArr(TH1D *hist, std::vector< std::vector<int> > *val){
  for( int i = 0; i < val->size(); i++){
    for( auto x : val->at(i)){
      hist->Fill(x);
    }
  }
}

// Input: vector of histogram name strings
// Output: None, initializes the simulation data histograms whose string is in the list
void SimNtupleData::InitializeHistsData(std::vector< std::string > hists){
  // Histograms for simulation data
  for( auto hist : hists ){
    if( hist == "EventNum"){ h_EventNum = new TH1D(Form("EventNum_%i",ID), Form("EventNum_%i",ID), 2000, -100, 100); }
    if( hist == "Ntriggers"){ h_Ntriggers = new TH1D(Form("Ntriggers_%i",ID), Form("Ntriggers_%i",ID), 2000, -100, 100); }
    if( hist == "real_x_muon"){ h_real_x_muon = new TH1D(Form("real_x_muon_%i",ID), Form("real_x_muon_%i",ID), 2000, -100, 100); }
    if( hist == "real_y_muon"){ h_real_y_muon = new TH1D(Form("real_y_muon_%i",ID), Form("real_y_muon_%i",ID), 2000, -100, 100); }
    if( hist == "iRoad_x"){ h_iRoad_x = new TH1D(Form("iRoad_x_%i",ID), Form("iRoad_x_%i",ID), 2000, -100, 100); }
    if( hist == "iRoad_u"){ h_iRoad_u = new TH1D(Form("iRoad_u_%i",ID), Form("iRoad_u_%i",ID), 2000, -100, 100); }
    if( hist == "iRoad_v"){ h_iRoad_v = new TH1D(Form("iRoad_v_%i",ID), Form("iRoad_v_%i",ID), 2000, -100, 100); }
    if( hist == "Hit_strips"){ h_Hit_strips = new TH1D(Form("Hit_strips_%i",ID), Form("Hit_strips_%i",ID), 2000, -100, 100); }
    if( hist == "Hit_planes"){ h_Hit_planes = new TH1D(Form("Hit_planes_%i",ID), Form("Hit_planes_%i",ID), 2000, -100, 100); }
    if( hist == "Hit_ages"){ h_Hit_ages = new TH1D(Form("Hit_ages_%i",ID), Form("Hit_ages_%i",ID), 2000, -100, 100); }
    if( hist == "trigger_BC"){ h_trigger_BC = new TH1D(Form("trigger_BC_%i",ID), Form("trigger_BC_%i",ID), 2000, -100, 100); }
    if( hist == "N_muon"){ h_N_muon = new TH1D(Form("N_muon_%i",ID), Form("N_muon_%i",ID), 2000, -100, 100); }
    if( hist == "N_xmuon"){ h_N_xmuon = new TH1D(Form("N_xmuon_%i",ID), Form("N_xmuon_%i",ID), 2000, -100, 100); }
    if( hist == "trig_x"){ h_trig_x = new TH1D(Form("trig_x_%i",ID), Form("trig_x_%i",ID), 2000, -100, 100); }
    if( hist == "trig_y"){ h_trig_y = new TH1D(Form("trig_y_%i",ID), Form("trig_y_%i",ID), 2000, -100, 100); }
    if( hist == "dtheta"){ h_dtheta = new TH1D(Form("dtheta_%i",ID), Form("dtheta_%i",ID), 2000, -100, 100); }   
  }
}




// Input: vector of histogram name strings
// Output: None, fills the simulation data histograms whose string is in the list
void SimNtupleData::FillHistsData(std::vector< std::string > hists){
  // Histograms for simulation data
  for( auto hist : hists ){
    if( hist == "EventNum"){ h_EventNum->Fill(EventNum); }
    if( hist == "Ntriggers"){ h_Ntriggers->Fill(Ntriggers); }
    if( hist == "real_x_muon"){ h_real_x_muon->Fill(real_x_muon); }
    if( hist == "real_y_muon"){ h_real_y_muon->Fill(real_y_muon); }
    if( hist == "iRoad_x"){ doFillArr(h_iRoad_x, iRoad_x); }
    if( hist == "iRoad_u"){ doFillArr(h_iRoad_u, iRoad_u); }
    if( hist == "iRoad_v"){ doFillArr(h_iRoad_v, iRoad_v); }
    if( hist == "Hit_strips"){ doFillArr(h_Hit_strips, Hit_strips); }
    if( hist == "Hit_planes"){ doFillArr(h_Hit_planes, Hit_planes); }
    if( hist == "Hit_ages"){ doFillArr(h_Hit_ages, Hit_ages); }
    if( hist == "trigger_BC"){ doFillArr(h_trigger_BC, trigger_BC); }
    if( hist == "N_muon"){ doFillArr(h_N_muon, N_muon); }
    if( hist == "N_xmuon"){ doFillArr(h_N_xmuon, N_xmuon); }
    if( hist == "trig_x"){ doFillArr(h_trig_x, trig_x); }
    if( hist == "trig_y"){ doFillArr(h_trig_y, trig_y); }
    if( hist == "dtheta"){ doFillArr(h_dtheta, dtheta); }
  }
}



// Input: None
// Output: None, initializes ALL of the data histograms
void SimNtupleData::InitializeHistsData_ALL(){
  // Histograms for simulation data
  h_EventNum = new TH1D(Form("EventNum_%i",ID), Form("EventNum_%i",ID), 10000, 0, 10000); 
  h_Ntriggers = new TH1D(Form("Ntriggers_%i",ID), Form("Ntriggers_%i",ID), 20, 0, 20); 
  h_real_x_muon = new TH1D(Form("real_x_muon_%i",ID), Form("real_x_muon_%i",ID), 360, 0, 3600); 
  h_real_y_muon = new TH1D(Form("real_y_muon_%i",ID), Form("real_y_muon_%i",ID), 360, 0, 2400); 
  h_iRoad_x = new TH1D(Form("iRoad_x_%i",ID), Form("iRoad_x_%i",ID), 120, 0, 1200); 
  h_iRoad_u = new TH1D(Form("iRoad_u_%i",ID), Form("iRoad_u_%i",ID), 120, 0, 1200); 
  h_iRoad_v = new TH1D(Form("iRoad_v_%i",ID), Form("iRoad_v_%i",ID), 120, 0, 1200); 
  h_Hit_strips = new TH1D(Form("Hit_strips_%i",ID), Form("Hit_strips_%i",ID), 900, 0, 9000); 
  h_Hit_planes = new TH1D(Form("Hit_planes_%i",ID), Form("Hit_planes_%i",ID), 8, 0, 8); 
  h_Hit_ages = new TH1D(Form("Hit_ages_%i",ID), Form("Hit_ages_%i",ID), 8, 0, 8); 
  h_trigger_BC = new TH1D(Form("trigger_BC_%i",ID), Form("trigger_BC_%i",ID), 12, 0, 12); 
  h_N_muon = new TH1D(Form("N_muon_%i",ID), Form("N_muon_%i",ID), 12, 0, 12); 
  h_N_xmuon = new TH1D(Form("N_xmuon_%i",ID), Form("N_xmuon_%i",ID), 20, 0, 20); 
  h_trig_x = new TH1D(Form("trig_x_%i",ID), Form("trig_x_%i",ID), 360, 0, 3600); 
  h_trig_y = new TH1D(Form("trig_y_%i",ID), Form("trig_y_%i",ID), 230, 0, 2300); 
  h_dtheta = new TH1D(Form("dtheta_%i",ID), Form("dtheta_%i",ID), 50, -0.05, 0.05);   
}



// Input: None
// Output: None, fills ALL of the data histograms
void SimNtupleData::FillHistsData_ALL(){
  // Histograms for simulation data
  h_EventNum->Fill(EventNum); 
  h_Ntriggers->Fill(Ntriggers); 
  h_real_x_muon->Fill(real_x_muon); 
  h_real_y_muon->Fill(real_y_muon); 
  doFillArr(h_iRoad_x, iRoad_x); 
  doFillArr(h_iRoad_u, iRoad_u); 
  doFillArr(h_iRoad_v, iRoad_v); 
  doFillArr(h_Hit_strips, Hit_strips); 
  doFillArr(h_Hit_planes, Hit_planes); 
  doFillArr(h_Hit_ages, Hit_ages); 
  doFillArr(h_trigger_BC, trigger_BC); 
  doFillArr(h_N_muon, N_muon); 
  doFillArr(h_N_xmuon, N_xmuon); 
  doFillArr(h_trig_x, trig_x); 
  doFillArr(h_trig_y, trig_y); 
  doFillArr(h_dtheta, dtheta); 
}

// Input: vector of histogram name strings
// Output: None, initializes the simulation parameter histograms whose string is in the list
void SimNtupleData::InitializeHistsArgs(std::vector< std::string > hists){
  // Histograms for simulation parameters
  for( auto hist : hists ){
    if( hist == "NEvent"){ h_NEvent = new TH1D(Form("NEvent_%i",ID), Form("NEvent_%i",ID), 10000, 0, 10000); }
    if( hist == "bkgrate"){  h_bkgrate = new TH1D(Form("bkgrate_%i",ID), Form("bkgrate_%i",ID), 10000, 0, 10000); }
    if( hist == "m_xroad"){ h_m_xroad = new TH1D(Form("m_xroad_%i",ID), Form("m_xroad_%i",ID), 2000, -100, 100); }
    if( hist == "m_NSTRIPS"){ h_m_NSTRIPS = new TH1D(Form("m_NSTRIPS_%i",ID), Form("m_NSTRIPS_%i",ID), 2000, -100, 100); }
    if( hist == "m_bcwind"){ h_m_bcwind = new TH1D(Form("m_bcwind_%i",ID), Form("m_bcwind_%i",ID), 2000, -100, 100); }
    if( hist == "m_sig_art"){ h_m_sig_art = new TH1D(Form("m_sig_art_%i",ID), Form("m_sig_art_%i",ID), 2000, -100, 100); }
    if( hist == "killran"){ h_killran = new TH1D(Form("killran_%i",ID), Form("dtheta_%i",ID), 2000, -100, 100); }
    if( hist == "killxran"){ h_killxran = new TH1D(Form("killxran_%i",ID), Form("killxran_%i",ID), 2000, -100, 100); }
    if( hist == "killuvran"){ h_killuvran = new TH1D(Form("killuvran_%i",ID), Form("killuvran_%i",ID), 2000, -100, 100); }
    if( hist == "m_sig_art_x"){ h_m_sig_art_x = new TH1D(Form("m_sig_art_x_%i",ID), Form("m_sig_art_x_%i",ID), 2000, -100, 100); }
    if( hist == "mm_eff"){ h_mm_eff = new TH1D(Form("mm_eff_%i",ID), Form("mm_eff_%i",ID), 2000, -100, 100); }
    if( hist == "m_xthr"){ h_m_xthr = new TH1D(Form("m_xthr_%i",ID), Form("m_xthr_%i",ID), 2000, -100, 100); }
    if( hist == "m_uvthr"){ h_m_uvthr = new TH1D(Form("m_uvthr_%i",ID), Form("m_uvthr_%i",ID), 2000, -100, 100); }
    if( hist == "bkgflag"){ h_bkgflag = new TH1D(Form("bkgflag_%i",ID), Form("bkgflag_%i",ID), 2000, -100, 100); }
    if( hist == "pltflag"){ h_pltflag = new TH1D(Form("pltflag_%i",ID), Form("pltflag_%i",ID), 2000, -100, 100); }
    if( hist == "uvrflag"){ h_uvrflag = new TH1D(Form("uvrflag_%i",ID), Form("uvrflag_%i",ID), 2000, -100, 100); }
    if( hist == "trapflag"){ h_trapflag = new TH1D(Form("trapflag_%i",ID), Form("trapflag_%i",ID), 2000, -100, 100); }
    if( hist == "ideal_tp"){ h_ideal_tp = new TH1D(Form("ideal_tp_%i",ID), Form("ideal_tp_%i",ID), 2000, -100, 100); }
    if( hist == "ideal_vmm"){ h_ideal_vmm = new TH1D(Form("ideal_vmm_%i",ID), Form("ideal_vmm_%i",ID), 2000, -100, 100); }
    if( hist == "ideal_addc"){ h_ideal_addc = new TH1D(Form("ideal_addc_%i",ID), Form("ideal_addc_%i",ID), 2000, -100, 100); }
    if( hist == "write_tree"){ h_write_tree = new TH1D(Form("write_tree_%i",ID), Form("write_tree_%i",ID), 2000, -100, 100); }
    if( hist == "bkgonly"){ h_bkgonly = new TH1D(Form("bkgonly_%i",ID), Form("bkgonly_%i",ID), 2000, -100, 100); }
    if( hist == "smear_art"){ h_smear_art = new TH1D(Form("smear_art_%i",ID), Form("smear_art_%i",ID), 2000, -100, 100); }
    if( hist == "funcsmear_art"){ h_funcsmear_art = new TH1D(Form("funcsmear_art_%i",ID), Form("funcsmear_art_%i",ID), 2000, -100, 100); }
    if( hist == "chamber"){ h_chamber = new TH1D(Form("chamber_%i",ID), Form("chamber_%i",ID), 2000, -100, 100); }
    if( hist == "legacy"){ h_legacy = new TH1D(Form("legacy_%i",ID), Form("legacy_%i",ID), 2000, -100, 100); }
    if( hist == "seed"){ h_seed = new TH1D(Form("seed_%i",ID), Form("seed_%i",ID), 2000, -100, 100); }
  } 
}



// Input: vector of histogram name strings
// Output: None, fills the simulation parameter histograms whose string is in the list
void SimNtupleData::FillHistsArgs(std::vector< std::string > hists){
  // Histograms for simulation parameters
  for( auto hist : hists ){
    if( hist == "NEvent"){ h_NEvent->Fill(NEvent); }
    if( hist == "bkgrate"){  h_bkgrate->Fill(bkgrate); }
    if( hist == "m_xroad"){ h_m_xroad->Fill(m_xroad); }
    if( hist == "m_NSTRIPS"){ h_m_NSTRIPS->Fill(m_NSTRIPS); }
    if( hist == "m_bcwind"){ h_m_bcwind->Fill(m_bcwind); }
    if( hist == "m_sig_art"){ h_m_sig_art->Fill(m_sig_art); }
    if( hist == "killran"){ h_killran->Fill(killran); }
    if( hist == "killxran"){ h_killxran->Fill(killxran); }
    if( hist == "killuvran"){ h_killuvran->Fill(killuvran); }
    if( hist == "m_sig_art_x"){ h_m_sig_art_x->Fill(m_sig_art_x); }
    if( hist == "mm_eff"){ doFillArr(h_mm_eff, mm_eff); }
    if( hist == "m_xthr"){ h_m_xthr->Fill(m_xthr); }
    if( hist == "m_uvthr"){ h_m_uvthr->Fill(m_uvthr); }
    if( hist == "bkgflag"){ h_bkgflag->Fill(bkgflag); }
    if( hist == "pltflag"){ h_pltflag->Fill(pltflag); }
    if( hist == "uvrflag"){ h_uvrflag->Fill(uvrflag); }
    if( hist == "trapflag"){ h_trapflag->Fill(trapflag); }
    if( hist == "ideal_tp"){ h_ideal_tp->Fill(ideal_tp); }
    if( hist == "ideal_vmm"){ h_ideal_vmm->Fill(ideal_vmm); }
    if( hist == "ideal_addc"){ h_ideal_addc->Fill(ideal_addc); }
    if( hist == "write_tree"){ h_write_tree->Fill(write_tree); }
    if( hist == "bkgonly"){ h_bkgonly->Fill(bkgonly); }
    if( hist == "smear_art"){ h_smear_art->Fill(smear_art); }
    if( hist == "funcsmear_art"){ h_funcsmear_art->Fill(funcsmear_art); }
    if( hist == "chamber"){ h_chamber->Fill(chamber); }
    if( hist == "legacy"){ h_legacy->Fill(legacy); }
    if( hist == "seed"){ h_seed->Fill(seed); }
  } 
}


// Input: None
// Output: None, initializes ALL the simulation parameter histograms
void SimNtupleData::InitializeHistsArgs_ALL(){
  h_NEvent = new TH1D(Form("NEvent_%i",ID), Form("NEvent_%i",ID), 2000, -100, 100);
  h_bkgrate = new TH1D(Form("bkgrate_%i",ID), Form("bkgrate_%i",ID), 2000, -100, 100);
  h_m_xroad = new TH1D(Form("m_xroad_%i",ID), Form("m_xroad_%i",ID), 2000, -100, 100);
  h_m_NSTRIPS = new TH1D(Form("m_NSTRIPS_%i",ID), Form("m_NSTRIPS_%i",ID), 2000, -100, 100);
  h_m_bcwind = new TH1D(Form("m_bcwind_%i",ID), Form("m_bcwind_%i",ID), 2000, -100, 100);
  h_m_sig_art = new TH1D(Form("m_sig_art_%i",ID), Form("m_sig_art_%i",ID), 2000, -100, 100);
  h_killran = new TH1D(Form("killran_%i",ID), Form("dtheta_%i",ID), 2000, -100, 100);
  h_killxran = new TH1D(Form("killxran_%i",ID), Form("killxran_%i",ID), 2000, -100, 100);
  h_killuvran = new TH1D(Form("killuvran_%i",ID), Form("killuvran_%i",ID), 2000, -100, 100);
  h_m_sig_art_x = new TH1D(Form("m_sig_art_x_%i",ID), Form("m_sig_art_x_%i",ID), 2000, -100, 100);
  h_mm_eff = new TH1D(Form("mm_eff_%i",ID), Form("mm_eff_%i",ID), 2000, -100, 100);
  h_m_xthr = new TH1D(Form("m_xthr_%i",ID), Form("m_xthr_%i",ID), 2000, -100, 100);
  h_m_uvthr = new TH1D(Form("m_uvthr_%i",ID), Form("m_uvthr_%i",ID), 2000, -100, 100);
  h_bkgflag = new TH1D(Form("bkgflag_%i",ID), Form("bkgflag_%i",ID), 2000, -100, 100);
  h_pltflag = new TH1D(Form("pltflag_%i",ID), Form("pltflag_%i",ID), 2000, -100, 100);
  h_uvrflag = new TH1D(Form("uvrflag_%i",ID), Form("uvrflag_%i",ID), 2000, -100, 100);
  h_trapflag = new TH1D(Form("trapflag_%i",ID), Form("trapflag_%i",ID), 2000, -100, 100);
  h_ideal_tp = new TH1D(Form("ideal_tp_%i",ID), Form("ideal_tp_%i",ID), 2000, -100, 100);
  h_ideal_vmm = new TH1D(Form("ideal_vmm_%i",ID), Form("ideal_vmm_%i",ID), 2000, -100, 100);
  h_ideal_addc = new TH1D(Form("ideal_addc_%i",ID), Form("ideal_addc_%i",ID), 2000, -100, 100);
  h_write_tree = new TH1D(Form("write_tree_%i",ID), Form("write_tree_%i",ID), 2000, -100, 100);
  h_bkgonly = new TH1D(Form("bkgonly_%i",ID), Form("bkgonly_%i",ID), 2000, -100, 100);
  h_smear_art = new TH1D(Form("smear_art_%i",ID), Form("smear_art_%i",ID), 2000, -100, 100);
  h_funcsmear_art = new TH1D(Form("funcsmear_art_%i",ID), Form("funcsmear_art_%i",ID), 2000, -100, 100);
  h_chamber = new TH1D(Form("chamber_%i",ID), Form("chamber_%i",ID), 2000, -100, 100);
  h_legacy = new TH1D(Form("legacy_%i",ID), Form("legacy_%i",ID), 2000, -100, 100);
  h_seed = new TH1D(Form("seed_%i",ID), Form("seed_%i",ID), 2000, -100, 100);
}



// Input: vector of histogram name strings
// Output: None, fills ALL the simulation parameter histograms whose string is in the list
void SimNtupleData::FillHistsArgs_ALL(){
  // Histograms for simulation parameters
  h_NEvent->Fill(NEvent); 
  h_bkgrate->Fill(bkgrate); 
  h_m_xroad->Fill(m_xroad); 
  h_m_NSTRIPS->Fill(m_NSTRIPS); 
  h_m_bcwind->Fill(m_bcwind); 
  h_m_sig_art->Fill(m_sig_art); 
  h_killran->Fill(killran); 
  h_killxran->Fill(killxran); 
  h_killuvran->Fill(killuvran); 
  h_m_sig_art_x->Fill(m_sig_art_x); 
  doFillArr(h_mm_eff, mm_eff);
  h_m_xthr->Fill(m_xthr); 
  h_m_uvthr->Fill(m_uvthr); 
  h_bkgflag->Fill(bkgflag); 
  h_pltflag->Fill(pltflag); 
  h_uvrflag->Fill(uvrflag); 
  h_trapflag->Fill(trapflag); 
  h_ideal_tp->Fill(ideal_tp); 
  h_ideal_vmm->Fill(ideal_vmm); 
  h_ideal_addc->Fill(ideal_addc); 
  h_write_tree->Fill(write_tree); 
  h_bkgonly->Fill(bkgonly); 
  h_smear_art->Fill(smear_art); 
  h_funcsmear_art->Fill(funcsmear_art); 
  h_chamber->Fill(chamber); 
  h_legacy->Fill(legacy); 
  h_seed->Fill(seed); 
}

void SimNtupleData::DeleteAllHists(){
  for( auto const& [key, val] : h_map )
  {
    //std::cout << "Deleting Histogram key: " << key << std::endl;
    delete val;
  }
}
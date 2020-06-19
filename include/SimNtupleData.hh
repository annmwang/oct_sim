/************** 

SIMULATION DATA TREES

Made by: Anthony Badea (June 2020)

**************/

//cpp dependencies
#include <vector>
#include <string>

//ROOT dependencies
#include <TTree.h>
#include <TVector3.h>

// DataFormat
class SimNtupleData{

public:

    SimNtupleData();

    static const int nMaxPart = 10000;

    // Variables to write out to the tree
    int EventNum = 0;
    int NEvent = 0; // number of events to be generated
    int Ntriggers = 0; // number of triggers on the roads

    double real_x_muon = 0; // x-location of a real muon
    double real_y_muon = 0; // y-location of a real muon

    vector<int> * iRoad_x = 0; // ??
    vector<int> * iRoad_u = 0; // ??
    vector<int> * iRoad_v = 0; // ??

    vector<vector<int>> * Hit_strips = 0; // ??
    vector<vector<int>> * Hit_planes = 0; // ??
    vector<vector<int>> * Hit_ages = 0; // ??
    vector<int> * trigger_BC = 0; // ??
    vector<int> * N_muon = 0; // ??
    vector<int> * N_xmuon = 0; // ??

    vector<double> * trig_x = 0; // x-locations of all triggers
    vector<double> * trig_y = 0; // y-locations of all triggers

    vector<double> * dtheta = 0; // ??

    // simulation parameters
    int bkgrate = -999.; // Hz per strip
    int m_xroad = -999.; // size of x road in strips
    int m_NSTRIPS = -999.; // number of x strips
    int m_bcwind = -999.; // fixed time window (in bunch crossings) during which the algorithm collects ART hits
    int m_sig_art = -999.; // art time resolution (in nanoseconds)
    int killran = -999.; // bool if you want to kill one plane randomly
    int killxran = -999.; // bool if you want to kill one X plane randomly 
    int killuvran = -999.; // bool if you want to kill one U or V plane randomly 
    int m_sig_art_x = -999.; // ART position resolution (in strips). used to smear ART position
    vector<double> * mm_eff = 0; // efficiency of each PCB Left/Right of the MM
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

    void SetBranchData(TTree *t);
    void SetBranchArgs(TTree *t);
    void SetAddressReadData(TTree *t);
    void SetAddressReadArgs(TTree *t);
       
private:
     
};

inline SimNtupleData::SimNtupleData(){}

// Input: TTree
// Output: Writeable tree
void SimNtupleData::SetBranchData(TTree *t) {
  t->Branch("EventNum", &EventNum);
  t->Branch("NEvent", &NEvent);
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
}   


// Input: TTree
// Output: Readable tree
void SimNtupleData::SetAddressReadArgs(TTree *t){
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

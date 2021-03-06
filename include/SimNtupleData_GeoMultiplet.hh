/************** 
SIMULATION DATA TREES
Made by: Anthony Badea (June 2020)
**************/

// To enable vector<vector<int> > branch in a TTree
#include "VectorDict.cxx"

class SimNtupleData{

public:
    // job parameters
    bool b_LM = -999;
    // road parameters
    bool uvrflag = -999; 
    int m_xthr = -999; 
    int m_uvthr = -999; 
    bool trapflag = -999; 
    // parameters of the finder algorithm
    bool saveHits = -999;
    bool ideal_vmm = -999;
    bool ideal_addc = -999;
    bool ideal_tp = -999; 
    // finder parameters
    int XROAD = -999;
    double B = -999;
    int ylow = -999;
    int yhigh = -999;
    int UVFACTOR = -999;
    int bc_wind = -999;
    int NSTRIPS_UP_UV = -999;
    int NSTRIPS_DN_UV = -999;
    int NSTRIPS_UP_XX = -999;
    int NSTRIPS_DN_XX = -999;
    int NPLANES = -999;
    int ADDC_BUFFER = -999;
    bool db = -999; 
    int seed = -999;

    // variables from slope_t struct in finder algorithm
    int event = -999;
    int ntrigroads = -999;
    std::vector<int> *count = 0;
    std::vector<int> *iroad = 0;
    std::vector<int> *iroadu = 0;
    std::vector<int> *iroadv = 0;
    std::vector<int> *imuonhits = 0;
    std::vector<int> *uvbkg = 0;
    std::vector<int> *xbkg = 0;
    std::vector<int> *xmuon = 0;
    std::vector<int> *age = 0; 
    std::vector<int> *mxl = 0;
    std::vector<int> *xavg = 0;
    std::vector<int> *yavg = 0;
    std::vector<int> *xcenter = 0;
    std::vector<int> *ycenter = 0;
    std::vector< std::vector<int> > *hit_layer = 0;
    std::vector< std::vector<int> > *hit_strip = 0;
    std::vector< std::vector<int> > *hit_bc = 0;

    // variables for tracks from fitter algorithm
    std::vector< std::vector<double> > *track_x = 0;
    std::vector< std::vector<double> > *track_y = 0;
    std::vector< std::vector<double> > *track_z = 0;

    // Initialize functions
    SimNtupleData();
    void SetBranchData(TTree *t);
    void SetBranchArgs(TTree *t);
    void SetAddressReadData(TTree *t);
    void SetAddressReadArgs(TTree *t);

private:
         
};

SimNtupleData::SimNtupleData(){}

void SimNtupleData::SetBranchData(TTree *t) {
    t->Branch("event", &event);
    t->Branch("ntrigroads", &ntrigroads);
    t->Branch("count", &count);
    t->Branch("iroad", &iroad);
    t->Branch("iroadu", &iroadu);
    t->Branch("iroadv", &iroadv);
    t->Branch("imuonhits", &imuonhits);
    t->Branch("uvbkg", &uvbkg);
    t->Branch("xbkg", &xbkg);
    t->Branch("xmuon", &xmuon);
    t->Branch("age", &age);
    t->Branch("mxl", &mxl);
    t->Branch("xavg", &xavg);
    t->Branch("yavg", &yavg);
    t->Branch("xcenter", &xcenter);
    t->Branch("ycenter", &ycenter);
    t->Branch("hit_layer", &hit_layer);
    t->Branch("hit_strip", &hit_strip);
    t->Branch("hit_bc", &hit_bc);
    t->Branch("track_x", &track_x);
    t->Branch("track_y", &track_y);
    t->Branch("track_z", &track_z);
}   

void SimNtupleData::SetAddressReadData(TTree *t) {
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("ntrigroads", &ntrigroads);
    t->SetBranchAddress("count", &count);
    t->SetBranchAddress("iroad", &iroad);
    t->SetBranchAddress("iroadu", &iroadu);
    t->SetBranchAddress("iroadv", &iroadv);
    t->SetBranchAddress("imuonhits", &imuonhits);
    t->SetBranchAddress("uvbkg", &uvbkg);
    t->SetBranchAddress("xbkg", &xbkg);
    t->SetBranchAddress("xmuon", &xmuon);
    t->SetBranchAddress("age", &age);
    t->SetBranchAddress("mxl", &mxl);
    t->SetBranchAddress("xavg", &xavg);
    t->SetBranchAddress("yavg", &yavg);
    t->SetBranchAddress("xcenter", &xcenter);
    t->SetBranchAddress("ycenter", &ycenter);
    t->SetBranchAddress("hit_layer", &hit_layer);
    t->SetBranchAddress("hit_strip", &hit_strip);
    t->SetBranchAddress("hit_bc", &hit_bc);
    t->SetBranchAddress("track_x", &track_x);
    t->SetBranchAddress("track_y", &track_y);
    t->SetBranchAddress("track_z", &track_z);
}   

void SimNtupleData::SetBranchArgs(TTree *t){
    t->Branch("b_LM", &b_LM);
    // road parameters
    t->Branch("uvrflag", &uvrflag); 
    t->Branch("m_xthr", &m_xthr); 
    t->Branch("m_uvthr", &m_uvthr); 
    t->Branch("trapflag", &trapflag); 
    // parameters of the finder algorithm
    t->Branch("saveHits", &saveHits);
    t->Branch("ideal_vmm", &ideal_vmm);
    t->Branch("ideal_addc", &ideal_addc);
    t->Branch("ideal_tp", &ideal_tp); 
    // finder parameters
    t->Branch("XROAD", &XROAD);
    t->Branch("B", &B);
    t->Branch("ylow", &ylow);
    t->Branch("yhigh", &yhigh);
    t->Branch("UVFACTOR", &UVFACTOR);
    t->Branch("bc_wind", &bc_wind);
    t->Branch("NSTRIPS_UP_UV", &NSTRIPS_UP_UV);
    t->Branch("NSTRIPS_DN_UV", &NSTRIPS_DN_UV);
    t->Branch("NSTRIPS_UP_XX", &NSTRIPS_UP_XX);
    t->Branch("NSTRIPS_DN_XX", &NSTRIPS_DN_XX);
    t->Branch("NPLANES", &NPLANES);
    t->Branch("ADDC_BUFFER", &ADDC_BUFFER);
    t->Branch("db", &db); 
    t->Branch("seed", &seed);
}

void SimNtupleData::SetAddressReadArgs(TTree *t){
    t->SetBranchAddress("b_LM", &b_LM);
    // road parameters
    t->SetBranchAddress("uvrflag", &uvrflag); 
    t->SetBranchAddress("m_xthr", &m_xthr); 
    t->SetBranchAddress("m_uvthr", &m_uvthr); 
    t->SetBranchAddress("trapflag", &trapflag); 
    // parameters of the finder algorithm
    t->SetBranchAddress("saveHits", &saveHits);
    t->SetBranchAddress("ideal_vmm", &ideal_vmm);
    t->SetBranchAddress("ideal_addc", &ideal_addc);
    t->SetBranchAddress("ideal_tp", &ideal_tp); 
    // finder parameters
    t->SetBranchAddress("XROAD", &XROAD);
    t->SetBranchAddress("B", &B);
    t->SetBranchAddress("ylow", &ylow);
    t->SetBranchAddress("yhigh", &yhigh);
    t->SetBranchAddress("UVFACTOR", &UVFACTOR);
    t->SetBranchAddress("bc_wind", &bc_wind);
    t->SetBranchAddress("NSTRIPS_UP_UV", &NSTRIPS_UP_UV);
    t->SetBranchAddress("NSTRIPS_DN_UV", &NSTRIPS_DN_UV);
    t->SetBranchAddress("NSTRIPS_UP_XX", &NSTRIPS_UP_XX);
    t->SetBranchAddress("NSTRIPS_DN_XX", &NSTRIPS_DN_XX);
    t->SetBranchAddress("NPLANES", &NPLANES);
    t->SetBranchAddress("ADDC_BUFFER", &ADDC_BUFFER);
    t->SetBranchAddress("db", &db); 
    t->SetBranchAddress("seed", &seed);
}





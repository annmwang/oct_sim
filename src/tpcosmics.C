// C++ includes
#include <iostream>
using namespace std;

// ROOT include
#include <TRandom3.h>
#include <TF1.h>

// Micromegas double wedge geometry
#include "GeoMultiplet.hh"
// Header includes
// #include <SimUtilities.hh>
#include "Road_GeoMultiplet.hh" // create_roads
#include "Finder_GeoMultiplet.hh"
#include "Hit_GeoMultiplet.hh"
// #include <ParticleUtilities.hh> 
// #include <GeneralUtilities.hh>
// #include <Finder.hh>
// #include <SimConstants.hh>
// #include <SimNtupleData.hh>


vector<Road*> create_roads(const GeoMultiplet& geometry, bool uvrflag, int m_xthr, int m_uvthr, string chamber, bool trapflag){
/*
Version of code with GeoMultiplet used instead of GeoOctuplet.
*/
    int nroad = NSTRIPS_PER_LAYER/XROAD;
    vector<Road*> m_roads;
    for ( int i = 0; i < nroad; i++){
        Road* myroad = nullptr;
        myroad = new Road(&geometry, m_xthr, m_uvthr, i);
        m_roads.push_back(myroad);

        int nuv = 0;
        if (uvrflag)
            nuv = UVFACTOR;
        for (int uv = 1; uv <= nuv; uv++){
            if (i-uv < 0)
                continue;
            Road* myroad_0 = new Road(&geometry, m_xthr, m_uvthr, i, i+uv,   i-uv);
            Road* myroad_1 = new Road(&geometry, m_xthr, m_uvthr, i, i-uv,   i+uv);
            Road* myroad_2 = new Road(&geometry, m_xthr, m_uvthr, i, i+uv-1, i-uv);
            Road* myroad_3 = new Road(&geometry, m_xthr, m_uvthr, i, i-uv,   i+uv-1);
            Road* myroad_4 = new Road(&geometry, m_xthr, m_uvthr, i, i-uv+1, i+uv);
            Road* myroad_5 = new Road(&geometry, m_xthr, m_uvthr, i, i+uv,   i-uv+1);
            m_roads.push_back(myroad_0);
            m_roads.push_back(myroad_1);
            m_roads.push_back(myroad_2);
            m_roads.push_back(myroad_3);
            m_roads.push_back(myroad_4);
            m_roads.push_back(myroad_5);
        }
    }
    return m_roads;
}

std::tuple< std::vector<int>, std::vector<Hit*> > get_hits(vector<int> *v_artHit_octupletLayer,vector<int> *v_artHit_chPosition, vector<int> *v_artHit_artBCID, GeoMultiplet *geometry){
    bool is_noise = false;
    vector<int> art_bc;
    vector<Hit*> hits;
    for(int i = 0; i < v_artHit_octupletLayer->size(); i++){
    	int layer = v_artHit_octupletLayer->at(i);
    	int channel = v_artHit_chPosition->at(i);
    	int art_bcid = v_artHit_artBCID->at(i);
    	Hit* newhit = new Hit(layer, art_bcid, channel, is_noise, geometry);
		art_bc.push_back(art_bcid);
    	hits.push_back(newhit);
    }
	return make_tuple(art_bc, hits);
}

int main(int argc, char* argv[]) {

	// check input from the user

	// configure the detector geometry copied from https://gitlab.cern.ch/anwang/bb5_analysis/-/blob/master/src/bb5_analysis.C#L190-199
	GeoMultiplet* GEOMETRY;
	bool b_SM = false;
	bool b_LM = true;
	if (b_SM) {
		GEOMETRY = new GeoMultiplet("SM");
		std::cout << "Assuming SM geometry" << std::endl;
	}
	if (b_LM) {
		GEOMETRY = new GeoMultiplet("LM");
		std::cout << "Assuming LM geometry" << std::endl;
	}
	GEOMETRY->SetRunNumber(0);
	// setup finder algorithm roads
	// coincidence params
	bool uvrflag = false; // TO DO: need to turn this on and edit the value of UVFACTOR to enable UV roads
    int m_xthr = 3; // required hits on x layers for coincidence
    int m_uvthr = 3; // required hits on x layers for coincidence
    string chamberType = b_LM ? "large" : "small";
    bool trapflag = false; // for trapezoidal geometry but shouldn't need this if using the correct detector geometry
    
	vector<Road*> roads = create_roads(*GEOMETRY, uvrflag, m_xthr, m_uvthr, string(chamberType), trapflag);
	// open the tree 
	string inFileName = "/Users/anthonybadea/Desktop/tree-2.root";
	TFile *f = new TFile(inFileName.c_str(),"read");
	TTree *t = (TTree*)f->Get("decodedData");
	int nevents = t->GetEntries();
	vector<int> *v_artHit_octupletLayer = 0;        t->SetBranchAddress("v_artHit_octupletLayer" ,  &v_artHit_octupletLayer);
	vector<int> *v_artHit_chPosition    = 0;        t->SetBranchAddress("v_artHit_chPosition"    ,  &v_artHit_chPosition);
	vector<int> *v_artHit_artBCID       = 0;        t->SetBranchAddress("v_artHit_artBCID"       ,  &v_artHit_artBCID);

	
	// next steps
	// load the data file
	// loop over the events
	// create hits
	// pass those hits to the finder
	for ( int evt = 0; evt < nevents; evt++) {
		cout<<"EVENT "<<evt<<endl;
		t->GetEntry(evt);
		if (v_artHit_octupletLayer->size() == 0) continue;
		// get the hits
		vector<int> art_bcid;
		vector<Hit*> hits;
		std::tie(art_bcid,hits) = get_hits(v_artHit_octupletLayer,v_artHit_chPosition,v_artHit_artBCID,GEOMETRY);
		// for ( int i = 0; i < hits.size(); i++){
		// 	cout<<"Hit layer: "<<hits.at(i)->MMFE8Index()<<endl;
		// 	cout<<"Hit vmm: "<<hits.at(i)->VMM()<<endl;
		// 	cout<<"ART BCID: "<<art_bcid.at(i)<<endl;
		// }

		int mu_firstbc = 999999.;
        for (unsigned int j = 0; j < art_bcid.size(); j++){
            if (art_bcid[j] == -1)
                continue;
            else if (art_bcid[j] < mu_firstbc)
                mu_firstbc = art_bcid[j];
        }
        // cout<<"Smallest ART BCID: "<<mu_firstbc<<endl;
		
		// run finder algorithm
	    bool saveHits = true;
		bool ideal_vmm = false;
		bool ideal_addc = false;
		bool ideal_tp = false; 
		vector<slope_t> m_slopes;
	    int ntrigroads;
		std::tie(ntrigroads, m_slopes) = finder(hits, mu_firstbc, roads, saveHits, ideal_vmm, ideal_addc, ideal_tp, evt);

		if(ntrigroads > 0){
			cout<<"Number of Road Triggers: "<<ntrigroads<<endl;
			for ( int i = 0; i < m_slopes.size(); i++){
				cout<<"Count: "<<m_slopes.at(i).count<<endl;
				cout<<"iroad: "<<m_slopes.at(i).iroad<<endl;
				cout<<"mxl: "<<m_slopes.at(i).mxl<<endl;
			}
		}
	}
}


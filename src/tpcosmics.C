
/* 
tpcosmics.C - a script to run the track finding algorithm of the NSW MM trigger processor

Run like:
    make tpcosmics
    ./bin/tpcosmics

Contributing authors (likely missing people): Ann wang, Alex Tuna, Nathan Felt, Melissa Franklin, Gabriel Rabanal, Anne Fortman, Larry Lee, John Huth, Anthony Badea
 */

// C++ includes
#include <iostream>
using namespace std;

// ROOT include
#include <TRandom3.h>
#include <TF1.h>
#include <TFile.h>

// Micromegas double wedge geometry
#include "GeoMultiplet.hh" // from bb5_analysis
// Header includes
#include "Road_GeoMultiplet.hh" 
#include "Finder_GeoMultiplet.hh"
#include "Hit_GeoMultiplet.hh"
#include "SimNtupleData_GeoMultiplet.hh"

// decide large (true) vs small (false) double wedge
bool b_LM = true;
// road parameters
bool uvrflag = true; // TO DO: need to turn this on and edit the value of UVFACTOR to enable UV roads
int m_xthr = 3; // required hits on x layers for coincidence
int m_uvthr = 3; // required hits on x layers for coincidence
string chamberType = b_LM ? "large" : "small";
bool trapflag = false; // for trapezoidal geometry but shouldn't need this if using the correct detector geometry
// parameters of the finder algorithm
bool saveHits = true;
bool ideal_vmm = false;
bool ideal_addc = false;
bool ideal_tp = false; 

vector<Road*> create_roads(const GeoMultiplet& geometry, bool uvrflag, int m_xthr, int m_uvthr, string chamber, bool trapflag){
	/* Returns a vector of Road objects that are used with the finder algorithm. This version uses updated GeoMultiplet detector geometry instead of the previous GeoOctuplet. */
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
	/* take in the art hit (layer,channel,bcid) vectors and return vectors of the art bcid and Hit objects that are used as input to the finder algorithm */ 
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

int firstbc(vector<int> art_bcid){
	/* return the earliest bcid from a vector of bcids */
	int mu_firstbc = 999999.;
		for (unsigned int j = 0; j < art_bcid.size(); j++){
			if (art_bcid[j] == -1)
				continue;
			else if (art_bcid[j] < mu_firstbc)
				mu_firstbc = art_bcid[j];
		}
	return mu_firstbc;
}

void msg(string m){
	printf("\r%s",m.c_str());                               
	std::cout << std::endl;
}

void print_parameters(){
    msg("NSW MM Trigger Processor Track Finder ✪ ‿ ✪");                               
    msg("-------------------------------------------");                                                                                 
	msg(Form("Using chamber type: %s", chamberType.c_str()));  
    msg(Form("Using UV roads: %s", (uvrflag) ? "true" : "false"));   
    msg(Form("Using thresholds (x, uv): (%d, %d)", m_xthr, m_uvthr));  
    msg(Form("Using trapezoidal geometry: %s", (trapflag) ? "true" : "false"));  
    msg(Form("Saving hits: %s", (saveHits) ? "true" : "false"));
    msg(Form("Ideal VMM: %s", (ideal_vmm) ? "true" : "false"));
	msg(Form("Ideal ADDC: %s", (ideal_addc) ? "true" : "false"));
	msg(Form("Ideal TP: %s", (ideal_tp) ? "true" : "false"));
	msg(Form("x-road size (in strips): %d, +/- neighbor roads (uv): %d", XROAD, UVFACTOR)); 
	msg(Form("B: %f", B));
	msg(Form("Strip length: (%d,%d)", ylow,yhigh));
	msg(Form("BCID Window: %d", bc_wind));
	msg(Form("Number of up strips on UV layer: %d", NSTRIPS_UP_UV));
	msg(Form("Number of down strips on UV layer: %d", NSTRIPS_DN_UV));
	msg(Form("Number of up strips on X layer: %d", NSTRIPS_UP_XX));
	msg(Form("Number of down strips on X layer: %d", NSTRIPS_DN_XX));
	msg(Form("Number of layers: %d", NPLANES));
	msg(Form("ADDC buffer: %d", ADDC_BUFFER));
	msg(Form("Track finder debug mode: %s", (db) ? "on" : "off"));
	msg(Form("Random seed (current time): %d", seed));                                      
    msg("-------------------------------------------");                               
    msg("Starting track finding");    
}

void newevent(SimNtupleData *SN){
    SN->count->clear();
    SN->iroad->clear();
    SN->iroadu->clear();
    SN->iroadv->clear();
    SN->imuonhits->clear();
    SN->uvbkg->clear();
    SN->xbkg->clear();
    SN->xmuon->clear();
    SN->age->clear(); //WRT first muon hit BC
    SN->mxl->clear();
    SN->xavg->clear();
    SN->yavg->clear();
    SN->xcenter->clear();
    SN->ycenter->clear();
    SN->hit_layer->clear();
    SN->hit_strip->clear();
    SN->hit_bc->clear();
}

void fillevent(SimNtupleData *SN, TTree* tree, int ntrigroads, vector<slope_t> m_slopes){
	SN->ntrigroads = ntrigroads;
	for (auto slope: m_slopes){
		SN->count->push_back(slope.count);
	    SN->iroad->push_back(slope.iroad);
	    SN->iroadu->push_back(slope.iroadu);
	    SN->iroadv->push_back(slope.iroadv);
	    SN->imuonhits->push_back(slope.imuonhits);
	    SN->uvbkg->push_back(slope.uvbkg);
	    SN->xbkg->push_back(slope.xbkg);
	    SN->xmuon->push_back(slope.xmuon); 
	    SN->age->push_back(slope.age);
	    SN->mxl->push_back(slope.mxl);
	    SN->xavg->push_back(slope.xavg);
	    SN->yavg->push_back(slope.yavg);
	    SN->xcenter->push_back(slope.xcenter);
	    SN->ycenter->push_back(slope.ycenter);
	    vector<int> hit_layer;
	    vector<int> hit_strip;
	    vector<int> hit_bc;
	    for (auto hit: slope.slopehits){
	    	hit_layer.push_back(hit.MMFE8Index());
	    	hit_strip.push_back(hit.Channel());
	    	hit_bc.push_back(hit.BC());
	    }
	    SN->hit_layer->push_back(hit_layer);
    	SN->hit_strip->push_back(hit_strip);
    	SN->hit_bc->push_back(hit_bc);
	}
	tree->Fill();
}

void fillargs(SimNtupleData *SN, TTree* tree){
	// fill the job parameters
    SN->b_LM = b_LM;
    // road parameters
    SN->uvrflag = uvrflag; 
    SN->m_xthr = m_xthr; 
    SN->m_uvthr = m_uvthr; 
    SN->trapflag = trapflag; 
    // parameters of the finder algorithm
    SN->saveHits = saveHits;
    SN->ideal_vmm = ideal_vmm;
    SN->ideal_addc = ideal_addc;
    SN->ideal_tp = ideal_tp; 
    // finder parameters
    SN->XROAD = XROAD;
    SN->B = B;
    SN->ylow = ylow;
    SN->yhigh = yhigh;
    SN->UVFACTOR = UVFACTOR;
    SN->bc_wind = bc_wind;
    SN->NSTRIPS_UP_UV = NSTRIPS_UP_UV;
    SN->NSTRIPS_DN_UV = NSTRIPS_DN_UV;
    SN->NSTRIPS_UP_XX = NSTRIPS_UP_XX;
    SN->NSTRIPS_DN_XX = NSTRIPS_DN_XX;
    SN->NPLANES = NPLANES;
    SN->ADDC_BUFFER = ADDC_BUFFER;
    SN->db = db; 
    SN->seed = seed;
    tree->Fill();
}

int main(int argc, char* argv[]) {
	// print the job parameters
	print_parameters();

	// configure the detector geometry copied from https://gitlab.cern.ch/anwang/bb5_analysis/-/blob/master/src/bb5_analysis.C#L190-199
	GeoMultiplet* GEOMETRY = (b_LM) ? new GeoMultiplet("LM") : new GeoMultiplet("SM");
	GEOMETRY->SetRunNumber(0);

	// setup the roads
	vector<Road*> roads = create_roads(*GEOMETRY, uvrflag, m_xthr, m_uvthr, string(chamberType), trapflag);

	// open the decoded data 
	string inFileName = "/Users/anthonybadea/Desktop/tree-2.root";
	TFile *fin = new TFile(inFileName.c_str(),"read");
	TTree *decodedData = (TTree*)fin->Get("decodedData");
	int nevents = decodedData->GetEntries();
	vector<int> *v_artHit_octupletLayer = 0;        decodedData->SetBranchAddress("v_artHit_octupletLayer" ,  &v_artHit_octupletLayer);
	vector<int> *v_artHit_chPosition    = 0;        decodedData->SetBranchAddress("v_artHit_chPosition"    ,  &v_artHit_chPosition);
	vector<int> *v_artHit_artBCID       = 0;        decodedData->SetBranchAddress("v_artHit_artBCID"       ,  &v_artHit_artBCID);

	// define output ntuple
	string outputFileName = "tpcosmics.root";
	TFile* fout = new TFile(outputFileName.c_str(), "RECREATE");
    TTree* data = new TTree("data","data");
    TTree* args = new TTree("args", "args");
    SimNtupleData *SN = new SimNtupleData();
    SN->SetBranchData(data);
    SN->SetBranchArgs(args);
    fillargs(SN,args);

    msg(Form("Number of events: %d",nevents));
	for ( int evt = 0; evt < nevents; evt++) {
		msg(Form("Event %d/%d",evt,nevents));
		decodedData->GetEntry(evt);
		newevent(SN);
		// if (v_artHit_octupletLayer->size() == 0) continue;
		// get the hits
		vector<int> art_bcid;
		vector<Hit*> hits;
		std::tie(art_bcid,hits) = get_hits(v_artHit_octupletLayer,v_artHit_chPosition,v_artHit_artBCID,GEOMETRY);
		// get the earliest bcid
		int mu_firstbc = firstbc(art_bcid);
		// run finder algorithm
		vector<slope_t> m_slopes;
		int ntrigroads;
		std::tie(ntrigroads, m_slopes) = finder(hits, mu_firstbc, roads, saveHits, ideal_vmm, ideal_addc, ideal_tp, evt);
		fillevent(SN,data,ntrigroads,m_slopes);
	}
	msg("Finished.");    
	msg("-------------------------------------------");                               
    msg(Form("Writing to: %s",outputFileName.c_str()));
    
	fout->cd();
	args->Write();
	data->Write();
	fout->Close();
	fin->Close();

	return 0;
}


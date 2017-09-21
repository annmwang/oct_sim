// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <dirent.h>
#include <ctime>
#include <bitset>
#include <cmath>
#include <chrono>

// ROOT includes
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TMath.h>
#include <GeoOctuplet.hh>
#include <Hit.hh>
#include <Road.hh>
#include <TStyle.h>

using namespace std;

TRandom3 *ran = new TRandom3;

bool db = false;

int NBOARDS = 8;
int NSTRIPS = 512;
  
double xlow = 0.;
double xhigh = 200.;
double ylow = 0.;
double yhigh = 217.9;

int bc_wind = 7;
double mm_eff[8] = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9}; // i apologize for this array

double sig_art = 32.;

// road size

int XROAD = 64;
int UVFACTOR = 1;
int UVROAD = XROAD*UVFACTOR;

struct slope_t {
  int count;
  double mxl;
};


int strip(int bo, int vmm, int ch){
  if (bo == 0 || bo == 3 || bo ==  5 || bo == 6)
    return 513-(vmm*64+ch);
  else
    return vmm*64+ch;
}

vector<Road*> create_roads(const GeoOctuplet& geometry){
  if (NSTRIPS % XROAD != 0)
    cout << "Not divisible!" << endl;
  int nroad = NSTRIPS/XROAD;
  vector<Road*> m_roads;
  for (int i = 0; i < nroad; i++){
    Road* myroad = nullptr;
    myroad = new Road(i, geometry);
    m_roads.push_back(myroad);
  }
  return m_roads;
}

vector<slope_t> finder(vector<Hit*> hits, vector<Road*> roads){
  int ntrigs = 0;
  int bc_start = 999999;
  int bc_end = -1;

  vector<slope_t> slopes;

  for (int i=0; i < hits.size(); i++){
    if (hits[i]->Age() < bc_start)
      bc_start = hits[i]->Age();
    if (hits[i]->Age() > bc_end)
      bc_end = hits[i]->Age();
  }
  bc_start = bc_start - bc_wind*2;
  bc_end = bc_end + bc_wind*2;


  // each road makes independent triggers
  for (int i = 0; i < roads.size(); i++){

    roads[i]->Reset();

    vector<Hit*> hits_now;

    for (int bc = bc_start; bc < bc_end; bc++){
      hits_now.clear();
      roads[i]->Increment_Age(bc_wind);

      for (int j = 0; j < hits.size(); j++){
        // BC window
        if (hits[j]->Age() == bc){
          // add into hits_now so that it is sorted by strip number
          bool added_hit = false;
          for (int k = 0; k < hits_now.size(); k++){
            if (hits_now[k]->Channel() > hits[j]->Channel()){
              hits_now.insert(hits_now.begin()+k,hits[j]);
              added_hit = true;
              break;
            }
          }
          if (!added_hit)
            hits_now.push_back(hits[j]);
        }
      }
      if (hits_now.size() > 0 && db){
        cout << "bunch crossing: " << bc << endl;
        cout << "nhits to be added: "<<hits_now.size() << endl;
      }
      roads[i]->Add_Hits(hits_now, XROAD, UVFACTOR);

      if (roads[i]->Coincidence(bc_wind)){
        cout << "---------------------------" << endl;
        cout << "FOUND COINCIDENCE @ BC " << bc << endl;
        cout << "Road (i,count): ("<< roads[i]->iRoad() <<", " << roads[i]->Count()<<")" << endl;
        cout << "---------------------------" << endl;
        for (int k = 0; k < roads[i]->Hits().size(); k++){
          printf("Hit (board, BC, channel*pitch): (%d,%d,%4.4f)\n",roads[i]->Hits()[k].MMFE8Index(),roads[i]->Hits()[k].Age(),roads[i]->Hits()[k].Channel()*0.4);
        }
        ntrigs++;
        slope_t m_slope;
        m_slope.count = roads[i]->Count();
        m_slope.mxl = roads[i]->Mxl();
        slopes.push_back(m_slope);
      }
    }
  }
  return slopes;
}

void setstyle(){
  gROOT->SetBatch();
  gStyle->SetOptStat(0);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadRightMargin(0.13);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPaintTextFormat(".2f");
  gStyle->SetTextFont(42);
  //  gStyle->SetOptFit(kTRUE);
}

int main() {


  GeoOctuplet* GEOMETRY = new GeoOctuplet(false,0.,0.);


  // RUN 3522 check

  // GBT event 335
  vector<Hit*> hits;
  
  Hit* newhit1 = nullptr;
  newhit1 = new Hit(4, 201, strip(4,5,58), false, *GEOMETRY);
  hits.push_back(newhit1);
  Hit* newhit2 = nullptr;
  newhit2 = new Hit(4, 209, strip(4,5,64), false, *GEOMETRY);
  hits.push_back(newhit2);
  Hit* newhit3 = nullptr;
  newhit3 = new Hit(5, 203, strip(5,2,30), false, *GEOMETRY);
  hits.push_back(newhit3);
  Hit* newhit4 = nullptr;
  newhit4 = new Hit(0, 203, strip(0,0,22), false, *GEOMETRY);
  hits.push_back(newhit4);
  Hit* newhit5 = nullptr;
  newhit5 = new Hit(2, 203, strip(2,7,14), false, *GEOMETRY);
  hits.push_back(newhit5);
  Hit* newhit6 = nullptr;
  newhit6 = new Hit(2, 210, strip(2,7,17), false, *GEOMETRY);
  hits.push_back(newhit6);
  Hit* newhit7 = nullptr;
  newhit7 = new Hit(7, 204, strip(7,5,5), false, *GEOMETRY);
  hits.push_back(newhit7);
  Hit* newhit8 = nullptr;
  newhit8 = new Hit(1, 204, strip(1,7,26), false, *GEOMETRY);
  hits.push_back(newhit8);
  Hit* newhit9 = nullptr;
  newhit9 = new Hit(6, 206, strip(6,2,42), false, *GEOMETRY);
  hits.push_back(newhit9);
  Hit* newhit10 = nullptr;
  newhit10 = new Hit(0, 206, strip(0,0,24), false, *GEOMETRY);
  hits.push_back(newhit10);
  
  vector<Road*> m_roads = create_roads(*GEOMETRY);
  
  vector<slope_t> m_slopes = finder(hits, m_roads);
  int ntrigroads = m_slopes.size();
  if (db)
    cout << "Ntriggered roads: " << ntrigroads << endl;
  slope_t myslope;
  myslope.mxl = 0.;
  myslope.count = 0;
  int myslopecount = 0;
  for (int j = 0; j < m_slopes.size(); j++){
    if (m_slopes[j].count > myslope.count){
      myslope.count = m_slopes[j].count;
      myslope.mxl = m_slopes[j].mxl;
    }
  }

  cout << "my slope: " << myslope.mxl << endl;

  // GBT event 365                                                                                                                                                                          

  hits.clear();

  newhit1 = new Hit(7, 2238, 2*64+6, false, *GEOMETRY);
  hits.push_back(newhit1);
  newhit2 = new Hit(7, 2242, 2*64+4, false, *GEOMETRY);
  hits.push_back(newhit2);
  newhit3 = new Hit(6, 2239, 513-(5*64+52), false, *GEOMETRY);
  hits.push_back(newhit3);
  newhit4 = new Hit(6, 2243, 513-(5*64+53), false, *GEOMETRY);
  hits.push_back(newhit4);
  newhit5 = new Hit(5, 2240, 513-(5*64+50), false, *GEOMETRY);
  hits.push_back(newhit5);
  newhit6 = new Hit(4, 2244, (2*64+36), false, *GEOMETRY);
  hits.push_back(newhit6);
  newhit7 = new Hit(3, 2242, 513-(5*64+11), false, *GEOMETRY);
  hits.push_back(newhit7);
  newhit8 = new Hit(2, 2240, (3*64+9), false, *GEOMETRY);
  hits.push_back(newhit8);
  newhit9 = new Hit(2, 2243, (3*64+11), false, *GEOMETRY);
  hits.push_back(newhit9);
  newhit10 = new Hit(1, 2241, (3*64+11), false, *GEOMETRY);
  hits.push_back(newhit10);
  Hit* newhit11 = nullptr;
  newhit11 = new Hit(0, 2240, 513-(4*64+46), false, *GEOMETRY);
  hits.push_back(newhit11);
  Hit* newhit12 = nullptr;
  newhit12 = new Hit(0, 2244, 513-(4*64+47), false, *GEOMETRY);
  hits.push_back(newhit12);
  Hit* newhit13 = nullptr;
  newhit13 = new Hit(0, 2246, 513-(4*64+44), false, *GEOMETRY);
  hits.push_back(newhit13);
  


  m_roads = create_roads(*GEOMETRY);
  
  m_slopes = finder(hits, m_roads);
  ntrigroads = m_slopes.size();
  if (db)
    cout << "Ntriggered roads: " << ntrigroads << endl;
  myslope.mxl = 0.;
  myslope.count = 0;
  for (int j = 0; j < m_slopes.size(); j++){
    if (m_slopes[j].count > myslope.count){
      myslope.count = m_slopes[j].count;
      myslope.mxl = m_slopes[j].mxl;
    }
  }
  cout << "my slope: " << myslope.mxl << endl;

  hits.clear();

  // GBT event 368
  newhit1 = new Hit(7, 1368, strip(7,5,36), false, *GEOMETRY);
  hits.push_back(newhit1);
  newhit2 = new Hit(6, 1368, strip(6,2,20), false, *GEOMETRY);
  hits.push_back(newhit2);
  newhit3 = new Hit(6, 1375, strip(6,2,17), false, *GEOMETRY);
  hits.push_back(newhit3);
  newhit4 = new Hit(5, 1366, strip(5,2,23), false, *GEOMETRY);
  hits.push_back(newhit4);
  newhit5 = new Hit(5, 1373, strip(5,2,26), false, *GEOMETRY);
  hits.push_back(newhit5);
  newhit6 = new Hit(4, 1367, strip(4,6,11), false, *GEOMETRY);
  hits.push_back(newhit6);
  newhit7 = new Hit(4, 1371, strip(4,6,12), false, *GEOMETRY);
  hits.push_back(newhit7);
  newhit8 = new Hit(2, 1368, strip(2,6,56), false, *GEOMETRY);
  hits.push_back(newhit8);
  newhit9 = new Hit(1, 1370, strip(1,6,51), false, *GEOMETRY);
  hits.push_back(newhit9);
  newhit10 = new Hit(1, 1373, strip(1,6,52), false, *GEOMETRY);
  hits.push_back(newhit10);
  newhit11 = new Hit(0, 1374, strip(0,1,3), false, *GEOMETRY);
  hits.push_back(newhit11);
  newhit12 = new Hit(0, 1369, strip(0,1,5), false, *GEOMETRY);
  hits.push_back(newhit12);

  m_roads = create_roads(*GEOMETRY);
  
  m_slopes = finder(hits, m_roads);
  ntrigroads = m_slopes.size();
  if (db)
    cout << "Ntriggered roads: " << ntrigroads << endl;
  myslope.mxl = 0.;
  myslope.count = 0;
  for (int j = 0; j < m_slopes.size(); j++){
    if (m_slopes[j].count > myslope.count){
      myslope.count = m_slopes[j].count;
      myslope.mxl = m_slopes[j].mxl;
    }
  }
  cout << "my slope: " << myslope.mxl << endl;

  return 0;
}

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
#include <time.h>

// ROOT includes
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TLatex.h>
#include <TMath.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TGraph.h>
#include <GeoOctuplet.hh>
#include <Hit.hh>
#include <Road.hh>
#include <TStyle.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <time.h>
#include "VectorDict.cxx"

using namespace std;

TRandom3 *ran = new TRandom3(time(NULL));


bool db = false; // debug output flag

// SOME CONSTANTS

int NPLANES = 8;
int NSTRIPS;
int ADDC_BUFFER = 8;
double xlow, xhigh, ylow, yhigh; // chamber dimensions
double mu_xlow, mu_xhigh, mu_ylow, mu_yhigh; // active chamber area to decouple edge effects

int XROAD, UVFACTOR;

int NSTRIPS_UP_UV, NSTRIPS_DN_UV;
int NSTRIPS_UP_XX, NSTRIPS_DN_XX;

int bc_wind;
int sig_art;

double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));

// colors
string pink = "\033[38;5;205m";
string green = "\033[38;5;84m";
string blue = "\033[38;5;27m";
string ending = "\033[0m";
string warning = "\033[38;5;227;48;5;232m";

struct slope_t {
  int count;
  int iroad;
  int iroadu;
  int iroadv;
  int imuonhits;
  int uvbkg;
  int xbkg;
  int xmuon;
  int age; //WRT first muon hit BC
  double mxl;
  double xavg;
  double yavg;
  double xcenter;
  double ycenter;
  vector<Hit> slopehits;
};

bool compare_age(Hit* a, Hit* b){
  return (a->Age() < b->Age());
}
bool compare_channel(Hit* a, Hit* b){
  return (a->Channel() < b->Channel());
}
bool compare_slope(slope_t a, slope_t b){
  return (a.iroad < b.iroad);
}
bool compare_second(std::pair<int, double> a, std::pair<int, double> b){
  return (a.second < b.second);
}

void set_chamber(string chamber, int m_wind, int m_sig_art, int m_xroad, bool uvrflag, bool trapflag, int m_nstrips){
  // function to set parameters in a smart way

  if (chamber == "small"){
    if (m_nstrips == -1){
      NSTRIPS = 8800; // has to be multiple of x road
    }
    else {
      NSTRIPS = m_nstrips;
    }
    xlow = 0.;
    xhigh = NSTRIPS*0.4-0.2;
    ylow = 0.;
    yhigh = trapflag ? 1821. : 500.;
  }
  else if (chamber == "large"){
    if (m_nstrips == -1){
      NSTRIPS = 8800; // has to be multiple of x road
    }
    else {
      NSTRIPS = m_nstrips;
    }
    xlow = 0.;
    xhigh = NSTRIPS*0.4-0.2;
    ylow = 0.;
    yhigh = 2200.;
  }
  else if (chamber == "oct"){
    if (m_nstrips == -1){
      NSTRIPS = 512; // has to be multiple of x road
    }
    else {
      NSTRIPS = m_nstrips;
    }
    xlow = 0.;
    xhigh = NSTRIPS*0.4-0.2;
    ylow = 17.9;
    yhigh = 217.9;
  }
  else {
    exit (EXIT_FAILURE);
  }

  // active area
  mu_xlow = 100*0.4+0.2;
  mu_xhigh = NSTRIPS*0.4-0.2-100*0.4;

  mu_ylow = ylow;
  mu_yhigh = yhigh;

  bc_wind = m_wind;

  sig_art = m_sig_art;

  XROAD = m_xroad;
  UVFACTOR = round((yhigh-ylow)/(B * 0.4 * 2)/XROAD);

  if (!uvrflag){
    // this is for 8 strip x-roads, i think
    NSTRIPS_UP_UV = UVFACTOR*XROAD+NSTRIPS_UP_XX;
    NSTRIPS_DN_UV = UVFACTOR*XROAD;
    NSTRIPS_UP_XX = 4;
    NSTRIPS_DN_XX = 0;
    // NSTRIPS_UP_UV = 80;
    // NSTRIPS_DN_UV = 80;
    // NSTRIPS_UP_XX = 8;
    // NSTRIPS_DN_XX = 8;
  }
  else{
    // NSTRIPS_UP_UV = 0;
    // NSTRIPS_DN_UV = 0;
    // NSTRIPS_UP_XX = 0;
    // NSTRIPS_DN_XX = 0;
    NSTRIPS_UP_UV = 4;
    NSTRIPS_DN_UV = 0;
    NSTRIPS_UP_XX = 4;
    NSTRIPS_DN_XX = 0;
  }
}

int inside_trapezoid(double x, double y, double inner_radius, double outer_radius, double length, double base_width, double top_width) {
  double slope  = (outer_radius - inner_radius) / ((top_width - base_width) / 2.0);
  double offset = inner_radius - (slope * base_width / 2.0);
  if (x > outer_radius)          return 0; // top
  if (x < inner_radius)          return 0; // bottom
  if (y >  (x - offset) / slope) return 0; // right
  if (y < -(x - offset) / slope) return 0; // left
  return 1;
}

int fiducial(double x, double y, string chamber) {

  if (chamber != "large" && chamber != "small"){
    cerr << "fiducial doesnt understand this chamber: " << chamber << endl;
    return -1;
  }
  int large = (chamber == "large");

  double NSW_MM1_InnerRadius = 0; // chambers starts at 0 // large ? 923.0  : 895.0;
  double NSW_MM1_Length      = large ? 2310.0 : 2210.0;
  double NSW_MM2_Length      = large ? 1410.0 : 1350.0;
  double NSW_MM1_baseWidth   = large ? 640.0  : 500.0;
  double NSW_MM1_topWidth    = large ? 2008.5 : 1319.2;
  double NSW_MM2_baseWidth   = large ? 2022.8 : 1321.1;
  double NSW_MM2_topWidth    = large ? 2220.0 : 1821.5;
  double NSW_MM1_outerRadius = NSW_MM1_InnerRadius + NSW_MM1_Length;
  double NSW_MM2_InnerRadius = NSW_MM1_outerRadius;
  double NSW_MM2_outerRadius = NSW_MM2_InnerRadius + NSW_MM2_Length;

  if (inside_trapezoid(x, y - (yhigh+ylow)/2.0,
                       NSW_MM1_InnerRadius,
                       NSW_MM1_outerRadius,
                       NSW_MM1_Length,
                       NSW_MM1_baseWidth,
                       NSW_MM1_topWidth)) return 1;
  if (inside_trapezoid(x, y - (yhigh+ylow)/2.0,
                       NSW_MM2_InnerRadius,
                       NSW_MM2_outerRadius,
                       NSW_MM2_Length,
                       NSW_MM2_baseWidth,
                       NSW_MM2_topWidth)) return 1;
  return 0;
}

tuple<double,double> cosmic_angle(){
  return make_tuple(0.,0.);
}

vector<Road*> create_roads(const GeoOctuplet& geometry, bool uvrflag, int m_xthr, int m_uvthr, string chamber, bool trapflag){
  int nroad = NSTRIPS/XROAD;
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

  if (trapflag){
    vector<Road*> m_roads_fiducial;
    for (auto road: m_roads){
      for (int corner = 0; corner <= 3; corner++){
        double x, y;
        std::tie(x, y) = road->CornerXY(corner, XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV);
        if (fiducial(x, y, chamber)){
          m_roads_fiducial.push_back(road);
          break;
        }
      }
    }
    return m_roads_fiducial;
  }
  else
    return m_roads;
}

double predicted_rate(int strip, string chamber) {

  if (chamber != "large" && chamber != "small"){
    cerr << "predicted_rate doesnt understand this chamber: " << chamber << endl;
    return -1.0;
  }
  int large = (chamber == "large");
  double pitch = 0.4;
  double offset = large ? 923.0 : 895.0;
  double r = offset + pitch*(double)(strip);

  // mm->cm
  r = r/10;

  double rate = 0.0;
  if (large) rate = (-9.938824) + (6288.351422)/r +  (45942.902843)/pow(r, 2);
  else       rate = (-5.018321) + (3396.877744)/r + (164524.202988)/pow(r, 2);

  // kHz->Hz
  return rate*1000;
}

tuple<double,double> generate_muon(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos, string chamber, bool trapflag){

  double x = 9e9;
  double y = 9e9;

  if (trapflag) {
    while (!fiducial(x, y, chamber)){
      x = ran->Uniform(mu_xlow,mu_xhigh);
      y = ran->Uniform(mu_ylow,mu_yhigh);
    }
  }
  else {
    x = ran->Uniform(mu_xlow,mu_xhigh);
    y = ran->Uniform(mu_ylow,mu_yhigh);
  }
  
  double thx, thy;

  std::tie(thx,thy) = cosmic_angle();

  double avgz = 0.5*(zpos[0]+zpos[NPLANES-1]);
  double x_b, y_b;
  // double z, x_b, y_b;
  for ( int j = 0; j < NPLANES; j++){
    // z = zpos[j];
    x_b = TMath::Tan(thx)*(zpos[j]-avgz)+x;
    y_b = TMath::Tan(thy)*(zpos[j]-avgz)+y;
    xpos[j] = x_b;
    ypos[j] = y_b;
  }    
  return make_tuple(x,y);
}

vector<Hit*> generate_bkg(int start_bc, const GeoOctuplet& geometry, int bkgrate, string chamber){

  vector<Hit*> bkghits;

  int noise_window = bc_wind * 5;
  int start_noise = start_bc - bc_wind * 2; // this takes into account overwriting tp hits for a plane+road
  //int start_noise = start_bc - bc_wind;

  // int end_noise = start_noise + noise_window - 1;
  // int end_noise = start_bc + bc_wind * 2 -1;

  //assume uniform distribution of background - correct for noise
  double time_window = noise_window * 25*pow(10,-9);
  double bkg_prob = bkgrate * time_window;
  for ( int j = 0; j < NPLANES; j++){
    //int nbkg = expbkg;
    for ( int k = 0; k < NSTRIPS; k++){
      double prob = ran->Uniform(0,1.);
      if (bkgrate == -1)
        bkg_prob = predicted_rate(k, chamber) * time_window;
      if (prob < bkg_prob){
        Hit* newhit = nullptr;
        newhit = new Hit(j, start_noise+ran->Integer(noise_window), k, true, geometry);
        bkghits.push_back(newhit);
      }
    }
  }
  return bkghits;
}

vector<int> oct_response(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos, vector<double> & mm_eff){
  //gives detector response to muon, returns list of which planes registered hit
  
  int n_mm = 0;
  vector<int> oct_hitmask(NPLANES,0);
  for ( int j=0; j < NPLANES; j++){
    if (ran->Uniform(0.,1.) < mm_eff[j]){
      oct_hitmask[j] = 1;
      n_mm++;
    }
  }
  return oct_hitmask;
}

tuple<int, vector < slope_t> > finder(vector<Hit*> hits, int mu_firstbc, vector<Road*> roads, bool saveHits, bool ideal_vmm, bool ideal_addc, bool ideal_tp, int evt){

  // applies the MMTP finder to a series of hits and roads
  // returns slope structs for roads which found a coincidence and have at least 1 real muon hit

  int ntrigs = 0;
  int bc_start = 999999;
  int bc_end = -1;
  unsigned int ibc = 0;

  vector<slope_t> slopes = {};

  if (hits.size() == 0)
    return make_tuple(ntrigs, slopes);

  std::sort(hits.begin(), hits.end(), compare_age);
  bc_start = hits.front()->Age();
  bc_end   = hits.back()->Age();
  bc_start = bc_start - bc_wind*2;
  bc_end   = bc_end   + bc_wind*2;

  // // silly hack to look only in one window                                                                        
  // bc_start = hits.front()->Age();
  // bc_end   = mu_firstbc;
  // bc_end   = mu_firstbc + 16;

  // setup the roads
  for (unsigned int i = 0; i < roads.size(); i++)
    roads[i]->Reset();
  vector<Hit*> hits_now = {};
  vector<int> vmm_same  = {};
  vector< pair<int, double> > addc_same = {};
  vector<int> to_erase = {};
  int n_vmm  = NSTRIPS/64;
  int n_addc = NSTRIPS/2048;

  // each road makes independent triggers, evaluated on each BC
  for (int bc = bc_start; bc < bc_end; bc++){

    hits_now.clear();

    for (unsigned int j = ibc; j < hits.size(); j++){
      if (hits[j]->Age() == bc){
        hits_now.push_back(hits[j]);
      }
      else if (hits[j]->Age() > bc){
        ibc = j;
        break;
      }
    }

    //implement vmm ART-like signal
    for (int ib = 0; ib < NPLANES; ib++){
      for (int j = 0; j < n_vmm; j++){
        vmm_same.clear();
        // save indices of all elements in vmm j
        for (unsigned int k = 0; k < hits_now.size(); k++){
          if (hits_now[k]->MMFE8Index() != ib)
            continue;
          if (hits_now[k]->VMM() == j){
            vmm_same.push_back(k);
          }
        }
        // if 2+ hits in same vmm, erase all except 1 randomly
        if (vmm_same.size() > 1){
          int the_chosen_one = ran->Integer((int)(vmm_same.size()));
          
          if (ideal_vmm)
            for (unsigned int k = 0; k < vmm_same.size(); k++)
              if (hits_now[vmm_same[k]]->IsReal()){
                the_chosen_one = k;
                break;
              }
          
          for (int k = vmm_same.size()-1; k > -1; k--)
            if (k != the_chosen_one)
              hits_now.erase(hits_now.begin()+vmm_same[k]);
        }
      }

      // implement ADDC-like handling
      for (int ia = 0; ia < n_addc; ia++){

        // collect all hits on one ADDC
        addc_same.clear();
        for (unsigned int k = 0; k < hits_now.size(); k++)
          if (hits_now[k]->MMFE8Index() == ib && hits_now[k]->ADDC() == ia)
            addc_same.push_back( std::make_pair(k, hits_now[k]->Channel()) );
        
        if ((int)(addc_same.size()) <= ADDC_BUFFER)
          continue;

        // priority encode the hits by channel number; remember hits 8+
        to_erase.clear();
        std::sort(addc_same.begin(), addc_same.end(), compare_second);
        for (int it = ADDC_BUFFER; it < (int)(addc_same.size()); it++)
          to_erase.push_back(addc_same[it].first);

        // reverse and erase
        std::sort(to_erase.rbegin(), to_erase.rend()); 
        for (auto k: to_erase)
          if (ideal_addc && hits_now[k]->IsReal())
            continue;
          else
            hits_now.erase(hits_now.begin() + k);
        
      }
    }

    for (unsigned int i = 0; i < roads.size(); i++){

      roads[i]->Increment_Age(bc_wind);
      roads[i]->Add_Hits(hits_now, XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV, ideal_tp);

      //if (roads[i]->Coincidence(bc_wind) && bc == (mu_firstbc - 1)){
      if (roads[i]->Coincidence(bc_wind) && bc >= (mu_firstbc - 1)){
	//if (roads[i]->Coincidence(bc_wind)){
        if (db){
          cout << "---------------------------" << endl;
          cout << "FOUND COINCIDENCE @ BC " << bc << endl;
          cout << "Road (i,count): ("<< roads[i]->iRoad() <<", " << roads[i]->Count()<<")" << endl;
          cout << "---------------------------" << endl;
          for (unsigned int k = 0; k < roads[i]->Hits().size(); k++){
            printf("Hit (board, BC, strip): (%d,%d,%4.4f)", roads[i]->Hits()[k].MMFE8Index(),roads[i]->Hits()[k].Age(),roads[i]->Hits()[k].Channel());
            cout << endl;
          }
        }
        ntrigs++;

        int nmuonhits = roads[i]->NMuon();
        double xcenter = 0;
        double ycenter = 0;
        std::tie(xcenter, ycenter) = roads[i]->Center(XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV);

//         if (nmuonhits < 1)
//           continue;

        slope_t m_slope;

        m_slope.count = roads[i]->Count();
        m_slope.iroad = roads[i]->iRoad();
        m_slope.iroadu = roads[i]->iRoadu();
        m_slope.iroadv = roads[i]->iRoadv();
        m_slope.imuonhits = nmuonhits;
        m_slope.uvbkg = roads[i]->UV_bkg();
        m_slope.xbkg = roads[i]->X_bkg();
        m_slope.xmuon = roads[i]->X_muon();
        m_slope.age = bc-mu_firstbc;
        m_slope.mxl = roads[i]->Mxl();
        m_slope.xavg = roads[i]->AvgXofX();
        m_slope.yavg = -B*( roads[i]->AvgXofU() - roads[i]->AvgXofV() + (roads[i]->AvgZofV()-roads[i]->AvgZofU())*roads[i]->Mxl() ) / 2 + yhigh;
        m_slope.xcenter = xcenter;
        m_slope.ycenter = ycenter;
        if (saveHits)
          m_slope.slopehits = roads[i]->Hits();
        slopes.push_back(m_slope);
      }
    }
  }
  return make_tuple(ntrigs,slopes);
}

void progress(double time_diff, int nprocessed, int ntotal){
  // PROGRESS BAR (alex)
  double rate = (double)(nprocessed+1)/time_diff;
  std::cout.precision(1);
  std::cout << "\r > " << nprocessed << " / " << ntotal 
            << " | "   << std::fixed << 100*(double)(nprocessed)/(double)(ntotal) << "%"
            << " | "   << std::fixed << rate << "Hz"
            << " | "   << std::fixed << time_diff/60 << "m elapsed"
            << " | "   << std::fixed << (double)(ntotal-nprocessed)/(rate*60) << "m remaining"
            << std::flush;
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

void plttrk(vector<Hit> hits, bool xflag, TString title, int ntrig, TFile * file){
  vector <double> pts;
  vector <double> zpts;
  vector <int> ib,bc;
  vector <double> ch;
  vector <double> bkgpts;
  vector <double> bkgzpts;
  vector <int> bkgib,bkgbc;
  vector <double> bkgch;

  Double_t zpos[8] = {0., 11.2, 32.4, 43.6,
                      113.6, 124.8, 146.0, 157.2};

  for (unsigned int i = 0; i < hits.size(); i++){
    if (hits[i].IsNoise() == false){
      ib.push_back(hits[i].MMFE8Index());
      if (xflag){
        pts.push_back(hits[i].x_pos_at_end());
      }
      else{
        pts.push_back(hits[i].y_pos());
      }
      zpts.push_back(zpos[hits[i].MMFE8Index()]);
      ch.push_back(hits[i].Channel());
      bc.push_back(hits[i].BC());
    }
    else{
      bkgib.push_back(hits[i].MMFE8Index());
      if (xflag){
        bkgpts.push_back(hits[i].x_pos_at_end());
      }
      else{
        bkgpts.push_back(hits[i].y_pos());
      }
      bkgzpts.push_back(zpos[hits[i].MMFE8Index()]);
      bkgch.push_back(hits[i].Channel());
      bkgbc.push_back(hits[i].BC());
    }
  }
  setstyle();
  TCanvas * c1 = new TCanvas("c1", "canvas", 800, 800);
  c1->cd();
  TMultiGraph * mg = new TMultiGraph();
  TLegend* leg = new TLegend(0.6,0.45,0.74,0.55);
  //define lines for planes
  vector<TGraph*> planes = {};
  
  for ( int k=0; k < NPLANES; k++){
      Double_t board_x[2];
      Double_t board_z[2];      
      if (xflag){
        if (bkgpts.size() > 0){
          double min_ptx = *min_element(pts.begin(),pts.end());
          double max_ptx = *max_element(pts.begin(),pts.end());
            board_x[0] = min(*min_element(bkgpts.begin(),bkgpts.end()),min_ptx)-10.;
          board_x[1] = max(*max_element(bkgpts.begin(),bkgpts.end()),max_ptx)+10;
        }
        else{
         board_x[0] = xlow;
         board_x[1] = xhigh;
        }
      }
      else {
        board_x[0] = ylow;
        board_x[1] = yhigh;
      }
      board_z[0]=zpos[k];
      board_z[1]=zpos[k];
      
      TGraph * g1 = new TGraph(2, board_x, board_z);
      planes.push_back(g1);
      planes[k]->SetLineColor(kBlue);
    }
    TGraph * gr = new TGraph(zpts.size(), &pts[0], &zpts[0]);
    gr->SetTitle("Cluster locations");
    gr->SetMarkerColor(kPink+6);
    gr->SetMarkerStyle(34);
    gr->SetMarkerSize(1.4);
    leg->AddEntry(gr,"muon","p");
    for (unsigned int j = 0; j < zpts.size(); j++){
      TLatex *latex = new TLatex(gr->GetX()[j], gr->GetY()[j], Form("%6.f, %d",ch[j],bc[j]));
      latex->SetTextSize(0.03);
      gr->GetListOfFunctions()->Add(latex);
    }
    TGraph * grbkg = new TGraph(bkgzpts.size(), &bkgpts[0], &bkgzpts[0]);
    grbkg->SetTitle("");
    grbkg->SetMarkerColor(kCyan-8);
    grbkg->SetMarkerStyle(20);
    grbkg->SetMarkerSize(1.);
    leg->AddEntry(grbkg, "bkgd.","p");
    for (unsigned int j = 0; j < bkgzpts.size(); j++){
      TLatex *latex = new TLatex(grbkg->GetX()[j], grbkg->GetY()[j], Form("%6.f, %d",bkgch[j],bkgbc[j]));
      latex->SetTextSize(0.03);
      grbkg->GetListOfFunctions()->Add(latex);
    }

    if (zpts.size()> 0)
      mg->Add(gr,"p");
    if (bkgzpts.size()> 0)
      mg->Add(grbkg,"p");
    for ( int k = 0; k < NPLANES; k++){
      mg->Add(planes[k], "l");
    }
    

    //    mg->SetTitle("Road with the most hits");
    mg->Draw("a");
    mg->GetXaxis()->SetTitleOffset(1.);
    mg->GetYaxis()->SetTitleOffset(1.4);
    mg->GetYaxis()->SetTitle("z (mm)");
    leg->Draw();
    
    TLatex* l1 = new TLatex();
    l1->SetTextSize(0.03);
    l1->SetTextColor(kBlack);
    l1->SetTextAlign(21);
    l1->SetNDC();
    //    l1->DrawLatex(0.4,0.5,Form("ntrig: %d for %d (x), ",ntrig, XROAD));
    //l1->DrawLatex(0.4,0.45,Form("+/- %d neighbors (uv)",UVFACTOR));
  
  
  if (xflag){
    mg->GetXaxis()->SetTitle("x (mm)");
  }
  else{
    mg->GetXaxis()->SetTitle("y (mm)");
  }
  TString filename = title;
  filename.Append(".pdf");
  c1->Print(filename);
  file->cd();
  file->cd("event_displays");
  c1->Write();
}

int main(int argc, char* argv[]) {

  int nevents = -1;
  int bkgrate = 0; // Hz per strip

  int m_xroad = 8; 
  int m_NSTRIPS = -1;

  int m_bcwind = 8;
  int m_sig_art = 32;

  int m_sig_art_x = 1; // smear ART position, in strips

  vector<double> mm_eff = {1., 1., 1., 1., 1., 1., 1., 1.};
  double chamber_eff = -1;

  string histograms = "histograms";

  // coincidence params
  int m_xthr = 2;
  int m_uvthr = 2;

  bool bkgflag = false;
  bool pltflag = false;
  bool uvrflag = false;
  bool trapflag = false;
  bool ideal_tp   = false;
  bool ideal_vmm  = false;
  bool ideal_addc = false;
  bool write_tree = false;
  bool bkgonly = false;
  bool smear_art = false;

  char outputFileName[400];
  char chamberType[400];

  if ( argc < 3 ){
    cout << "Error at Input: please specify number of events to generate "<< endl;
    cout << "Example:   ./sim -n 100 -ch <chamber type> -o output.root" << endl;
    cout << "Example:   ./sim -n 100 -ch <chamber type> -b <bkg rate in kHz/strip> -o output.root" << endl;
    cout << "Example:   ./sim -n 100 -ch <chamber type> -b <bkg rate in Hz/strip> -p <make event displays> -o output.root" << endl;
    cout << "Other options include: -w <bc_wind> -sig <art res (ns)>" << endl;
    cout << "If art res = 0, then we do bkg only" << endl;
    return 0;
  }

  bool b_out = false;
  bool ch_type = false;

  for (int i=1; i<argc; i++){
    if (strncmp(argv[i],"-n",2)==0){
      nevents = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"-o",2)==0){
      sscanf(argv[i+1],"%s", outputFileName);
      b_out = true;
    }
    if (strncmp(argv[i],"-x",2)==0){
      m_xroad = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"-w",2)==0){
      m_bcwind = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"-thrx",5)==0){
      m_xthr = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"-thruv",6)==0){
      m_uvthr = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"--trap",6)==0){
      trapflag = true;
    }
    if (strncmp(argv[i],"-ch",3)==0){
      sscanf(argv[i+1],"%s", chamberType);
      ch_type = true;
    }
    if (strncmp(argv[i],"-sig",4)==0){
      m_sig_art = atoi(argv[i+1]);
      if (m_sig_art == 0)
        bkgonly = true;
    }
    if (strncmp(argv[i],"-b",2)==0){
      bkgrate = atoi(argv[i+1]);
      bkgflag = true;
    }
    if (strncmp(argv[i],"-p",2)==0){
      pltflag = true;
    }
    if (strncmp(argv[i],"-uvr",4)==0){
      uvrflag = true;
    }
    if (strncmp(argv[i],"-hdir",4)==0){
      histograms = argv[i+1];
    }
    if (strncmp(argv[i],"-e",2)==0){
      chamber_eff = atof(argv[i+1]);
      for (unsigned int i = 0; i < mm_eff.size(); i++)
        mm_eff[i] = chamber_eff;
    }
    if (strncmp(argv[i],"-ideal-vmm", 10)==0){
      ideal_vmm = true;
    }
    if (strncmp(argv[i],"-ideal-addc", 11)==0){
      ideal_addc = true;
    }
    if (strncmp(argv[i],"-ideal-tp", 9)==0){
      ideal_tp = true;
    }
    if (strncmp(argv[i],"-seed", 5)==0){
      ran->SetSeed( atoi(argv[i+1]) );
    }
    if (strncmp(argv[i],"-tree", 5)==0){
      write_tree = true;
    }
    if (strncmp(argv[i],"-strips", 7)==0){
      m_NSTRIPS = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"-smear",6)==0){
      smear_art = true;
    }
  }
  if (!b_out){
    cout << "Error at Input: please specify output file (-o flag)" << endl;
    return 0;
  }

  if (!ch_type){
    cout << "Error at Input: please specify chamber type (-ch flag, options: large, small, oct)" << endl;
    return 0;
  }

  if (nevents == -1){
    cout << "Didn't set the number of generated events! Exiting." << endl;
    return 0;
  }

  set_chamber( string(chamberType), m_bcwind, m_sig_art, m_xroad, uvrflag, trapflag, m_NSTRIPS);
  
  if (NSTRIPS % XROAD != 0) {
    cout << "Number of strips not divisible by the road size!" << endl;
    return 0;
  }
  
  if ( ( (mu_xlow || mu_xhigh) < xlow) || ( (mu_xlow || mu_xhigh) > xhigh) || (mu_xlow > mu_xhigh) ){
    cout << "Muon active area is outside the chamber area!" << endl;
    return 0;
  } 


  cout << endl;
  cout << blue << "--------------" << ending << endl;
  cout << blue << "OCT SIM ✪ ‿ ✪ " << ending << endl;
  cout << blue << "--------------" << ending << endl;
  cout << endl;
  cout << endl;
  printf("\r >> plot flag: %s", pltflag ? "true" : "false");
  cout << endl;
  printf("\r >> bkgonly flag: %s", bkgonly ? "true" : "false");
  cout << endl;
  printf("\r >> smear art position: %s", smear_art ? "true" : "false");
  cout << endl;
  printf("\r >> x-road size (in strips): %d, +/- neighbor roads (uv): %d", XROAD, UVFACTOR);
  cout << endl;
  printf("\r >> art res (in ns): %d", m_sig_art);
  cout << endl;
  cout << "\r >> Using BCID window: " << bc_wind << endl;
  printf("\r >> Background rate of %d Hz per strip",bkgrate);
  cout << endl;
  printf("\r >> Assuming chamber size: (%4.1f,%4.1f) in mm",xhigh-xlow, yhigh-ylow);
  cout << endl;
  printf("\r >> Assuming muon active area: (%4.1f,%4.1f) in mm",mu_xhigh-mu_xlow, mu_yhigh-mu_ylow);
  cout << endl;
  printf("\r >> Using UV roads: %s", (uvrflag) ? "true" : "false");
  cout << endl;
  printf("\r >> Using trapezoidal geometry: %s", (trapflag) ? "true" : "false");
  cout << endl;
  printf("\r >> Using thresholds (x, uv): (%d, %d)", m_xthr, m_uvthr);
  cout << endl;
  for (unsigned int i = 0; i < mm_eff.size(); i++){
    printf("\r >> MM efficiency, chamber %i: %f", i, mm_eff[i]);
    cout << endl;
  }
  printf("\r >> Seed for TRandom3: %d", ran->GetSeed());
  cout << endl;
  cout << endl;
  cout << endl;
    

  cout << pink << "Generating " << nevents << " events" << ending << endl;

  TFile* fout = new TFile(outputFileName, "RECREATE");
  fout->mkdir("event_displays");

  // define output ntuple

  TTree* tree = new TTree("gingko","gingko");
  
  int EventNum = 0;
  int Ntriggers;

  vector<int> iRoad_x;
  vector<int> iRoad_u;
  vector<int> iRoad_v;

  vector<vector<int>> Hit_strips;
  vector<vector<int>> Hit_planes;
  vector<vector<int>> Hit_ages;
  vector<int> trigger_BC;

  if (write_tree) {

    tree->Branch("EventNum",  &EventNum);
    tree->Branch("Ntriggers",  &Ntriggers);
    tree->Branch("iRoad_x", &iRoad_x);
    tree->Branch("iRoad_u", &iRoad_u);
    tree->Branch("iRoad_v", &iRoad_v);
    tree->Branch("Hit_strips", &Hit_strips);
    tree->Branch("Hit_planes", &Hit_planes);
    tree->Branch("Hit_ages", &Hit_ages);
    tree->Branch("trigger_BC", &trigger_BC);

  }

  // geometry stuff
  double xlen = xhigh-xlow;
  double ylen = yhigh-ylow; 

  GeoOctuplet* GEOMETRY;
  if (string(chamberType) == "oct")
    GEOMETRY = new GeoOctuplet(false,xlen,ylen);
  else
    GEOMETRY = new GeoOctuplet(true,xlen,ylen);

  // counters
  int nmuon_trig = 0;
  int nuv_bkg = 0;
  int nevent_uvbkg = 0;
  int neventtrig = 0;

  int nevent_uvbkg1 = 0;
  int nevent_uvbkg2 = 0;
  int nevent_uvbkg3 = 0;
  int nevent_uvbkg4 = 0;

  int extratrig = 0;
  
  int nevent_allnoise = 0;


  bool muon_trig_ok = false;

  // book histos

  std::map< string, TH1F* > hists;
  std::map< string, TH2D* > hists_2d;
  
  //TH1F * h_mxres = new TH1F("h_mxres", "#Delta#Theta", 30, -1.5, 1.5);
  hists["h_mxres"] = new TH1F("h_mxres", "#Delta#Theta", 201, -100.5, 100.5);
  hists["h_yres"] = new TH1F("h_yres", "#DeltaY", 700, -3500, 3500);
  //TH1F * h_xres = new TH1F("h_xres", "#DeltaX", 50, -2.5, 2.5);
  hists["h_xres"] = new TH1F("h_xres", "#DeltaX", 123, -20.5, 20.5);
  hists["h_xres_center"] = new TH1F("h_xres_center", "#DeltaX", 123, -20.5, 20.5);
  hists["h_yres_center"] = new TH1F("h_yres_center", "#DeltaY", 700, -3500, 3500);

  hists["h_nmu"] = new TH1F("h_nmu", "h_nmu", 9, -0.5, 8.5);
  hists_2d["h_nmuvsdx"] = new TH2D("h_nmuvsdx", "h_nmuvsdx", 9, -0.5, 8.5,82, -20.5, 20.5);
  hists["h_dx"] = new TH1F("h_dx", "h_dx", 500, -3500,3500);
  hists["h_nuv_bkg"] = new TH1F("h_nuv_bkg", "", 5, -0.5, 4.5);
  hists["h_nx_bkg"] = new TH1F("h_nx_bkg", "", 5, -0.5, 4.5);
  hists_2d["h_xres_nxbkg"] = new TH2D("h_xres_nxbkg", "h_xres_nxbkg", 5, -0.5, 4.5 , 122, -30.5, 30.5);
  hists_2d["h_xres_nxmuon"] = new TH2D("h_xres_nxmuon", "h_xres_nxmuon", 5, -0.5, 4.5 , 122, -30.5, 30.5);

  hists_2d["h_xy_all"]  = new TH2D("h_xy_all",  "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);
  hists_2d["h_xy_trig"] = new TH2D("h_xy_trig", "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);
  hists_2d["h_xy_eff"]  = new TH2D("h_xy_eff",  "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);
  hists_2d["h_xy_bkg"]  = new TH2D("h_xy_bkg",  "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);

  for (auto kv: hists){
    kv.second->Sumw2();
    kv.second->StatOverflows(kTRUE);
  }

  // multiplicity studies
  hists["h_ntrig"] = new TH1F("h_ntrig", "h_ntrig", 101, -0.5, 100.5);
  hists["h_ntrig_bkgonly"] = new TH1F("h_ntrig_bkgonly", "h_ntrig_bkgonly", 101, -0.5, 100.5);
  hists_2d["h_ntrig_bc"] = new TH2D("h_ntrig_bc", "h_ntrig_bc", 49,-24.5,24.5, 101, -0.5, 100.5);

  vector<Road*> m_roads = create_roads(*GEOMETRY, uvrflag, m_xthr, m_uvthr, string(chamberType), trapflag);

  time_t timer = time(NULL);
  time_t curr_time;
  for ( int i = 0; i < nevents; i++){

    if (write_tree){
      EventNum++;
      iRoad_x.clear();
      iRoad_u.clear();
      iRoad_v.clear();

      Hit_strips.clear();
      Hit_planes.clear();
      Hit_ages.clear();
      trigger_BC.clear();
    }

    if (nevents > 10){
      if (i % ((int)nevents/10) == 0){
        curr_time = time(NULL);
        progress(curr_time-timer, i, nevents);
      }
    }
    muon_trig_ok = false;

    // generate muon
     double co = 2.7;
    co = 0;
    vector<double> zpos = {-co, 11.2+co, 32.4-co, 43.6+co, 
                           113.6-co, 124.8+co, 146.0-co, 157.2+co};
    vector<double> xpos(NPLANES,-1.);
    vector<double> ypos(NPLANES,-1.);
    
    double xmuon,ymuon;
    std::tie(xmuon,ymuon) = generate_muon(xpos, ypos, zpos, string(chamberType), trapflag);

    if (db){
      printf("generated muon! @ (%4.4f,%4.4f)\n",xmuon,ymuon);
    }
    hists_2d["h_xy_all"]->Fill(xmuon, ymuon);

    vector<int> oct_hitmask = oct_response(xpos, ypos, zpos, mm_eff);
  
    vector<int> art_bc(NPLANES, -1.);
    double smallest_bc = 999999.;
    
    vector<Hit*> hits;

    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;
    
    double art_time;
    
    double strip, strip_smear;
    for ( int j = 0; j < NPLANES; j++){
      if (oct_hitmask[j] == 1){
        if (j < 2)
          n_x1++;
        else if (j > 5)
          n_x2++;
        else if (j == 2|| j ==4)
          n_u++;
        else
          n_v++;
      art_time = ran->Gaus(400.,(double)(sig_art));
      art_bc[j] = (int)floor(art_time/25.);
      Hit* newhit = nullptr;

      strip = GEOMETRY->Get(j).channel_from_pos(xpos[j],ypos[j]);
      if (smear_art){
        strip_smear = round(ran->Gaus(strip,m_sig_art_x));
      }
      else{
        strip_smear = strip;
      }
      newhit = new Hit(j, art_bc[j], strip_smear, false, *GEOMETRY);
      //newhit = new Hit(j, art_bc[j], xpos[j], ypos[j], false, *GEOMETRY);
      if (!bkgonly)
	hits.push_back(newhit);
      }
    }

    if (db){
      cout << "N muonhits: " << hits.size() << endl;
      for (unsigned int j = 0; j < hits.size(); j++){
        printf("Muon hit (board, BC, strip): (%d,%d,%4.4f)\n", hits[j]->MMFE8Index(),hits[j]->Age(),hits[j]->Channel());
      }
    }
    if (n_x1 > 0 && n_x2 > 0 && n_u > 0 && n_v > 0){
      nmuon_trig++;
      muon_trig_ok= true; 
    }

    for (unsigned int j = 0; j < art_bc.size(); j++){
      if (art_bc[j] == -1)
        continue;
      else if (art_bc[j] < smallest_bc)
        smallest_bc = art_bc[j];
    }

    // assume bkg rate has oct_response factored in!
    
    vector<Hit*> all_hits = hits;    
    if (bkgflag){
      vector<Hit*> bkghits = generate_bkg(smallest_bc, *GEOMETRY, bkgrate, string(chamberType));
      if (db)
        cout << "Nbkg hits: " << bkghits.size() << endl;
      all_hits.insert(all_hits.end(), bkghits.begin(), bkghits.end());
    }


    for (unsigned int ihit = 0; ihit < all_hits.size(); ihit++){
      int ib = all_hits[ihit]->MMFE8Index();
      if (all_hits[ihit]->IsNoise() &&
	  (ib < 2 || ib > 5))
	hists["h_dx"]->Fill(GEOMETRY->Get(ib).LocalXatYend(all_hits[ihit]->Channel())+GEOMETRY->Get(ib).Origin().X()-xmuon);
    }

    for (auto road: m_roads)
      road->Reset();
    if (db)
      cout << "Number of roads: " << m_roads.size() << endl;


    if (db)
      cout << "Total number of hits: " << all_hits.size() << endl;

    vector<slope_t> m_slopes;
    int ntrigroads;
    int ntrigroads_bkgonly = 0;


    std::tie(ntrigroads, m_slopes) = finder(all_hits, smallest_bc, m_roads, (pltflag||write_tree), ideal_vmm, ideal_addc, ideal_tp, i);
    hists["h_ntrig"]->Fill(ntrigroads);
    if (write_tree)
      Ntriggers = ntrigroads;
    for (auto sl: m_slopes)
      if (sl.imuonhits == 0)
        hists_2d["h_xy_bkg"]->Fill(sl.xavg, sl.yavg);

    if (db)
      cout << "Ntriggered roads: " << ntrigroads << endl;
    if (ntrigroads == 0 && muon_trig_ok){
      //      if (db)
      cout << "no triggered roads?" << endl;
      if (write_tree)
	tree->Fill();
      continue;
    }

    // got a trigger, but none with real hits
    if (m_slopes.size() == 0 && ntrigroads != 0 ){
      nevent_allnoise++;
//       if (db)
//         cout << "Didn't trigger with real hits:( " << endl;
//       continue;
    }

    slope_t myslope;
    myslope.mxl = 0.;
    myslope.count = 0;
    myslope.xavg = 0.;
    myslope.yavg = 0.;
    myslope.xcenter = 0.;
    myslope.ycenter = 0.;
    myslope.imuonhits = 0;
    myslope.xmuon = 0;
    myslope.age = -999;
    // pick road with the most muon x hits
    int most_hits = 0;
    vector<int> iroads = {};
    int myage = smallest_bc-bc_wind*3;
    int ntrig_age = 0;

    vector <int> slopehits_ch;
    vector <int> slopehits_planes;
    vector <int> slopehits_ages;

    for (unsigned int k = 0; k < m_slopes.size(); k++){
      while (m_slopes[k].age != myage){
        hists_2d["h_ntrig_bc"]->Fill(myage,ntrig_age);
        myage++;
        ntrig_age = 0;
      }
      ntrig_age++;
      
      if (write_tree) {
        iRoad_x.push_back(m_slopes[k].iroad);
        iRoad_u.push_back(m_slopes[k].iroadu);
        iRoad_v.push_back(m_slopes[k].iroadv);
        
        slopehits_ch.clear();
        slopehits_ages.clear();
        slopehits_planes.clear();
        
        for (unsigned int n = 0; n < m_slopes[k].slopehits.size(); n++) {
          slopehits_ch.push_back(m_slopes[k].slopehits[n].Channel());
          slopehits_planes.push_back(m_slopes[k].slopehits[n].MMFE8Index());
          slopehits_ages.push_back(m_slopes[k].slopehits[n].Age());
        }
        
        trigger_BC.push_back(m_slopes[k].age);
        Hit_strips.push_back(slopehits_ch);
        Hit_planes.push_back(slopehits_planes);
        Hit_ages.push_back(slopehits_ages);
      }

      if (m_slopes[k].imuonhits == 0)
        ntrigroads_bkgonly++;
      if (m_slopes[k].imuonhits < most_hits)
        continue;
      if (m_slopes[k].imuonhits > most_hits)
        iroads.clear();
      iroads.push_back(k);
      most_hits = m_slopes[k].imuonhits;
    }
    hists_2d["h_ntrig_bc"]->Fill(myage,ntrig_age);
    myage++;
    while (myage < smallest_bc+bc_wind*2){
      hists_2d["h_ntrig_bc"]->Fill(myage,0);
      myage++;
    }
    hists["h_ntrig_bkgonly"]->Fill(ntrigroads_bkgonly);

    int the_chosen_one = iroads[ran->Integer((int)(iroads.size()))];
    myslope.count       = m_slopes[the_chosen_one].count;
    myslope.mxl         = m_slopes[the_chosen_one].mxl;
    myslope.uvbkg       = m_slopes[the_chosen_one].uvbkg;
    myslope.xavg        = m_slopes[the_chosen_one].xavg;
    myslope.yavg        = m_slopes[the_chosen_one].yavg;
    myslope.xcenter     = m_slopes[the_chosen_one].xcenter;
    myslope.ycenter     = m_slopes[the_chosen_one].ycenter;
    myslope.iroad       = m_slopes[the_chosen_one].iroad;
    myslope.imuonhits   = m_slopes[the_chosen_one].imuonhits;
    if (pltflag)
      myslope.slopehits = m_slopes[the_chosen_one].slopehits;

    if (fabs(myslope.xavg-xmuon) > 5. && pltflag){
      TString test = Form("event_disp_%d",i);
      TString test2 = Form("event_disp_alt_%d",i);
      plttrk(myslope.slopehits, true, test, ntrigroads, fout);
    }
    double deltaMX = TMath::ATan(myslope.mxl); // change to subtract angle of muon, which is 0 right now
    if (db) {
      printf ("art (x,y): (%4.4f,%4.4f)", myslope.xavg, myslope.yavg);
      cout << endl;
      cout << endl;
    }

    hists["h_mxres"]->Fill(deltaMX*1000.);
    hists["h_yres"]->Fill(myslope.yavg-ymuon);
    hists["h_xres"]->Fill(myslope.xavg-xmuon);
    hists["h_yres_center"]->Fill(myslope.ycenter-ymuon);
    hists["h_xres_center"]->Fill(myslope.xcenter-xmuon);
    hists["h_nmu"]->Fill(myslope.imuonhits);
    hists_2d["h_nmuvsdx"]->Fill(myslope.imuonhits, myslope.xavg-xmuon);
    if (muon_trig_ok)
      neventtrig++;
    else
      extratrig++;

    hists["h_nuv_bkg"]->Fill(myslope.uvbkg);
    hists["h_nx_bkg"]->Fill(myslope.xbkg);
    hists_2d["h_xres_nxbkg"]->Fill(myslope.xbkg,myslope.xavg-xmuon);
    hists_2d["h_xres_nxmuon"]->Fill(myslope.xmuon,myslope.xavg-xmuon);

    if (myslope.uvbkg > 0){
      nevent_uvbkg++;
      nuv_bkg += myslope.uvbkg;
      if (myslope.uvbkg == 1)
        nevent_uvbkg1++;
      else if (myslope.uvbkg== 2)
        nevent_uvbkg2++;
      else if (myslope.uvbkg== 3)
        nevent_uvbkg3++;
      else if (myslope.uvbkg== 4)
        nevent_uvbkg4++;
    }

    if (m_slopes.size() > 0 && muon_trig_ok)
      hists_2d["h_xy_trig"]->Fill(xmuon, ymuon);

    tree->Fill();

  }
  cout << endl;
  cout << endl;
  cout << blue << "SIMULATION SUMMARY:" << ending << endl;
  cout << neventtrig << " muons triggered out of " << nmuon_trig << " muons that should trigger"<< endl;
  cout << nevent_uvbkg << " triggers with spoiled uv hits"<< endl;
  cout << "n=1: " << nevent_uvbkg1 << " | n=2: "<< nevent_uvbkg2 << " | n=3: "<< nevent_uvbkg3 <<" | n=4: "<< nevent_uvbkg4 << endl;
  cout << nuv_bkg << " bkg uv hits total"<< endl;
  cout << extratrig << " extra trigger events " << endl;
  cout << nevent_allnoise << " events where triggers were only made with bkg hits" << endl;
  cout << endl;

  setstyle();



  ofstream mylog;
  string logtitle = string(outputFileName) + ".log.txt";
  mylog.open(logtitle);
  mylog << "x-road size(in strips): "<< XROAD <<", +/- neighbor roads (uv): "<< UVFACTOR <<"\n";
  mylog << "art res (in ns): " << m_sig_art << "\n";
  mylog << "Using BCID window: " << bc_wind << "\n";
  //  mylog << "Background rate of " << bkgrate << " Hz per square mm\n";
  mylog << "Background rate of " << bkgrate << " Hz per strip\n";
  mylog << "Assuming chamber size: (" << xhigh-xlow << ", " << yhigh-ylow << ") in mm\n";
  mylog << "Assuming muon active area: ("<< mu_xhigh-mu_xlow<< ", "<< mu_yhigh-mu_ylow <<") in mm\n";

  mylog << "\n";
  mylog << "SIMULATION SUMMARY:\n";
  mylog << neventtrig << " muons triggered out of " << nmuon_trig << " muons that should trigger\n";
  mylog << nevent_uvbkg << " triggers with spoiled uv hits\n";
  mylog << "n=1: " << nevent_uvbkg1 << " | n=2: "<< nevent_uvbkg2 << " | n=3: "<< nevent_uvbkg3 <<" | n=4: "<< nevent_uvbkg4 << "\n";
  mylog << nuv_bkg << " bkg uv hits total\n";
  mylog << extratrig << " extra trigger events\n";
  mylog << nevent_allnoise << " events where triggers were only made with bkg hits\n";

  mylog.close();

  // FYI: incompatible when batched. this is only for checks.
  hists_2d["h_xy_eff"]->Divide(hists_2d["h_xy_trig"], hists_2d["h_xy_all"], 1.0, 1.0, "B");

  fout->cd();
  if (write_tree)
    tree->Write();
  fout->mkdir(histograms.c_str());
  fout->cd(histograms.c_str());
  
  std::map<string, TH1F*>::iterator it = hists.begin();
  while (it != hists.end())
    {
      it->second->Write();
      it++;
    }
  std::map<string, TH2D*>::iterator it2 = hists_2d.begin();
  while (it2 != hists_2d.end())
    {
      it2->second->Write();
      it2++;
    }
  fout->cd();
  fout->Close();
  return 0;
}

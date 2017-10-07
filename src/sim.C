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

using namespace std;

TRandom3 *ran = new TRandom3(time(NULL));


bool db = false; // debug output flag

// SOME CONSTANTS

int NBOARDS = 8;
int NSTRIPS;
double xlow, xhigh, ylow, yhigh; // chamber dimensions
double mu_xlow, mu_xhigh, mu_ylow, mu_yhigh; // active chamber area to decouple edge effects

int XROAD, UVFACTOR;

int NSTRIPS_UP_UV, NSTRIPS_DN_UV;
int NSTRIPS_UP_XX, NSTRIPS_DN_XX;

int bc_wind;
double sig_art;

double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));
double mm_eff[8] = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9}; // i apologize for this array

// colors
string pink = "\033[38;5;205m";
string green = "\033[38;5;84m";
string blue = "\033[38;5;27m";
string ending = "\033[0m";
string warning = "\033[38;5;227;48;5;232m";

struct slope_t {
  int count;
  int iroad;
  int imuonhits;
  bool uvbkg;
  double mxl;
  double xavg;
  double yavg;
  vector<Hit> slopehits;
};

void set_chamber(string chamber, int m_wind, int m_sig_art, int m_xroad){
  // function to set parameters in a smart way

  if (chamber == "small"){

    NSTRIPS = 8800; // has to be multiple of x road
    xlow = 0.;
    xhigh = NSTRIPS*0.4-0.2;
    ylow = 0.;
    yhigh = 500.;
  }
  else if (chamber == "large"){

    NSTRIPS = 8800; // has to be multiple of x road
    xlow = 0.;
    xhigh = NSTRIPS*0.4-0.2;
    ylow = 0.;
    yhigh = 2200.;
  }
  else if (chamber == "oct"){

    NSTRIPS = 512; // has to be multiple of x road
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

  NSTRIPS_UP_UV = 72;
  NSTRIPS_DN_UV = 72;
  NSTRIPS_UP_XX = 8;
  NSTRIPS_DN_XX = 8;
}


tuple<double,double> cosmic_angle(){
  return make_tuple(0.,0.);
}

vector<Road*> create_roads(const GeoOctuplet& geometry){
  if (NSTRIPS % XROAD != 0)
    cout << "Not divisible!" << endl;
  int nroad = NSTRIPS/XROAD;
  vector<Road*> m_roads;
  for ( int i = 0; i < nroad; i++){

    Road* myroad = nullptr;
    myroad = new Road(&geometry, i);
    m_roads.push_back(myroad);

    int nuv = 0;
    for (int uv = 1; uv <= nuv; uv++){
      if (i-uv < 0)
        continue;
      Road* myroad_0 = new Road(&geometry, i, i+uv,   i-uv);
      Road* myroad_1 = new Road(&geometry, i, i-uv,   i+uv);
      Road* myroad_2 = new Road(&geometry, i, i+uv-1, i-uv);
      Road* myroad_3 = new Road(&geometry, i, i-uv,   i+uv-1);
      Road* myroad_4 = new Road(&geometry, i, i-uv+1, i+uv);
      Road* myroad_5 = new Road(&geometry, i, i+uv,   i-uv+1);
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

tuple<double,double> generate_muon(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos){

//   double x = ran->Uniform(xlow,xhigh);
//   double y = ran->Uniform(ylow,yhigh);

  double x = ran->Uniform(mu_xlow,mu_xhigh);
  double y = ran->Uniform(mu_ylow,mu_yhigh);

  double thx, thy;

  std::tie(thx,thy) = cosmic_angle();

  double avgz = 0.5*(zpos[0]+zpos[NBOARDS-1]);
  double z, x_b, y_b;
  for ( int j = 0; j < NBOARDS; j++){
    z = zpos[j];
    x_b = TMath::Tan(thx)*(zpos[j]-avgz)+x;
    y_b = TMath::Tan(thy)*(zpos[j]-avgz)+y;
    xpos[j] = x_b;
    ypos[j] = y_b;
  }    
  return make_tuple(x,y);
}

// vector<Hit*> generate_bkg(int start_bc, const GeoOctuplet& geometry, int bkgrate){

//   double plane_area = (xhigh-xlow) * (yhigh-ylow);
//   vector<Hit*> bkghits;

  
//   int noise_window = bc_wind * 3;
//   int start_noise = start_bc - bc_wind;
//   int end_noise = start_bc + bc_wind * 2 -1;

//   //assume uniform distribution of background - correct for noise
//   double bkgrate_bc = bkgrate * (25*pow(10,-9));
//   double expbkg = bkgrate_bc * noise_window  * plane_area;


//   for ( int j = 0; j < NBOARDS; j++){
//     //int nbkg = expbkg;
//     int nbkg = ran->Poisson(expbkg);
//     double x, y;
//     for ( int k = 0; k < nbkg; k++){
//       x = ran->Uniform(xlow, xhigh);
//       y = ran->Uniform(ylow, yhigh);
//       Hit* newhit = nullptr;
//       newhit = new Hit(j, start_noise+ran->Integer(end_noise+1-start_noise), x,y,true, geometry);
//       bkghits.push_back(newhit);
//     }
//   }
//   return bkghits;
// }

vector<Hit*> generate_bkg(int start_bc, const GeoOctuplet& geometry, int bkgrate){

  vector<Hit*> bkghits;

  int noise_window = bc_wind * 3;
  int start_noise = start_bc - bc_wind;
  int end_noise = start_noise + noise_window - 1;
  // int end_noise = start_bc + bc_wind * 2 -1;

  //assume uniform distribution of background - correct for noise
  double bkgrate_bc = bkgrate * (25*pow(10,-9));
  double bkg_prob = bkgrate_bc*noise_window;
  for ( int j = 0; j < NBOARDS; j++){
    //int nbkg = expbkg;
    for ( int k = 0; k < NSTRIPS; k++){
      double prob = ran->Uniform(0,1.);
      if (prob < bkg_prob){
        Hit* newhit = nullptr;
        newhit = new Hit(j, start_noise+ran->Integer(noise_window), k, true, geometry);
        bkghits.push_back(newhit);
      }
    }
  }
  return bkghits;
}

vector<int> oct_response(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos){
  //gives detector response to muon, returns list of which planes registered hit
  
  int n_mm = 0;
  vector<int> oct_hitmask(NBOARDS,0);
  for ( int j=0; j < NBOARDS; j++){
    if (ran->Uniform(0.,1.) < mm_eff[j]){
      oct_hitmask[j] = 1;
      n_mm++;
    }
  }
  return oct_hitmask;
}

tuple<int, vector < slope_t> > finder(vector<Hit*> hits, vector<Road*> roads, bool saveHits){

  // applies the MMTP finder to a series of hits and roads
  // returns slope structs for roads which found a coincidence and have at least 1 real muon hit

  int ntrigs = 0;
  int bc_start = 999999;
  int bc_end = -1;

  vector<slope_t> slopes;

  for (unsigned int i=0; i < hits.size(); i++){
    if (hits[i]->Age() < bc_start)
      bc_start = hits[i]->Age();
    if (hits[i]->Age() > bc_end)
      bc_end = hits[i]->Age();
  }
  bc_start = bc_start - bc_wind*2;
  bc_end = bc_end + bc_wind*2;


  // each road makes independent triggers
  for (unsigned int i = 0; i < roads.size(); i++){

    roads[i]->Reset();

    vector<Hit*> hits_now;
    vector<int> vmm_same;
    int n_vmm = NSTRIPS/64;

    for ( int bc = bc_start; bc < bc_end; bc++){
      //cout << "bunch crossing: " << bc << endl;
      hits_now.clear();

      roads[i]->Increment_Age(bc_wind);

      for (unsigned int j = 0; j < hits.size(); j++){
        // BC window
        if (hits[j]->Age() == bc){
          // add into hits_now so that it is sorted by strip number
          bool added_hit = false;
          for (unsigned int k = 0; k < hits_now.size(); k++){
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
      
      // implement vmm ART-like signal
      for (int ib = 0; ib < NBOARDS; ib++){
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
            std::random_shuffle(vmm_same.begin(),vmm_same.end());
            vmm_same.erase(vmm_same.begin());
            std::sort(vmm_same.begin(), vmm_same.end());
            for (int k = vmm_same.size()-1; k > -1; k--){
              hits_now.erase(hits_now.begin()+vmm_same[k]);
            }
          }
        }
      }
      roads[i]->Add_Hits(hits_now, XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV);

      if (roads[i]->Coincidence(bc_wind)){
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

        if (nmuonhits < 1)
          continue;

        slope_t m_slope;
        
        m_slope.count = roads[i]->Count();
        m_slope.iroad = roads[i]->iRoad();
        m_slope.imuonhits = nmuonhits;
	m_slope.uvbkg = roads[i]->UV_bkg();
        m_slope.mxl = roads[i]->Mxl();
        m_slope.xavg = roads[i]->AvgXofX();
        m_slope.yavg = -B*( roads[i]->AvgXofU() - roads[i]->AvgXofV() + (roads[i]->AvgZofV()-roads[i]->AvgZofU())*roads[i]->Mxl() ) / 2 + yhigh;
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
  vector <int> ib;
  vector <double> ch;
  vector <double> bkgpts;
  vector <double> bkgzpts;
  vector <int> bkgib;
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
    }
  }
  setstyle();
  TCanvas * c1 = new TCanvas("c1", "canvas", 800, 800);
  c1->cd();
  TMultiGraph * mg = new TMultiGraph();
  TLegend* leg = new TLegend(0.6,0.45,0.74,0.55);
  //define lines for planes
  vector<TGraph*> planes = {};
  
  for ( int k=0; k < NBOARDS; k++){
      Double_t board_x[2];
      Double_t board_z[2];      
      if (xflag){
        if (bkgpts.size() > 0){
        board_x[0] = *min_element(bkgpts.begin(),bkgpts.end())-10.;
        board_x[1] = *max_element(bkgpts.begin(),bkgpts.end())+10;
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
      TLatex *latex = new TLatex(gr->GetX()[j], gr->GetY()[j], Form("%6.f",ch[j]));
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
      TLatex *latex = new TLatex(grbkg->GetX()[j], grbkg->GetY()[j], Form("%6.f",bkgch[j]));
      latex->SetTextSize(0.03);
      grbkg->GetListOfFunctions()->Add(latex);
    }

    if (zpts.size()> 0)
      mg->Add(gr,"p");
    if (bkgzpts.size()> 0)
      mg->Add(grbkg,"p");
    for ( int k = 0; k < NBOARDS; k++){
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
  int bkgrate = -1; // Hz per strip

  int m_xroad = 8;
  int m_bcwind = 8;
  double m_sig_art = 32.;

  bool bkgflag = false;
  bool pltflag = false;

  char outputFileName[400];
  char chamberType[400];

  if ( argc < 3 ){
    cout << "Error at Input: please specify number of events to generate "<< endl;
    cout << "Example:   ./sim -n 100 -ch <chamber type> -o output.root" << endl;
    cout << "Example:   ./sim -n 100 -ch <chamber type> -b <bkg rate in kHz/strip> -o output.root" << endl;
    cout << "Example:   ./sim -n 100 -ch <chamber type> -b <bkg rate in Hz/strip> -p <make event displays> -o output.root" << endl;
    cout << "Other options include: -w <bc_wind> -sig <art res (ns)>" << endl;
    return 0;
  }

  bool b_out = false;
  bool ch_type = false;

  for ( int i=1;i<argc-1;i++){
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
    if (strncmp(argv[i],"-ch",3)==0){
      sscanf(argv[i+1],"%s", chamberType);
      ch_type = true;
    }
    if (strncmp(argv[i],"-sig",4)==0){
      m_sig_art = atof(argv[i+1]);
    }
    if (strncmp(argv[i],"-b",2)==0){
      bkgrate = atoi(argv[i+1]);
      bkgflag = true;
    }
    if (strncmp(argv[i],"-p",2)==0){
      pltflag = true;
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

  set_chamber( string(chamberType), m_bcwind, m_sig_art, m_xroad);
  
  cout << endl;
  cout << blue << "--------------" << ending << endl;
  cout << blue << "OCT SIM ✪ ‿ ✪ " << ending << endl;
  cout << blue << "--------------" << ending << endl;
  cout << endl;
  cout << endl;
  printf("\r >> x-road size (in strips): %d, +/- neighbor roads (uv): %d", XROAD, UVFACTOR);
  cout << endl;
  printf("\r >> art res (in ns): %f", m_sig_art);
  cout << endl;
  cout << "\r >> Using BCID window: " << bc_wind << endl;
  printf("\r >> Background rate of %d Hz per strip",bkgrate);
//   printf("\r >> Background rate of %d Hz per square mm",bkgrate);
  cout << endl;
  printf("\r >> Assuming chamber size: (%4.1f,%4.1f) in mm",xhigh-xlow, yhigh-ylow);
  cout << endl;
  cout << endl;
    

  cout << pink << "Generating " << nevents << " events" << ending << endl;

  TFile* fout = new TFile(outputFileName, "RECREATE");
  fout->mkdir("event_displays");

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
  int neventtrig = 0;
  int extratrig = 0;
  
  int nevent_allnoise = 0;


  bool muon_trig_ok = false;

  // book histos

  std::map< string, TH1F* > hists;
  std::map< string, TH2D* > hists_2d;
  
  //TH1F * h_mxres = new TH1F("h_mxres", "#Delta#Theta", 30, -1.5, 1.5);
  hists["h_mxres"] = new TH1F("h_mxres", "#Delta#Theta", 201, -100.5, 100.5);
  hists["h_yres"] = new TH1F("h_yres", "#DeltaY", 140, -3500, 3500);
  //TH1F * h_xres = new TH1F("h_xres", "#DeltaX", 50, -2.5, 2.5);
  hists["h_xres"] = new TH1F("h_xres", "#DeltaX", 123, -20.5, 20.5);

  hists["h_nmu"] = new TH1F("h_nmu", "h_nmu", 9, -0.5, 8.5);
  hists_2d["h_nmuvsdx"] = new TH2D("h_nmuvsdx", "h_nmuvsdx", 9, -0.5, 8.5,82, -20.5, 20.5);
  hists["h_dx"] = new TH1F("h_dx", "h_dx", 500, -3500,3500);

  hists_2d["h_xy_all"]  = new TH2D("h_xy_all",  "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);
  hists_2d["h_xy_trig"] = new TH2D("h_xy_trig", "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);
  hists_2d["h_xy_eff"]  = new TH2D("h_xy_eff",  "",  1000, xlow-100, xhigh+100, 1000, ylow-100, yhigh+100);

  hists["h_mxres"]->Sumw2();
  hists["h_yres"]->Sumw2();
  hists["h_xres"]->Sumw2();

  hists["h_mxres"]->StatOverflows(kTRUE);
  hists["h_yres"]->StatOverflows(kTRUE);
  hists["h_xres"]->StatOverflows(kTRUE);

  vector<Road*> m_roads = create_roads(*GEOMETRY);

  time_t timer = time(NULL);
  time_t curr_time;
  for ( int i = 0; i < nevents; i++){

    if (nevents > 1000){
      if (i % ((int)nevents/1000) == 0){
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
    vector<double> xpos(NBOARDS,-1.);
    vector<double> ypos(NBOARDS,-1.);
    
    double xmuon,ymuon;
    std::tie(xmuon,ymuon) = generate_muon(xpos,ypos,zpos);

    if (db){
      printf("generated muon! @ (%4.4f,%4.4f)\n",xmuon,ymuon);
    }
    hists_2d["h_xy_all"]->Fill(xmuon, ymuon);

    vector<int> oct_hitmask = oct_response(xpos,ypos,zpos);
  
    vector<int> art_bc(NBOARDS, -1.);
    double smallest_bc = 999999.;
    
    vector<Hit*> hits;

    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;
    
    double art_time;
  
    for ( int j = 0; j < NBOARDS; j++){
      if (oct_hitmask[j] == 1){
        if (j < 2)
          n_x1++;
        else if (j > 5)
          n_x2++;
        else if (j == 2|| j ==4)
          n_u++;
        else
          n_v++;
      art_time = ran->Gaus(400.,sig_art);
      art_bc[j] = (int)floor(art_time/25.);
      Hit* newhit = nullptr;
      newhit = new Hit(j, art_bc[j], xpos[j], ypos[j], false, *GEOMETRY);
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
      vector<Hit*> bkghits = generate_bkg(smallest_bc, *GEOMETRY, bkgrate);
      if (db)
        cout << "Nbkg hits: " << bkghits.size() << endl;
      all_hits.insert(all_hits.end(), bkghits.begin(), bkghits.end());
    }


    for (unsigned int i = 0; i < all_hits.size(); i++){
      int ib = all_hits[i]->MMFE8Index();
      if (all_hits[i]->IsNoise() &&
	  (ib < 2 || ib > 5))
	hists["h_dx"]->Fill(GEOMETRY->Get(ib).LocalXatYend(all_hits[i]->Channel())+GEOMETRY->Get(ib).Origin().X()-xmuon);
    }

    for (auto road: m_roads)
      road->Reset();
    if (db)
      cout << "Number of roads: " << m_roads.size() << endl;


    if (db)
      cout << "Total number of hits: " << all_hits.size() << endl;

    vector<slope_t> m_slopes;
    int ntrigroads;
    std::tie(ntrigroads, m_slopes) = finder(all_hits, m_roads, pltflag);
    if (db)
      cout << "Ntriggered roads: " << ntrigroads << endl;
    if (ntrigroads == 0 && muon_trig_ok){
      //      if (db)
      cout << "no triggered roads?" << endl;
      continue;
    }

    // got a trigger, but none with real hits
    if (m_slopes.size() == 0){
      nevent_allnoise++;
      if (db)
        cout << "Didn't trigger with real hits:( " << endl;
      continue;
    }

    slope_t myslope;
    myslope.mxl = 0.;
    myslope.count = 0;
    myslope.xavg = 0.;
    myslope.yavg = 0.;
    myslope.imuonhits = 0;

    vector<int> iroads;
    for (unsigned int k = 0; k < m_slopes.size(); k++){
      iroads.push_back(k);
    }
    
    // shuffle roads so we don't have a bias from iterating through roads by index
    std::random_shuffle(iroads.begin(),iroads.end());

    // pick road with the most real muon hits
    for (unsigned int k = 0; k < iroads.size(); k++){
      int j = iroads[k];
      if (m_slopes[j].imuonhits > myslope.imuonhits){
        myslope.count = m_slopes[j].count;
        myslope.mxl = m_slopes[j].mxl;
	myslope.uvbkg = m_slopes[j].uvbkg;
        myslope.xavg = m_slopes[j].xavg;
        myslope.yavg = m_slopes[j].yavg;
        myslope.imuonhits = m_slopes[j].imuonhits;
        if (pltflag)
          myslope.slopehits = m_slopes[j].slopehits;
      }
    }

    if (i < 10 && pltflag){
      TString test = Form("event_disp_%d",i);
      plttrk(myslope.slopehits, true, test, ntrigroads, fout);
    }
    double deltaMX = TMath::ATan(myslope.mxl); // change to subtract angle of muon, which is 0 right now
    //cout << "delta x " << deltaMX*1000. << endl;
    if (db) {
      printf ("art (x,y): (%4.4f,%4.4f)", myslope.xavg, myslope.yavg);
      cout << endl;
      cout << endl;
    }
    //      cout << "art (x,y): (" << myslope.xavg << "," << myslope.yavg << ")"<< endl;
    hists["h_mxres"]->Fill(deltaMX*1000.);
    hists["h_yres"]->Fill(myslope.yavg-ymuon);
    hists["h_xres"]->Fill(myslope.xavg-xmuon);
    // if (myslope.xavg-xmuon < -2){
    //   TString test = Form("event_disp_%d",i);
    //   plttrk(myslope.slopehits, true, test, ntrigroads, fout);
    // }
      
    hists["h_nmu"]->Fill(myslope.imuonhits);
    hists_2d["h_nmuvsdx"]->Fill(myslope.imuonhits, myslope.xavg-xmuon);
    if (muon_trig_ok)
      neventtrig++;
    else
      extratrig++;
    if (myslope.uvbkg)
      nuv_bkg++;

    if (m_slopes.size() > 0 && muon_trig_ok)
      hists_2d["h_xy_trig"]->Fill(xmuon, ymuon);

  }
  cout << endl;
  cout << endl;
  cout << blue << "SIMULATION SUMMARY:" << ending << endl;
  cout << neventtrig << " muons triggered out of " << nmuon_trig << " muons that should trigger"<< endl;
  cout << nuv_bkg << " triggers with spoiled uv hits"<< endl;
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
  mylog << "Assuming chamber size: ("<<xhigh-xlow <<", "<< yhigh-ylow << ") in mm\n";


  mylog << "\n";
  mylog << "SIMULATION SUMMARY:\n";
  mylog << neventtrig << " muons triggered out of " << nmuon_trig << " muons that should trigger\n";
  mylog << extratrig << " extra trigger events\n";
  mylog << nevent_allnoise << " events where triggers were only made with bkg hits\n";

  mylog.close();

  // FYI: incompatible when batched. this is only for checks.
  hists_2d["h_xy_eff"]->Divide(hists_2d["h_xy_trig"], hists_2d["h_xy_all"], 1.0, 1.0, "B");

  fout->cd();
  fout->mkdir("histograms");
  fout->cd("histograms");
  
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

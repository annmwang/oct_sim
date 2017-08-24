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
#include <TCanvas.h>
#include <TRandom3.h>
#include <TLatex.h>
#include <TMath.h>
#include <GeoOctuplet.hh>
#include <Hit.hh>
#include <Road.hh>
#include <TStyle.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>

using namespace std;

TRandom3 *ran = new TRandom3;


bool db = false; // debug output flag

// SOME CONSTANTS

int NBOARDS = 8;
int NSTRIPS = 4000;
  
double xlow = 0.;
double xhigh = NSTRIPS*0.4-0.2;
double ylow = 0.;
double yhigh = 500.;

// octuplet
// int NBOARDS = 8;
// int NSTRIPS = 512;
  
// double xlow = 0.;
// double xhigh = NSTRIPS*0.4-0.2;
// double ylow = 17.9;
// double yhigh = 217.9;

double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));

int bc_wind = 8;
double mm_eff[8] = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9}; // i apologize for this array

double sig_art = 32.;

// road size

int XROAD = 8;
int UVFACTOR = 7; // 2~ small chamber
int UVROAD = XROAD*UVFACTOR;

// rates

//int bkgrate = 10; // Hz per square mm = 10 kHz/cm^2

// colors                                                                                                                                                                                   
string pink = "\033[38;5;205m";
string green = "\033[38;5;84m";
string blue = "\033[38;5;27m";
string ending = "\033[0m";
string warning = "\033[38;5;227;48;5;232m";

struct slope_t {
  int count;
  double mxl;
  double xavg;
  double yavg;
};


tuple<double,double> cosmic_angle(){
  return make_tuple(0.,0.);
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

tuple<double,double> generate_muon(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos){

  double x = ran->Uniform(xlow,xhigh);
  double y = ran->Uniform(ylow,yhigh);

  double thx, thy;

  std::tie(thx,thy) = cosmic_angle();

  double avgz = 0.5*(zpos[0]+zpos[NBOARDS-1]);
  double z, x_b, y_b;
  for (int j = 0; j < NBOARDS; j++){
    z = zpos[j];
    x_b = TMath::Tan(thx)*(zpos[j]-avgz)+x;
    y_b = TMath::Tan(thy)*(zpos[j]-avgz)+y;
    xpos[j] = x_b;
    ypos[j] = y_b;
  }    
  return make_tuple(x,y);
}

vector<Hit*> generate_bkg(int start_bc, const GeoOctuplet& geometry, int bkgrate){

  double plane_area = (xhigh-xlow) * (yhigh-ylow);
  vector<Hit*> bkghits;

  
  int noise_window = bc_wind * 3;
  int start_noise = start_bc - bc_wind;
  int end_noise = start_bc + bc_wind * 2 -1;

  //assume uniform distribution of background - correct for noise
  double bkgrate_bc = bkgrate / (4.*pow(10,7));
  double expbkg = bkgrate_bc * noise_window  * plane_area;


  for (int j = 0; j < NBOARDS; j++){
    int nbkg = ran->Poisson(expbkg);
    double x, y;
    for (int k = 0; k < nbkg; k++){
      x = ran->Uniform(xlow, xhigh);
      y = ran->Uniform(ylow, yhigh);
      Hit* newhit = nullptr;
      newhit = new Hit(j, start_noise+ran->Integer(end_noise+1-start_noise), x,y,false, geometry);
      bkghits.push_back(newhit);
    }
  }
  return bkghits;

}

vector<int> oct_response(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos){
  //gives detector response to muon, returns list of which planes registered hit
  
  int n_mm = 0;
  vector<int> oct_hitmask(NBOARDS,0);
  for (int j=0; j < NBOARDS; j++){
    if (ran->Uniform(0.,1.) < mm_eff[j]){
      oct_hitmask[j] = 1;
      n_mm++;
    }
  }
  return oct_hitmask;
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
      //cout << "bunch crossing: " << bc << endl;
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

      //cout << "nhits to be added: "<<hits_now.size() << endl;
      roads[i]->Add_Hits(hits_now, XROAD, UVFACTOR);

      if (roads[i]->Coincidence(bc_wind)){
        if (db){
        cout << "---------------------------" << endl;
        cout << "FOUND COINCIDENCE @ BC " << bc << endl;
        cout << "Road (i,count): ("<< roads[i]->iRoad() <<", " << roads[i]->Count()<<")" << endl;
        cout << "---------------------------" << endl;
        for (int k = 0; k < roads[i]->Hits().size(); k++){
          printf("Hit (board, BC, strip): (%d,%d,%4.4f)", roads[i]->Hits()[k].MMFE8Index(),roads[i]->Hits()[k].Age(),roads[i]->Hits()[k].Channel());
          cout << endl;
          //          cout << "Hit (board, BC, strip): (" << roads[i]->Hits()[k].MMFE8Index() <<", "<< roads[i]->Hits()[k].Age() << ", " << roads[i]->Hits()[k].Channel()<<")"<< endl;
        }
        }
        ntrigs++;
        slope_t m_slope;

        m_slope.count = roads[i]->Count();
        m_slope.mxl = roads[i]->Mxl();
        m_slope.xavg = roads[i]->AvgXofX();
        m_slope.yavg = -B*( roads[i]->AvgXofU() - roads[i]->AvgXofV() + (roads[i]->AvgZofV()-roads[i]->AvgZofU())*roads[i]->Mxl() ) / 2 + yhigh;
        slopes.push_back(m_slope);
      }
    }
  }
  return slopes;
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

int main(int argc, char* argv[]) {

  int nevents = -1;
  int bkgrate = -1; // Hz per square mm = 10 kHz/cm^2

  bool bkgflag = false;

  if ( argc < 3 ){
    cout << "Error at Input: please specify number of events to generate ";
    cout << "Example:   ./sim -n 100" << endl;
    cout << "Example:   ./sim -n 100 -b <bkg rate in Hz/mm^2>" << endl;
    return 0;
  }

  for (int i=1;i<argc-1;i++){
    if (strncmp(argv[i],"-n",2)==0){
      nevents = atoi(argv[i+1]);
    }
    if (strncmp(argv[i],"-b",2)==0){
      bkgrate = atoi(argv[i+1]);
      bkgflag = true;
    }
  }

  cout << endl;
  cout << blue << "--------------" << ending << endl;
  cout << blue << "OCT SIM ✪ ‿ ✪ " << ending << endl;
  cout << blue << "--------------" << ending << endl;
  cout << endl;
  cout << endl;
  printf("\r >> x-road size, uv-road size (in strips): (%d, %d)", XROAD, UVROAD);
  cout << endl;
  cout << "\r >> Using BCID window: " << bc_wind << endl;
  printf("\r >> Background rate of %d Hz per square mm",bkgrate);
  cout << endl;
  printf("\r >> Assuming chamber size: (%4.1f,%4.1f) in mm",xhigh-xlow, yhigh-ylow);
  cout << endl;
  cout << endl;

  if (nevents == -1){
    cout << "Didn't set the number of generated events! Exiting." << endl;
    return 0;
  }
  
  cout << pink << "Generating " << nevents << " events" << ending << endl;

  GeoOctuplet* GEOMETRY = new GeoOctuplet(true);

  // counters
  int nmuon_trig = 0;
  int neventtrig = 0;
  int extratrig = 0;
  bool muon_trig_ok = false;

  // book histos
  TH1F * h_mxres = new TH1F("h_mxres", "#DeltaX", 201, -100.5, 100.5);
  TH1F * h_yres = new TH1F("h_yres", "#DeltaY", 82, -20.5, 20.5);
  TH1F * h_xres = new TH1F("h_xres", "#DeltaX", 82, -20.5, 20.5);

  h_mxres->StatOverflows(kTRUE);
  h_yres->StatOverflows(kTRUE);
  h_xres->StatOverflows(kTRUE);

  time_t timer = time(NULL);
  time_t curr_time;
  for (int i = 0; i < nevents; i++){

    if (i % ((int)nevents/100) == 0){
      curr_time = time(NULL);
      progress(curr_time-timer, i, nevents);
    }

    muon_trig_ok = false;

    // generate muon
    double co = 2.7;
    vector<double> zpos = {-co, 11.2+co, 32.4-co, 43.6+co, 
                           113.6-co, 124.8+co, 146.0-co, 157.2+co};
    vector<double> xpos(NBOARDS,-1.);
    vector<double> ypos(NBOARDS,-1.);
    
    double xmuon,ymuon;
    std::tie(xmuon,ymuon) = generate_muon(xpos,ypos,zpos);

    if (db){
      printf("generated muon! @ (%4.4f,%4.4f)\n",xmuon,ymuon);
    }

    vector<int> oct_hitmask = oct_response(xpos,ypos,zpos);
  
    vector<int> art_bc(NBOARDS, -1.);
    double smallest_bc = 999999.;
    
    vector<Hit*> hits;

    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;
    
    double art_time;
  
    for (int j = 0; j < NBOARDS; j++){
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

    if (db)
      cout << "N muonhits: " << hits.size() << endl;
    
    if (n_x1 > 0 && n_x2 > 0 && n_u > 0 && n_v > 0){
      nmuon_trig++;
      muon_trig_ok= true; 
    }

    for (int j = 0; j < art_bc.size(); j++){
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

    vector<Road*> m_roads = create_roads(*GEOMETRY);
    if (db)
      cout << "Number of roads: " << m_roads.size() << endl;


    if (db)
      cout << "Total number of hits: " << all_hits.size() << endl;

    vector<slope_t> m_slopes = finder(all_hits, m_roads);
    int ntrigroads = m_slopes.size();
    if (db)
      cout << "Ntriggered roads: " << ntrigroads << endl;
    if (ntrigroads == 0)
      continue;
    slope_t myslope;
    myslope.mxl = 0.;
    myslope.count = 0;
    myslope.xavg = 0.;
    myslope.yavg = 0.;
    int myslopecount = 0;
    for (int j = 0; j < m_slopes.size(); j++){
      if (m_slopes[j].count > myslope.count){
        myslope.count = m_slopes[j].count;
        myslope.mxl = m_slopes[j].mxl;
        myslope.xavg = m_slopes[j].xavg;
        myslope.yavg = m_slopes[j].yavg;
      }
    }

    double deltaMX = TMath::ATan(myslope.mxl); // change to subtract angle of muon, which is 0 right now
    //cout << "delta x " << deltaMX*1000. << endl;
    if (db) {
      printf ("art (x,y): (%4.4f,%4.4f)", myslope.xavg, myslope.yavg);
      cout << endl;
      cout << endl;
    }
    //      cout << "art (x,y): (" << myslope.xavg << "," << myslope.yavg << ")"<< endl;
    h_mxres->Fill(deltaMX*1000.);
    h_yres->Fill(myslope.yavg-ymuon);
    h_xres->Fill(myslope.xavg-xmuon);

    if (muon_trig_ok)
      neventtrig++;
    else
      extratrig++;

  }
  cout << endl;
  cout << endl;
  cout << blue << "SIMULATION SUMMARY:" << ending << endl;
  cout << neventtrig << " muons triggered out of " << nmuon_trig << " muons that should trigger"<< endl;
  cout << extratrig << " extra trigger events " << endl;
  cout << endl;
  setstyle();


  // make directory                                                                                                                                                                           
  
  string dirname = "octsimplots_";
  if (!bkgflag)
    dirname = "octsimplots";
  else
    dirname += to_string(bkgrate);

  mkdir(dirname.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  chdir(dirname.c_str());  

  // plot dump!
  TCanvas * c = new TCanvas("c", "canvas", 800, 800);
  c->cd();

  // theta res
  h_mxres->GetXaxis()->SetTitle("#Delta#theta (mrad)");
  h_mxres->GetYaxis()->SetTitle("Events");
  h_mxres->GetYaxis()->SetTitleOffset(1.9);
  h_mxres->SetTitle("");
  h_mxres->SetLineColor(kTeal-5);
  h_mxres->SetFillColorAlpha(kTeal-5,0.4);
  h_mxres->Draw();

  TLatex* l1 = new TLatex();
  l1->SetTextSize(0.03);
  l1->SetTextColor(kRed);
  l1->SetTextAlign(21);
  l1->SetNDC();
  l1->DrawLatex(0.35,0.6,Form("RMS = %3.1f mrad",h_mxres->GetRMS()));
  c->Print("mxres.pdf");
  c->SetLogy(1);
  c->Print("mxres_log.pdf");
  c->SetLogy(0);
  c->Clear();


  h_yres->GetXaxis()->SetTitle("#Deltay (mm)");
  h_yres->GetYaxis()->SetTitle("Events");
  h_yres->GetYaxis()->SetTitleOffset(1.9);
  h_yres->SetTitle("");
  h_yres->SetLineColor(kGreen+3);
  h_yres->SetFillColorAlpha(kGreen+3,0.4);
  h_yres->Draw();
  l1->DrawLatex(0.35,0.6,Form("RMS = %3.1f mm",h_yres->GetRMS()));
  c->Print("yres.pdf");
  c->SetLogy(1);
  c->Print("yres_log.pdf");
  c->SetLogy(0);
  c->Clear();

  h_xres->GetXaxis()->SetTitle("#Deltax (mm)");
  h_xres->GetYaxis()->SetTitle("Events");
  h_xres->GetYaxis()->SetTitleOffset(1.9);
  h_xres->SetLineColor(kBlue-9);
  h_xres->SetFillColorAlpha(kBlue-9,0.4);
  h_xres->SetTitle("");
  h_xres->Draw();
  l1->DrawLatex(0.35,0.6,Form("RMS = %3.1f mm",h_xres->GetRMS()));
  c->Print("xres.pdf");
  c->SetLogy(1);
  c->Print("xres_log.pdf");
  c->SetLogy(0);

  return 0;
}

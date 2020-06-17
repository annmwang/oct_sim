/************** 

MAIN SIMULATION METHOD 

Original code by: Ann Wang and Alexander Tuna
Commented / Modified by: Anthony Badea (June 2020)

**************/


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

// Header includes
#include <SimUtilities.hh>
#include <ChamberUtilities.hh>
#include <ParticleUtilities.hh>
#include <GeneralUtilities.hh>
#include <Finder.hh>
#include <SimConstants.hh>
#include <SimNtupleData.hh>

int main(int argc, char* argv[]) {

  // Check the input from the user
  int good_input = read_parameters_from_user(argc, argv);
  if (!good_input) return 0;

  // Check that the parameters are sufficient for simulation
  int good_params = check_good_params();
  if (!good_params) return 0;

  // Check that the chamber is prepared for simulaiton
  set_chamber( string(chamberType), m_bcwind, m_sig_art, m_xroad, uvrflag, trapflag, m_NSTRIPS);
  int good_chamber = check_good_chamber();
  if(!good_chamber) return 0;

  // Print out the simulation parameters
  print_parameters();

  std::cout << pink << "Generating " << nevents << " events" << ending << std::endl;
  
  // ######################### ############################ ######################## //
  // ######################### CREATE OUTPUT FILE AND TTREE ######################## //

  TFile* fout = new TFile(outputFileName, "RECREATE");
  fout->mkdir("event_displays");

  // define output ntuple

  TTree* tree = new TTree("gingko","gingko");
  TTree* tree_args = new TTree("sim_args", "sim_args");
  SimNtupleData SN;
  SN.SetBranchData(tree);
  SN.SetBranchArgs(tree_args);

  // ######################### ######################### ######################### //
  // ######################### ######################### ######################### //



  // ######################### ########################## ######################## //
  // ######################### CONFIGURE CHAMBER GEOMETRY ######################## //

  double xlen = xhigh-xlow;
  double ylen = yhigh-ylow; 

  GeoOctuplet* GEOMETRY;
  if (string(chamberType) == "oct")
    GEOMETRY = new GeoOctuplet(false,xlen,ylen);
  else
    GEOMETRY = new GeoOctuplet(true,xlen,ylen);

  // ######################### ######################### ######################### //
  // ######################### ######################### ######################### //



  // ######################### ######################### ######################### //
  // ######################### SETUP SIMULATION COUNTERS ######################## //

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

  // ######################### ######################### ######################### //
  // ######################### ######################### ######################### //


  // UNCLEAR WHERE TO PUT THIS
  bool muon_trig_ok = false;


  // ######################### ########################### ######################### //
  // ######################### SETUP SIMULATION HISTOGRAMS ######################### //

  std::map< string, TH1F* > hists;
  std::map< string, TH2D* > hists_2d;
  
  //TH1F * h_mxres = new TH1F("h_mxres", "#Delta#Theta", 30, -1.5, 1.5);
  hists["h_mxres"] = new TH1F("h_mxres", "#Delta#Theta", 201, -100.5, 100.5);
  hists["h_yres"] = new TH1F("h_yres", "#DeltaY", 700, -3500, 3500);
  //TH1F * h_xres = new TH1F("h_xres", "#DeltaX", 50, -2.5, 2.5);
  hists["h_xres"] = new TH1F("h_xres", "#DeltaX", 123, -20.5, 20.5);
  hists["h_xres_center"] = new TH1F("h_xres_center", "#DeltaX", 123, -20.5, 20.5);
  hists["h_yres_center"] = new TH1F("h_yres_center", "#DeltaY", 700, -3500, 3500);
  hists["h_xres_strip"] = new TH1F("h_xres_strip", "#DeltaX", 200, -5, 5);

  hists["h_nmu"] = new TH1F("h_nmu", "h_nmu", 9, -0.5, 8.5);
  hists_2d["h_nmuvsdx"] = new TH2D("h_nmuvsdx", "h_nmuvsdx", 9, -0.5, 8.5,82, -20.5, 20.5);
  hists["h_dx"] = new TH1F("h_dx", "h_dx", 500, -3500,3500);
  hists["h_nuv_bkg"] = new TH1F("h_nuv_bkg", "", 5, -0.5, 4.5);
  hists["h_nx_bkg"] = new TH1F("h_nx_bkg", "", 5, -0.5, 4.5);
  hists_2d["h_xres_nxbkg"] = new TH2D("h_xres_nxbkg", "h_xres_nxbkg", 5, -0.5, 4.5 , 122, -30.5, 30.5);
  hists_2d["h_xres_nxmuon"] = new TH2D("h_xres_nxmuon", "h_xres_nxmuon", 5, -0.5, 4.5 , 122, -30.5, 30.5);
  hists["h_phi"]        = new TH1F("h_phi",        "", 200, -10, 10);
  hists["h_phi_trig"]   = new TH1F("h_phi_trig",   "", 200, -10, 10);
  hists["h_theta"]      = new TH1F("h_theta",      "", 200, -10, 10);
  hists["h_theta_trig"] = new TH1F("h_theta_trig", "", 200, -10, 10);

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

  // ######################### ########################### ######################### //
  // ######################### ########################### ######################### //



  // ######################### ########################### ######################### //
  // ######################### START OF SIMULATION PROCESS ######################### //

  vector<Road*> m_roads = create_roads(*GEOMETRY, uvrflag, m_xthr, m_uvthr, string(chamberType), trapflag);

  time_t timer = time(NULL);
  time_t curr_time;
  for ( int i = 0; i < nevents; i++) {

    if (write_tree){
      SN.EventNum++;
      SN.NEvent = nevents;
      SN.real_x_muon = -1;
      SN.real_y_muon = -1;
      SN.iRoad_x->clear();
      SN.iRoad_u->clear();
      SN.iRoad_v->clear();

      SN.Hit_strips->clear();
      SN.Hit_planes->clear();
      SN.Hit_ages->clear();
      SN.trigger_BC->clear();
      SN.N_muon->clear();
      SN.N_xmuon->clear();
      SN.trig_x->clear();
      SN.trig_y->clear();
      SN.dtheta->clear();


      // Added by Anthony Badea (June 2020)
      if( i == 0){
      	SN.bkgrate = bkgrate; // Hz per strip
    	SN.m_xroad = m_xroad; // size of x road in strips
    	SN.m_NSTRIPS = m_NSTRIPS; // number of x strips
	    SN.m_bcwind = m_bcwind; // fixed time window (in bunch crossings) during which the algorithm collects ART hits
	    SN.m_sig_art = m_sig_art; // art time resolution (in nanoseconds)
	    SN.killran = killran; // bool if you want to kill one plane randomly
	    SN.killxran = killxran; // bool if you want to kill one X plane randomly 
	    SN.killuvran = killuvran; // bool if you want to kill one U or V plane randomly 
	    SN.m_sig_art_x = m_sig_art_x; // ART position resolution (in strips). used to smear ART position
	    SN.mm_eff = &mm_eff; // efficiency of each PCB Left/Right of the MM
	    SN.m_xthr = m_xthr; // required total number of hits on all x channels combined required for a trigger. Used in create_roads function which uses Road.hh in include folder
	    SN.m_uvthr = m_uvthr; // required total number of hits on all u and v channels combined required for a trigger. Used in create_roads function which uses Road.hh in include folder
	    SN.bkgflag = bkgflag; // decides if background should be generated
	    SN.pltflag = pltflag; // decides if event displays should be plotted
	    SN.uvrflag = uvrflag; // decides if ??? used in set_chamber
	    SN.trapflag = trapflag; // decides if ??? used in create_roads
	    SN.ideal_tp = ideal_tp; // decides if ??? used in Road.hh
	    SN.ideal_vmm = ideal_vmm; // decides if ??? used in finder function
	    SN.ideal_addc = ideal_addc; // decides if ??? used in finder function
	    SN.write_tree = write_tree; // decides if an output TTree is produced
	    SN.bkgonly = bkgonly; // decides if only the background should be produced
	    SN.smear_art = smear_art; // decides if the arrival time of the ART hits due to muon tracks is smeared with a gaussian with a σ of 32 ns to emulate the ART time distribution
	    SN.funcsmear_art = funcsmear_art; // ONLY used if smear_art is false. Uses a custom smearing function rather than a gaussian. 
	    //SN.chamber = chamber; // Chamber value
      	tree_args->Fill();
      }
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
    
    double xmuon,ymuon,thx,thy;
    std::tie(xmuon,ymuon,thx,thy) = generate_muon(xpos, ypos, zpos, string(chamberType), angcos, angx, angy, trapflag);

    SN.real_x_muon = xmuon;
    SN.real_y_muon = ymuon;

    if (db){
      printf("generated muon! @ (%4.4f,%4.4f)\n",xmuon,ymuon);
    }
    hists_2d["h_xy_all"]->Fill(xmuon, ymuon);
    hists["h_theta"]->Fill(thx * 180/TMath::Pi());
    hists["h_phi"]  ->Fill(thy * 180/TMath::Pi());

	//################################################################//
	//################## Add up the number of hits ###################//
	// Modified by Anthony Badea on June 16, 2020
	// Input: detector response to a muon via oct_response(...)
    
    vector<int> oct_hitmask = oct_response(xpos, ypos, zpos, mm_eff);

    kill_random(killran, 
                killxran, 
                killuvran, 
                NPLANES, 
                NPCB_PER_PLANE, 
                ran, 
                oct_hitmask);

    vector<int> art_bc(NPLANES, -1.);
    vector<Hit*> hits;
    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;

    std::tie(art_bc,hits, n_x1, n_u, n_v, n_x2) = get_hits(NPLANES, 
                                                           NPCB_PER_PLANE, 
                                                           GEOMETRY, 
                                                           hists["h_xres_strip"], 
                                                           ran, 
                                                           sig_art,
                                                           m_sig_art_x,
                                                           bc_length,
                                                           xpos,
                                                           ypos,
                                                           smear_art,
                                                           funcsmear_art,
                                                           func,
                                                           bkgonly,
                                                           oct_hitmask);

    //################################################################//
    //################################################################//

    if (db){
      std::cout << "N muonhits: " << hits.size() << std::endl;
      for (unsigned int j = 0; j < hits.size(); j++){
        printf("Muon hit (board, BC, strip): (%d,%d,%4.4f)\n", hits[j]->MMFE8Index(),hits[j]->Age(),hits[j]->Channel());
      }
    }
    if (n_x1 > 0 && n_x2 > 0 && n_u > 0 && n_v > 0){
      nmuon_trig++;
      muon_trig_ok= true; 
    }

    double smallest_bc = 999999.;
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
        std::cout << "Nbkg hits: " << bkghits.size() << std::endl;
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
      std::cout << "Number of roads: " << m_roads.size() << std::endl;


    if (db)
      std::cout << "Total number of hits: " << all_hits.size() << std::endl;

    vector<slope_t> m_slopes;
    int ntrigroads;
    int ntrigroads_bkgonly = 0;

    std::tie(ntrigroads, m_slopes) = finder(all_hits, smallest_bc, m_roads, (pltflag||write_tree), ideal_vmm, ideal_addc, ideal_tp, i);
    hists["h_ntrig"]->Fill(ntrigroads);
    if (write_tree)
      SN.Ntriggers = ntrigroads;
    for (auto sl: m_slopes)
      if (sl.imuonhits == 0)
        hists_2d["h_xy_bkg"]->Fill(sl.xavg, sl.yavg);

    if (db)
      std::cout << "Ntriggered roads: " << ntrigroads << std::endl;
    if (ntrigroads == 0){
      //      if (db)
      if (angx < 0.1 && angy < 0.1 && !angcos)
        std::cout << "no triggered roads?" << std::endl;
      if (write_tree)
        tree->Fill();
      continue;
    }

    // got a trigger, but none with real hits
    if (m_slopes.size() == 0 && ntrigroads != 0 ){
      nevent_allnoise++;
//       if (db)
//         std::cout << "Didn't trigger with real hits:( " << std::endl;
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
        SN.iRoad_x->push_back(m_slopes[k].iroad);
        SN.iRoad_u->push_back(m_slopes[k].iroadu);
        SN.iRoad_v->push_back(m_slopes[k].iroadv);
        
        slopehits_ch.clear();
        slopehits_ages.clear();
        slopehits_planes.clear();
        
        for (unsigned int n = 0; n < m_slopes[k].slopehits.size(); n++) {
          slopehits_ch.push_back(m_slopes[k].slopehits[n].Channel());
          slopehits_planes.push_back(m_slopes[k].slopehits[n].MMFE8Index());
          slopehits_ages.push_back(m_slopes[k].slopehits[n].Age());
        }
        
        SN.trigger_BC->push_back(m_slopes[k].age);
        SN.N_muon->push_back(m_slopes[k].imuonhits);
        SN.N_xmuon->push_back(m_slopes[k].xmuon);
        SN.Hit_strips->push_back(slopehits_ch);
        SN.Hit_planes->push_back(slopehits_planes);
        SN.Hit_ages->push_back(slopehits_ages);
        SN.trig_x->push_back(m_slopes[k].xavg);
        SN.trig_y->push_back(m_slopes[k].yavg);
        SN.dtheta->push_back( TMath::ATan(m_slopes[k].mxl) );
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
      std::cout << std::endl;
      std::cout << std::endl;
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

    hists["h_theta_trig"]->Fill(thx * 180/TMath::Pi());
    hists["h_phi_trig"]  ->Fill(thy * 180/TMath::Pi());

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

  
  // ######################### END OF SIMULATION PROCESS ######################### //
  // ######################### ######################### ######################### //



  // ######################### ######################## ######################### //
  // ######################### LOG THE SIMULATION STATS ######################### //

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << blue << "SIMULATION SUMMARY:" << ending << std::endl;
  std::cout << neventtrig << " muons triggered out of " << nmuon_trig << " muons that should trigger"<< std::endl;
  std::cout << nevent_uvbkg << " triggers with spoiled uv hits"<< std::endl;
  std::cout << "n=1: " << nevent_uvbkg1 << " | n=2: "<< nevent_uvbkg2 << " | n=3: "<< nevent_uvbkg3 <<" | n=4: "<< nevent_uvbkg4 << std::endl;
  std::cout << nuv_bkg << " bkg uv hits total"<< std::endl;
  std::cout << extratrig << " extra trigger events " << std::endl;
  std::cout << nevent_allnoise << " events where triggers were only made with bkg hits" << std::endl;
  std::cout << std::endl;

  setstyle();



  ofstream mylog;
  string logtitle = string(outputFileName) + ".log.txt";
  mylog.open(logtitle);
  mylog << "LOG FILE FROM SIM_PCBEFF_MODULAR.C" << "\n";
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

  // ######################### ######################## ######################### //
  // ######################### ######################## ######################### //



  // ######################### ########################### ######################### //
  // ######################### FINISH WRITING OUTPUT FILES ######################### //

  // FYI: incompatible when batched. this is only for checks.
  hists_2d["h_xy_eff"]->Divide(hists_2d["h_xy_trig"], hists_2d["h_xy_all"], 1.0, 1.0, "B");

  fout->cd();
  tree_args->Write();
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

  // ######################### ######################## ######################### //
  // ######################### ######################## ######################### //

  return 0;
}

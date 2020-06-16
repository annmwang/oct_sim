#ifndef GeneralUtilities_HH
#define GeneralUtilities_HH

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


// Input:
// Output: 
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


// Input:
// Output: 
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


// Input:
// Output: 
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

#endif
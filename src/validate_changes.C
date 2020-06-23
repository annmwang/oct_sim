// c and c++ dependencies
#include <iostream>
#include <numeric>
#include <fstream>

// root dependencies
#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <TDatime.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TGaxis.h>

// local dependencies 
#include "SimNtupleData.hh"
#include "VectorDict.cxx"
#include "HttStyles.cc"
#include "styleUtil.h"
#include "vanGoghPalette.h"

int plot_sim(const std::string inFileName1, // First input file
             const std::string inFileName2 = "", // Second input file
             bool legacyFile1 = false, // Comparison to legacy simulation file
             bool legacyFile2 = false, // Comparison to legacy simulation file
			       const std::string outFileName = "outFileName.root", // Output file
			       const std::string treeName = "gingko", // Tree name for simulation data
             const std::string treeArgsName = "sim_args" // Tree name for simulation args
           )
{
  // ROOT Global settings
  TH1::SetDefaultSumw2();

  TFile *inFile1 = new TFile(inFileName1.c_str(), "READ");
  TTree *t1 = (TTree*)inFile1->Get(treeName.c_str());
  TTree *t1_args = (TTree*)inFile1->Get(treeArgsName.c_str());
  SimNtupleData SN1(1);
  SN1.SetAddressReadData(t1);
  SN1.InitializeHistsData_ALL();
 
  if(inFileName2 == "") inFileName2 = inFileName1;
  TFile *inFile2 = new TFile(inFileName2.c_str(), "READ");
  TTree *t2 = (TTree*)inFile2->Get(treeName.c_str());
  TTree *t2_args = (TTree*)inFile2->Get(treeArgsName.c_str());
  SimNtupleData SN2(2);
  SN2.SetAddressReadData(t2);
  SN2.InitializeHistsData_ALL();
  
  // Ratio plots
  SimNtupleData SNr(3);
  SNr.InitializeHistsData_ALL();
  
  // Difference plots
  SimNtupleData SNd(4);
  SNd.InitializeHistsData_ALL();
  

  // Initialize legacy mode setting for file 1
  if(!legacyFile1){
    SN1.SetAddressReadArgs(t1_args);
    SN1.InitializeHistsArgs_ALL();
    SN1.InitializeHistsMap_ALL();
  }
  else{
    SN1.InitializeHistsMap_DATA();
  }

  // Initialize legacy mode setting for file 2
  if(!legacyFile2){
    SN2.SetAddressReadArgs(t2_args);
    SN2.InitializeHistsArgs_ALL();
    SN2.InitializeHistsMap_ALL();
  }
  else{
    SN2.InitializeHistsMap_DATA();
  }

  // Initialize legacy mode setting for ratio and difference plots
  if(!legacyFile1 && !legacyFile2){
    SNr.InitializeHistsArgs_ALL();
    SNr.InitializeHistsMap_ALL();
    SNd.InitializeHistsArgs_ALL();
    SNd.InitializeHistsMap_ALL();
  }
  else{
    SNr.InitializeHistsMap_DATA();
    SNd.InitializeHistsMap_DATA();
  }


  static const unsigned int nevent = (int)t1->GetEntries();

  for( unsigned int i = 0; i < nevent; i++){
    // Print the event number
    if( i % 100 == 0 ) std::cout<<"Event: "<<i<<"/"<<nevent<<std::endl;

    // Get the next entry
    t1->GetEntry(i);
    t2->GetEntry(i);

    SN1.FillHistsData_ALL();
    SN2.FillHistsData_ALL();

    // Initialize legacy mode setting for file 1
    if(!legacyFile1 && i == 0){
      t1_args->GetEntry(i);
      SN1.FillHistsArgs_ALL();
    }

    if(!legacyFile2 && i == 0){
      t2_args->GetEntry(i);
      SN2.FillHistsArgs_ALL();
    }
  }

  // Create the output file and write histograms
  TFile *outFile = new TFile(outFileName.c_str(), "recreate");

  // Take ratio of the plots
  //static const int nHists = SN1.h_map.size();

  std::map<std::string, TH1D*> hr_map = SNr.h_map;
  std::map<std::string, TH1D*> hd_map = SNd.h_map;

  for( auto const& [key, val] : hd_map )
  {
    //hr_map[key.c_str()] = (TH1D*) val->Clone((Form("%s_ratio",key.c_str())));
    //hr_map[key.c_str()]->Divide(SN2.h_map[key.c_str()]);
    //hr_map[key.c_str()]->Divide(SN1.h_map[key.c_str()],SN2.h_map[key.c_str()]);

    //hd_map[key.c_str()] = new TH1D((Form("%s_diff",key.c_str())),(Form("%s_diff",key.c_str())), 100, -1, 1);
    int nbinsx = SN1.h_map[key.c_str()]->GetXaxis()->GetNbins();
    for( int bin = 0; bin < nbinsx; bin++){
      if( key == "EventNum" && bin >= SN1.h_map[key.c_str()]->GetEntries() ) break;
      else hd_map[key.c_str()]->SetBinContent(bin, SN1.h_map[key.c_str()]->GetBinContent(bin) - SN2.h_map[key.c_str()]->GetBinContent(bin));
      
      // fill the ratio histogram
      if(SN2.h_map[key.c_str()]->GetBinContent(bin) == 0){
        hr_map[key.c_str()]->SetBinContent(bin, 0);
      }
      else{
        hr_map[key.c_str()]->SetBinContent(bin, SN1.h_map[key.c_str()]->GetBinContent(bin)/SN2.h_map[key.c_str()]->GetBinContent(bin));
      }      
    }
    //hd_map[key.c_str()] = (TH1D*) val->Clone((Form("%s_diff",key.c_str())));
    //hd_map[key.c_str()]->Add(SN2.h_map[key.c_str()], -1);

    val->Print();
    SN2.h_map[key.c_str()]->Print();
  }

  /*
  for( auto hist : hr_map){
    hist->Print();
    hist->Write();
  }*/

  std::vector<std::string> listOfPdf;
  std::vector<std::string> listOfVar;
  const char *chamber_names[3];
  chamber_names[0] = "small";
  chamber_names[1] = "large";
  chamber_names[2] = "oct";

  TDatime* date = new TDatime();
  vanGoghPalette vGP = vanGoghPalette();
  const Double_t splitPoint = 0.5;
  for( auto const& [key, val] : hd_map ){
    if(hd_map[key.c_str()]->GetBinContent(hd_map[key.c_str()]->GetXaxis()->FindBin(0.0)) != hd_map[key.c_str()]->Integral()){
      printf("Only %f / %f entries are in the zero bin for hist %s \n",hd_map[key.c_str()]->GetBinContent(hd_map[key.c_str()]->GetXaxis()->FindBin(0.0)), hd_map[key.c_str()]->Integral(), hd_map[key.c_str()]->GetName());
    }
    hr_map[key.c_str()]->Write();

    hd_map[key.c_str()]->Write();
    //hist->Print();

    TGaxis::SetMaxDigits(3);

    TCanvas* canv_p = new TCanvas("canv_c", "canv_c", 2000, 1000);
    canv_p->SetTopMargin(0.1);
    canv_p->SetRightMargin(0.1);
    canv_p->SetLeftMargin(0.1);
    canv_p->SetBottomMargin(0.1);

    TPad* pad1_p = new TPad("pad1", "pad1", 0.0, splitPoint, 0.50, 1.0);
    pad1_p->Draw();
    pad1_p->SetTopMargin(0.1);
    pad1_p->SetRightMargin(0.1);
    pad1_p->SetBottomMargin(0.25);
    pad1_p->SetLeftMargin(0.2);

    TPad* pad2_p = new TPad("pad2", "pad2", 0.0, 0.0, 0.50, splitPoint);
    pad2_p->Draw();
    pad2_p->SetTopMargin(0.125);
    pad2_p->SetRightMargin(0.1);
    pad2_p->SetBottomMargin(0.25);
    pad2_p->SetLeftMargin(0.2);

    TPad* pad3_p = new TPad("pad3", "pad3", 0.5, 0.0, 1.0, 1.0);
    pad3_p->Draw();
    pad3_p->SetTopMargin(0.1);
    pad3_p->SetRightMargin(0.1);
    pad3_p->SetLeftMargin(0.15);
    pad3_p->SetBottomMargin(0.15);

    std::string xAxisLabel = key;
    if (xAxisLabel.find("_") < xAxisLabel.length()){
      xAxisLabel.replace(xAxisLabel.find("_"), 1, " ");
    }
    
    // make plots pretty
    InitHist(SN1.h_map[key.c_str()], xAxisLabel.c_str(), "Counts", kBlack, 1001,0);
    SN1.h_map[key.c_str()]->SetLineColorAlpha(vGP.getColor(1),0.85);
    InitHist(SN2.h_map[key.c_str()], xAxisLabel.c_str(), "Counts", kBlue, 1001,0);
    SN2.h_map[key.c_str()]->SetLineColorAlpha(vGP.getColor(2),0.85);
    InitHist(hr_map[key.c_str()], xAxisLabel.c_str(), "Ratio by Bin", kBlack, 1001,0);
    hr_map[key.c_str()]->SetMarkerColorAlpha(vGP.getColor(5),0.85);
    InitHist(hd_map[key.c_str()], xAxisLabel.c_str(), "Difference by Bin", kBlack, 1001,0);
    hd_map[key.c_str()]->SetLineColorAlpha(vGP.getColor(5),0.85);

    
    pad1_p->cd();
    setTH1Ratio(hd_map[key.c_str()], SN1.h_map[key.c_str()], 2);
    hd_map[key.c_str()]->GetXaxis()->SetNdivisions(210);
    hd_map[key.c_str()]->Draw("HIST E1");

    pad2_p->cd();
    setTH1Ratio(hr_map[key.c_str()], SN1.h_map[key.c_str()], 2);
    hr_map[key.c_str()]->SetMaximum(1.3);
    hr_map[key.c_str()]->SetMinimum(0.7);
    hr_map[key.c_str()]->SetMarkerSize (1.15);
    hr_map[key.c_str()]->Draw("HIST P");

    pad3_p->cd();
    
    int lowBin = TMath::Min(SN1.h_map[key.c_str()]->FindFirstBinAbove(), SN2.h_map[key.c_str()]->FindFirstBinAbove())-1;
    int highBin = TMath::Max(SN1.h_map[key.c_str()]->FindLastBinAbove(), SN2.h_map[key.c_str()]->FindLastBinAbove())+1;
    float yMax = 1.2*TMath::Max(SN1.h_map[key.c_str()]->GetMaximum(), SN2.h_map[key.c_str()]->GetMaximum());

    SN1.h_map[key.c_str()]->SetMaximum(yMax);
    SN1.h_map[key.c_str()]->GetXaxis()->SetRange(lowBin,highBin);
    SN2.h_map[key.c_str()]->SetMaximum(yMax);
    SN2.h_map[key.c_str()]->GetXaxis()->SetRange(lowBin,highBin);
    //SN2.h_map[key.c_str()]->SetMaximum(1.2*TMath::Max(SN1.h_map[key.c_str()]->GetMaximum(), SN2.h_map[key.c_str()]->GetMaximum()));

    SN1.h_map[key.c_str()]->GetXaxis()->SetNdivisions(510);
    setTH1Final(SN1.h_map[key.c_str()]);
    SN2.h_map[key.c_str()]->GetXaxis()->SetNdivisions(510);
    setTH1Final(SN2.h_map[key.c_str()]);

    if(key == "EventNum"){ 
      SN1.h_map[key.c_str()]->Draw("HIST P"); 
      SN2.h_map[key.c_str()]->Draw("HIST P SAME"); 
    }
    else{ 
      SN1.h_map[key.c_str()]->Draw("HIST E1"); 
      SN2.h_map[key.c_str()]->Draw("HIST E1 SAME"); 
    }

    TLegend *leg = new TLegend();
    setLegendPosition(leg,"NW",pad1_p,0.20,0.75,0,0,0);
    setLegendFinal(leg);
    leg->AddEntry(SN1.h_map[key.c_str()],"File 1","l");\
    leg->AddEntry(SN2.h_map[key.c_str()],"File 2","l");
    leg->Draw();


    std::string pdfStr = key + "_" + std::to_string(date->GetDate()) + ".pdf";
    canv_p->SaveAs(("pdfDir/" + pdfStr).c_str());
    listOfPdf.push_back(pdfStr);
    listOfVar.push_back(key.c_str());

    //delete hist;
  }

  std::string inFile1TexStr = inFileName1;
  std::string inFile2TexStr = inFileName2;

  std::string tempStr;
  while(inFile1TexStr.find("_") != std::string::npos){
    tempStr = tempStr + inFile1TexStr.substr(0, inFile1TexStr.find("_"));
    tempStr = tempStr + "\\_";
    inFile1TexStr.replace(0, inFile1TexStr.find("_")+1, "");
  }
  tempStr = tempStr + inFile1TexStr;
  inFile1TexStr = tempStr;

  //if(doLocalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;

  tempStr = "";
  while(inFile2TexStr.find("_") != std::string::npos){
    tempStr = tempStr + inFile2TexStr.substr(0, inFile2TexStr.find("_"));
    tempStr = tempStr + "\\_";
    inFile2TexStr.replace(0, inFile2TexStr.find("_")+1, "");
  }
  tempStr = tempStr + inFile2TexStr;
  inFile2TexStr = tempStr;


  std::string inFileNameCombo = inFileName1;
  while(inFileNameCombo.find("/") != std::string::npos) inFileNameCombo.replace(0, inFileNameCombo.find("/")+1, "");
  while(inFileNameCombo.find(".root") != std::string::npos) inFileNameCombo.replace(inFileNameCombo.find(".root"), 5, "");
  
  std::string appOutFileName = inFileName2;
  while(appOutFileName.find("/") != std::string::npos) appOutFileName.replace(0, appOutFileName.find("/")+1, "");
  inFileNameCombo = inFileNameCombo + "_" + appOutFileName;

  while(inFileNameCombo.find(".root") != std::string::npos) inFileNameCombo.replace(inFileNameCombo.find(".root"), 5, "");

  const std::string outTexFileName = "pdfDir/" + inFileNameCombo + "_" + std::to_string(date->GetDate()) + ".tex";
  std::ofstream fileTex(outTexFileName.c_str());

  fileTex << "\\RequirePackage{xspace}" << std::endl;
  fileTex << "\\RequirePackage{amsmath}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\documentclass[xcolor=dvipsnames]{beamer}" << std::endl;
  fileTex << "\\usetheme{Warsaw}" << std::endl;
  fileTex << "\\setbeamercolor{structure}{fg=NavyBlue!90!NavyBlue}" << std::endl;
  fileTex << "\\setbeamercolor{footlinecolor}{fg=white,bg=lightgray}" << std::endl;
  fileTex << "\\newcommand{\\pt}{\\ensuremath{p_{\\mathrm{T}}}\\xspace}" << std::endl;
  fileTex << "\\setbeamersize{text margin left=5pt,text margin right=5pt}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\setbeamertemplate{frametitle}" << std::endl;
  fileTex << "{" << std::endl;
  fileTex << "  \\nointerlineskip" << std::endl;
  fileTex << "  \\begin{beamercolorbox}[sep=0.3cm, ht=1.8em, wd=\\paperwidth]{frametitle}" << std::endl;
  fileTex << "    \\vbox{}\\vskip-2ex%" << std::endl;
  fileTex << "    \\strut\\insertframetitle\\strut" << std::endl;
  fileTex << "    \\vskip-0.8ex%" << std::endl;
  fileTex << "  \\end{beamercolorbox}" << std::endl;
  fileTex << "}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\setbeamertemplate{footline}{%" << std::endl;
  fileTex << "  \\begin{beamercolorbox}[sep=.8em,wd=\\paperwidth,leftskip=0.5cm,rightskip=0.5cm]{footlinecolor}" << std::endl;
  fileTex << "    \\hspace{0.3cm}%" << std::endl;
  fileTex << "    \\hfill\\insertauthor \\hfill\\insertpagenumber" << std::endl;
  fileTex << "  \\end{beamercolorbox}%" << std::endl;
  fileTex << "}" << std::endl;

  fileTex << std::endl;
  
  fileTex << "\\setbeamertemplate{navigation symbols}{}" << std::endl;
  fileTex << "\\setbeamertemplate{itemize item}[circle]" << std::endl;
  fileTex << "\\setbeamertemplate{itemize subitem}[circle]" << std::endl;
  fileTex << "\\setbeamertemplate{itemize subsubitem}[circle]" << std::endl;
  fileTex << "\\setbeamercolor{itemize item}{fg=black}" << std::endl;
  fileTex << "\\setbeamercolor{itemize subitem}{fg=black}" << std::endl;
  fileTex << "\\setbeamercolor{itemize subsubitem}{fg=black}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\definecolor{links}{HTML}{00BFFF}" << std::endl;
  fileTex << "\\hypersetup{colorlinks,linkcolor=,urlcolor=links}" << std::endl;

  fileTex << std::endl;

  fileTex << "\\author[CM]{Anthony Badea -- MM Trigger Simulation Validation}" << std::endl;
  fileTex << "\\begin{document}" << std::endl;

  fileTex << std::endl;

  // First page of argument parameters
  fileTex << "\\begin{frame}" << std::endl;
  fileTex << "\\frametitle{\\centerline{MM Trigger Simulation Validation (" << date->GetYear() << "." << date->GetMonth() << "." << date->GetDay() << ")}}" << std::endl;
  fileTex << " \\begin{itemize}" << std::endl;
  fileTex << "  \\fontsize{8}{8}\\selectfont" << std::endl;
  fileTex << "  \\item{" << "File 1: " << inFile1TexStr << "}" << std::endl;
  fileTex << "  \\item{" << "File 2: " << inFile2TexStr << "}" << std::endl;
  fileTex << "\\newline" << std::endl;
  if(legacyFile1) fileTex << "Legacy mode for file 1 turned on. Don't trust file 1's column below." << std::endl;
  if(legacyFile2) fileTex << "Legacy mode for file 2 turned on. Don't trust file 2's column below." << std::endl;
  fileTex << "\\newline" << std::endl;
  fileTex << "\\begin{table}[t]\\centering" << std::endl;
  fileTex << "\\begin{tabular}{c|c|c}" << std::endl;
  //fileTex << "\\hline" << std::endl;
  fileTex << "Parameter & File 1 & File 2 \\\\" << std::endl;
  fileTex << "\\hline" << std::endl;
  fileTex << "N Events &" << SN1.NEvent << " & " << SN2.NEvent << "\\\\" << std::endl;
  fileTex << "Background rate &" << SN1.bkgrate << " & " << SN2.bkgrate << "\\\\" << std::endl;
  fileTex << "Chamber Type &" << chamber_names[SN1.chamber] << " & " << chamber_names[SN2.chamber] << "\\\\" << std::endl;
  fileTex << "Size of x road in strips &" << SN1.m_xroad << " & " << SN2.m_xroad << "\\\\" << std::endl;
  fileTex << "Number of x strips &" << SN1.m_NSTRIPS << " & " << SN2.m_NSTRIPS << "\\\\" << std::endl;
  fileTex << "Algorithm collection window (in bc) &" << SN1.m_bcwind << " & " << SN2.m_bcwind << "\\\\" << std::endl;
  fileTex << "Art time resolution &" << SN1.m_sig_art << " & " << SN2.m_sig_art << "\\\\" << std::endl;
  fileTex << "Kill one plane randomly &" << (SN1.killran ? "true" : "false") << " & " << (SN2.killran ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Kill one X plane randomly &" << (SN1.killxran ? "true" : "false") << " & " << (SN2.killxran ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Kill one U or V plane randomly &" << (SN1.killuvran ? "true" : "false") << " & " << (SN2.killuvran ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "X Hit Threshold &" << SN1.m_xthr << " & " << SN2.m_xthr << "\\\\" << std::endl;
  fileTex << "UV Hit Threshold &" << SN1.m_uvthr << " & " << SN2.m_uvthr << "\\\\" << std::endl;
  fileTex << "TRandom Seed &" << SN1.seed << " & " << SN2.seed << "\\\\" << std::endl;
  fileTex << "\\end{tabular}" << std::endl;
  fileTex << "\\caption{Simulation parameters.}" << std::endl;
  fileTex << "\\label{}" << std::endl;
  fileTex << "\\end{table}" << std::endl;
  fileTex << " \\end{itemize}" << std::endl;
  fileTex << "\\end{frame}" << std::endl;

  fileTex << std::endl;

  // Second page of argument parameters
  fileTex << "\\begin{frame}" << std::endl;
  fileTex << "\\frametitle{\\centerline{MM Trigger Simulation Validation (" << date->GetYear() << "." << date->GetMonth() << "." << date->GetDay() << ")}}" << std::endl;
  fileTex << " \\begin{itemize}" << std::endl;
  fileTex << "  \\fontsize{8}{8}\\selectfont" << std::endl;
  fileTex << "  \\item{" << "File 1: " << inFile1TexStr << "}" << std::endl;
  fileTex << "  \\item{" << "File 2: " << inFile2TexStr << "}" << std::endl;
  fileTex << "\\newline" << std::endl;
  if(legacyFile1) fileTex << "Legacy mode for file 1 turned on. Don't trust file 1's column below." << std::endl;
  if(legacyFile2) fileTex << "Legacy mode for file 2 turned on. Don't trust file 2's column below." << std::endl;  
  fileTex << "\\newline" << std::endl;
  fileTex << "\\begin{table}[t]\\centering" << std::endl;
  fileTex << "\\begin{tabular}{c|c|c}" << std::endl;
  //fileTex << "\\hline" << std::endl;
  fileTex << "Parameter & File 1 & File 2 \\\\" << std::endl;
  fileTex << "\\hline" << std::endl;
  fileTex << "Generate Background &" << (SN1.bkgflag ? "true" : "false") << " & " << (SN2.bkgflag ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Plot Event Displays &" << (SN1.pltflag ? "true" : "false") << " & " << (SN2.pltflag ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "UVR Flag &" << (SN1.uvrflag ? "true" : "false") << " & " << (SN2.uvrflag ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Trap Flag &" << (SN1.trapflag ? "true" : "false") << " & " << (SN2.trapflag ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Ideal TP Flag &" << (SN1.ideal_tp ? "true" : "false") << " & " << (SN2.ideal_tp ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Ideal VMM Flag &" << (SN1.ideal_vmm ? "true" : "false") << " & " << (SN2.ideal_vmm ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Ideal ADDC Flag &" << (SN1.ideal_addc ? "true" : "false") << " & " << (SN2.ideal_addc ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Write Tree &" << (SN1.write_tree ? "true" : "false") << " & " << (SN2.write_tree ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Background Only &" << (SN1.bkgonly ? "true" : "false") << " & " << (SN2.bkgonly ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Smear ART Arrival Time &" << (SN1.smear_art ? "true" : "false") << " & " << (SN2.smear_art ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Use Custom ART Smear Fuction &" << (SN1.funcsmear_art ? "true" : "false") << " & " << (SN2.funcsmear_art ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "Legacy Mode &" << (SN1.legacy ? "true" : "false") << " & " << (SN2.legacy ? "true" : "false") << "\\\\" << std::endl;
  fileTex << "\\end{tabular}" << std::endl;
  fileTex << "\\hfill" << std::endl;
  fileTex << "\\caption{Simulation parameters.}" << std::endl;
  fileTex << "\\label{}" << std::endl;
  fileTex << "\\end{table}" << std::endl;
  fileTex << " \\end{itemize}" << std::endl;
  fileTex << "\\end{frame}" << std::endl;

  // Third page of argument parameters for MM efficiencies
  
  fileTex << "\\begin{frame}" << std::endl;
  fileTex << "\\frametitle{\\centerline{MM Trigger Simulation Validation (" << date->GetYear() << "." << date->GetMonth() << "." << date->GetDay() << ")}}" << std::endl;
  fileTex << " \\begin{itemize}" << std::endl;
  fileTex << "  \\fontsize{8}{8}\\selectfont" << std::endl;
  fileTex << "  \\item{" << "File 1: " << inFile1TexStr << "}" << std::endl;
  fileTex << "  \\item{" << "File 2: " << inFile2TexStr << "}" << std::endl;
  fileTex << "\\newline" << std::endl;
  if(legacyFile1) fileTex << "Legacy mode for file 1 turned on. Don't trust file 1's column below." << std::endl;
  if(legacyFile2) fileTex << "Legacy mode for file 2 turned on. Don't trust file 2's column below." << std::endl;  
  fileTex << "\\newline" << std::endl;
  
  
  if(!legacyFile1 && !legacyFile2 && SN1.mm_eff->size() == SN2.mm_eff->size()){
    fileTex << "\\begin{table}[t]\\centering" << std::endl;
    fileTex << "\\begin{tabular}{c|c|c}" << std::endl;
    //fileTex << "\\hline" << std::endl;
    fileTex << "Location & Efficiency File 1 & Efficiency File 2 \\\\" << std::endl;
    fileTex << "\\hline" << std::endl;
    for( int i = 0; i < SN1.mm_eff->size(); i++){
      if(SN1.mm_eff->at(i) < 0 || SN1.mm_eff->at(i) > 1 || SN2.mm_eff->at(i) < 0 || SN2.mm_eff->at(i) > 1) break;
      if(SN1.mm_eff->size() == 8){
        fileTex << "Layer " << i << " & " << SN1.mm_eff->at(i) << " & " << SN2.mm_eff->at(i) << "\\\\" << std::endl;
      }
      else if(SN1.mm_eff->size() == 64){
        fileTex << "Layer " << i/8 << ", PCB " << i % 8 << " & " << SN1.mm_eff->at(i) << " & " << SN2.mm_eff->at(i) << "\\\\" << std::endl;
      }
      else if(SN1.mm_eff->size() == 128){
        std::string leftRight = "right";
        if( i % 2 == 0) leftRight = "left";
        fileTex << "Layer " << i/8.0 << ", PCB " << i % 8 << " " << leftRight << " & " << SN1.mm_eff->at(i) << " & " << SN2.mm_eff->at(i) << "\\\\" << std::endl;
      }
    }
    fileTex << "\\end{tabular}" << std::endl;
    fileTex << "\\hfill" << std::endl;
    fileTex << "\\caption{Simulation parameters.}" << std::endl;
    fileTex << "\\label{}" << std::endl;
    fileTex << "\\end{table}" << std::endl;
  }
  else{
    fileTex << "The two files have different length efficiency lists! Check what you are trying to compare." << std::endl;
  }
  fileTex << " \\end{itemize}" << std::endl;
  fileTex << "\\end{frame}" << std::endl;

  fileTex << std::endl;


  for(unsigned int i = 0; i < listOfPdf.size(); ++i){
    std::string varStr = listOfVar.at(i);
    std::string newVarStr;
    while(varStr.find("_") != std::string::npos){
      newVarStr = newVarStr + varStr.substr(0, varStr.find("_"));
      newVarStr = newVarStr + "\\_";
      varStr.replace(0, varStr.find("_")+1, "");
    }
    newVarStr = newVarStr + varStr;

    fileTex << std::endl;
    fileTex << "\\begin{frame}" << std::endl;
    fileTex << "\\frametitle{\\centerline{" << newVarStr << "}}" << std::endl;
    fileTex << "\\begin{center}" << std::endl;
    fileTex << "\\includegraphics[width=\\textwidth]{" << listOfPdf.at(i) << "}" << std::endl;
    fileTex << "\\end{center}" << std::endl;
    fileTex << "\\begin{itemize}" << std::endl;
    fileTex << "\\fontsize{8}{8}\\selectfont" << std::endl;
    fileTex << "\\item{" << "File 1" << ", Entries: " << SN1.h_map[listOfVar.at(i).c_str()]->GetEntries() << ", Mean: " << SN1.h_map[listOfVar.at(i).c_str()]->GetMean() << ", Std Dev: " << SN1.h_map[listOfVar.at(i).c_str()]->GetStdDev() << "}" << std::endl;
    fileTex << "\\item{" << "File 2" << ", Entries: " << SN2.h_map[listOfVar.at(i).c_str()]->GetEntries() << ", Mean: " << SN2.h_map[listOfVar.at(i).c_str()]->GetMean() << ", Std Dev: " << SN2.h_map[listOfVar.at(i).c_str()]->GetStdDev() << "}" << std::endl;
    fileTex << "\\item{" << "Difference by bin" << ", Entries: " << hd_map[listOfVar.at(i).c_str()]->GetEntries() << ", Mean: " << hd_map[listOfVar.at(i).c_str()]->GetMean() << ", Std Dev: " << hd_map[listOfVar.at(i).c_str()]->GetStdDev() << "}" << std::endl;
    fileTex << "\\item{" << "Ratio by bin" << ", Entries: " << hr_map[listOfVar.at(i).c_str()]->GetEntries() << ", Mean: " << hr_map[listOfVar.at(i).c_str()]->GetMean() << ", Std Dev: " << hr_map[listOfVar.at(i).c_str()]->GetStdDev() << "  (Inifite bins set to 0)" << "}" << std::endl;
    fileTex << "\\end{itemize}" << std::endl;
    fileTex << "\\end{frame}" << std::endl;
  }

  //if(doLocalDebug) std::cout << __FILE__ << ", " << __LINE__ << std::endl;
  
  fileTex << std::endl;
  fileTex << "\\end{document}" << std::endl;

  fileTex.close();

  outFile->Close();
  delete outFile;
 
  SN2.DeleteAllHists();
  SN1.DeleteAllHists();

  delete t2_args;
  delete t2;
  delete inFile2;

  delete t1_args;
  delete t1;
  delete inFile1;

  return 1;
}


int main(int argc, char*argv[]){

  int retVal = 0;
  if(argc == 2) { retVal += plot_sim(argv[1]); }
  else if(argc == 3) { retVal += plot_sim(argv[1], argv[2]); }

  std::cout<<__LINE__<<std::endl;
  return retVal;
}
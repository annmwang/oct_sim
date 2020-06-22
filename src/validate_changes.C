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

// local dependencies 
#include "../include/SimNtupleData.hh"
#include "VectorDict.cxx"



int plot_sim(const std::string inFileName1, // First input file
             const std::string inFileName2, // Second input file
             bool legacy = true, // Comparison to legacy simulation file
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
  

  // If not legacy mode
  if(!legacy){
    SN1.SetAddressReadArgs(t1_args);
    SN2.SetAddressReadArgs(t2_args);
    SN1.InitializeHistsArgs_ALL();
    SN1.InitializeHistsMap_ALL();
    SN2.InitializeHistsArgs_ALL();
    SN2.InitializeHistsMap_ALL();
    SNr.InitializeHistsArgs_ALL();
    SNr.InitializeHistsMap_ALL();
    SNd.InitializeHistsArgs_ALL();
    SNd.InitializeHistsMap_ALL();
  }
  else{
    SN1.InitializeHistsMap_DATA();
    SN2.InitializeHistsMap_DATA();
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

    if( !legacy && i == 0){
      t1_args->GetEntry(i);
      t2_args->GetEntry(i);

      SN1.FillHistsArgs_ALL();
      SN2.FillHistsArgs_ALL();
    }
  }

  // Create the output file and write histograms
  TFile *outFile = new TFile(outFileName.c_str(), "recreate");

  // Take ratio of the plots
  //static const int nHists = SN1.h_map.size();

  std::map<std::string, TH1D*> hr_map;
  std::map<std::string, TH1D*> hd_map;

  for( auto const& [key, val] : SN1.h_map )
  {
    hr_map[key.c_str()] = (TH1D*) val->Clone((Form("%s_ratio",key.c_str())));
    hr_map[key.c_str()]->Divide(SN2.h_map[key.c_str()]);

    hd_map[key.c_str()] = new TH1D((Form("%s_diff",key.c_str())),(Form("%s_diff",key.c_str())), 100, -1, 1);
    int nbinsx = SN1.h_map[key.c_str()]->GetXaxis()->GetNbins();
    for( int bin = 0; bin < nbinsx; bin++){
      hd_map[key.c_str()]->Fill( SN1.h_map[key.c_str()]->GetBinContent(bin) - SN2.h_map[key.c_str()]->GetBinContent(bin));
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
  TDatime* date = new TDatime();
  const Double_t splitPoint = 0.35;
  for( auto const& [key, val] : hd_map ){
    if(hd_map[key.c_str()]->GetBinContent(hd_map[key.c_str()]->GetXaxis()->FindBin(0.0)) != hd_map[key.c_str()]->Integral()){
      printf("Only %f / %f entries are in the zero bin for hist %s \n",hd_map[key.c_str()]->GetBinContent(hd_map[key.c_str()]->GetXaxis()->FindBin(0.0)), hd_map[key.c_str()]->Integral(), hd_map[key.c_str()]->GetName());
    }
    hr_map[key.c_str()]->Write();
    hd_map[key.c_str()]->Write();
    //hist->Print();



    TCanvas* canv_p = new TCanvas("canv_c", "canv_c", 1000, 500);
    canv_p->SetTopMargin(0.01);
    canv_p->SetRightMargin(0.01);
    canv_p->SetLeftMargin(0.01);
    canv_p->SetBottomMargin(0.01);

    TPad* pad1_p = new TPad("pad1", "pad1", 0.0, splitPoint, 0.5, 1.0);
    pad1_p->Draw();
    pad1_p->SetTopMargin(0.01);
    pad1_p->SetRightMargin(0.01);
    pad1_p->SetBottomMargin(0.01);
    pad1_p->SetLeftMargin(pad1_p->GetLeftMargin()*1.3);

    TPad* pad2_p = new TPad("pad2", "pad2", 0.0, 0.0, 0.5, splitPoint);
    pad2_p->Draw();
    pad2_p->SetTopMargin(0.01);
    pad2_p->SetRightMargin(0.01);
    pad2_p->SetBottomMargin(pad1_p->GetLeftMargin()*1./splitPoint);
    pad2_p->SetLeftMargin(pad1_p->GetLeftMargin());

    TPad* pad3_p = new TPad("pad3", "pad3", 0.5, 0.0, 1.0, 1.0);
    pad3_p->Draw();
    pad3_p->SetTopMargin(0.01);
    pad3_p->SetRightMargin(0.01);
    pad3_p->SetLeftMargin(pad1_p->GetLeftMargin());
    pad3_p->SetBottomMargin(pad1_p->GetLeftMargin());

    pad1_p->cd();

    SN1.h_map[key.c_str()]->GetXaxis()->SetTitleFont(43);
    SN2.h_map[key.c_str()]->GetXaxis()->SetTitleFont(43);
    hr_map[key.c_str()]->GetXaxis()->SetTitleFont(43);
    hd_map[key.c_str()]->GetXaxis()->SetTitleFont(43);
    SN1.h_map[key.c_str()]->GetXaxis()->SetTitleSize(20);
    SN2.h_map[key.c_str()]->GetXaxis()->SetTitleSize(20);
    hr_map[key.c_str()]->GetXaxis()->SetTitleSize(20);
    hd_map[key.c_str()]->GetXaxis()->SetTitleSize(20);

    SN1.h_map[key.c_str()]->GetYaxis()->SetTitleFont(43);
    SN2.h_map[key.c_str()]->GetYaxis()->SetTitleFont(43);
    hr_map[key.c_str()]->GetYaxis()->SetTitleFont(43);
    hd_map[key.c_str()]->GetYaxis()->SetTitleFont(43);
    SN1.h_map[key.c_str()]->GetYaxis()->SetTitleSize(20);
    SN2.h_map[key.c_str()]->GetYaxis()->SetTitleSize(20);
    hr_map[key.c_str()]->GetYaxis()->SetTitleSize(20);
    hd_map[key.c_str()]->GetYaxis()->SetTitleSize(20);


    SN1.h_map[key.c_str()]->GetXaxis()->SetLabelFont(43);
    SN2.h_map[key.c_str()]->GetXaxis()->SetLabelFont(43);
    hr_map[key.c_str()]->GetXaxis()->SetLabelFont(43);
    hd_map[key.c_str()]->GetXaxis()->SetLabelFont(43);
    SN1.h_map[key.c_str()]->GetXaxis()->SetLabelSize(20);
    SN2.h_map[key.c_str()]->GetXaxis()->SetLabelSize(20);
    hr_map[key.c_str()]->GetXaxis()->SetLabelSize(20);
    hd_map[key.c_str()]->GetXaxis()->SetLabelSize(20);

    SN1.h_map[key.c_str()]->GetYaxis()->SetLabelFont(43);
    SN2.h_map[key.c_str()]->GetYaxis()->SetLabelFont(43);
    hr_map[key.c_str()]->GetYaxis()->SetLabelFont(43);
    hd_map[key.c_str()]->GetYaxis()->SetLabelFont(43);
    SN1.h_map[key.c_str()]->GetYaxis()->SetLabelSize(20);
    SN2.h_map[key.c_str()]->GetYaxis()->SetLabelSize(20);
    hr_map[key.c_str()]->GetYaxis()->SetLabelSize(20);
    hd_map[key.c_str()]->GetYaxis()->SetLabelSize(20);

    SN1.h_map[key.c_str()]->GetYaxis()->SetTitleOffset(SN1.h_map[key.c_str()]->GetYaxis()->GetTitleOffset()*3.);
    hr_map[key.c_str()]->GetYaxis()->SetTitleOffset(SN1.h_map[key.c_str()]->GetYaxis()->GetTitleOffset());
    hr_map[key.c_str()]->GetXaxis()->SetTitleOffset(hr_map[key.c_str()]->GetXaxis()->GetTitleOffset()*3.);

    SN1.h_map[key.c_str()]->SetMaximum(1.2*TMath::Max(SN1.h_map[key.c_str()]->GetMaximum(), SN2.h_map[key.c_str()]->GetMaximum()));

    SN1.h_map[key.c_str()]->DrawCopy("HIST E1");
    SN2.h_map[key.c_str()]->DrawCopy("SAME *HIST E1");

    pad2_p->cd();
    hr_map[key.c_str()]->SetMaximum(1.3);
    hr_map[key.c_str()]->SetMinimum(0.7);
    hr_map[key.c_str()]->DrawCopy("P");

    pad3_p->cd();
    hd_map[key.c_str()]->DrawCopy("HIST E1");
    gPad->SetLogy();

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

  fileTex << "\\begin{frame}" << std::endl;
  fileTex << "\\frametitle{\\centerline{Sample Validation (" << date->GetYear() << "." << date->GetMonth() << "." << date->GetDay() << ")}}" << std::endl;
  fileTex << " \\begin{itemize}" << std::endl;
  fileTex << "  \\fontsize{8}{8}\\selectfont" << std::endl;
  fileTex << "  \\item{" << inFile1TexStr << "}" << std::endl;
  fileTex << "  \\item{" << inFile2TexStr << "}" << std::endl;
  fileTex << " \\end{itemize}" << std::endl;
  fileTex << "\\end{frame}" << std::endl;

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
    fileTex << "\\includegraphics[width=0.8\\textwidth]{" << listOfPdf.at(i) << "}" << std::endl;
    fileTex << "\\end{center}" << std::endl;
    fileTex << "\\begin{itemize}" << std::endl;
    fileTex << "\\fontsize{8}{8}\\selectfont" << std::endl;
    fileTex << "\\item{" << newVarStr << "}" << std::endl;
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
  if(argc == 2) { 
    std::cout<<"BAD INPUT NEED 2 FILES"<<std::endl;
    retVal += -1; 
  }
  else if(argc == 3) { retVal += plot_sim(argv[1], argv[2]); }

  std::cout<<__LINE__<<std::endl;
  return retVal;
}
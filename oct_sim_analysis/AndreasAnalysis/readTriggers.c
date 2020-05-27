{
#include <vector>
    
    TFile *myfile;
    Double_t xReal;
    Double_t yReal;
    TString bkg;
    Int_t numb;
    Int_t nentries;
    std::vector<Double_t> *ratio = new std::vector<Double_t>();
    std::vector<Double_t> *xTrig = new std::vector<Double_t>();
    std::vector<Double_t> *yTrig = new std::vector<Double_t>();
    xCut = 5;
    yCut = 5;
    
    /* from bkg range 0 - 1000 */
    
    for (Int_t k = 0; k < 1000; k += 100){

        bkg = "";
        bkg += k;
        bkg += ".root";
        myfile = new TFile(bkg);
        
       
        gingko->SetBranchAddress("real_x_muon",&xReal);
        gingko->SetBranchAddress("real_y_muon",&yReal);
        gingko->SetBranchAddress("trig_x",&xTrig);
        gingko->SetBranchAddress("trig_y",&yTrig);
        
        
        nentries = gingko->GetEntries();
        numb = 0;
        for (Int_t i = 0; i < nentries; i++) {
            gingko->GetEntry(i);
            for (Int_t j = 0; j < xTrig->size(); j++) {
                if ((abs(xReal - (*xTrig)[j]) < xCut) && (abs(yReal - (*yTrig)[j]) < yCut)) {
                    numb++;
                    break;
                }
            }
        }
        ratio->push_back(numb);
    }
    
    /* from bkg range 1000 - 10000 */
    
    for (Int_t k = 1000; k < 10000; k += 1000){
        
        bkg = "";
        bkg += k;
        bkg += ".root";
        myfile = new TFile(bkg);
        
        gingko->SetBranchAddress("real_x_muon",&xReal);
        gingko->SetBranchAddress("real_y_muon",&yReal);
        gingko->SetBranchAddress("trig_x",&xTrig);
        gingko->SetBranchAddress("trig_y",&yTrig);
        
        numb = 0;
        nentries = gingko->GetEntries();
        for (Int_t i = 0; i < nentries; i++) {
            gingko->GetEntry(i);
            for (Int_t j = 0; j < xTrig->size(); j++) {
                if ((abs(xReal - (*xTrig)[j]) < xCut) && (abs(yReal - (*yTrig)[j]) < yCut)) {
                    numb++;
                    break;
                }
            }
        }
        ratio->push_back(numb);
    }
    
    /* from bkg range 10000 - 70000 */
    
    for (Int_t k = 10000; k < 80000; k += 10000){
        
        bkg = "";
        bkg += k;
        bkg += ".root";
        myfile = new TFile(bkg);
        
        gingko->SetBranchAddress("real_x_muon",&xReal);
        gingko->SetBranchAddress("real_y_muon",&yReal);
        gingko->SetBranchAddress("trig_x",&xTrig);
        gingko->SetBranchAddress("trig_y",&yTrig);
        
        numb = 0;
        nentries = gingko->GetEntries();
        for (Int_t i = 0; i < nentries; i++) {
            gingko->GetEntry(i);
            for (Int_t j = 0; j < xTrig->size(); j++) {
                if ((abs(xReal - (*xTrig)[j]) < xCut) && (abs(yReal - (*yTrig)[j]) < yCut)) {
                    numb++;
                    break;
                }
            }
        }
        ratio->push_back(numb);
    }
    TFile* fout = new TFile("ratio.root", "RECREATE");
    TTree* tree = new TTree("tree","tree");
    tree->Branch("vec", &ratio);
    tree->Fill();
    fout->Write();
}

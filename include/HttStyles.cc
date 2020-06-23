/*
 * utilities related to TCanvas, TPad objects.
 * Thanks to the MIT Particle Physics Collaboration (PPC)
 */

#include "HttStyles.h"

void HttStyles() {
  SetStyle() ;
}

void SetStyle()
{
  TStyle *HttStyle = new TStyle("Htt-Style","The Perfect Style for Plots ;-)");
  gStyle = HttStyle;

  // Canvas
  HttStyle->SetCanvasColor     (0);
  HttStyle->SetCanvasBorderSize(10);
  HttStyle->SetCanvasBorderMode(0);
  HttStyle->SetCanvasDefH      (700);
  HttStyle->SetCanvasDefW      (700);
  HttStyle->SetCanvasDefX      (100);
  HttStyle->SetCanvasDefY      (100);

  // color palette for 2D temperature plots
  HttStyle->SetPalette(1,0);
  
  // Pads
  HttStyle->SetPadColor       (0);
  HttStyle->SetPadBorderSize  (10);
  HttStyle->SetPadBorderMode  (0);
  HttStyle->SetPadBottomMargin(0.13);
  HttStyle->SetPadTopMargin   (0.08);
  HttStyle->SetPadLeftMargin  (0.15);
  HttStyle->SetPadRightMargin (0.05);
  HttStyle->SetPadGridX       (0);
  HttStyle->SetPadGridY       (0);
  HttStyle->SetPadTickX       (1);
  HttStyle->SetPadTickY       (1);

  // Frames
  HttStyle->SetLineWidth(3);
  HttStyle->SetFrameFillStyle ( 0);
  HttStyle->SetFrameFillColor ( 0);
  HttStyle->SetFrameLineColor ( 1);
  HttStyle->SetFrameLineStyle ( 0);
  HttStyle->SetFrameLineWidth ( 2);
  HttStyle->SetFrameBorderSize(10);
  HttStyle->SetFrameBorderMode( 0);

  // Histograms
  HttStyle->SetHistFillColor(2);
  HttStyle->SetHistFillStyle(0);
  HttStyle->SetHistLineColor(1);
  HttStyle->SetHistLineStyle(0);
  HttStyle->SetHistLineWidth(3);
  HttStyle->SetNdivisions(505, "X");

  // Functions
  HttStyle->SetFuncColor(1);
  HttStyle->SetFuncStyle(0);
  HttStyle->SetFuncWidth(2);

  // Various
  HttStyle->SetMarkerStyle(20);
  HttStyle->SetMarkerColor(kBlack);
  HttStyle->SetMarkerSize (1.1);

  HttStyle->SetTitleBorderSize(0);
  HttStyle->SetTitleFillColor (0);
  HttStyle->SetTitleX         (0.2);

  HttStyle->SetTitleSize  (0.055,"X");
  HttStyle->SetTitleOffset(1.200,"X");
  HttStyle->SetLabelOffset(0.005,"X");
  HttStyle->SetLabelSize  (0.040,"X");
  HttStyle->SetLabelFont  (42   ,"X");

  HttStyle->SetStripDecimals(kFALSE);
  HttStyle->SetLineStyleString(11,"20 10");

  HttStyle->SetTitleSize  (0.055,"Y");
  HttStyle->SetTitleOffset(1.600,"Y");
  HttStyle->SetLabelOffset(0.010,"Y");
  HttStyle->SetLabelSize  (0.040,"Y");
  HttStyle->SetLabelFont  (42   ,"Y");

  HttStyle->SetTextSize   (0.055);
  HttStyle->SetTextFont   (42);

  HttStyle->SetStatFont   (42);
  HttStyle->SetTitleFont  (42);
  HttStyle->SetTitleFont  (42,"X");
  HttStyle->SetTitleFont  (42,"Y");

  HttStyle->SetOptStat    (0);
  return;
}



void setTDRStyle() {
  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

// For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //POsition on screen
  tdrStyle->SetCanvasDefY(0);

// For the Pad:
  tdrStyle->SetPadBorderMode(0);
  // tdrStyle->SetPadBorderSize(Width_t size = 1);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);

// For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);
  
// For the histo:
  // tdrStyle->SetHistFillColor(1);
  // tdrStyle->SetHistFillStyle(0);
  tdrStyle->SetHistLineColor(1);
  tdrStyle->SetHistLineStyle(0);
  tdrStyle->SetHistLineWidth(1);
  // tdrStyle->SetLegoInnerR(Float_t rad = 0.5);
  // tdrStyle->SetNumberContours(Int_t number = 20);

  tdrStyle->SetEndErrorSize(2);
  // tdrStyle->SetErrorMarker(20);
  //tdrStyle->SetErrorX(0.);
  
  tdrStyle->SetMarkerStyle(20);
  tdrStyle->SetLineStyleString(11,"20 10");
  
//For the fit/function:
  tdrStyle->SetOptFit(1);
  tdrStyle->SetFitFormat("5.4g");
  tdrStyle->SetFuncColor(2);
  tdrStyle->SetFuncStyle(1);
  tdrStyle->SetFuncWidth(1);

//For the date:
  tdrStyle->SetOptDate(0);
  // tdrStyle->SetDateX(Float_t x = 0.01);
  // tdrStyle->SetDateY(Float_t y = 0.01);

// For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  // tdrStyle->SetStatStyle(Style_t style = 1001);
  // tdrStyle->SetStatX(Float_t x = 0);
  // tdrStyle->SetStatY(Float_t y = 0);

// Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.02);

// For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFontSize(0.05);
  // tdrStyle->SetTitleH(0); // Set the height of the title box
  // tdrStyle->SetTitleW(0); // Set the width of the title box
  // tdrStyle->SetTitleX(0); // Set the position of the title box
  // tdrStyle->SetTitleY(0.985); // Set the position of the title box
  // tdrStyle->SetTitleStyle(Style_t style = 1001);
  // tdrStyle->SetTitleBorderSize(2);

// For the axis titles:

  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(0.06, "XYZ");
  // tdrStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // tdrStyle->SetTitleYSize(Float_t size = 0.02);
  tdrStyle->SetTitleXOffset(0.9);
  tdrStyle->SetTitleYOffset(1.25);
  // tdrStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

// For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.007, "XYZ");
  tdrStyle->SetLabelSize(0.05, "XYZ");

// For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(0.03, "XYZ");
  tdrStyle->SetNdivisions(510, "XYZ");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

// Change for log plots:
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(0);

// Postscript options:
  tdrStyle->SetPaperSize(20.,20.);
  // tdrStyle->SetLineScalePS(Float_t scale = 3);
  // tdrStyle->SetLineStyleString(Int_t i, const char* text);
  // tdrStyle->SetHeaderPS(const char* header);
  // tdrStyle->SetTitlePS(const char* pstitle);

  // tdrStyle->SetBarOffset(Float_t baroff = 0.5);
  // tdrStyle->SetBarWidth(Float_t barwidth = 0.5);
  // tdrStyle->SetPaintTextFormat(const char* format = "g");
  // tdrStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->SetHatchesLineWidth(5);
  tdrStyle->SetHatchesSpacing(0.05);

  tdrStyle->cd();

}

TCanvas* MakeCanvas(const char* name, const char *title, int dX, int dY)
{
  // Start with a canvas
  TCanvas *canvas = new TCanvas(name,title,0,0,dX,dY);
  // General overall stuff
    /*
  canvas->SetFillColor      (0);
  canvas->SetBorderMode     (0);
  canvas->SetBorderSize     (10);
     */
  // Set margins to reasonable defaults
  canvas->SetLeftMargin     (0.18);
  canvas->SetRightMargin    (0.05);
  canvas->SetTopMargin      (0.08);
  canvas->SetBottomMargin   (0.15);
  // Setup a frame which makes sense
  canvas->SetFrameFillStyle (0);
  canvas->SetFrameLineStyle (0);
  canvas->SetFrameBorderMode(0);
  canvas->SetFrameBorderSize(10);
  canvas->SetFrameFillStyle (0);
  canvas->SetFrameLineStyle (0);
  canvas->SetFrameBorderMode(0);
  canvas->SetFrameBorderSize(10);

  return canvas;
}

void InitSubPad(TPad* pad, int i)
{
  pad->cd(i);
  TPad *tmpPad = (TPad*) pad->GetPad(i);
  tmpPad->SetLeftMargin  (0.18);
  tmpPad->SetTopMargin   (0.05);
  tmpPad->SetRightMargin (0.07);
  tmpPad->SetBottomMargin(0.15);
  return;
}

void InitSignal(TH1 *hist)
{
  hist->SetFillStyle(0);
  hist->SetLineStyle(11);
  hist->SetLineWidth(3);
  hist->SetLineColor(kBlue);
  //hist->SetLineColor(kBlue+3);
}

void InitHist(TH1 *hist, const char *xtit, const char *ytit, int color, int style, int doFill)
{
  hist->SetXTitle(xtit);
  hist->GetXaxis()->CenterTitle();
  hist->SetYTitle(ytit);
  hist->GetYaxis()->CenterTitle();
  hist->SetLineColor(color);
  hist->SetLineWidth(    2);
  if (doFill)
  {
      hist->SetFillColor(color );
      hist->SetFillStyle(style );
  }
  hist->SetTitleSize  (0.055,"Y");
  hist->SetTitleOffset(1.300,"Y");
  hist->SetLabelOffset(0.014,"Y");
  hist->SetLabelSize  (0.040,"Y");
  hist->SetLabelFont  (42   ,"Y");
  hist->SetTitleSize  (0.055,"X");
  hist->SetTitleOffset(1.200,"X");
  hist->SetLabelOffset(0.014,"X");
  hist->SetLabelSize  (0.040,"X");
  hist->SetLabelFont  (42   ,"X");
  hist->SetMarkerStyle(20);
  hist->SetMarkerColor(color);
  hist->SetMarkerSize (1.3);
  hist->GetYaxis()->SetTitleFont(42);
  hist->GetXaxis()->SetTitleFont(42);
  hist->SetTitle("");
  hist->SetStats(0);
    
  return;
}

void InitGraph(TGraph *hist, const char *xtit, const char *ytit, int color, int style, int doFill)
{
    hist->GetHistogram()->SetXTitle(xtit);
    hist->GetHistogram()->GetXaxis()->CenterTitle();
    hist->GetHistogram()->SetYTitle(ytit);
    hist->GetHistogram()->GetYaxis()->CenterTitle();
    hist->SetLineColor(kBlack);
    hist->SetLineWidth(    1);
    if (doFill)
    {
        hist->SetFillColor(color );
        hist->SetFillStyle(style );
    }
    hist->GetHistogram()->SetTitleSize  (0.055,"Y");
    hist->GetHistogram()->SetTitleOffset(1.300,"Y");
    hist->GetHistogram()->SetLabelOffset(0.014,"Y");
    hist->GetHistogram()->SetLabelSize  (0.040,"Y");
    hist->GetHistogram()->SetLabelFont  (42   ,"Y");
    hist->GetHistogram()->SetTitleSize  (0.055,"X");
    hist->GetHistogram()->SetTitleOffset(1.200,"X");
    hist->GetHistogram()->SetLabelOffset(0.014,"X");
    hist->GetHistogram()->SetLabelSize  (0.040,"X");
    hist->GetHistogram()->SetLabelFont  (42   ,"X");
    hist->SetMarkerStyle(20);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize (0.6);
    hist->GetYaxis()->SetTitleFont(42);
    hist->GetXaxis()->SetTitleFont(42);
    hist->SetTitle("");
    hist->GetHistogram()->SetStats(0);
    
    return;
}

void InitFunc(TF1 *hist, const char *xtit, const char *ytit, int color, int style)
{
  hist->GetXaxis()->SetTitle(xtit);
  hist->GetXaxis()->CenterTitle();
  hist->GetYaxis()->SetTitle(ytit);
  hist->GetYaxis()->CenterTitle();
  hist->SetLineColor(color);
  hist->SetLineWidth(    2);
  hist->SetFillColor(color );
  hist->SetFillStyle(style );
  hist->GetYaxis()->SetTitleSize  (0.055);
  hist->GetYaxis()->SetTitleOffset(1.300);
  hist->GetYaxis()->SetLabelOffset(0.014);
  hist->GetYaxis()->SetLabelSize  (0.040);
  hist->GetYaxis()->SetLabelFont  (42);
  hist->GetXaxis()->SetTitleSize  (0.055);
  hist->GetXaxis()->SetTitleOffset(1.200);
  hist->GetXaxis()->SetLabelOffset(0.014);
  hist->GetXaxis()->SetLabelSize  (0.040);
  hist->GetXaxis()->SetLabelFont  (42);
  hist->SetMarkerStyle(20);
  hist->SetMarkerColor(color);
  hist->SetMarkerSize (0.6);
  hist->GetYaxis()->SetTitleFont(42);
  hist->GetXaxis()->SetTitleFont(42);
  hist->SetTitle("");  
  return;
}


void InitStandardHist(TH1 *hist, const char *xtit, const char *ytit, int color)
{
  hist->SetXTitle(xtit);
  hist->SetYTitle(ytit);
  hist->SetLineColor(color);
  hist->SetLineWidth(    3);
  //hist->SetLineStyle(11);
  hist->SetTitleSize  (0.055,"Y");
  hist->SetTitleOffset(1.600,"Y");
  hist->SetLabelOffset(0.014,"Y");
  hist->SetLabelSize  (0.040,"Y");
  hist->SetLabelFont  (42   ,"Y");
  hist->SetTitleSize  (0.055,"X");
  hist->SetTitleOffset(1.300,"X");
  hist->SetLabelOffset(0.014,"X");
  hist->SetLabelSize  (0.040,"X");
  hist->SetLabelFont  (42   ,"X");
  hist->SetMarkerStyle(20);
  hist->SetMarkerColor(color);
  hist->SetMarkerSize (0.6);
  hist->GetYaxis()->SetTitleFont(42);
  hist->GetXaxis()->SetTitleFont(42);
  hist->SetTitle("");
  return;
}

void InitData(TH1* hist)
{
  hist->SetMarkerStyle(20);
  hist->SetMarkerSize (1.3);
  hist->SetLineWidth  ( 3);
}

void SetLegendStyle(TLegend* leg) 
{
  leg->SetFillStyle (0);
  leg->SetFillColor (0);
  leg->SetBorderSize(0);
}

void CMSPrelim(const char* dataset, const char* channel, double lowX, double lowY)
{
  /*
  TPaveText* cmsprel  = new TPaveText(lowX, lowY+0.06, lowX+0.30, lowY+0.16, "NDC");
  cmsprel->SetBorderSize(   0 );
  cmsprel->SetFillStyle(    0 );
  cmsprel->SetTextAlign(   12 );
  cmsprel->SetTextSize ( 0.03 );
  cmsprel->SetTextColor(    1 );
  cmsprel->SetTextFont (   62 );
  cmsprel->AddText("CMS");
  cmsprel->Draw();

  TPaveText* lumi     = new TPaveText(lowX+0.08, lowY+0.061, lowX+0.45, lowY+0.161, "NDC");
  */

  TPaveText* lumi  = new TPaveText(lowX, lowY+0.06, lowX+0.30, lowY+0.16, "NDC");
  lumi->SetBorderSize(   0 );
  lumi->SetFillStyle(    0 );
  lumi->SetTextAlign(   12 );
  lumi->SetTextSize ( 0.035 );
  lumi->SetTextColor(    1 );
  lumi->SetTextFont (   62 );
  lumi->AddText(dataset);
  lumi->Draw();

  TPaveText* chan     = new TPaveText(lowX+0.68, lowY+0.061, lowX+0.80, lowY+0.161, "NDC");
  chan->SetBorderSize(   0 );
  chan->SetFillStyle(    0 );
  chan->SetTextAlign(   12 );
  chan->SetTextSize ( 0.04 );
  chan->SetTextColor(    1 );
  chan->SetTextFont (   62 );
  chan->AddText(channel);
  chan->Draw();
}
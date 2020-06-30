#ifndef HttStyles_h
#define HttStyles_h

#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TPad.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>

/// define common styles
void SetStyle();
void setTDRStyle();
/// create a canveas in common style (default is square)
TCanvas* MakeCanvas(const char* name, const char* title, int dX=600, int dY=600);
/// define common style of pads
void InitSubPad(TPad* pad, int i);
/// define a histogram file (not yet including poly markers, which are used fro data)
void InitHist(TH1* hist, const char* xtit, const char* ytit="Number of Entries", int color=kBlack, int style=0);
void InitHist(TH1* hist, const char* xtit, const char* ytit="Number of Entries", int color=kBlack, int style=0, int doFill=0);
void InitStandardHist(TH1 *hist, const char *xtit, const char *ytit, int color);
void InitFunc(TF1* hist, const char* xtit, const char* ytit="Number of Entries", int color=kBlack, int style=0);
void InitGraph(TGraph* hist, const char* xtit, const char* ytit="Number of Entries", int color=kBlack, int style=0);
void InitGraph(TGraph* hist, const char* xtit, const char* ytit="Number of Entries", int color=kBlack, int style=0, int doFill=0);
/// define poly markers and line width for data
void InitData(TH1* hist);
/// define line style, width and color for signal sample
void InitSignal(TH1* hist);
/// define common legend style 
void SetLegendStyle(TLegend* leg);
/// add CMSPreliminary logo, lumi and channel
void CMSPrelim(const char* dataset, const char* channel, double lowX=0.4, double lowY=0.74);

#endif
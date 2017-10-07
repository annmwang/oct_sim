#!/usr/bin/python

import ROOT, sys, os, math
import time as tm
from array import array
import argparse
import random

def main(argv):


    inputfile = ''
    outputfile = ''

    parser = argparse.ArgumentParser()
    parser.add_argument("-i", default="", help="input file")
    parser.add_argument("-j", default="", help="input file #2")
    parser.add_argument("-l", action="store_true", help="log")

    args = parser.parse_args()

    outdir = args.i[:-5]
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    setstyle()
    
    ROOT.gStyle.SetOptStat(0)

    ifile = ROOT.TFile(args.i)
    hists = {}
    hist_titles = {}

    hists["h_mxres"] = ifile.Get("histograms/h_mxres")
    hists["h_xres"] = ifile.Get("histograms/h_xres")
    hists["h_yres"] = ifile.Get("histograms/h_yres")

    hist_titles["h_mxres"] = "#Delta #theta (mrad)"
    hist_titles["h_xres"] = "#Delta x (mm)"
    hist_titles["h_yres"] = "#Delta y (mm)"

    if args.j:
        jfile = ROOT.TFile(args.j)
        hists["h2_mxres"] = jfile.Get("histograms/h_mxres")
        hists["h2_xres"] = jfile.Get("histograms/h_xres")
        hists["h2_yres"] = jfile.Get("histograms/h_yres")

    os.chdir(outdir)
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()
    #show_overflow(h1)
    if (args.l):
        c.SetLogy()

    for hname, hist in hists.iteritems():
        show_overflow(hist)
        hist.SetMarkerStyle(8)
        hist.SetMarkerSize(1)
        hist.SetLineColor(ROOT.kGreen+3)
        hist.SetFillColorAlpha(ROOT.kGreen+3,0.4) 
        hist.SetLineWidth(3)
        hist.SetTitle("")
        hist.GetXaxis().SetTitle(hist_titles[hname])
        hist.GetYaxis().SetTitle("Events")
        hist.GetXaxis().SetTitleOffset(1.0)
        hist.GetYaxis().SetTitleOffset(1.8)
        hist.GetXaxis().SetLabelSize(0.045)
        hist.GetXaxis().SetTitleSize(0.045)
        hist.GetYaxis().SetLabelSize(0.045)
        hist.GetYaxis().SetTitleSize(0.045)
        hist.Draw("hist")

        l1 = ROOT.TLatex()
        l1.SetTextSize(0.04)
        l1.SetTextColor(ROOT.kRed)
        l1.SetTextAlign(21)
        l1.SetNDC()
        if hname is "h_mxres":
            l1.DrawLatex(0.35,0.8,"RMS = %3.2f mrad"%(hist.GetRMS()));  
        else:
            l1.DrawLatex(0.35,0.8,"RMS = %3.2f mm"%(hist.GetRMS()));  

        if not args.l:
            filename = hname + ".pdf"
        else:
            filename = hname + "_log.pdf"
        c.Print(filename)
        c.Clear()

def show_overflow(hist, show_underflow=True, show_overflow=True):
    """ h/t Josh """
    nbins          = hist.GetNbinsX()
    underflow      = hist.GetBinContent(   0   )
    underflowerror = hist.GetBinError  (   0   )
    overflow       = hist.GetBinContent(nbins+1)
    overflowerror  = hist.GetBinError  (nbins+1)
    firstbin       = hist.GetBinContent(   1   )
    firstbinerror  = hist.GetBinError  (   1   )
    lastbin        = hist.GetBinContent( nbins )
    lastbinerror   = hist.GetBinError  ( nbins )
    if show_underflow and underflow != 0:
        newcontent = underflow + firstbin
        if firstbin == 0 :
            newerror = underflowerror
        else:
            newerror = math.sqrt( underflowerror * underflowerror + firstbinerror * firstbinerror )
        hist.SetBinContent(1, newcontent)
        hist.SetBinError  (1, newerror)
        hist.SetBinContent(0, 0)
        hist.SetBinError  (0, 0)
    if show_overflow and overflow != 0:
        newcontent = overflow + lastbin
        if lastbin == 0 :
            newerror = overflowerror
        else:
            newerror = math.sqrt( overflowerror * overflowerror + lastbinerror * lastbinerror )
        hist.SetBinContent(nbins,   newcontent)
        hist.SetBinError  (nbins,   newerror)
        hist.SetBinContent(nbins+1, 0)
        hist.SetBinError  (nbins+1, 0)

        
def progress(time_diff, nprocessed, ntotal):
    nprocessed, ntotal = float(nprocessed), float(ntotal)
    rate = (nprocessed+1)/time_diff
    msg = "\r > %6i / %6i | %2i%% | %8.2fHz | %6.1fm elapsed | %6.1fm remaining"
    msg = msg % (nprocessed, ntotal, 100*nprocessed/ntotal, rate, time_diff/60, (ntotal-nprocessed)/(rate*60))
    sys.stdout.write(msg)
    sys.stdout.flush()

def setstyle():
    ROOT.gROOT.SetBatch()
    ROOT.gStyle.SetStatY(0.85)
    ROOT.gStyle.SetPadTopMargin(0.1)
    # ROOT.gStyle.SetPadRightMargin(0.1) # deal with it
    ROOT.gStyle.SetPadRightMargin(0.05)
    ROOT.gStyle.SetPadBottomMargin(0.12)
    ROOT.gStyle.SetPadLeftMargin(0.18)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetPaintTextFormat(".2f")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptFit(ROOT.kTRUE)

if __name__ == "__main__":
    main(sys.argv[1:])

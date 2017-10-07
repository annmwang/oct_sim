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

    hists["h_mxres"] = ifile.Get("histograms/h_mxres")
    hists["h_xres"] = ifile.Get("histograms/h_xres")
    hists["h_yres"] = ifile.Get("histograms/h_yres")

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

    show_overflow(hists["h_mxres"])
    hists["h_mxres"].SetMarkerStyle(8)
    hists["h_mxres"].SetMarkerSize(1)
    hists["h_mxres"].SetLineColor(ROOT.kBlue-7)
    hists["h_mxres"].SetLineWidth(3)
    hists["h_mxres"].SetTitle("")
    hists["h_mxres"].GetXaxis().SetTitle("#Delta #theta (mrad)")
    hists["h_mxres"].GetYaxis().SetTitle("Events")
    hists["h_mxres"].GetXaxis().SetTitleOffset(1.0)
    hists["h_mxres"].GetYaxis().SetTitleOffset(1.8)
    hists["h_mxres"].GetXaxis().SetLabelSize(0.045)
    hists["h_mxres"].GetXaxis().SetTitleSize(0.045)
    hists["h_mxres"].GetYaxis().SetLabelSize(0.045)
    hists["h_mxres"].GetYaxis().SetTitleSize(0.045)
    hists["h_mxres"].Draw("hist")


    l1 = ROOT.TLatex()
    l1.SetTextSize(0.04)
    l1.SetTextColor(ROOT.kRed)
    l1.SetTextAlign(21)
    l1.SetNDC()
    l1.DrawLatex(0.35,0.8,"RMS = %3.2f mrad"%(hists["h_mxres"].GetRMS()));  
    if not args.l:
        c.Print("theta.pdf")
    else:
        c.Print("theta_log.pdf")
    c.Clear()

    hists["h_xres"].SetMarkerStyle(8)
    hists["h_xres"].SetMarkerSize(1)
    hists["h_xres"].SetLineColor(ROOT.kBlue-7)
    hists["h_xres"].SetLineWidth(3)
    hists["h_xres"].SetTitle("")
    hists["h_xres"].GetXaxis().SetTitle("#Delta x (mm)")
    hists["h_xres"].GetYaxis().SetTitle("Events")
    hists["h_xres"].GetXaxis().SetTitleOffset(1.0)
    hists["h_xres"].GetYaxis().SetTitleOffset(1.8)
    hists["h_xres"].GetXaxis().SetLabelSize(0.045)
    hists["h_xres"].GetXaxis().SetTitleSize(0.045)
    hists["h_xres"].GetYaxis().SetLabelSize(0.045)
    hists["h_xres"].GetYaxis().SetTitleSize(0.045)
    hists["h_xres"].Draw("hist")

    l1 = ROOT.TLatex()
    l1.SetTextSize(0.04)
    l1.SetTextColor(ROOT.kRed)
    l1.SetTextAlign(21)
    l1.SetNDC()
    l1.DrawLatex(0.35,0.8,"RMS = %3.2f mm"%(hists["h_xres"].GetRMS()));  
    if not args.l:
        c.Print("x.pdf")
    else:
        c.Print("x_log.pdf")
    c.Clear()
    hists["h_yres"].SetMarkerStyle(8)
    hists["h_yres"].SetMarkerSize(1)
    hists["h_yres"].SetLineColor(ROOT.kBlue-7)
    hists["h_yres"].SetLineWidth(3)
    hists["h_yres"].SetTitle("")
    hists["h_yres"].GetXaxis().SetTitle("#Delta y (mm)")
    if args.j:
        hists["h_yres"].GetYaxis().SetTitle("A.U.")
        hists["h_yres"].Scale(1/hists["h_yres"].Integral())
    else:
        hists["h_yres"].GetYaxis().SetTitle("Events")

    hists["h_yres"].GetXaxis().SetLabelSize(0.045)
    hists["h_yres"].GetYaxis().SetLabelSize(0.045)
    hists["h_yres"].GetXaxis().SetTitleSize(0.045)
    hists["h_yres"].GetYaxis().SetTitleSize(0.045)
    hists["h_yres"].GetXaxis().SetTitleOffset(1)
    hists["h_yres"].GetYaxis().SetTitleOffset(2)
    hists["h_yres"].Draw("hist")

    if args.j:
        hists["h2_yres"].SetMarkerStyle(8)
        hists["h2_yres"].SetMarkerSize(1)
        hists["h2_yres"].SetLineColor(ROOT.kGreen-7)
        hists["h2_yres"].SetLineWidth(3)
        hists["h2_yres"].SetTitle("")
        hists["h2_yres"].GetXaxis().SetTitleOffset(1.0)
        hists["h2_yres"].GetYaxis().SetTitleOffset(1.8)
        hists["h2_yres"].GetXaxis().SetLabelSize(0.045)
        hists["h2_yres"].GetXaxis().SetTitleSize(0.045)
        hists["h2_yres"].GetYaxis().SetLabelSize(0.045)
        hists["h2_yres"].GetYaxis().SetTitleSize(0.045)
        hists["h2_yres"].Scale(1/hists["h2_yres"].Integral())
        hists["h2_yres"].Draw("hist same")
    l1 = ROOT.TLatex()
    l1.SetTextSize(0.04)
    l1.SetTextColor(ROOT.kRed)
    l1.SetTextAlign(21)
    l1.SetNDC()
    l1.DrawLatex(0.35,0.8,"RMS = %3.2f mm"%(hists["h_yres"].GetRMS()));  
    if not args.l:
        c.Print("y.pdf")
    else:
        c.Print("y_log.pdf")
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

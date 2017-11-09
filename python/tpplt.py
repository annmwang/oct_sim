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
    hists2 = {}
    hists_2d = {}
    hist_titles = {}
    hist_2d_titles = {}

    hists["h_mxres"] = ifile.Get("histograms/h_mxres")
    hists["h_xres"] = ifile.Get("histograms/h_xres")
    hists["h_yres"] = ifile.Get("histograms/h_yres")
    hists["h_nuv_bkg"] = ifile.Get("histograms/h_nuv_bkg")
    hists["h_ntrig"] = ifile.Get("histograms/h_ntrig")
    hists["h_ntrig_bkgonly"] = ifile.Get("histograms/h_ntrig_bkgonly")


    nx = hists["h_ntrig"].GetNbinsX()
    ntrig = 0
    for i in range(nx):
        ntrig += i*hists["h_ntrig"].GetBinContent(i+1)
    print "num triggers:",ntrig
    print "% events with 1+ trigger:", (hists["h_ntrig"].GetEntries()-hists["h_ntrig"].GetBinContent(1))/hists["h_ntrig"].GetEntries()*100

    hists_2d["h_xres_nxmuon"] = ifile.Get("histograms/h_xres_nxmuon")
    hists_2d["h_ntrig_bc"] = ifile.Get("histograms/h_ntrig_bc")

    ntrig = 0
    h_ntrig_16 = hists_2d["h_ntrig_bc"].ProjectionY("h_ntrig_16",25,40)
    for i in range(h_ntrig_16.GetNbinsX()):
        ntrig += i*h_ntrig_16.GetBinContent(i+1)
    print "from BC 0 to 15 num triggers:",ntrig


#    hists_2d["h_xres_nxmaxmuon"] = ifile.Get("histograms/h_xres_nxmaxmuon")

    hist_titles["h_mxres"] = "#Delta #theta (mrad)"
    hist_titles["h_xres"] = "#Delta x (mm)"
    hist_titles["h_yres"] = "#Delta y (mm)"
    hist_titles["h_nuv_bkg"] = "Number of bkg. hits in UV planes"
    hist_titles["h_ntrig"] = "Number of triggers"
    hist_titles["h_ntrig_bkgonly"] = "Number of triggers from bkg."
    hist_2d_titles["h_xres_nxmuon"] = ["Number of real muon x-hits", "#Deltax (mm)"]
    hist_2d_titles["h_ntrig_bc"] = ["BC wrt. earliest real muon hit", "N(trig)"]
#    hist_2d_titles["h_xres_nxmaxmuon"] = ["Number of real muon x-hits", "#Deltax (mm)"]
    if args.j:
        jfile = ROOT.TFile(args.j)
        hists2["h_mxres"] = jfile.Get("histograms/h_mxres")
        hists2["h_xres"] = jfile.Get("histograms/h_xres")
        hists2["h_yres"] = jfile.Get("histograms/h_yres")
        hists2["h_nuv_bkg"] = jfile.Get("histograms/h_nuv_bkg")
        hists2["h_ntrig"] = jfile.Get("histograms/h_ntrig")
        hists2["h_ntrig_bkgonly"] = jfile.Get("histograms/h_ntrig_bkgonly")
    os.chdir(outdir)
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()
    #show_overflow(h1)
    if (args.l):
        c.SetLogy()

    for hname, hist in hists.iteritems():

        legend = ROOT.TLegend(0.70,0.61,0.88,0.78)
        #legend = ROOT.TLegend(0.70,0.61,0.88,0.78)
        legend.SetMargin(0.14)
        legend.SetBorderSize(0)
        legend.SetFillColor(0)
        legend.SetFillStyle(0)
        legend.SetTextSize(0.045)
        show_overflow(hist)
        hist.SetMarkerStyle(8)
        hist.SetMarkerSize(1)
        hist.SetLineColor(ROOT.kMagenta)
        hist.SetFillColorAlpha(ROOT.kMagenta,0.4) 
#         hist.SetLineColor(ROOT.kGreen+3)
#         hist.SetFillColorAlpha(ROOT.kGreen+3,0.4) 
        hist.SetLineWidth(3)
        hist.SetTitle("")
        hist.GetXaxis().SetTitle(hist_titles[hname])
        hist.GetYaxis().SetTitle("Events")
        if hname is "h_ntrig":
            hist.GetXaxis().SetRangeUser(0.,40.)
        hist.GetXaxis().SetTitleOffset(1.0)
        hist.GetYaxis().SetTitleOffset(1.8)
        hist.GetXaxis().SetLabelSize(0.045)
        hist.GetXaxis().SetTitleSize(0.045)
        hist.GetYaxis().SetLabelSize(0.045)
        hist.GetYaxis().SetTitleSize(0.045)
        hist.Draw("hist")
        legend.AddEntry(hist,    " no bkg","f")
        #legend.AddEntry(hist,    " best trig","f")
        if args.j:
            show_overflow(hists2[hname])
            hists2[hname].SetMarkerStyle(8)
            hists2[hname].SetMarkerSize(1)
            hists2[hname].SetLineColor(ROOT.kBlue)
            hists2[hname].SetFillColorAlpha(ROOT.kBlue,0.4) 
            hists2[hname].SetLineWidth(3)
            hists2[hname].Draw("hist same")
            legend.AddEntry(hists2[hname],    " 40 kHz","f")
            #legend.AddEntry(hists2[hname],    "#splitline{ trig w/ }{ max hits}","f")
            legend.Draw()
        l1 = ROOT.TLatex()
        l1.SetTextSize(0.04)
        l1.SetTextColor(ROOT.kRed)
        l1.SetTextAlign(21)
        l1.SetNDC()
        #if hname is "h_mxres":
        #    l1.DrawLatex(0.35,0.8,"RMS = %3.2f mrad"%(hist.GetRMS()));  
        #elif hname is "h_nuv_bkg" or "h_ntrig":
        #    pass
        #else:
        #    l1.DrawLatex(0.35,0.8,"RMS = %3.2f mm"%(hist.GetRMS()));  

        if not args.l:
            filename = hname + ".pdf"
        else:
            filename = hname + "_log.pdf"
        c.Print(filename)
        c.Clear()

    ROOT.gStyle.SetPadRightMargin(0.2) # deal with it
    c1 = ROOT.TCanvas("c", "canvas", 800, 800)
    c1.cd()
    #show_overflow(h1)
    if (args.l):
        c1.SetLogy()
#     h1 = hists_2d["h_xres_nxmaxmuon"].ProjectionX()
#     hname = "h_nxmaxmuon"
#     show_overflow(h1)
#     h1.SetMarkerStyle(8)
#     h1.SetMarkerSize(1)
#     h1.SetLineColor(ROOT.kGreen+3)
#     h1.SetFillColorAlpha(ROOT.kGreen+3,0.4) 
#     h1.SetLineWidth(3)
#     h1.SetTitle("")
#     h1.GetXaxis().SetTitle(hist_2d_titles["h_xres_nxmaxmuon"][0])
#     h1.GetYaxis().SetTitle("Events")
# #        h1.GetXaxis().SetNdivisions(1)
#     h1.GetXaxis().SetTitleOffset(1.0)
#     h1.GetYaxis().SetTitleOffset(1.8)
#     h1.GetXaxis().SetLabelSize(0.045)
#     h1.GetXaxis().SetTitleSize(0.045)
#     h1.GetYaxis().SetLabelSize(0.045)
#     h1.GetYaxis().SetTitleSize(0.045)
#     h1.Draw("hist text")
# #     for i in range(1,h1.GetNbinsX()+1):
# #         eff = h1.GetBinContent(i)/h1.GetEntries()
# #         print "bin",i,": ",eff
# #         t = ROOT.TText(0.5+i*0.1,0.5,"%1.5f"%(eff*100))
# #         #t.SetNDC()
# #         t.SetTextSize(0.03)        
# #         t.DrawTextNDC(locs[i-1][0],locs[i-1][1],"%1.5f"%(eff))


#     if not args.l:
#         filename = hname + ".pdf"
#     else:
#         filename = hname + "_log.pdf"
#     c1.Print(filename)
#     c1.Clear()
    

    for hname, hist in hists_2d.iteritems():
        show_overflow(hist)
        hist.SetMarkerStyle(8)
        hist.SetMarkerSize(1)
        hist.SetLineColor(ROOT.kGreen+3)
        hist.SetFillColorAlpha(ROOT.kGreen+3,0.4) 
        hist.SetLineWidth(3)
        hist.SetTitle("")
        hist.GetXaxis().SetTitle(hist_2d_titles[hname][0])
        hist.GetXaxis().SetRangeUser(-15,22.)
        hist.GetYaxis().SetTitle(hist_2d_titles[hname][1])
        hist.GetYaxis().SetRangeUser(0.,25.)
        hist.GetZaxis().SetTitle("Events")
#        hist.GetXaxis().SetNdivisions(1)
        hist.GetXaxis().SetTitleOffset(1.0)
        hist.GetYaxis().SetTitleOffset(1.8)
        hist.GetZaxis().SetTitleOffset(1.6)
        hist.GetXaxis().SetLabelSize(0.045)
        hist.GetXaxis().SetTitleSize(0.045)
        hist.GetYaxis().SetLabelSize(0.045)
        hist.GetYaxis().SetTitleSize(0.045)
        hist.Draw("colz")

        if not args.l:
            filename = hname + ".pdf"
        else:
            filename = hname + "_log.pdf"
        c1.Print(filename)
        c1.Clear()

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

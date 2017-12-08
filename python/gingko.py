#!/usr/bin/python

# some plotting for the ttree produced by oct_sim

import ROOT, sys
import time as tm
from array import array
import argparse

def main(argv):

    inputfile = ''
    outputfile = ''

    parser = argparse.ArgumentParser()
    parser.add_argument("-i", default="", help="input file")
    parser.add_argument("-l", action="store_true", help="log")

    args = parser.parse_args()


    ifile = ROOT.TFile(args.i)
    _tree = ifile.Get("gingko")
    _branches = _tree.GetListOfBranches()

    i = 0

    h1 = ROOT.TH1D("max_road", "mx_road", 16, -0.5, 15.5)


    # get all fit information
    tstart = tm.time()
    nevents = _tree.GetEntries()

    print "Number of events", nevents


    i = 0
    j = 0
    tstart = tm.time()
    while _tree.GetEntry(i):
        for j in range(len(_tree.Hit_strips)):
            h1.Fill(max(_tree.Hit_strips[j])-min(_tree.Hit_strips[j]) + 1)
        i += 1
    setstyle()
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()
    #show_overflow(h1)
    if (args.l):
        c.SetLogy()
#     h1.SetLineColor(ROOT.kGreen+3)
#     h1.SetFillColor(ROOT.kGreen+3)
#     h1.SetFillColorAlpha(ROOT.kGreen+3,0.2)
#     h1.SetLineWidth(3)
#     h1.SetMarkerColor(ROOT.kBlack)
#     h1.SetMarkerStyle(20)
#     h1.SetMarkerSize(0.7)
    h1.SetTitle("")
    h1.GetXaxis().SetTitle("effective road size")
    h1.GetYaxis().SetTitle("events")
    h1.GetXaxis().SetLabelSize(0.025)
    h1.GetYaxis().SetLabelSize(0.025)
    h1.GetXaxis().SetTitleSize(0.04)
    h1.GetYaxis().SetTitleSize(0.04)
    h1.GetXaxis().SetTitleOffset(1)
    h1.GetYaxis().SetTitleOffset(1.3)
    h1.Draw()

    c.Print("test.pdf")

def progress(time_diff, nprocessed, ntotal):
    nprocessed, ntotal = float(nprocessed), float(ntotal)
    rate = (nprocessed+1)/time_diff
    msg = "\r > %6i / %6i | %2i%% | %8.2fHz | %6.1fm elapsed | %6.1fm remaining"
    msg = msg % (nprocessed, ntotal, 100*nprocessed/ntotal, rate, time_diff/60, (ntotal-nprocessed)/(rate*60))
    sys.stdout.write(msg)
    sys.stdout.flush()

def setstyle():
    ROOT.gROOT.SetBatch()
    ROOT.gStyle.SetOptStat(0)                                                                                                                         
    ROOT.gStyle.SetPadTopMargin(0.1)
    ROOT.gStyle.SetPadRightMargin(0.1)
    ROOT.gStyle.SetPadBottomMargin(0.12)
    ROOT.gStyle.SetPadLeftMargin(0.12)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetPaintTextFormat(".2f")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptFit(ROOT.kTRUE)



if __name__ == "__main__":
    main(sys.argv[1:])

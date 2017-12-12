#!/usr/bin/python

# combines GBT and tp root files to one
# cannibalized code from Alex

import ROOT, sys
import time as tm
from array import array
import argparse

harvard = ROOT.TColor.GetColor("#A51C30")
color_large = ROOT.kRed
color_small = ROOT.kBlue
global nroads
nroads = -1

def main(argv):

    args = options()

    global nroads
    if "272" in args.ib:
        nroads = 272
    elif "136" in args.ib:
        nroads = 136
    else:
        print "N(roads) not found! Exiting."
        sys.exit(1)


    ifile = ROOT.TFile(args.ib)
    _bkg_tree = ifile.Get("gingko")
    _bkg_branches = _bkg_tree.GetListOfBranches()

    sigbkg_file = ROOT.TFile(args.isb)
    _sigbkg_tree = sigbkg_file.Get("gingko")
    _sigbkg_branches = _sigbkg_tree.GetListOfBranches()

    trees = [_bkg_tree,_sigbkg_tree]

    i = 0

    all_hists = [{},{}]
    all_hists_2d = [{},{}]

    all_hists_titles = [{},{}]
    all_hists_2d_x = [{},{}]
    all_hists_2d_y = [{},{}]


    for hists in all_hists:
        hists["max_road"] = ROOT.TH1D("max_road", "mx_road", 16, -0.5, 15.5)
        hists["road_spread"] = ROOT.TH1D("road_spread", "road_spread", 111, -10.5, 100.5)
        hists["trigger_BC"]  = ROOT.TH1D("trigger_BC", "trigger_BC", 17, -1.5, 15.5)
        hists["planes"]  = ROOT.TH1D("planes", "planes", 10, -1.5, 8.5)
        hists["nhits"]  = ROOT.TH1D("nhits", "nhits", 10, -0.5, 9.5)
        hists["ages"]  = ROOT.TH1D("ages", "ages", 12, -2.5, 9.5)
        #hists["ntrig"] = ifile.Get("histograms/h_ntrig") if i == 0 else sigbkg_file.Get("histograms/h_ntrig")
        hists["ntrig"] = ROOT.TH1D("ntrig", "ntrig", 41, -0.5, 40.5)
        hists["ntrig_BC5"] = ROOT.TH1D("ntrig_BC5", "ntrig_BC5", 21, -0.5, 20.5)
        hists["ntrig_BC7"] = ROOT.TH1D("ntrig_BC7", "ntrig_BC7", 21, -0.5, 20.5)
        hists["ntrig_BC3to10"] = ROOT.TH1D("ntrig_BC3to10", "ntrig_BC3to10", 41, -0.5, 40.5)
        hists["ntrig_BCm1"] = ROOT.TH1D("ntrig_BCm1", "ntrig_BCm1", 15, -0.5, 14.5)
        i+=1
    for hists in all_hists:
        for key,h1 in hists.iteritems():
            h1.StatOverflows(ROOT.kTRUE)

    for hists_titles in all_hists_titles:
        hists_titles["max_road"] = "effective road size"
        hists_titles["road_spread"] = "x road spread"
        hists_titles["trigger_BC"] = "Trigger BC"
        hists_titles["planes"] = "Plane Index"
        hists_titles["nhits"] = "N(hits) / trigger"
        hists_titles["ages"] = "Trigger hit ages"
        hists_titles["ntrig"] = "N(triggers)"
        hists_titles["ntrig_BC7"] = "N(triggers) / BC"
        hists_titles["ntrig_BC5"] = "N(triggers) / BC"
        hists_titles["ntrig_BC3to10"] = "N(triggers)"
        hists_titles["ntrig_BCm1"] = "N(triggers) / BC"

    for hists_2d in all_hists_2d:
        hists_2d["BC_ntrig"]  = ROOT.TH2D("BC_ntrig", "BC_ntrig", 17, -1.5, 15.5, 21, -0.5, 20.5)
        hists_2d["road_spread_ntrig"] = ROOT.TH2D("road_spread_ntrig", "road_spread_ntrig", 111, -10.5, 100.5, 21, -0.5, 20.5)

    for hists_2d_x in all_hists_2d_x:
        hists_2d_x["BC_ntrig"] = "BC"
        hists_2d_x["road_spread_ntrig"] = "road spread"

    for hists_2d_y in all_hists_2d_y:
        hists_2d_y["BC_ntrig"] = "N(triggers)"
        hists_2d_y["road_spread_ntrig"] = "N(triggers)"

    # get all fit information
    tstart = tm.time()
    nevents = trees[0].GetEntries()

    print "Number of events", nevents



    itree = 0

    tstart = tm.time()
    tdict = {} # dictionary number of triggers per BC

    for _tree in trees:
        i = 0
        nevents = _tree.GetEntries()
        while _tree.GetEntry(i):

            # clear dictionary
            for k in range(-1,16):
                tdict[k] = 0

            # count ntrig per BC
            for j in range(_tree.trigger_BC.size()):
                all_hists[itree]["trigger_BC"].Fill(_tree.trigger_BC[j])
                iBC = _tree.trigger_BC[j]
                if (iBC > 15):
                    continue
                tdict[iBC] += 1

            # fill ntrig BC histograms
            tot = 0
            for iBC, ntr in tdict.iteritems():
                all_hists_2d[itree]["BC_ntrig"].Fill(iBC,ntr)
                tot += ntr
                if iBC == -1:
                    all_hists[itree]["ntrig_BCm1"].Fill(ntr)
                    continue            
                if iBC == 5:
                    all_hists[itree]["ntrig_BC5"].Fill(ntr)
                    continue
                if iBC == 7:
                    all_hists[itree]["ntrig_BC7"].Fill(ntr)
                    continue


            all_hists[itree]["ntrig_BC3to10"].Fill(tdict[3]+tdict[4]+tdict[5]+tdict[6]+tdict[7]+tdict[8]+tdict[9]+tdict[10])
            all_hists[itree]["ntrig"].Fill(tot)
            
            i += 1
            #if (i>100):
            #    break
            progress(tm.time()-tstart, i, nevents)
            continue

            for j in range(_tree.Hit_strips.size()):
                all_hists[itree]["max_road"].Fill(max(_tree.Hit_strips[j])-min(_tree.Hit_strips[j]) + 1)
                all_hists[itree]["nhits"].Fill(len(_tree.Hit_strips[j]))
            if (_tree.iRoad_x.size() < 2):
                all_hists[itree]["road_spread"].Fill(0)
                all_hists_2d[itree]["road_spread_ntrig"].Fill(0,_tree.Ntriggers)
            else:
                all_hists[itree]["road_spread"].Fill(max(_tree.iRoad_x)-min(_tree.iRoad_x))
                all_hists_2d[itree]["road_spread_ntrig"].Fill(max(_tree.iRoad_x)-min(_tree.iRoad_x),_tree.Ntriggers)

            for j in range(_tree.Hit_planes.size()):
                x1 = 0
                x2 = 0
                x3 = 0
                x4 = 0
                for k in range(len(_tree.Hit_planes[j])):
                    all_hists[itree]["planes"].Fill(_tree.Hit_planes[j][k])
                    if (_tree.Hit_planes[j][k] == 0):
                        x1 += 1
                    if (_tree.Hit_planes[j][k] == 1):
                        x2 += 1
                    if (_tree.Hit_planes[j][k] == 6):
                        x3 += 1
                    if (_tree.Hit_planes[j][k] == 7):
                        x4 += 1
    #             if (x1 is not 1 or x2 is not 1 or x3 is not 1 or x4 is not 1):
    #                 print "nevent", _tree.EventNum
    #                 print "help:", x1, x2, x3, x4
    #                 sys.exit(1)
            for j in range(_tree.Hit_ages.size()):
                for k in range(_tree.Hit_ages[j].size()):
                    all_hists[itree]["ages"].Fill(_tree.Hit_ages[j][k])

            i += 1
            progress(tm.time()-tstart, i, nevents)
        itree += 1
    #hists["ntrig_BC5"].Fill(0, 100000-_tree.GetEntries())
    #hists["ntrig_BCm1"].Fill(0, 100000-_tree.GetEntries())
    setstyle()
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()

    
    if (args.l):
        c.SetLogy()
    for key,h1 in all_hists[0].iteritems():
        style(h1)
        h1.GetXaxis().SetNdivisions(505)
        h1.SetTitle("")
        h1.GetXaxis().SetTitle(hists_titles[key])
        h1.GetYaxis().SetTitle("Events")
        h1.SetLineWidth(2)
        h1.SetLineColor(ROOT.kAzure+7)
        h1.SetFillColorAlpha(ROOT.kAzure+7,0.4)
        h1.Draw("hist")
        show_overflow(h1)
        h2 = all_hists[1][key]
        style(h2)
        h2.SetLineWidth(2)
        h2.SetLineColor(ROOT.kPink-8)
        h2.SetFillColorAlpha(ROOT.kPink-8,0.4)
        if "ntrig" in key:
            show_overflow(h2)
            h2.Draw("hist same")

        texs = logo()
        for tex in texs:
            tex.Draw()
        if "ntrig" in key:
            legend = leg(h1,h2)
            legend.Draw("same")

        if args.l:
            title = key+"_log.pdf"
        else:
            title = key+".pdf"
        c.Print(title)
        c.Clear()

    setstyle_2d()
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()

    if args.l:
        c.SetLogz()
    for key,h1 in all_hists_2d[1].iteritems():
        print key
        print h1
        h1.SetTitle("")
        h1.GetXaxis().SetTitle(hists_2d_x[key])
        h1.GetYaxis().SetTitle(hists_2d_y[key])
        h1.GetXaxis().SetLabelSize(0.025)
        h1.GetYaxis().SetLabelSize(0.025)
        h1.GetXaxis().SetTitleSize(0.04)
        h1.GetYaxis().SetTitleSize(0.04)
        h1.GetXaxis().SetTitleOffset(1)
        h1.GetYaxis().SetTitleOffset(1)
        h1.GetZaxis().SetTitleOffset(1.3)
        style(h1)
        h1.Draw("colz")
        if args.l:
            title = key+"_log.pdf"
        else:
            title = key+".pdf"
        c.Print(title)
        c.Clear()

def progress(time_diff, nprocessed, ntotal):
    nprocessed, ntotal = float(nprocessed), float(ntotal)
    rate = (nprocessed+1)/time_diff
    msg = "\r > %6i / %6i | %2i%% | %8.2fHz | %6.1fm elapsed | %6.1fm remaining"
    msg = msg % (nprocessed, ntotal, 100*nprocessed/ntotal, rate, time_diff/60, (ntotal-nprocessed)/(rate*60))
    sys.stdout.write(msg)
    sys.stdout.flush()

def setstyle():
    from ROOT import gStyle
    gStyle.SetOptStat(0)
    gStyle.SetPadTopMargin(0.07)
    gStyle.SetPadRightMargin(0.05)
    gStyle.SetPadBottomMargin(0.13)
    gStyle.SetPadLeftMargin(0.19)
    gStyle.SetTitleOffset(1.2, 'x')
    gStyle.SetTitleOffset(1.8, 'y')
    gStyle.SetTextSize(0.05)
    gStyle.SetLabelSize(0.05, 'xyz')
    gStyle.SetTitleSize(0.05, 'xyz')
    gStyle.SetTitleSize(0.05, 't')
    gStyle.SetPadTickX(1)
    gStyle.SetPadTickY(1)

def setstyle_2d():
    from ROOT import gStyle
    gStyle.SetOptStat(0)
    gStyle.SetPadTopMargin(0.07)
    gStyle.SetPadRightMargin(0.18)
    gStyle.SetPadBottomMargin(0.13)
    gStyle.SetPadLeftMargin(0.19)
    gStyle.SetTitleOffset(1.2, 'x')
    gStyle.SetTitleOffset(1.8, 'y')
    gStyle.SetTextSize(0.05)
    gStyle.SetLabelSize(0.05, 'xyz')
    gStyle.SetTitleSize(0.05, 'xyz')
    gStyle.SetTitleSize(0.05, 't')
    gStyle.SetPadTickX(1)
    gStyle.SetPadTickY(1)

def style(hist):
    size = 0.045
    hist.GetXaxis().SetTitleSize(size)
    hist.GetXaxis().SetLabelSize(size)
    hist.GetYaxis().SetTitleSize(size)
    hist.GetYaxis().SetLabelSize(size)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.85)

def options():
    parser = argparse.ArgumentParser()
    parser.add_argument("-ib", default="", help="bkg input file")
    parser.add_argument("-isb", default="", help="sig + bkg input file")
    parser.add_argument("-l", action="store_true", help="log")

    return parser.parse_args()    

def logo():
    #tex_large = ROOT.TLatex(0.65, 0.85, "Strip length: 2.2m")
    #tex_small = ROOT.TLatex(0.65, 0.70, "Strip length: 0.5m")
    #tex_large.SetTextColor(color_large)
    #tex_small.SetTextColor(color_small)

    xlogo, ylogo = 0.65, 0.85
    tex0 = ROOT.TLatex(0.82, 0.94,          "HOTPOT")
    tex1 = ROOT.TLatex(xlogo, ylogo-1*0.04, "#sigma(t, ART): 32 ns")
    tex2 = ROOT.TLatex(xlogo, ylogo-2*0.04, "hit eff.: 100%")
    tex3 = ROOT.TLatex(xlogo, ylogo-3*0.04, "Rate: 40 kHz/strip")
    tex4 = ROOT.TLatex(xlogo, ylogo-4*0.04, "TP window: 8 BC")
    tex5 = ROOT.TLatex(xlogo, ylogo-5*0.04, "C: 3X3UV")
    tex6 = ROOT.TLatex(xlogo, ylogo-6*0.04, "N(roads): %d"%(nroads))    
    #tex6 = ROOT.TLatex(xlogo, ylogo-6*0.04, "N(roads): 136")
    tex_large = ROOT.TLatex(0.65, 0.85, "Strip length: 2.2m")

    texs = [tex0, tex1, tex2, tex3, tex4, tex5, tex6,
            #tex_small, 
            tex_large,
            ]
    for tex in texs:
        tex.SetNDC()
        tex.SetTextSize(0.032)
        tex.SetTextFont(42)
    tex0.SetTextColor(harvard)
    return texs

def leg(h1,h2):
    legend = ROOT.TLegend(0.30,0.75,0.4,0.85)
    legend.SetMargin(0.5)
    legend.AddEntry(h1,    " bkg","f")
    legend.AddEntry(h2,  " bkg + sig", "f")
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.035)
    return legend
def show_overflow(hist, show_underflow=True, show_overflow=True):
    """ h/t Josh """
    import math
    nbins = hist.GetNbinsX()
    xaxis = hist.GetXaxis()
    underflow      = sum([hist.GetBinContent(bin) for bin in xrange(0, xaxis.GetFirst())])
    overflow       = sum([hist.GetBinContent(bin) for bin in xrange(xaxis.GetLast()+1, nbins+2)])
    underflowerror = math.sqrt(sum([pow(hist.GetBinError(bin), 2.0) for bin in xrange(0, xaxis.GetFirst())]))
    overflowerror  = math.sqrt(sum([pow(hist.GetBinError(bin), 2.0) for bin in xrange(xaxis.GetLast()+1, nbins+2)]))
    firstbin       = hist.GetBinContent(xaxis.GetFirst())
    firstbinerror  = hist.GetBinError  (xaxis.GetFirst())
    lastbin        = hist.GetBinContent(xaxis.GetLast())
    lastbinerror   = hist.GetBinError  (xaxis.GetLast())
    if show_underflow and underflow != 0:
        newcontent = underflow + firstbin
        if firstbin == 0 :
            newerror = underflowerror
        else:
            newerror = math.sqrt( underflowerror * underflowerror + firstbinerror * firstbinerror )
        hist.SetBinContent(xaxis.GetFirst(), newcontent)
        hist.SetBinError  (xaxis.GetFirst(), newerror)
        for bin in xrange(0, xaxis.GetFirst()):
            hist.SetBinContent(bin, 0)
            hist.SetBinError  (bin, 0)
    if show_overflow and overflow != 0:
        newcontent = overflow + lastbin
        if lastbin == 0 :
            newerror = overflowerror
        else:
            newerror = math.sqrt( overflowerror * overflowerror + lastbinerror * lastbinerror )
        hist.SetBinContent(xaxis.GetLast(), newcontent)
        hist.SetBinError  (xaxis.GetLast(), newerror)
        for bin in xrange(xaxis.GetLast()+1, nbins+2):
            hist.SetBinContent(bin, 0)
            hist.SetBinError  (bin, 0)


if __name__ == "__main__":
    main(sys.argv[1:])

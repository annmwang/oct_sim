"""
Run like: 
> python dup.py

Does duplicate analysis (unique triggers), takes oct_sim output

"""


import ROOT, sys
import time as tm
from array import array
import argparse
ROOT.PyConfig.IgnoreCommandLineOptions = True

harvard = ROOT.TColor.GetColor("#A51C30")
color_large = ROOT.kRed
color_small = ROOT.kBlue
global nroads
nroads = -1

def main(argv):

    args = options()

    global nroads
    if "272" in args.isb:
        nroads = 272
    elif "136" in args.isb:
        nroads = 136
    elif "96" in args.isb:
        nroads = 96
    else:
        print "N(roads) not found! Exiting."
        sys.exit(1)

    my_bc = int(args.bc)

    sigbkg_file = ROOT.TFile(args.isb)
    _sigbkg_tree = sigbkg_file.Get("gingko")
    _sigbkg_branches = _sigbkg_tree.GetListOfBranches()

    trees = [_sigbkg_tree]

    i = 0

    all_hists = [{}]
    all_hists_titles = [{}]

    for hists in all_hists:
        hists["all_triggers"] = ROOT.TH1D("all_triggers", "all_triggers", 16, -0.5, 15.5)
        hists["unique_triggers_all"] = ROOT.TH1D("unique_triggers_all", "unique_triggers_all", 16, -0.5, 15.5)
        if int(args.thruv) != 0:
            hists["unique_trigs_per_x"] = ROOT.TH1D("unique_trigs_per_x", "unique_trigs_per_x", 16, -0.5, 15.5)
            hists["unique_xroads"] = ROOT.TH1D("unique_xroads", "unique_xroads", 16, -0.5, 15.5)

    for hists in all_hists:
        for key,h1 in hists.iteritems():
            h1.StatOverflows(ROOT.kTRUE)

    for hists_titles in all_hists_titles:
        hists_titles["unique_triggers_all"] = "N(unique triggers @ BC"+str(my_bc)+")"
        hists_titles["all_triggers"] = "N(triggers @ BC"+str(my_bc)+")"
        if int(args.thruv) != 0:
            hists_titles["unique_trigs_per_x"] = "N(unique triggers per x road @ BC"+str(my_bc)+")"
            hists_titles["unique_xroads"] = "N(unique x-roads @ BC"+str(my_bc)+")"
    
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
            ntrig = _tree.Hit_strips.size()
            d_dups = {}
            dups = []
            ntrig_7 = 0
            for j in range(ntrig):
                if _tree.trigger_BC[j] == my_bc:
                    ntrig_7 += 1
            all_hists[0]["all_triggers"].Fill(ntrig_7)
            for j in range(ntrig):
                if _tree.trigger_BC[j] != my_bc:
                    continue
                # print hits
#                 print "%d: "%(j),
#                 for hit in _tree.Hit_strips[j]:
#                     print hit,
#                     print ", ",
#                 print

                if j in dups:
                    continue
                for k in range(j,ntrig):
                    if _tree.trigger_BC[k] != my_bc:
                        continue
                    if k in dups:
                        continue
                    jtrigger = set(_tree.Hit_strips[j])
                    ktrigger = set(_tree.Hit_strips[k])
                    if len(ktrigger.difference(jtrigger)) == 0:
                        if j not in d_dups.keys():
                            d_dups[j] = [k]
                        else:
                            d_dups[j].append(k)
                        if k not in dups:
                            dups.append(k)
            xroads = {}
            if args.v:
                print
                print "EVENT", _tree.EventNum
                print
                print "ntrig @ BC 7:",ntrig_7
                print "duplicates:",d_dups
            for key in d_dups.keys():
                my_xroad = _tree.iRoad_x[key]
                if my_xroad not in xroads.keys():
                    xroads[my_xroad] = [key]
                else:
                    xroads[my_xroad].append(key)
            if args.v:
                print "xroads + assoc. itrig:", xroads
            if int(args.thruv) != 0:
                for key, value in xroads.iteritems():
                    all_hists[0]["unique_trigs_per_x"].Fill(len(value))
                all_hists[0]["unique_xroads"].Fill(len(xroads.keys()))
            if args.v:
                print "unique triggers:"
                for key in d_dups.keys():
                    print "itrig %d, (xroad %d, uroad %d, vroad %d): "%(key, _tree.iRoad_x[key], _tree.iRoad_u[key], _tree.iRoad_v[key]),
                    for m,hit in enumerate(_tree.Hit_strips[key]):   
                        print "%d:"%(_tree.Hit_planes[key][m]),
                        print hit,                                                                                             
                        print ", ",                                                                                            
                    print 
            all_hists[0]["unique_triggers_all"].Fill(len(d_dups.keys()))
            i += 1
            if not(args.v):
                progress(tm.time()-tstart, i, nevents)
        itree += 1

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
        tex0 = ROOT.TLatex(0.28, 0.75,"#mu = %3.3f"%(h1.GetMean()))
        tex0.SetNDC()
        tex0.SetTextSize(0.032)
        tex0.SetTextFont(42)
        tex0.Draw()
        tex1 = ROOT.TLatex(0.28, 0.75-0.04,"%d events"%(h1.Integral()))
        tex1.SetNDC()
        tex1.SetTextSize(0.032)
        tex1.SetTextFont(42)
        tex1.Draw()
        texs = []
        if args.large is True:
            texs = logo(args.thruv, "large")
        else:
            texs = logo(args.thruv, "small")
        for tex in texs:
            tex.Draw()
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
    parser.add_argument("-isb", default="", help="sig + bkg input file")
    parser.add_argument("-l", action="store_true", help="log")
    parser.add_argument("-bc", default=-1, help="bc")
    parser.add_argument("-thruv", default=-1, help="threshold uv")
    parser.add_argument("--large", action="store_true", help="large")
    parser.add_argument("--small", action="store_true", help="small")
    parser.add_argument("-v", action="store_true", help="verbosity")

    return parser.parse_args()    

def logo(thr, ch):
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
    if (int(thr) == 0):
        tex5 = ROOT.TLatex(xlogo, ylogo-5*0.04, "C: 3X+")
    else:
        tex5 = ROOT.TLatex(xlogo, ylogo-5*0.04, "C: 3X3UV")
    tex6 = ROOT.TLatex(xlogo, ylogo-6*0.04, "N(roads): %d"%(nroads))    
    #tex6 = ROOT.TLatex(xlogo, ylogo-6*0.04, "N(roads): 136")
    tex_large = ROOT.TLatex(0.65, 0.85, "Strip length: 2.2m")
    tex_small = ROOT.TLatex(0.65, 0.85, "Strip length: 0.5m")

    texs = []
    if ch == "large":
        texs = [tex0, tex1, tex2, tex3, tex4, tex5, tex6,
                tex_large,
                ]
    else:
        texs = [tex0, tex1, tex2, tex3, tex4, tex5, tex6,
                tex_small,
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

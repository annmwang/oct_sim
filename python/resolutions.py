"""
Run like:
> python resolutions.py -i hadd-2017-10-25-ideal.root -b 20000

Assumes the histograms are hella finely binned.
"""
import argparse
import array
import copy
import sys
import ROOT
ROOT.gROOT.SetBatch()
ROOT.PyConfig.IgnoreCommandLineOptions = True

harvard = ROOT.TColor.GetColor("#A51C30")

def main():

    ops = options()
    rootlogon()

    if not ops.i:
        fatal("Please provide input ROOT file of histograms with -i")
    if not ops.b:
        fatal("Please provide the rate of background youd like to plot with -b")

    input = ROOT.TFile.Open(ops.i)
    bkg   = int(ops.b)

    color_large = ROOT.kRed
    color_small = ROOT.kBlue

    yres_large_old = input.Get("histograms_large_e1.00_b%05i_nUVR/h_yres"  % (bkg))
    yres_small_old = input.Get("histograms_small_e1.00_b%05i_nUVR/h_yres"  % (bkg))
    yres_large_new = input.Get("histograms_large_e1.00_b%05i_yUVR/h_yres"  % (bkg))
    yres_small_new = input.Get("histograms_small_e1.00_b%05i_yUVR/h_yres"  % (bkg))
    yres_large_cen = input.Get("histograms_large_e1.00_b%05i_yUVR/h_yres_center"  % (bkg))
    yres_small_cen = input.Get("histograms_small_e1.00_b%05i_yUVR/h_yres_center"  % (bkg))
    xres_large_old = input.Get("histograms_large_e1.00_b%05i_nUVR/h_xres"  % (bkg))
    xres_small_old = input.Get("histograms_small_e1.00_b%05i_nUVR/h_xres"  % (bkg))
    xres_large_new = input.Get("histograms_large_e1.00_b%05i_yUVR/h_xres"  % (bkg))
    xres_small_new = input.Get("histograms_small_e1.00_b%05i_yUVR/h_xres"  % (bkg))
    xres_large_cen = input.Get("histograms_large_e1.00_b%05i_yUVR/h_xres_center"  % (bkg))
    xres_small_cen = input.Get("histograms_small_e1.00_b%05i_yUVR/h_xres_center"  % (bkg))
    mres_large_old = input.Get("histograms_large_e1.00_b%05i_nUVR/h_mxres" % (bkg))
    mres_small_old = input.Get("histograms_small_e1.00_b%05i_nUVR/h_mxres" % (bkg))
    mres_large_new = input.Get("histograms_large_e1.00_b%05i_yUVR/h_mxres" % (bkg))
    mres_small_new = input.Get("histograms_small_e1.00_b%05i_yUVR/h_mxres" % (bkg))
    for h in [yres_large_old,
              yres_small_old,
              yres_large_new,
              yres_small_new,
              yres_large_cen,
              yres_small_cen,
              xres_large_old,
              xres_small_old,
              xres_large_new,
              xres_small_new,
              xres_large_cen,
              xres_small_cen,
              mres_large_old,
              mres_small_old,
              mres_large_new,
              mres_small_new,
              ]:
        if not h:
            fatal("At least one histogram you desire is not in the ROOT file.")
        name = h.GetName()
        h.GetXaxis().SetNdivisions(505)
        h.SetLineWidth(2)
        h.SetTitle("")
        h.GetYaxis().SetTitle("Triggers")
        if "mxres" in name:
            #h.Rebin(5)
            h.Rebin(3)
            h.GetXaxis().SetTitle("#theta_{reco.} #minus #theta_{truth} [mrad]")
            h.GetXaxis().SetRangeUser(-40, 40)
        elif "xres" in name:
            h.GetXaxis().SetTitle("x_{reco.} #minus x_{truth} [mm]")
            #h.GetXaxis().SetRangeUser(-10, 10)
        elif "yres" in name:
            h.GetXaxis().SetTitle("y_{reco.} #minus y_{truth} [mm]")
            if bkg > 0:
                h.Rebin(10)
                h.GetXaxis().SetRangeUser(-2400, 2400)
            else:
                h.GetXaxis().SetRangeUser(-40, 40)
            #h.GetXaxis().SetRangeUser(-200, 200)
        style(h)
        show_overflow(h)
        h.SetMinimum(0.3)

    yres_large_old.SetLineColor(color_large)
    yres_small_old.SetLineColor(color_small)
    yres_large_new.SetLineColor(color_large)
    yres_small_new.SetLineColor(color_small)
    xres_large_old.SetLineColor(color_large)
    xres_small_old.SetLineColor(color_small)
    xres_large_new.SetLineColor(color_large)
    xres_small_new.SetLineColor(color_small)
    mres_large_old.SetLineColor(color_large)
    mres_small_old.SetLineColor(color_small)
    mres_large_new.SetLineColor(color_large)
    mres_small_new.SetLineColor(color_small)

    yres_large_cen.SetLineColor(color_large)
    yres_small_cen.SetLineColor(color_small)
    xres_large_cen.SetLineColor(color_large)
    xres_small_cen.SetLineColor(color_small)


    tex_blurb_old = ROOT.TLatex(0.64, 0.80, "Nominal MMTP alg.")
    tex_blurb_new = ROOT.TLatex(0.63, 0.80, "Proposed MMTP alg.")
    tex_blurb_cen = ROOT.TLatex(0.65, 0.80, "Road coordinates")
    tex_large = ROOT.TLatex(0.65, 0.75, "Strip length: 2.2m")
    tex_small = ROOT.TLatex(0.65, 0.70, "Strip length: 0.5m")
    tex_large.SetTextColor(color_large)
    tex_small.SetTextColor(color_small)

    xlogo, ylogo = 0.25, 0.85
    tex0 = ROOT.TLatex(0.82, 0.94,          "HOTPOT")
    tex1 = ROOT.TLatex(xlogo, ylogo-1*0.04, "#sigma(t, ART): 32 ns")
    tex2 = ROOT.TLatex(xlogo, ylogo-2*0.04, "hit eff.: 100%")
    tex3 = ROOT.TLatex(xlogo, ylogo-3*0.04, "Rate: %i kHz/strip" % (bkg/1000))
    tex4 = ROOT.TLatex(xlogo, ylogo-4*0.04, "TP window: 8 BC")
    tex5 = ROOT.TLatex(xlogo, ylogo-5*0.04, "C: 3X3UV")
    texs = [tex0, tex1, tex2, tex3, tex4, tex5,
            tex_blurb_old, tex_blurb_new, tex_blurb_cen, tex_small, tex_large,
            ]
    for tex in texs:
        tex.SetNDC()
        tex.SetTextSize(0.032)
        tex.SetTextFont(42)
    tex0.SetTextColor(harvard)
    
    # y resolution
    canv = ROOT.TCanvas("yres_old", "yres_old", 800, 800)
    canv.Draw()
    yres_small_old.Draw("histsame")
    yres_large_old.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_new and tex != tex_blurb_cen:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    canv = ROOT.TCanvas("yres_new", "yres_new", 800, 800)
    canv.Draw()
    yres_small_new.Draw("histsame")
    yres_large_new.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_old and tex != tex_blurb_cen:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    # x resolution
    canv = ROOT.TCanvas("xres_old", "xres_old", 800, 800)
    canv.Draw()
    xres_small_old.Draw("histsame")
    xres_large_old.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_new and tex != tex_blurb_cen:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    canv = ROOT.TCanvas("xres_new", "xres_new", 800, 800)
    canv.Draw()
    xres_small_new.Draw("histsame")
    xres_large_new.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_old and tex != tex_blurb_cen:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    # theta resolution
    canv = ROOT.TCanvas("mres_old", "mres_old", 800, 800)
    canv.Draw()
    mres_small_old.Draw("histsame")
    mres_large_old.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_new and tex != tex_blurb_cen:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    canv = ROOT.TCanvas("mres_new", "mres_new", 800, 800)
    canv.Draw()
    mres_small_new.Draw("histsame")
    mres_large_new.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_old and tex != tex_blurb_cen:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    # also do the center comparison

    # y resolution
    canv = ROOT.TCanvas("yres_cen", "yres_cen", 800, 800)
    canv.Draw()
    yres_small_cen.Draw("histsame")
    yres_large_cen.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_new and tex != tex_blurb_old:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

    # x resolution
    canv = ROOT.TCanvas("xres_cen", "xres_cen", 800, 800)
    canv.Draw()
    xres_small_cen.Draw("histsame")
    xres_large_cen.Draw("histsame")
    for tex in texs:
        if tex != tex_blurb_new and tex != tex_blurb_old:
            tex.Draw()
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(canv.GetName()+".pdf")

def fatal(msg):
    sys.exit("Fatal error: %s" % (msg))

def style(hist):
    size = 0.045
    hist.GetXaxis().SetTitleSize(size)
    hist.GetXaxis().SetLabelSize(size)
    hist.GetYaxis().SetTitleSize(size)
    hist.GetYaxis().SetLabelSize(size)
    hist.GetXaxis().SetTitleOffset(1.1)
    hist.GetYaxis().SetTitleOffset(1.85)

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

def options():
    parser = argparse.ArgumentParser(usage=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-i", default="", help="Input ROOT file")
    parser.add_argument("-b", default=0,  help="Desired rate of background")
    return parser.parse_args()

def rootlogon():
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

if __name__ == "__main__":
    main()


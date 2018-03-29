import copy
import ROOT
ROOT.gROOT.SetBatch()

def main():

    rootlogon()

    # put batch files here
    # f8 :: no boards killed
    # f7 :: -killran
    # f6 :: -killxran -killuvran
    f8 = ROOT.TFile.Open("batch-2018-03-27-10h16m14s.root")
    f7 = ROOT.TFile.Open("batch-2018-03-27-10h17m25s.root")
    f6 = ROOT.TFile.Open("batch-2018-03-27-10h18m35s.root")

    # retrieve the histograms
    hnumer = "histograms/h_theta_trig"
    hdenom = "histograms/h_theta"

    numer8 = f8.Get(hnumer)
    denom8 = f8.Get(hdenom)
    numer7 = f7.Get(hnumer) 
    denom7 = f7.Get(hdenom)
    numer6 = f6.Get(hnumer)
    denom6 = f6.Get(hdenom)

    # make the efficiencies
    ratio8 = efficiency(numer8, denom8)
    ratio7 = efficiency(numer7, denom7)
    ratio6 = efficiency(numer6, denom6)

    ratio8.SetMarkerColor(color(8))
    ratio8.SetLineColor(color(8))
    ratio7.SetMarkerColor(color(7))
    ratio7.SetLineColor(color(7))
    ratio6.SetMarkerColor(color(6))
    ratio6.SetLineColor(color(6))

    # get the weighted efficiency
    ratioW = copy.copy(ratio8)
    ratioW.SetLineColor(ROOT.kBlack)
    ratioW.SetMarkerColor(ROOT.kBlack)
    for bin in xrange(0, ratioW.GetNbinsX()+2):
        ratioW.SetBinContent(bin, 0)
        ratioW.SetBinError(bin, 0)
    ratioW.Add(ratio8, weight(8))
    ratioW.Add(ratio7, weight(7))
    ratioW.Add(ratio6, weight(6))
    ratioW.SetMaximum(1.05)

    # split by N(hits)
    can = ROOT.TCanvas("can", "can", 800, 800)
    can.Draw()
    ratio8.Draw("pesame")
    ratio6.Draw("pesame")
    ratio7.Draw("pesame")
    ratio8.Draw("pesame")
    xleg, yleg = 0.72, 0.67
    legend = ROOT.TLegend(xleg, yleg, xleg+0.18, yleg+0.20)
    legend.AddEntry(ratio8, "8 hits",   "p")
    legend.AddEntry(ratio7, "7 hits",   "p")
    legend.AddEntry(ratio6, "6 hits",   "p")
    #legend.AddEntry(ratioW, "Weighted", "p")
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.045)
    legend.Draw()
    can.SetLogy(0)
    can.SaveAs("sim_eff_nhits_lin.pdf")
    can.SetLogy(1)
    can.SaveAs("sim_eff_nhits_log.pdf")

    # weighted
    can = ROOT.TCanvas("can2", "can2", 800, 800)
    can.Draw()
    ratioW.Draw("pesame")
    can.SetLogy(0)
    can.SaveAs("sim_eff_weight_lin.pdf")
    can.SetLogy(1)
    can.SaveAs("sim_eff_weight_log.pdf")

def efficiency(numer, denom):

    numer.Rebin(2)
    denom.Rebin(2)

    ratio = copy.copy(numer)
    ratio.Divide(numer, denom, 1.0, 1.0, "B")

    # style
    ratio.GetXaxis().SetTitleSize(0.045)
    ratio.GetXaxis().SetLabelSize(0.045)
    ratio.GetYaxis().SetTitleSize(0.045)
    ratio.GetYaxis().SetLabelSize(0.045)
    ratio.GetXaxis().SetRangeUser(-5, 5)
    ratio.SetMaximum(1.05)
    # ratio.SetMinimum(0.7)
    ratio.GetXaxis().SetTitle("Angle [deg.]")
    ratio.GetYaxis().SetTitle("Efficiency")
    ratio.SetMarkerStyle(20)
    # ratio.SetMarkerStyle(24)
    ratio.SetMarkerSize(1.0)
    ratio.GetXaxis().SetNdivisions(505)
    ratio.GetYaxis().SetNdivisions(515)

    return ratio

def weight(nhits):
    norm = 245.0
    if nhits == 6: return  32 / norm
    if nhits == 7: return 140 / norm
    if nhits == 8: return  73 / norm

def color(nhits):
    if nhits == 6: return ROOT.kGreen-7
    if nhits == 7: return ROOT.kMagenta-7
    if nhits == 8: return ROOT.kAzure+1
    return ROOT.kBlack

def rootlogon():
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPadTopMargin(0.06)
    ROOT.gStyle.SetPadRightMargin(0.05)
    ROOT.gStyle.SetPadBottomMargin(0.12)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetPaintTextFormat(".2f")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetFillColor(10)

if __name__ == "__main__":
    main()

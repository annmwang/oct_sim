"""
Run like:
> python perf_vs_rate.py --large

Assumes the histograms are hella finely binned.
"""
import array
import copy
import sys
import ROOT
ROOT.gROOT.SetBatch()
ROOT.PyConfig.IgnoreCommandLineOptions = True

harvard = ROOT.TColor.GetColor("#A51C30")

def main():

    rootlogon()
    chamber = None
    if "--large" in sys.argv: chamber = "large"
    if "--small" in sys.argv: chamber = "small"
    if not chamber:
        fatal("Please provide --large or --small chamber")
    if chamber == "large":
        distance_R = 4.5
    else:
        distance_R = 1.0

    input = ROOT.TFile.Open("all_res_hadd.root")
    histnames_nUVR = ["histograms_%s_e1.00_b00000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b02000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b05000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b10000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b15000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b20000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b25000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b30000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b40000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b50000_nUVR/h_yres" % (chamber),
                      "histograms_%s_e1.00_b60000_nUVR/h_yres" % (chamber),
                      ]
    histnames_yUVR = [h.replace("nUVR", "yUVR") for h in histnames_nUVR]
    hists_nUVR = [input.Get(name) for name in histnames_nUVR]
    hists_yUVR = [input.Get(name) for name in histnames_yUVR]
    for (hist, name) in zip(hists_nUVR+hists_yUVR, histnames_nUVR+histnames_yUVR):
        if not hist:
            fatal("Couldnt find %s" % (name))
    
    xs = [0, 2, 5, 10, 15, 20, 25, 30, 40, 50, 60]

    rms_nUVR = get_rms(hists_nUVR)
    rms_yUVR = get_rms(hists_yUVR)
    titley = "RMS of y [mm]"
    output = "rms_y_%s_vs_rate.pdf" % (chamber)
    graphify(chamber, xs, rms_nUVR, rms_yUVR, titley, output, log=False)

    rms_nUVR = [rms/distance_R for rms in rms_nUVR]
    rms_yUVR = [rms/distance_R for rms in rms_yUVR]
    titley = "RMS of #phi at R = %s m [mrad]" % (distance_R)
    output = "rms_phi_%s_vs_rate.pdf" % (chamber)
    graphify(chamber, xs, rms_nUVR, rms_yUVR, titley, output, log=False)

    eff_nUVR = [h.Integral(h.FindBin(-20*distance_R), h.FindBin(20*distance_R)) / h.Integral() for h in hists_nUVR]
    eff_yUVR = [h.Integral(h.FindBin(-20*distance_R), h.FindBin(20*distance_R)) / h.Integral() for h in hists_yUVR]
    titley = "Efficiency of #Delta#phi < 20 mrad (R = %s m)" % (distance_R)
    output = "eff_phi_%s_vs_rate.pdf" % (chamber)
    graphify(chamber, xs, eff_nUVR, eff_yUVR, titley, output, log=False, adjust_logo=(1 if chamber=="large" else 2))

    for eff in [68, 95, 99]:
    #for eff in []:

        intervals_nUVR = get_intervals(hists_nUVR, eff/100.0)
        intervals_yUVR = get_intervals(hists_yUVR, eff/100.0)
        titley = "%i%% efficiency interval [mm]" % (eff)
        output = "interval%i_y_%s_vs_rate.pdf" % (eff, chamber)
        graphify(chamber, xs, intervals_nUVR, intervals_yUVR, titley, output, log=False)
        
        intervals_nUVR = [interval/distance_R for interval in intervals_nUVR]
        intervals_yUVR = [interval/distance_R for interval in intervals_yUVR]
        titley = "%i%% eff. interval at R = %s m [mrad]" % (eff, distance_R)
        output = "interval%i_phi_%s_vs_rate.pdf" % (eff, chamber)
        graphify(chamber, xs, intervals_nUVR, intervals_yUVR, titley, output, log=False)

    
def graphify(chamber, xs, ys_nUVR, ys_yUVR, titley, output, log, adjust_logo=False):

    if len(xs) != len(ys_nUVR): fatal("Inputs have different lengths!")
    if len(xs) != len(ys_yUVR): fatal("Inputs have different lengths!")

    gr_nUVR = ROOT.TGraph(len(xs), array.array("d", xs), array.array("d", ys_nUVR))
    gr_yUVR = ROOT.TGraph(len(xs), array.array("d", xs), array.array("d", ys_yUVR))
    titlex = "Uncorrelated hit rate [kHz / strip]"
    for gr in [gr_nUVR, gr_yUVR]:
        color = 210 if gr==gr_yUVR else ROOT.kBlue
        gr.SetLineWidth(2)
        gr.SetLineColor(color)
        gr.SetMarkerColor(color)
        gr.SetMarkerStyle(20)
        gr.SetMarkerSize(1.5)

    canv = ROOT.TCanvas("canv", "canv", 800, 800)
    canv.Draw()

    chamber_width = 2.2 if chamber=="large" else 0.5
    xlogo, ylogo = 0.25, 0.85
    if adjust_logo == 1:
        xlogo, ylogo = 0.6, 0.59
    if adjust_logo == 2:
        xlogo, ylogo = 0.6, 0.39
    tex0 = ROOT.TLatex(0.72, 0.95,          "HOTPOT")
    tex1 = ROOT.TLatex(xlogo, ylogo,        "Strip length: %s m" % (chamber_width))
    tex2 = ROOT.TLatex(xlogo, ylogo-1*0.05, "#sigma(t, ART): 32 ns")
    tex3 = ROOT.TLatex(xlogo, ylogo-2*0.05, "hit eff.: 100%")
    tex4 = ROOT.TLatex(xlogo, ylogo-3*0.05, "TP window: 8 BC")
    tex5 = ROOT.TLatex(xlogo, ylogo-4*0.05, "C: 3X3UV")

    xlogo, ylogo = 0.54, 0.46
    if adjust_logo == 1:
        xlogo, ylogo = 0.40, 0.75
    if adjust_logo == 2:
        xlogo, ylogo = 0.25, 0.55
    tex_old = ROOT.TLatex(xlogo, ylogo,      "Nominal algorithm")
    tex_new = ROOT.TLatex(xlogo, ylogo-0.06, "Small stereo roads")

    texs = [tex0, tex1, tex2, tex3, tex4, tex5, tex_old, tex_new]
    for tex in texs:
        tex.SetNDC()
        tex.SetTextSize(0.035)
        tex.SetTextFont(42)
    tex_old.SetTextColor(ROOT.kBlue)
    tex_new.SetTextColor(210)
    tex_old.SetTextSize(0.045)
    tex_new.SetTextSize(0.045)

    tex0.SetTextColor(harvard)
    #tex0.SetTextColor(1)
    tex0.SetTextSize(0.04)

    multi = ROOT.TMultiGraph()
    for gr in [gr_nUVR, gr_yUVR]:
        multi.Add(gr, "PL")
    multi.SetTitle(";%s;%s" % (titlex, titley))
    multi.SetMinimum(0)
    # multi.SetMaximum(500)
    multi.Draw("A")
    # multi.GetXaxis().SetLimits(0, 50)
    multi.Draw("A")
    for tex in texs:
        tex.Draw()

    ROOT.gPad.SetLogy(1 if log else 0)
    ROOT.gPad.RedrawAxis()
    canv.SaveAs(output)

def get_rms(hists, eff=0.997):
    # return [hist.GetRMS() for hist in hists]
    rmss = []
    for hist in hists:
        interval = get_interval(hist, eff)
        hist.GetXaxis().SetRangeUser(-interval, interval)
        rmss.append(hist.GetRMS())
        hist.GetXaxis().UnZoom()
    return rmss

def get_intervals(hists, eff):
    return [get_interval(hist, eff) for hist in hists]

def get_interval(hist, eff):
    total = hist.Integral(0, hist.GetNbinsX()+1)
    for mm in xrange(0, 30000):
        mm = mm/10.0
        this_eff = hist.Integral(hist.FindBin(-mm), hist.FindBin(mm)) / total
        if this_eff >= eff:
            return mm
    fatal("Histogram %s never reached %f efficiency!" % (hist, eff))

def fatal(msg):
    sys.exit("Fatal error: %s" % (msg))

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


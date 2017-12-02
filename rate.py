import array
import math
import ROOT
ROOT.gROOT.SetBatch()

def main():

    rootlogon()
    ROOT.gStyle.SetLabelSize(0.05, 'xyz')
    ROOT.gStyle.SetTitleSize(0.05, 'xyz')
    ROOT.gStyle.SetPadTopMargin(0.06)
    ROOT.gStyle.SetPadRightMargin(0.05)
    ROOT.gStyle.SetPadBottomMargin(0.13)
    ROOT.gStyle.SetPadLeftMargin(0.14)
    ROOT.gStyle.SetTitleOffset(1.2, 'x')
    ROOT.gStyle.SetTitleOffset(1.2, 'y')

    xs, ys = [], []
    for (x, y) in data():
        xs.append(x)
        ys.append(y)
    xs = array.array("d", xs)
    ys = array.array("d", ys)
    gr = ROOT.TGraph(len(xs), xs, ys)

    titlex = "Distance from beam line, R [cm]"
    titley = "Predicted hit rate [ kHz/cm^{2} ]"
    gr.SetTitle(";%s;%s" % (titlex, titley))
    gr.SetMarkerColor(210)
    gr.SetMarkerStyle(20)
    gr.SetMarkerSize(1.5)

    # try your favorite fit here
    fit = ROOT.TF1("fit", "[0]/(x) + [1]/(x*x)", 90, 450)
    #fit = ROOT.TF1("fit", "[0] + [1]/(x)", 90, 450)
    #fit = ROOT.TF1("fit", "[0] + [1]/(x*x)", 90, 450)
    #fit = ROOT.TF1("fit", "[1]/(x)", 90, 200)
    #fit = ROOT.TF1("fit", "[0] + [1]/(x) + [2]/(x*x)", 90, 200)
    #fit.FixParameter(0, 0.0)
    gr.Fit(fit, "RQN")
    fit.SetLineStyle(1)
    fit.SetLineColor(ROOT.kBlack)

    tex = ROOT.TLatex(0.30, 0.82, "")
    #tex = ROOT.TLatex(0.35, 0.62, "f(R) = #frac{%.1f cm}{R} + #frac{(%.1f cm)^{2}}{R^{2}}" 
    #                  % (fit.GetParameter(1),
    #                     math.sqrt(fit.GetParameter(2)),
    #                     ))
    tex.SetNDC()
    tex.SetTextSize(0.045)
    tex.SetTextFont(132)

    canv = ROOT.TCanvas("canv", "canv", 800, 800)
    canv.Draw()

    multi = ROOT.TMultiGraph()
    multi.Add(gr, "P")
    multi.SetTitle(gr.GetTitle())
    multi.SetMinimum(0)
    multi.SetMaximum(25)
    multi.Draw("A")
    multi.GetXaxis().SetLimits(90, 450)
    multi.Draw("A")
    fit.Draw("same")
    tex.Draw()
    
    ROOT.gPad.RedrawAxis()
    canv.SaveAs("rate.pdf")

    rate_per_strip(fit)

def rate_per_strip(fit):

    ROOT.gStyle.SetPadTopMargin(0.08)

    titlex = "Distance from beam line, R [cm]"
    titley = "Predicted hit rate [ kHz/strip ]"

    # dimensions
    # cf. MuonSpectrometer/MuonG4/NSW_Sim/trunk/data/stations.v1.75.xml

    # L
    baseWidthL_1 = 640.0
    topWidthL_1  = 2008.5
    baseWidthL_2 = 2022.8
    topWidthL_2  = 2220.0
    baseRL_1     = 923.0
    topRL_1      = 923.0 + 2310.0
    baseRL_2     = 923.0 + 2310.0 + 5.0
    topRL_2      = 923.0 + 2310.0 + 5.0 + 1410.0
    pitchL       = 0.04

    # S
    baseWidthS_1 = 500.0
    topWidthS_1  = 1319.2
    baseWidthS_2 = 1321.1
    topWidthS_2  = 1821.5
    baseRS_1     = 895.0
    topRS_1      = 895.0 + 2210.0
    baseRS_2     = 895.0 + 2210.0 + 5.0
    topRS_2      = 895.0 + 2210.0 + 5.0 + 1350.0
    pitchS       = 0.04

    # make TGraphs, L
    xsL, ysL = [], []
    for r in xrange( int(baseRL_1), int(topRL_1) ):
        xsL.append(r/10.0)
        ysL.append(fit.Eval(r/10.0)*pitchL*width(r, baseWidthL_1, topWidthL_1, baseRL_1, topRL_1)/10)
    for r in xrange( int(baseRL_2), int(topRL_2) ):
        xsL.append(r/10.0)
        ysL.append(fit.Eval(r/10.0)*pitchL*width(r, baseWidthL_2, topWidthL_2, baseRL_2, topRL_2)/10)
    xs = array.array("d", xsL)
    ys = array.array("d", ysL)

    grL = ROOT.TGraph(len(xs), xs, ys)
    grL.SetTitle(";%s;%s" % (titlex, titley))
    grL.SetMarkerColor(ROOT.kRed)
    grL.SetMarkerStyle(20)
    grL.SetMarkerSize(0.9)

    # make TGraphs, S
    xsS, ysS = [], []
    for r in xrange( int(baseRS_1), int(topRS_1) ):
        xsS.append(r/10.0)
        ysS.append(fit.Eval(r/10.0)*pitchS*width(r, baseWidthS_1, topWidthS_1, baseRS_1, topRS_1)/10)
    for r in xrange( int(baseRS_2), int(topRS_2) ):
        xsS.append(r/10.0)
        ysS.append(fit.Eval(r/10.0)*pitchS*width(r, baseWidthS_2, topWidthS_2, baseRS_2, topRS_2)/10)
    xs = array.array("d", xsS)
    ys = array.array("d", ysS)

    grS = ROOT.TGraph(len(xs), xs, ys)
    grS.SetTitle(";%s;%s" % (titlex, titley))
    grS.SetMarkerColor(ROOT.kBlue)
    grS.SetMarkerStyle(20)
    grS.SetMarkerSize(0.9)

    # fits!
    fitL = ROOT.TF1("fitL", "[0] + [1]/(x) + [2]/(x*x)", 90, 450)
    fitS = ROOT.TF1("fitS", "[0] + [1]/(x) + [2]/(x*x)", 90, 450)
    grL.Fit(fitL, "RQN")
    grS.Fit(fitS, "RQN")
    fitL.SetLineStyle(7)
    fitS.SetLineStyle(7)
    fitL.SetLineColor(ROOT.kBlack)
    fitS.SetLineColor(ROOT.kGray)

    print
    print "Large sector, rate per strip: (%f) + (%f)/r + (%f)/r^2" % (fitL.GetParameter(0),
                                                                      fitL.GetParameter(1),
                                                                      fitL.GetParameter(2),
                                                                      )
    print "Small sector, rate per strip: (%f) + (%f)/r + (%f)/r^2" % (fitS.GetParameter(0),
                                                                      fitS.GetParameter(1),
                                                                      fitS.GetParameter(2),
                                                                      )
    print

    canv = ROOT.TCanvas("canv2", "canv2", 800, 800)
    canv.Draw()

    strip_botL = ROOT.TLatex(0.15, 0.97, "l = %4i mm @ R = %4i mm" % (baseWidthL_1, baseRL_1))
    strip_topL = ROOT.TLatex(0.15, 0.94, "l = %4i mm @ R = %4i mm" % (topWidthL_2,  topRL_2))
    strip_botS = ROOT.TLatex(0.60, 0.97, "l = %4i mm @ R = %4i mm" % (baseWidthS_1, baseRS_1))
    strip_topS = ROOT.TLatex(0.60, 0.94, "l = %4i mm @ R = %4i mm" % (topWidthS_2,  topRS_2))
    for tex in [strip_botL, strip_topL, strip_botS, strip_topS]:
        if tex in [strip_botL, strip_topL]: tex.SetTextColor(ROOT.kRed)
        if tex in [strip_botS, strip_topS]: tex.SetTextColor(ROOT.kBlue)
        tex.SetNDC()
        tex.SetTextSize(0.025)
        tex.SetTextFont(82)

    texZ = ROOT.TLatex(0.35, 0.81, "L = 7.5#times10^{34} Hz/cm^{2}")
    texL = ROOT.TLatex(0.35, 0.75, "Large wedge, %i #mum pitch" % (pitchL*10000))
    texS = ROOT.TLatex(0.35, 0.69, "Small wedge, %i #mum pitch" % (pitchS*10000))
    texX = ROOT.TLatex(0.35, 0.63, "Fits overlaid")
    texZ.SetTextColor(ROOT.kBlack)
    texL.SetTextColor(ROOT.kRed)
    texS.SetTextColor(ROOT.kBlue)
    texX.SetTextColor(ROOT.kBlack)
    for tex in [texZ, texL, texS, texX]:
        tex.SetNDC()
        tex.SetTextSize(0.045)
        tex.SetTextFont(132)

    texfitL = ROOT.TLatex(0.45, 0.50, "f(R) = %.2f + #frac{%i}{R} + #frac{%i}{R^{2}}"
                          % (fitL.GetParameter(0), fitL.GetParameter(1), fitL.GetParameter(2)))
    texfitS = ROOT.TLatex(0.45, 0.40, "f(R) = %.2f + #frac{%i}{R} + #frac{%i}{R^{2}}"
                          % (fitS.GetParameter(0), fitS.GetParameter(1), fitS.GetParameter(2)))
    texfitL.SetTextColor(ROOT.kRed)
    texfitS.SetTextColor(ROOT.kBlue)
    for tex in [texfitL, texfitS]:
        tex.SetNDC()
        tex.SetTextSize(0.040)
        tex.SetTextFont(132)

    textuna = ROOT.TLatex(0.96, 0.8, "Tunaplot")
    textuna.SetNDC()
    textuna.SetTextSize(0.035)
    textuna.SetTextFont(132)
    textuna.SetTextAngle(270)
                             
    texs = [#strip_botL, strip_topL,
            #strip_botS, strip_topS,
            #texfitL, texfitS,
            texZ, texL, texS, texX,
            textuna,
            ]
    
    multi = ROOT.TMultiGraph()
    multi.Add(grL, "P")
    multi.Add(grS, "P")
    multi.SetTitle(grL.GetTitle())
    multi.SetMinimum(0)
    multi.SetMaximum(70)
    multi.Draw("A")
    multi.GetXaxis().SetLimits(80, 480)
    multi.Draw("A")
    for tex in texs:
        tex.Draw()

    fitL.Draw("same")
    fitS.Draw("same")

    ROOT.gPad.RedrawAxis()
    canv.SaveAs("rate_per_strip.pdf")

    output = ROOT.TFile.Open("rate.root", "recreate")
    hL = ROOT.TH1F("rate_L", grL.GetTitle(), 501, -0.5, 500.5)
    hS = ROOT.TH1F("rate_S", grL.GetTitle(), 501, -0.5, 500.5)
    for (x, y) in zip(xsL, ysL):
        hL.SetBinContent(hL.GetXaxis().FindBin(x), y)
    for (x, y) in zip(xsS, ysS):
        hS.SetBinContent(hS.GetXaxis().FindBin(x), y)
    hL.Write()
    hS.Write()
    output.Close()

def width(r, baseWidth, topWidth, r1, r2):
    if r < r1:
        fatal()
    if r > r2:
        fatal()
    frac = (r - r1) / (r2 - r1)
    return baseWidth + (topWidth-baseWidth)*frac

def data():
    # shoutout:
    # https://twiki.cern.ch/twiki/pub/Atlas/MuonPhase2Upgrade/MuonPhase2TDR_20171105.pdf
    # https://automeris.io/WebPlotDigitizer/
    return ((1.0106e2, 2.0629e1),
            (1.2072e2, 1.4070e1),
            (1.3992e2, 9.9137e0),
            (1.6036e2, 7.0040e0),
            (1.8030e2, 5.7570e0),
            (2.0023e2, 4.5100e0),
            (2.2019e2, 3.6325e0),
            (2.4015e2, 2.9398e0),
            (2.6012e2, 2.3855e0),
            (2.8011e2, 2.0622e0),
            (3.0009e2, 1.7390e0),
            (3.2007e2, 1.3695e0),
            (3.4005e2, 1.0462e0),
            (3.6004e2, 8.1526e-1),
            (3.8004e2, 7.2289e-1),
            (4.0003e2, 5.8434e-1),
            (4.2003e2, 5.3815e-1),
            (4.4002e2, 3.9960e-1),
            )

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


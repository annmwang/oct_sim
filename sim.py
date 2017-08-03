import ROOT
import os, sys
import math, random
import numpy as np
from array import array
import time
from scipy import stats
import matplotlib.pyplot as plt
import oct as oct


NBOARDS = 8
NSTRIPS = 512
xlow = 0.
xhigh = 200.
ylow = 0.
yhigh = 200.

bc_wind = 8
mm_eff = [0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9]
sig_art = 32 #ns

def cosmic_angle():
    ''' return x, y angle of cosmic, better if it uses the cos^2 distribution '''
    return (0.,0.)

#def art_tdis():
    


def cluster_pdf():
    ''' returns a cluster size given a hand-defined pdf'''
    ''' how do i use this tho '''
    xk = range(1,7)
    pk = (0.15/0.95, 0.275/0.95, 0.3/0.95, 0.15/0.95, 0.05/0.95, 0.025/0.95)
    mult = stats.rv_discrete(name="clustmult",values=(xk,pk))
    return mult.rvs()

def generate_muon():

    myoct = oct.octgeo()
    # assume uniform distribution of cosmics passing through board
    x = np.random.uniform(xlow, xhigh)
    y = np.random.uniform(ylow, yhigh)
    thx, thy = cosmic_angle()

    co = 2.7
    zpos = [-co, 11.2+co, 32.4-co, 43.6+co, 
            113.6-co, 124.8+co, 146.0-co, 157.2+co]
    xpos = [-1.]*NBOARDS
    ypos = [-1.]*NBOARDS
    channels = [-1.]*NBOARDS


    end = NBOARDS-1
    avgz = 0.5*(zpos[0]+zpos[NBOARDS-1])
    for ib, elem in enumerate(zpos):
        z = zpos[ib]
        x_b = math.tan(thx)*(zpos[ib]-avgz)+x
        y_b = math.tan(thy)*(zpos[ib]-avgz)+y
        xpos[ib] = x_b
        ypos[ib] = y_b
        channels[ib] = myoct.channel(x_b,y_b, ib)
    
    return xpos, ypos, zpos

def oct_response(xpos, ypos, zpos):
    ''' gives detector response to muon, returns list of which planes registered hit '''
    n_mm = 0
    oct_hitmask = [0]*NBOARDS
    for i in range(NBOARDS):
        if np.random.uniform(0.,1.) < mm_eff[i]:
            oct_hitmask[i] = 1
            n_mm += 1 
    return oct_hitmask

def trigger(hits, bcwindow):
    n_x1, n_x2, n_uv = 0, 0, 0
    mintime = 99999.
    for hit in hits:
        if hit.time < mintime:
            mintime = hit.time
    for hit in hits:
        if (hit.time-mintime) >= bcwindow:
            continue
        if hit.ib < 2:
            n_x1 += 1
        if hit.ib > 5:
            n_x2 += 1
        if hit.ib > 1 or hit.ib < 6:
            n_uv += 1
    if n_x1 > 0 and n_x2 > 0 and n_uv > 1:
        return True
    else:
        return False
               
def main():

    n = 1000
    ntrigcand = 0
    ntrig = 0

    for i in range(n):
        xpos, ypos, zpos = generate_muon()

        #plttrk(xpos,zpos,True)
        #plttrk(ypos,zpos,False)
    
        oct_hitmask = oct_response(xpos, ypos, zpos)


        art_bc = [-1]*NBOARDS
        smallest_bc = 99999


        hits = []

        # smear strip with art resolution
        for j, bit in enumerate(oct_hitmask):
            if bit==1:
                art_time = np.random.normal(400.,sig_art)
                art_bc[j] = math.floor(art_time / 25.)
                hits.append(oct.hit(j,art_bc[j]))


        if not(trigger(hits,100000)): 
            continue

        ntrigcand += 1

        if not(trigger(hits,bc_wind)):
            continue
        ntrig += 1

    print n, ntrigcand, ntrig

def plttrk(pts,zpts, xflag):


    setstyle()
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()

    mg = ROOT.TMultiGraph();

    # convert hit pos into arrays for tgraph
    x,z = array('d'), array('d')
    for i,elem in enumerate(zpts):
        if pts[i] == -1.: 
            continue
        else: 
            x.append(pts[i])
            z.append(zpts[i])

    # define lines for planes
    planes = []
    board_x, board_z = array('d'), array('d')
    for k in range(NBOARDS):
        if xflag:
            board_x.append(xlow)
            board_x.append(xhigh)
        else:
            board_x.append(ylow)
            board_x.append(yhigh)
        board_z.append(zpts[k])
        board_z.append(zpts[k])
        
        planes.append(ROOT.TGraph(2, board_x, board_z))
        planes[k].SetLineColor(ROOT.kBlue)
        board_x = array('d')
        board_z = array('d')

    
    gr = ROOT.TGraph(len(z), x, z);
    gr.SetTitle("Cluster locations");
    gr.SetMarkerColor(46);
    gr.SetMarkerStyle(20);
    gr.SetMarkerSize(1);


    mg.Add(gr,"p")
    for plane in planes:
        mg.Add(plane, "l")
    mg.Draw("a")
    mg.GetXaxis().SetTitleOffset(1.)
    mg.GetYaxis().SetTitleOffset(1.4)
    mg.GetYaxis().SetTitle("z (mm)")
    if xflag:
        mg.GetXaxis().SetTitle("x (mm)")
        c.Print("mushroom_x.pdf")
    else:
        mg.GetXaxis().SetTitle("y (mm)")
        c.Print("mushroom_y.pdf")

def setstyle():
    ROOT.gROOT.SetBatch()
    ROOT.gStyle.SetStatY(0.85)
    ROOT.gStyle.SetPadTopMargin(0.1)
    ROOT.gStyle.SetPadRightMargin(0.1)
    ROOT.gStyle.SetPadBottomMargin(0.12)
    ROOT.gStyle.SetPadLeftMargin(0.12)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetPaintTextFormat(".2f")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptFit(ROOT.kTRUE)

if __name__=="__main__":
    main()

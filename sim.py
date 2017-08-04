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
yhigh = 217.9

bc_wind = 7
mm_eff = [0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9]

sig_art = 32 #ns

# road size
XROAD = 8
UVFACTOR = 3
UVROAD = XROAD*UVFACTOR

# rates
muonrate = 1 #Hz
bkgrate = 1000 # Hz per square mm = 10 kHz/cm^2

myoct = oct.Octgeo()

class rates:
    ''' to keep in mind '''
    def __init__(self, muonrate, bkgrate):
        self.muonrate_bc = muonrate / (4.*pow(10,7)) 
        self.bkgrate_bc = bkgrate / (4.*pow(10,7))
    

def cosmic_angle():
    ''' return x, y angle of cosmic, better if it uses the cos^2 distribution '''
    xtheta = np.random.normal(0.,0.1)
    ytheta = np.random.normal(0.,0.1)
    #return (xtheta, ytheta)
    return (0,0)

def create_roads():

    # given octplane dimensions, divide into roads
    # index from 0 like a normal person

    if (NSTRIPS % XROAD != 0):
        print "not divisible!"

    nroad = NSTRIPS/XROAD

    roads = [(oct.Road(i,myoct)) for i in range(nroad)]

    return roads

def cluster_pdf():
    ''' returns a cluster size given a hand-defined pdf'''
    ''' how do i use this tho '''
    xk = range(1,7)
    pk = (0.15/0.95, 0.275/0.95, 0.3/0.95, 0.15/0.95, 0.05/0.95, 0.025/0.95)
    mult = stats.rv_discrete(name="clustmult",values=(xk,pk))
    return mult.rvs()

def generate_muon():

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

def generate_bkg(start_bc):

    #myoct = oct.Octgeo()

    rate = rates(muonrate, bkgrate)

    plane_area = (xhigh-xlow) * (yhigh-ylow)

    bkghits = []

    noise_window = bc_wind * 3
    start_noise = start_bc - bc_wind
    end_noise = start_bc + bc_wind * 2 -1 

    # assume uniform distribution of background - correct for noise
    expbkg = rate.bkgrate_bc * noise_window  * plane_area

    for i in range(NBOARDS):
        nbkg = stats.poisson.rvs(expbkg)
        for j in range(nbkg):
            x = np.random.uniform(xlow, xhigh)
            y = np.random.uniform(ylow, yhigh)
            z = myoct.planes[i].originz
            bkghits.append(oct.Hit(i, np.random.randint(start_noise,end_noise +1), (x,y,z), False))

    return bkghits

def oct_response(xpos, ypos, zpos):
    ''' gives detector response to muon, returns list of which planes registered hit '''
    n_mm = 0
    oct_hitmask = [0]*NBOARDS
    for i in range(NBOARDS):
        if np.random.uniform(0.,1.) < mm_eff[i]:
            oct_hitmask[i] = 1
            n_mm += 1 
    return oct_hitmask

def finder(hits, roads):
    ''' applies roads '''
    for hit in hits:
        for road in roads:
            road.in_road_neighbors(hit, XROAD, UVFACTOR)
    for road in roads:
        road.sort()
        road.prune(bc_wind)
    return roads

def trigger(hits, bcwindow, iroad):
    ''' return number of triggers in sliding window '''
    hits_copy = hits[:]
    mintime = 99999.
    min_ihit = -1
    nhits = len(hits_copy)
    ntrig = 0
    while nhits > 3:
        triggered_hits = []
        n_x1, n_x2, n_uv = 0, 0, 0
        for i, hit in enumerate(hits_copy):
            if hit.time < mintime:
                mintime = hit.time
                min_ihit = i
        for hit in hits_copy:
            if (hit.time-mintime) >= bcwindow:
                continue
            if hit.ib < 2:
                n_x1 += 1
            if hit.ib > 5:
                n_x2 += 1
            if hit.ib > 1 or hit.ib < 6:
                n_uv += 1
            triggered_hits.append(hit)
        if n_x1 > 0 and n_x2 > 0 and n_uv > 1:
            #title = "snapshot_road%d_"%(iroad) + str(bcwindow) + "_" + str(ntrig)
            #plttrk(triggered_hits,True, 1, title)
            ntrig += 1
        old_ihits = [i for i, dum in enumerate(hits_copy) if dum.time == mintime]
        for j in sorted(old_ihits, reverse=True):
            hits_copy.pop(j)
        nhits -= 1
        mintime = 99999.
        min_ihit = -1
    return ntrig
               
def main():

    n = 1000
    nmuon_trig = 0
    neventtrig = 0
    ntrigcand = 0
    ntrig = 0
    
    # plotting stuff
    maxplots = 2
    nplot = 0

    for i in range(n):
        xpos, ypos, zpos = generate_muon()

        #plttrk(xpos,zpos,True)
        #plttrk(ypos,zpos,False)
    
        oct_hitmask = oct_response(xpos, ypos, zpos)
        

        art_bc = [-1]*NBOARDS
        smallest_bc = 99999


        hits = []

        n_uv, n_x1, n_x2 = 0,0,0
        # smear strip with art resolution
        for j, bit in enumerate(oct_hitmask):
            if bit == 1:
                if j < 2:
                    n_x1 += 1
                elif j > 5:
                    n_x2 += 1
                else:
                    n_uv += 1
                art_time = np.random.normal(400.,sig_art)
                art_bc[j] = math.floor(art_time / 25.)
                hits.append(oct.Hit(j,art_bc[j],(xpos[j],ypos[j],zpos[j]),True))

        if n_x1 > 0 and n_x2 > 0 and n_uv > 0:
            nmuon_trig += 1

        for bc in art_bc:
            if bc == -1:
                continue
            if bc < smallest_bc:
                smallest_bc = bc

        # assume bkg rate has oct_response factored in
        bkg_hits = generate_bkg(smallest_bc)
        # change this to 2*bc_wind after

        allhits = hits + bkg_hits

        roads = create_roads()
        roads = finder(allhits, roads)

        thisevttrig = 0

        filtered_hits = [] #may include duplicates
        for road in roads:
            #title = "road_%d"%(road.iroad)
            #plttrk(road.hits,True,-1,title)
            ntrigcand += trigger(road.hits,100000, road.iroad) 
            ntrigreal = trigger(road.hits,bc_wind, road.iroad)
            ntrig += ntrigreal
            thisevttrig += ntrigreal
            filtered_hits += road.hits #pruned with overlapping, not with bc
        if len(bkg_hits) > 0 and nplot < maxplots and thisevttrig == 0:
            file = open("nontrig.txt","w")
            for road in roads:
                for hit in road.hits:
                    file.write("road: %d, bc: %d, board: %d, muon: %d"%(road.iroad, hit.time, hit.ib, int(hit.real)))
                    file.write("\n")
            file.write("fin")
            file.close()
            print "had bkg hit!"
            nplot += 1
        #plttrk(filtered_hits,True, thisevttrig, "filtered")
        #plttrk(filtered_hits,False, thisevttrig, "filtered")
        #plttrk(allhits,True, thisevttrig, "all")
        if thisevttrig != 0:
            neventtrig += 1

    print n, ntrigcand, ntrig
    print neventtrig, "muons triggered out of", nmuon_trig, "muons"

def plttrk(hits, xflag, ntrig, title):


    pts = []
    zpts = []
    ib = []
    bkgpts = []
    bkgzpts = []
    bkgib = []
    for hit in hits:
        if hit.real is True:
            ib.append(hit.ib)
            if xflag:
                pts.append(hit.pos[0])
            else:
                pts.append(hit.pos[1])
            zpts.append(hit.pos[2])
        else:
            bkgib.append(hit.ib)
            if xflag:
                bkgpts.append(hit.pos[0])
            else:
                bkgpts.append(hit.pos[1])
            bkgzpts.append(hit.pos[2])

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

    # same for bkg hits
    bkgx,bkgz = array('d'), array('d')
    for i,elem in enumerate(bkgzpts):
        if bkgpts[i] == -1.: 
            continue
        else: 
            bkgx.append(bkgpts[i])
            bkgz.append(bkgzpts[i])

    # define lines for planes
    planes = []
    
    co = 2.7
    zpos = [-co, 11.2+co, 32.4-co, 43.6+co,
            113.6-co, 124.8+co, 146.0-co, 157.2+co]

    board_x, board_z = array('d'), array('d')
    for k in range(NBOARDS):
        if xflag:
            board_x.append(xlow)
            board_x.append(xhigh)
        else:
            board_x.append(ylow)
            board_x.append(yhigh)
        board_z.append(zpos[k])
        board_z.append(zpos[k])
        
        planes.append(ROOT.TGraph(2, board_x, board_z))
        planes[k].SetLineColor(ROOT.kBlue)
        board_x = array('d')
        board_z = array('d')

    if len(z) > 0:
        gr = ROOT.TGraph(len(z), x, z);
        gr.SetTitle("Cluster locations");
        gr.SetMarkerColor(ROOT.kPink+6);
        gr.SetMarkerStyle(20);
        gr.SetMarkerSize(1.4);

    if len(bkgz) > 0:
        grbkg = ROOT.TGraph(len(bkgz), bkgx, bkgz);
        grbkg.SetTitle("");
        grbkg.SetMarkerColor(ROOT.kCyan-8);
        grbkg.SetMarkerStyle(20);
        grbkg.SetMarkerSize(1.4);

    if len(z)> 0:
        mg.Add(gr,"p")
    if len(bkgz) > 0:
        mg.Add(grbkg,"p")
    for plane in planes:
        mg.Add(plane, "l")
    mg.Draw("a")
    mg.GetXaxis().SetTitleOffset(1.)
    mg.GetYaxis().SetTitleOffset(1.4)
    mg.GetYaxis().SetTitle("z (mm)")


    l1 = ROOT.TLatex()
    l1.SetTextSize(0.03)
    l1.SetTextColor(ROOT.kBlack)
    l1.SetTextAlign(21)
    l1.SetNDC()
    l1.DrawLatex(0.5,0.5,"ntrig: %d for %d (x), %d (uv) strip roads"%(ntrig,XROAD,UVROAD));


    if xflag:
        mg.GetXaxis().SetTitle("x (mm)")
        ctitle = title + "_x.pdf"
        c.Print(ctitle)
    else:
        mg.GetXaxis().SetTitle("y (mm)")
        ctitle = title + "_y.pdf"
        c.Print(ctitle)

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

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

bc_wind = 2
mm_eff = [0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9]
sig_art = 32 #ns

# road size
XROAD = 64
UVROAD = 64

muonrate = 1 #Hz

bkgrate = 100 # Hz per square mm


class rates:
    ''' to keep in mind '''
    def __init__(self, muonrate, bkgrate):
        self.muonrate_bc = muonrate / (4.*pow(10,7)) 
        self.bkgrate_bc = bkgrate / (4.*pow(10,7))
    

def cosmic_angle():
    ''' return x, y angle of cosmic, better if it uses the cos^2 distribution '''
    return (0.,0.)

#def art_tdis():
    

def create_roads():
    # how do i road
    # given octplane dimensions, divide into roads
    # index from 0 like a normal person
    if (NSTRIPS % XROAD != 0):
        print "not divisible!"

    nxroad = NSTRIPS/XROAD
    nuvroad = NSTRIPS/UVROAD

    x_roads = {"0":[], "1":[], "6":[], "7":[]}
    uv_roads = {"2":[], "3":[], "4":[], "5":[]}

    #x board
    for key in x_roads:
        for i in range(nxroad):
            striplow = i*XROAD
            striphigh = (i+1)*XROAD-1
            x_roads[key].append(oct.road(int(key), striplow, striphigh))

    #uv board
    for key in uv_roads:
        for i in range(nuvroad):
            striplow = i*UVROAD
            striphigh = (i+1)*UVROAD-1
            uv_roads[key].append(oct.road(int(key), striplow, striphigh))


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

def generate_bkg(start_bc):

    myoct = oct.octgeo()

    rate = rates(muonrate, bkgrate)

#     if (rate.bkgrate_bc > 1.):
#         print "this isn't gonna work, you messed up"
#         sys.exit()

    plane_area = (xhigh-xlow) * (yhigh-ylow)
#     nthrows = int(plane_area) # assuming the bkg rate is in mm, and the plane area is also in mm
#     xscan = int(xhigh-xlow)
#     yscan = int(yhigh-ylow)

    bkghits = []

    # this is toooooooo slow
#     for bc in range(bc_wind):
#         for i in range(NBOARDS):
#             for j in range(xscan):
#                 for k in range(yscan):
#                     if np.random.uniform(0,1) < rate.bkgrate_bc:
#                         x = np.random.uniform(xlow,xlow + xscan)
#                         y = np.random.uniform(ylow, ylow+yscan)
#                         z = myoct.planes[i].originz
#                         bkghits.append(oct.hit( i, bc, (x,y,z), False))

    # assume uniform distribution of background - correct for noise
    expbkg = rate.bkgrate_bc * bc_wind * plane_area
    nbkg = int(expbkg)

#     # deal with rates that are between integers
#     if np.random.uniform(0,1) < (expbkg-int(expbkg)):
#         nbkg += 1
    
    # need to figure out how to incorporate freq in a better way
    for i in range(NBOARDS):
        planebkg = nbkg
        # deal with rates that are between integers
        if np.random.uniform(0,1) < (expbkg-int(expbkg)):
            planebkg += 1
        for j in range(planebkg):
            x = np.random.uniform(xlow, xhigh)
            y = np.random.uniform(ylow, yhigh)
            z = myoct.planes[i].originz
            bkghits.append(oct.hit(i, start_bc + np.random.randint(0,bc_wind), (x,y,z), False))

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
                hits.append(oct.hit(j,art_bc[j],(xpos[j],ypos[j],zpos[j]),True))

        for bc in art_bc:
            if bc == -1:
                continue
            if bc < smallest_bc:
                smallest_bc = bc

        # assume bkg rate has oct_response factored in
        bkg_hits = generate_bkg(smallest_bc)
        for h in bkg_hits:
            print h.pos

        allhits = hits + bkg_hits

        if len(bkg_hits) > 0:
            print "had bkg hit!"
            plttrk(allhits,True)
            plttrk(allhits,False)

        if not(trigger(allhits,100000)): 
            continue

        ntrigcand += 1

        if not(trigger(allhits,bc_wind)):
            continue

        ntrig += 1
        

    print n, ntrigcand, ntrig

def plttrk(hits, xflag):


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

    
    gr = ROOT.TGraph(len(z), x, z);
    gr.SetTitle("Cluster locations");
    gr.SetMarkerColor(46);
    gr.SetMarkerStyle(20);
    gr.SetMarkerSize(1);

    grbkg = ROOT.TGraph(len(bkgz), bkgx, bkgz);
    grbkg.SetTitle("");
    grbkg.SetMarkerColor(ROOT.kCyan-8);
    grbkg.SetMarkerStyle(20);
    grbkg.SetMarkerSize(1);


    mg.Add(gr,"p")
    mg.Add(grbkg,"p")
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

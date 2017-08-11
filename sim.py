import ROOT
import copy
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

bc_wind = 20
mm_eff = [0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9]

sig_art = 32 #ns

# road size
XROAD = 8 #8
UVFACTOR = 8 #8
UVROAD = XROAD*UVFACTOR

# rates
muonrate = 1 #Hz
bkgrate = 100 # Hz per square mm = 10 kHz/cm^2

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
    #print "(%f,%f)"%(xtheta,ytheta)
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
 
    
    triggers = []
    ntrigs = 0

    # a useful time window for the event
    bc_start = min([hit.age for hit in hits]) - bc_wind*2
    bc_end   = max([hit.age for hit in hits]) + bc_wind*2
 
    slopes = []
    # each road makes independent triggers
    for road in roads:
 
        road.reset()
        
        #temphits = copy.deepcopy(hits) # WHY DO I NEED THIS??? inexplicable

        for bc in xrange(bc_start, bc_end):


            road.increment_age(bc_wind)
            
            hits_now = filter(lambda hit: hit.age==bc, hits)
            hits_now = sorted(hits_now, key=lambda hit: hit.strip)

            road.add_hits(hits_now, XROAD, UVFACTOR)

            #road_copy = copy.deepcopy(road)
            #print road.iroad, road.count()
            if road.coincidence(bc_wind):
                slopes.append((road.count(),road.mxl()))
                #print "found coincidence!"
                ntrigs += 1
                #print road.iroad, road.count()
                #road.trig = True
                #triggers.append(road_copy)

    # cav: can only make 8 triggers per BC.
    #      keep the 8 from lowest-number roads.
 
    return ntrigs,slopes
    #return triggers

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
    start = time.time()

    dx = []

    for i in range(n):
        xpos, ypos, zpos = generate_muon()

        #plttrk(xpos,zpos,True)
        #plttrk(ypos,zpos,False)
    
        oct_hitmask = oct_response(xpos, ypos, zpos)
        

        art_bc = [-1]*NBOARDS
        smallest_bc = 99999


        hits = []

        n_u, n_v, n_x1, n_x2 = 0,0,0,0
        # smear strip with art resolution
        for j, bit in enumerate(oct_hitmask):
            if bit == 1:
                if j < 2:
                    n_x1 += 1
                elif j > 5:
                    n_x2 += 1
                elif j == 2 or j ==4: 
                    n_u += 1
                else:
                    n_v += 1
                art_time = np.random.normal(400.,sig_art)
                art_bc[j] = int(math.floor(art_time / 25.))
                hits.append(oct.Hit(j,art_bc[j],(xpos[j],ypos[j],zpos[j]),True))

        if n_x1 > 0 and n_x2 > 0 and n_u > 0 and n_v > 0:
            nmuon_trig += 1

        for bc in art_bc:
            if bc == -1:
                continue
            if bc < smallest_bc:
                smallest_bc = bc

        # assume bkg rate has oct_response factored in

        #bkg_hits = []
        bkg_hits = generate_bkg(smallest_bc)
        # change this to 2*bc_wind after

        allhits = hits + bkg_hits

        roads = create_roads()
        #trigroads = finder(allhits, roads)
        #ntrigroads = len(trigroads)
        ntrigroads,slopes = finder(allhits,roads)
        if ntrigroads > 0:
            try:
                myslope = sorted(slopes,key=lambda x:x[0], reverse=True)[1]
                dx.append(np.arctan(myslope[1]))
            except:
                print slopes

        if ntrigroads > 0:
            neventtrig += 1
        else:
            if nplot < maxplots:
                file = open("nontrig.txt","w")
                file.write("list of hits\n")
                for hit in hits:
                    file.write("hit: %d, %d, %d, bc: %d"%(hit.pos[0], hit.pos[1], hit.pos[2], hit.age))
                    file.write("\n")
#                 for road in original_roads:
#                     for bo in xrange(NBOARDS):
#                         if road.hits[bo]:
#                             file.write("road: %d, bc: %d, board: %d, muon: %d"%(road.hits[bo].iroad, road.hits[bo].age, bo, int(road.hits[bo].real)))
#                             file.write("\n")
                file.write("fin")
                file.close()
                nplot += 1
        nt = 0
        ntrig += ntrigroads
        
#         for road in trigroads:
#             nt += 1
#             #print "n:%d"%(road.count())
#             acchits = []
#             for bo in xrange(NBOARDS):
#                 if road.hits[bo]:
#                     acchits.append(road.hits[bo])
#             title = "road_" + str(road.iroad) + "_%d"%(nt)
#             if len(acchits) > 0:
#                 plttrk(acchits,True, -1, title)
        if i % 10 == 0: 
            progress(time.time()-start, i, n)

    print n, ntrig
    print neventtrig, "muons triggered out of", nmuon_trig, "muons"
    pltres(dx,"mxres")
def pltres(data, title):
    h1 = ROOT.TH1F("h1", title, 100, -100, 100)
    for i in data:
        h1.Fill(i*1000)
    setstyle()
    h1.GetXaxis().SetTitle("#Delta#theta (mrad)")
    h1.GetYaxis().SetTitle("Events")
    h1.SetTitle("")
    c = ROOT.TCanvas("c", "canvas", 800, 800)
    c.cd()
    h1.Draw()
    fulltitle = title + ".pdf"
    c.Print(fulltitle)
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

def progress(time_diff, nprocessed, ntotal):
    import sys
    nprocessed, ntotal = float(nprocessed), float(ntotal)
    rate = (nprocessed+1)/time_diff
    msg = "\r > %6i / %6i | %2i%% | %8.2fHz | %6.1fm elapsed | %6.1fm remaining"
    msg = msg % (nprocessed, ntotal, 100*nprocessed/ntotal, rate, time_diff/60, (ntotal-nprocessed)/(rate*60))
    sys.stdout.write(msg)
    sys.stdout.flush()

if __name__=="__main__":
    main()

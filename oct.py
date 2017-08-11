#!/usr/bin/python

# Common stuff for scripts

import sys, getopt,binstr, time
import math, copy

NBOARDS = 8
pitch = 0.4
class Road:
    def __init__(self, iroad, myoct):
        self.iroad = iroad
        self.hits = [None]*NBOARDS
        self.octgeo = myoct
        self.trig = False

    def count(self):
        nhits = 0
        for bo in xrange(NBOARDS):
            if self.hits[bo]:
                nhits += 1
        return nhits
    
    def reset(self):
        self.hits = [None]*NBOARDS
        
    def contains(self, hit, roadsize, uvfactor):
        if hit.ib > 1 and hit.ib < 6:
            addstrip = uvfactor*roadsize-roadsize
            slow = roadsize*(self.iroad)-addstrip/2.
            shigh = roadsize*(self.iroad+1)+addstrip/2.
        else:
            slow = roadsize*(self.iroad)
            shigh = roadsize*(self.iroad+1)
        if hit.strip >= slow and hit.strip <= shigh:
            return True
        else:
            return False
    def contains_neighbors(self, hit, roadsize, uvfactor):
        if hit.ib > 1 and hit.ib < 6:
            addstrip = uvfactor*roadsize-roadsize
            slow = roadsize*(self.iroad-1)-uvfactor*roadsize-addstrip
            shigh = roadsize*(self.iroad+2)+uvfactor*roadsize+addstrip
        else:
            slow = roadsize*(self.iroad-1)
            shigh = roadsize*(self.iroad+2)
        if hit.strip >= slow and hit.strip <= shigh:
            return True
        else:
            return False
    def sort(self):
        for bo in xrange(NBOARDS):
            self.hits[bo].sort(key=lambda x:x.age)
    def add_hits(self, hits, roadsize, uv_factor):
        for hit in hits:
            bo = hit.ib
            #print hit.pos, self.iroad, hit.strip
            if self.contains_neighbors(hit, roadsize, uv_factor) and not self.hits[bo]:
                self.hits[bo] = copy.copy(hit)
                self.hits[bo].age = 0
    def increment_age(self, wind):
        for bo in xrange(NBOARDS):
            if self.hits[bo]:
                self.hits[bo].age += 1
                if self.hits[bo].age > wind:
                    self.hits[bo] = None

    def coincidence(self, wind):
        #print self.horiz_ok(), self.stereo_ok(), self.mature(wind)
        return self.horiz_ok() and self.stereo_ok() and self.mature(wind)
 
    def horiz_ok(self):
        return (self.hits[0] or self.hits[1]) and (self.hits[6] or self.hits[7])
 
    def stereo_ok(self):
        return (self.hits[2] or self.hits[4]) and (self.hits[3] or self.hits[5])
   
    def mature(self, wind):
        maturehits = []
        for bo in xrange(NBOARDS):
            if self.hits[bo]:
                if self.hits[bo].age == wind:
                    return True
    def mxl(self):
        xs, zs = [], []
        for bo in xrange(NBOARDS):
            if not bo in [0, 1, 6, 7]:
                continue
            if self.hits[bo]:
                vmm_ch = self.hits[bo].strip * pitch
                xs.append(vmm_ch)
                zs.append(self.hits[bo].pos[2])
        # calculate slope                                                                                                                                                       
        mxl_offline = slope(xs, zs)
        return mxl_offline

def slope(xs, zs):
    if len(xs) != len(zs): return -100
    if len(xs) == 1:       return -101
    return sum([x * ((z - avg(zs)) / (sum([zj*zj for zj in zs]) - len(zs)*pow(avg(zs), 2))) for (x,z) in zip(xs, zs)])
def avg(li):
    return float(sum(li)) / len(li)

class Hit:
    def __init__(self, ind, age, pos, real):
        self.ib = ind
        self.age = age
        self.pos = pos
        self.real = real # muon or noise
        myoct = Octgeo()
        self.strip = myoct.channel(self.pos[0],self.pos[1],self.ib)
class Octplane:
    def __init__(self, ind, alpha, origin):
        self.ib = ind
        self.alpha = alpha
        self.originx = origin[0]
        self.originy = origin[1]
        self.originz = origin[2]

class Octgeo:

    co = 2.7
    yo = 17.9

    def __init__(self):
        self.planes = []

        # plane 0
        self.planes.append(Octplane(0,0.,(102.3, 100., -self.co)))
        # plane 1
        self.planes.append(Octplane(1,0.,(102.3, 100., 11.2+self.co)))
        # plane 2
        self.planes.append(Octplane(2,-0.0261799,(102.3, 100.+self.yo, 32.4-self.co)))
        # plane 3
        self.planes.append(Octplane(3,0.0261799,(102.3, 100.+self.yo, 43.6+self.co)))
        # plane 4
        self.planes.append(Octplane(4,-0.0261799,(102.3, 100.+self.yo, 113.6-self.co)))
        # plane 5
        self.planes.append(Octplane(5,0.0261799,(102.3, 100.+self.yo, 124.8+self.co)))
        # plane 6
        self.planes.append(Octplane(6,0.,(102.3, 100., 146.0-self.co)))
        # plane 7
        self.planes.append(Octplane(7,0.,(102.3, 100., 157.2+self.co)))
            

    def channel(self, xpos, ypos, ib):
        myplane = self.planes[ib]
        channel  = ((xpos - myplane.originx - math.tan(myplane.alpha)*(ypos-myplane.originy)) / 0.4 ) + 256.5;
        return channel

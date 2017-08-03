#!/usr/bin/python

# Common stuff for scripts

import sys, getopt,binstr, time
import math

NBOARDS = 8

class road:
    def __init__(self, ind, slow, shigh):
        self.ib = ind
        self.slow = slow
        self.shigh = shigh
    def in_road(self, strip):
        if strip >= self.slow and strip <= self.shigh:
            return True
        else:
            return False

class hit:
    def __init__(self, ind, time, pos, real):
        self.ib = ind
        self.time = time
        self.pos = pos
        self.real = real # muon or noise

class octplane:
    def __init__(self, ind, alpha, origin):
        self.ib = ind
        self.alpha = alpha
        self.originx = origin[0]
        self.originy = origin[1]
        self.originz = origin[2]
    
class octgeo:

    co = 2.7
    yo = 17.9

    def __init__(self):
        self.planes = []

        # plane 0
        self.planes.append(octplane(0,0.,(102.3, 100., -self.co)))
        # plane 1
        self.planes.append(octplane(1,0.,(102.3, 100., 11.2+self.co)))
        # plane 2
        self.planes.append(octplane(2,-0.0261799,(102.3, 100.+self.yo, 32.4-self.co)))
        # plane 3
        self.planes.append(octplane(3,0.0261799,(102.3, 100.+self.yo, 43.6+self.co)))
        # plane 4
        self.planes.append(octplane(4,-0.0261799,(102.3, 100.+self.yo, 113.6-self.co)))
        # plane 5
        self.planes.append(octplane(5,0.0261799,(102.3, 100.+self.yo, 124.8+self.co)))
        # plane 6
        self.planes.append(octplane(6,0.,(102.3, 100., 146.0-self.co)))
        # plane 7
        self.planes.append(octplane(7,0.,(102.3, 100., 157.2+self.co)))
            

    def channel(self, xpos, ypos, ib):
        myplane = self.planes[ib]
        channel  = ((xpos - myplane.originx - math.tan(myplane.alpha)*(ypos-myplane.originy)) / 0.4 ) + 256.5;
        return channel

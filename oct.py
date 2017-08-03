#!/usr/bin/python

# Common stuff for scripts

import sys, getopt,binstr, time
import math

NBOARDS = 8

# class Road:
#     def __init__(self, ind, slow, shigh):
#         self.ib = ind
#         self.slow = slow
#         self.shigh = shigh
#     def in_road(self, strip):
#         if strip >= self.slow and strip <= self.shigh:
#             return True
#         else:
#             return False

class Road:
    def __init__(self, iroad, myoct):
        self.iroad = iroad
        self.hits = []
        self.octgeo = myoct
        
    def in_road(self, hit, roadsize):
        strip = self.octgeo.channel(hit.pos[0],hit.pos[1],hit.ib)
        slow = roadsize*(self.iroad)
        shigh = roadsize*(self.iroad+1)
        if strip >= slow and strip <= shigh:
            self.hits.append(hit)
            return True
        else:
            return False
    def in_road_neighbors(self, hit, roadsize):
        strip = self.octgeo.channel(hit.pos[0],hit.pos[1],hit.ib)
        slow = roadsize*(self.iroad-1)
        shigh = roadsize*(self.iroad+2)
        if strip >= slow and strip <= shigh:
            self.hits.append(hit)
            return True
        else:
            return False
    def find_coincidences(self):
        nx1, nx2, nuv = 0
        for hit in self.hits:
            if hit.ib < 2:
                nx1 += 1
            elif hit.ib > 5:
                nx2 += 1
            else:
                nuv += 1
        if (nx1 > 0 and nx2 > 0 and nuv > 1):
            return True
        else:
            return False



class Hit:
    def __init__(self, ind, time, pos, real):
        self.ib = ind
        self.time = time
        self.pos = pos
        self.real = real # muon or noise

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

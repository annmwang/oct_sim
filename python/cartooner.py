import array
import copy
import math
import ROOT
ROOT.gROOT.SetBatch()

alpha = 0.2
epsilon = 0.0001

def main():

    rootlogon()
    pitch  = 0.4
    stereo = 1.5*math.pi/180.0
    # x_lo, x_hi = 0.0, 1250.0
    # y_lo, y_hi = 240.0, 370.0
    y_lo, y_hi = 200.0, 410.0
    x_lo, x_hi = 0.0, 5500.0
    hist = ROOT.TH2F("hist", ";non-prec. [strips];prec. [strips]", 1000, x_lo-100, x_hi+100, 1000, x_lo-100, x_hi+100)
    style(hist)

    # draw the chamber
    chamber = Polygon(x_lo, y_lo, x_hi, y_hi)
    
    # road parameters
    roi = 29
    spread_up_xx = 0.5
    spread_dn_xx = 0
    #spread_up_uv = 1
    #spread_dn_uv = 1
    spread_up_uv = 0.5
    spread_dn_uv = 0

    # draw some roads
    roadx    = Road("X", number=roi,   size=8, spread_up=spread_up_xx, spread_dn=spread_dn_xx).poly(x_lo, x_hi)
    roadu    = Road("U", number=roi,   size=8, spread_up=3, spread_dn=3).poly(x_lo, x_hi) # accurate
    roadv    = Road("V", number=roi,   size=8, spread_up=3, spread_dn=3).poly(x_lo, x_hi)
    roadu_0  = Road("U", number=roi,   size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi) # accurate
    roadv_0  = Road("V", number=roi,   size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadu_1p = Road("U", number=roi+1, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadv_1p = Road("V", number=roi-1, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadu_1n = Road("U", number=roi-1, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadv_1n = Road("V", number=roi+1, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadu_2p = Road("U", number=roi+2, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadv_2p = Road("V", number=roi-2, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadu_2n = Road("U", number=roi-2, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
    roadv_2n = Road("V", number=roi+2, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)

    road_test = Road("X", number=roi, size=8, spread_up=0.5, spread_dn=0)
    road_test.dump()

    #roadu.SetFillColor(0)
    #roadv.SetFillColor(0)
    roaduv_0  = intersection(roadu_0, roadv_0)
    roaduv_1p = intersection(roadu_1p, roadv_1p)
    roaduv_1n = intersection(roadu_1n, roadv_1n)
    roaduv_2p = intersection(roadu_2p, roadv_2p)
    roaduv_2n = intersection(roadu_2n, roadv_2n)

    roaduvx_0  = intersection(roaduv_0,  roadx)
    roaduvx_1p = intersection(roaduv_1p, roadx)
    roaduvx_1n = intersection(roaduv_1n, roadx)
    roaduvx_2p = intersection(roaduv_2p, roadx)
    roaduvx_2n = intersection(roaduv_2n, roadx)

    road_u1p_v00 = intersection(roadu_1p, roadv_0)
    road_u2p_v1p = intersection(roadu_2p, roadv_1p)

    road_u1p_v00_x = intersection(roadx, road_u1p_v00)
    road_u2p_v1p_x = intersection(roadx, road_u2p_v1p)
    
    for ro in [roaduvx_0, 
               roaduvx_1p, 
               roaduvx_1n,
               road_u1p_v00_x,
               road_u2p_v1p_x,
               ]:
        ro.SetFillColor(ROOT.kBlack)
        ro.SetFillStyle(3002)

    # road loop!
    roadloop = 10
    roaddict = {}
    for ir in xrange(1, roadloop):
        num = roi
        tag = str(ir)
        roaddict["u"+tag+"p"] = Road("U", number=num+ir, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
        roaddict["v"+tag+"p"] = Road("V", number=num-ir, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
        roaddict["u"+tag+"n"] = Road("U", number=num-ir, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)
        roaddict["v"+tag+"n"] = Road("V", number=num+ir, size=8, spread_up=spread_up_uv, spread_dn=spread_dn_uv).poly(x_lo, x_hi)

        roaddict["uv"+tag+"p"] = intersection(roaddict["u"+tag+"p"], roaddict["v"+tag+"p"])
        roaddict["uv"+tag+"n"] = intersection(roaddict["u"+tag+"n"], roaddict["v"+tag+"n"])

        roaddict["uvx"+tag+"p"] = intersection(roaddict["uv"+tag+"p"], roadx)
        roaddict["uvx"+tag+"n"] = intersection(roaddict["uv"+tag+"n"], roadx)

    roads = [#roadu, roadv,
             roadx, 
             #roadu_l, roadv_l,
             #roadu_r, roadv_r,
             roadu_0, roadv_0,
             roadu_1p, roadv_1p,
             #roaduv_0,
             #roaduvx_1p,
             #roaduvx_2n,
             #roaduvx_1n,
             #roadux_0,
             #roaduvx_2p,
             #roaduvx_1p,
             #roaduvx_0,
             #roaduvx_1n,
             #roaduvx_2n,
             #roaduv_2n,
             #roadu_0, roadv_0,
             #roaduv_0,
             #roaduvx_0,
             #roadu_1p, roadv_1p,
             #roadu_1n, roadv_1n,
             #roadu_2p, roadv_2p,
             #roadu_2n, roadv_2n,
             #road_u1p_v00_x,
             #road_u2p_v1p_x,
             ]
    for ir in xrange(1, roadloop):
        tag = str(ir)
        #roads.append( roaddict["uvx"+tag+"p"] )
        #roads.append( roaddict["uvx"+tag+"n"] )
    roads = filter(lambda ro: ro, roads)

    canv = ROOT.TCanvas("canv", "canv", 800, 800)
    canv.Draw()
    hist.GetYaxis().SetRangeUser(y_lo, y_hi)
    hist.Draw("histsame")
    chamber.Draw("")
    for road in roads:
        road.Draw("f")
        road.Draw("")

    ROOT.gPad.RedrawAxis()
    canv.SaveAs("plot.pdf")

class Road():
    def __init__(self, type, number, size, spread_up, spread_dn):
        self.type      = type
        self.size      = size
        self.spread_up = spread_up
        self.spread_dn = spread_dn
        self.strip_lo  = int(self.offset() + self.size * (number - self.spread_dn))
        self.strip_hi  = int(self.offset() + self.size * (number + self.spread_up + 1))

    def dump(self):
        print
        print "Road info"
        print "%-10s :: %s" % ("type",      self.type)
        print "%-10s :: %s" % ("size",      self.size)
        print "%-10s :: %s" % ("spread_up", self.spread_up)
        print "%-10s :: %s" % ("spread_dn", self.spread_dn)
        print "%-10s :: %s" % ("strip_lo",  self.strip_lo)
        print "%-10s :: %s" % ("strip_hi",  self.strip_hi)
        print "%-10s :: %s" % ("offset",    self.offset())
        print

    def points(self, edge_lo, edge_hi):
        delta = edge_hi - edge_lo
        points = [(edge_lo, self.strip_lo),
                  (edge_lo, self.strip_hi),
                  (edge_hi, self.strip_hi + delta*self.angle()),
                  (edge_hi, self.strip_lo + delta*self.angle()),
                  ]
        return points

    def line(self, edge_lo, edge_hi, location):
        points = self.points(edge_lo, edge_hi)
        if location == "top":
            x1, y1 = points[1]
            x2, y2 = points[3]
        if location == "bot":
            x1, y1 = points[0]
            x2, y2 = points[2]
        return ROOT.TLine(x1, y1, x2, y2)

    def poly(self, edge_lo, edge_hi):
        points = self.points(edge_lo, edge_hi)
        points.append(points[0])
        poly = ROOT.TPolyLine(len(points), 
                              array.array("d", [p[0] for p in points]), 
                              array.array("d", [p[1] for p in points]))
        poly.SetLineWidth(1)
        poly.SetLineColor(ROOT.kBlack)
        poly.SetFillColorAlpha(self.color(), alpha)
        return poly

    def color(self):
        if self.type == "X": return ROOT.kBlack
        if self.type == "U": return ROOT.kBlue
        if self.type == "V": return ROOT.kRed

    def offset(self):
        if False:
            if self.type == "X": return 64
            if self.type == "U": return 64-7
            if self.type == "V": return 64+7
        else:
            if self.type == "X": return 64
            #if self.type == "U": return 64-16 + 4
            #if self.type == "V": return 64+17 + 3
            if self.type == "U": return 64-72 - 0
            if self.type == "V": return 64+72 - 0

    def angle(self):
        if self.type == "X": return 0
        if self.type == "U": return  1.5*math.pi/180
        if self.type == "V": return -1.5*math.pi/180

def Polygon(x_lo, y_lo, x_hi, y_hi):
    xs, ys = [], []
    xs.append(x_lo) ; ys.append(y_lo)
    xs.append(x_lo) ; ys.append(y_hi)
    xs.append(x_hi) ; ys.append(y_hi)
    xs.append(x_hi) ; ys.append(y_lo)
    xs.append(x_lo) ; ys.append(y_lo)
    return ROOT.TPolyLine(5, array.array("d", xs), array.array("d", ys))

def style(hist):
    size = 0.045
    hist.GetXaxis().SetTitleSize(size)
    hist.GetXaxis().SetLabelSize(size)
    hist.GetYaxis().SetTitleSize(size)
    hist.GetYaxis().SetLabelSize(size)
    if isinstance(hist, ROOT.TH2):
        hist.GetXaxis().SetTitleOffset(1.1)
        hist.GetYaxis().SetTitleOffset(1.8)
        hist.GetZaxis().SetTitleOffset(1.4)
        hist.GetZaxis().SetTitleSize(size)
        hist.GetZaxis().SetLabelSize(size)

def points(poly):
    # https://root-forum.cern.ch/t/pydoublebuffer/14042/2
    xs = poly.GetX()
    ys = poly.GetY()
    return [ (xs[i], ys[i]) for i in xrange(poly.GetN()) ]

def intersection(obj1, obj2, debug=False):

    # https://en.wikipedia.org/wiki/Line-line_intersection
    if isinstance(obj1, ROOT.TLine) and isinstance(obj2, ROOT.TLine):
        if slope(obj1) == slope(obj2):
            if offset(obj1) != offset(obj2):
                return None
            # weird case: two overlapping segments
            if vertical(obj1):
                xint = obj1.GetX1()
                y1a, y1b = obj1.GetY1(), obj1.GetY2()
                y2a, y2b = obj2.GetY1(), obj2.GetY2()
                if min(y1a, y1b) > max(y2a, y2b):
                    return None
                if min(y2a, y2b) > max(y1a, y1b):
                    return None
                yint_min = max(min(y1a, y1b), min(y2a, y2b))
                yint_max = min(max(y1a, y1b), max(y2a, y2b))
                return [(xint, yint_min),
                        (xint, yint_max)]
            else:
                xint_min = max(min(x1a, x1b), min(x2a, x2b))
                xint_max = min(max(x1a, x1b), max(x2a, x2b))
                if xint_min == x1a: yint_min = y1a
                if xint_min == x1b: yint_min = y1b
                if xint_min == x2a: yint_min = y2a
                if xint_min == x2b: yint_min = y2b
                if xint_max == x1a: yint_max = y1a
                if xint_max == x1b: yint_max = y1b
                if xint_max == x2a: yint_max = y2a
                if xint_max == x2b: yint_max = y2b
                return [(xint_min, yint_min),
                        (xint_max, yint_max)]

        x1, y1 = obj1.GetX1(), obj1.GetY1()
        x2, y2 = obj1.GetX2(), obj1.GetY2()
        x3, y3 = obj2.GetX1(), obj2.GetY1()
        x4, y4 = obj2.GetX2(), obj2.GetY2()
        xint = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4)) / ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
        yint = ((x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)) / ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
            
        if xint > max(x1, x2) + epsilon: return None
        if xint < min(x1, x2) - epsilon: return None
        if xint > max(x3, x4) + epsilon: return None
        if xint < min(x3, x4) - epsilon: return None
        if yint > max(y1, y2) + epsilon: return None
        if yint < min(y1, y2) - epsilon: return None
        if yint > max(y3, y4) + epsilon: return None
        if yint < min(y3, y4) - epsilon: return None
        return [ (xint, yint) ]

    if isinstance(obj1, ROOT.TPolyLine) and isinstance(obj2, ROOT.TPolyLine):

        points_of_intersection = []
        points1 = points(obj1)
        points2 = points(obj2)

        if debug:
            print "Points 1"
            for p in points1:
                print p
            print
            print "Points 2"
            for p in points2:
                print p
            print

        # first: make all pairs of line segments, and look for overlap
        for (x1a, y1a) in points1[:-1]:
            (x1b, y1b) = points1[points1.index( (x1a, y1a) ) + 1]
            line1 = ROOT.TLine(x1a, y1a, x1b, y1b)
            for (x2a, y2a) in points2[:-1]:
                (x2b, y2b) = points2[points2.index( (x2a, y2a) ) + 1]
                line2 = ROOT.TLine(x2a, y2a, x2b, y2b)
                if intersection(line1, line2):
                    for p in intersection(line1, line2):
                        points_of_intersection.append(p)

                if debug:
                    db = "Segs: (%f, %f)-(%f, %f) vs. (%f, %f)-(%f, %f) :: %s"
                    db = db % (x1a, y1a, x1b, y1b, x2a, y2a, x2b, y2b, intersection(line1, line2))
                    print db

        if debug:
            print
            print "PoI, segment crossings"
            for p in points_of_intersection:
                print p
            print

        # second: check for points which lie within other polygon
        # https://stackoverflow.com/questions/217578
        for (x, y) in points1[:-1]:
            if within(obj2, x, y, debug):
                points_of_intersection.append( (x, y) )
                if debug:
                    print "Found within obj2", x, y
        for (x, y) in points2[:-1]:
            if within(obj1, x, y, debug):
                points_of_intersection.append( (x, y) )
                if debug:
                    print "Found within obj1", x, y

        if not points_of_intersection:
            return None

        if debug:
            print
            print "PoI, unordered"
            for p in points_of_intersection:
                print p

        points_of_intersection = list(set(points_of_intersection))
        points_ordered = order(points_of_intersection)
        points_ordered.append( points_ordered[0] )

        if debug:
            print
            print "PoI, ordered"
            for p in points_ordered:
                print p
            print

        xs = [p[0] for p in points_ordered]
        ys = [p[1] for p in points_ordered]
        return ROOT.TPolyLine(len(xs), array.array("d", xs), array.array("d", ys))

def within(poly, x, y, debug=False):
    ps   = points(poly)
    xinf = 2*max([p[0] for p in ps])
    yinf = 2*max([p[1] for p in ps])
    line_inf = ROOT.TLine(x, y, xinf, yinf)
    nint = 0
    for ip in xrange(len(ps)-1):
        x1, y1 = ps[ip]
        x2, y2 = ps[ip+1]
        line_poly = ROOT.TLine(x1, y1, x2, y2)
        if intersection(line_inf, line_poly, debug):
            nint += 1
        if debug:
            db = "(%f, %f)-(%f, %f) vs (%f, %f)-(%f, %f) :: %s"
            db = db % (x, y, xinf, yinf, x1, y1, x2, y2, intersection(line_inf, line_poly))
            print db
    return nint % 2

def order(points):
    # https://stackoverflow.com/questions/6989100/
    center_x = sum([p[0] for p in points]) / len(points)
    center_y = sum([p[1] for p in points]) / len(points)
    def less(p1, p2, pr=False):
        if p1[0] - center_x >= 0 and p2[0] - center_x < 0:
            return 1
        if p1[0] - center_x < 0 and p2[0] - center_x >= 0:
            return 0
        if p1[0] - center_x == 0 and p2[0] - center_x == 0:
            if p1[1] - center_y >= 0 or p2[1] - center_y >= 0:
                return p1[1] > p2[1]
            return p2[1] > p1[1]
        det = (p1[0] - center_x) * (p2[1] - center_y) - (p2[0] - center_x) * (p1[1] - center_y);
        return det < 0

    def bubblesort(orig):
        li = copy.copy(orig)
        for passnum in range(len(li)-1, 0, -1):
            for i in range(passnum):
                if less(li[i+1], li[i]):
                    temp = li[i]
                    li[i] = li[i+1]
                    li[i+1] = temp
        return li

    # cmp fails here! no idea why.
    #return sorted(points, cmp=less)
    return bubblesort(points)

def vertical(line):
    return line.GetX1() == line.GetX2()

def slope(line):
    if vertical(line):
        return None
    return (line.GetY2() - line.GetY1()) / (line.GetX2() - line.GetX1())

def offset(line):
    if vertical(line):
        return line.GetX1()
    return line.GetY1() - slope(line)*line.GetX1()

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
    # ROOT.gStyle.SetFillColor(10)

if __name__ == "__main__":
    main()

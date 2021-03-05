/************** 
SIMULATION CHAMBER UTILITIES
Original code by: Ann Wang and Alexander Tuna
Commented / Modified by: Anthony Badea (June 2020)
**************/

#ifndef ChamberUtilities_HH
#define ChamberUtilities_HH

// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <dirent.h>
#include <ctime>
#include <bitset>
#include <cmath>
#include <chrono>
#include <time.h>
// Header includes
#include <Hit.hh>
#include <GeoOctuplet.hh>
#include <SimConstants.hh>

// Input:
// Output: 
bool compare_age(Hit* a, Hit* b){
    return (a->Age() < b->Age());
}

// Input:
// Output: 
bool compare_channel(Hit* a, Hit* b){
    return (a->Channel() < b->Channel());
}

// Input:
// Output: 
bool compare_second(std::pair<int, double> a, std::pair<int, double> b){
    return (a.second < b.second);
}

// Input:
// Output: 
void set_chamber(string chamber, int m_wind, int m_sig_art, int m_xroad, bool uvrflag, bool trapflag, int m_nstrips){
    // function to set parameters in a smart way

    if (chamber == "small"){
        if (m_nstrips == -1){
            // has to be multiple of x road
            NSTRIPS = 8800;
        }
        else {
            NSTRIPS = m_nstrips;
        }
        xlow = 0.;
        xhigh = NSTRIPS*0.4-0.2;
        ylow = 0.;
        yhigh = trapflag ? 1821. : 500.;
    }
    else if (chamber == "large"){
        if (m_nstrips == -1){
            // has to be multiple of x road
            NSTRIPS = 8800; 
        }
        else {
            NSTRIPS = m_nstrips;
        }
        xlow = 0.;
        xhigh = NSTRIPS*0.4-0.2;
        ylow = 0.;
        yhigh = 2200.;
    }
    else if (chamber == "oct"){
        if (m_nstrips == -1){
            // has to be multiple of x road
            NSTRIPS = 512; 
        }
        else {
            NSTRIPS = m_nstrips;
        }
        xlow = 0.;
        xhigh = NSTRIPS*0.4-0.2;
        ylow = 17.9;
        yhigh = 217.9;
    }
    else {
        exit (EXIT_FAILURE);
    }

    // active area
    mu_xlow = 100*0.4+0.2;
    mu_xhigh = NSTRIPS*0.4-0.2-100*0.4;

    mu_ylow = ylow;
    mu_yhigh = yhigh;

    bc_wind = m_wind;

    sig_art = m_sig_art;

    XROAD = m_xroad;
    UVFACTOR = round((yhigh-ylow)/(B * 0.4 * 2)/XROAD);

    if (!uvrflag){
        // this is for 8 strip x-roads, i think
        NSTRIPS_UP_UV = UVFACTOR*XROAD+NSTRIPS_UP_XX;
        NSTRIPS_DN_UV = UVFACTOR*XROAD;
        NSTRIPS_UP_XX = 4;
        NSTRIPS_DN_XX = 0;
        // NSTRIPS_UP_UV = 80;
        // NSTRIPS_DN_UV = 80;
        // NSTRIPS_UP_XX = 8;
        // NSTRIPS_DN_XX = 8;
    }
    else{
        // NSTRIPS_UP_UV = 0;
        // NSTRIPS_DN_UV = 0;
        // NSTRIPS_UP_XX = 0;
        // NSTRIPS_DN_XX = 0;
        NSTRIPS_UP_UV = 4;
        NSTRIPS_DN_UV = 0;
        NSTRIPS_UP_XX = 4;
        NSTRIPS_DN_XX = 0;
    }
}

// Input:
// Output:
int inside_trapezoid(double x, double y, double inner_radius, double outer_radius, double length, double base_width, double top_width) {
    double slope  = (outer_radius - inner_radius) / ((top_width - base_width) / 2.0);
    double offset = inner_radius - (slope * base_width / 2.0);
    if (x > outer_radius)          return 0; // top
    if (x < inner_radius)          return 0; // bottom
    if (y >  (x - offset) / slope) return 0; // right
    if (y < -(x - offset) / slope) return 0; // left
    return 1;
}

// Input:
// Output:
int fiducial(double x, double y, string chamber) {

    if (chamber != "large" && chamber != "small"){
        cerr << "fiducial doesnt understand this chamber: " << chamber << endl;
        return -1;
    }
    int large = (chamber == "large");

    double NSW_MM1_InnerRadius = 0; // chambers starts at 0 // large ? 923.0  : 895.0;
    double NSW_MM1_Length      = large ? 2310.0 : 2210.0;
    double NSW_MM2_Length      = large ? 1410.0 : 1350.0;
    double NSW_MM1_baseWidth   = large ? 640.0  : 500.0;
    double NSW_MM1_topWidth    = large ? 2008.5 : 1319.2;
    double NSW_MM2_baseWidth   = large ? 2022.8 : 1321.1;
    double NSW_MM2_topWidth    = large ? 2220.0 : 1821.5;
    double NSW_MM1_outerRadius = NSW_MM1_InnerRadius + NSW_MM1_Length;
    double NSW_MM2_InnerRadius = NSW_MM1_outerRadius;
    double NSW_MM2_outerRadius = NSW_MM2_InnerRadius + NSW_MM2_Length;

    if (inside_trapezoid(x, y - (yhigh+ylow)/2.0,
                                             NSW_MM1_InnerRadius,
                                             NSW_MM1_outerRadius,
                                             NSW_MM1_Length,
                                             NSW_MM1_baseWidth,
                                             NSW_MM1_topWidth)) return 1;
    if (inside_trapezoid(x, y - (yhigh+ylow)/2.0,
                                             NSW_MM2_InnerRadius,
                                             NSW_MM2_outerRadius,
                                             NSW_MM2_Length,
                                             NSW_MM2_baseWidth,
                                             NSW_MM2_topWidth)) return 1;
    return 0;
}

// Input:
// Output:
vector<Road*> create_roads(const GeoOctuplet& geometry, bool uvrflag, int m_xthr, int m_uvthr, string chamber, bool trapflag){
    int nroad = NSTRIPS/XROAD;
    vector<Road*> m_roads;
    for ( int i = 0; i < nroad; i++){

        Road* myroad = nullptr;
        myroad = new Road(&geometry, m_xthr, m_uvthr, i);
        m_roads.push_back(myroad);

        int nuv = 0;
        if (uvrflag)
            nuv = UVFACTOR;
        for (int uv = 1; uv <= nuv; uv++){
            if (i-uv < 0)
                continue;
            Road* myroad_0 = new Road(&geometry, m_xthr, m_uvthr, i, i+uv,   i-uv);
            Road* myroad_1 = new Road(&geometry, m_xthr, m_uvthr, i, i-uv,   i+uv);
            Road* myroad_2 = new Road(&geometry, m_xthr, m_uvthr, i, i+uv-1, i-uv);
            Road* myroad_3 = new Road(&geometry, m_xthr, m_uvthr, i, i-uv,   i+uv-1);
            Road* myroad_4 = new Road(&geometry, m_xthr, m_uvthr, i, i-uv+1, i+uv);
            Road* myroad_5 = new Road(&geometry, m_xthr, m_uvthr, i, i+uv,   i-uv+1);
            m_roads.push_back(myroad_0);
            m_roads.push_back(myroad_1);
            m_roads.push_back(myroad_2);
            m_roads.push_back(myroad_3);
            m_roads.push_back(myroad_4);
            m_roads.push_back(myroad_5);
        }
    }

    if (trapflag){
        vector<Road*> m_roads_fiducial;
        for (auto road: m_roads){
            for (int corner = 0; corner <= 3; corner++){
                double x, y;
                std::tie(x, y) = road->CornerXY(corner, XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV);
                if (fiducial(x, y, chamber)){
                    m_roads_fiducial.push_back(road);
                    break;
                }
            }
        }
        return m_roads_fiducial;
    }
    else
        return m_roads;
}

// Input:
// Output:
double predicted_rate(int strip, string chamber) {

    if (chamber != "large" && chamber != "small"){
        cerr << "predicted_rate doesnt understand this chamber: " << chamber << endl;
        return -1.0;
    }
    int large = (chamber == "large");
    double pitch = 0.4;
    double offset = large ? 923.0 : 895.0;
    double r = offset + pitch*(double)(strip);

    // mm->cm
    r = r/10;

    double rate = 0.0;
    if (large) rate = (-9.938824) + (6288.351422)/r +  (45942.902843)/pow(r, 2);
    else       rate = (-5.018321) + (3396.877744)/r + (164524.202988)/pow(r, 2);

    // kHz->Hz
    return rate*1000;
}

// Input:
// Output:
vector<int> oct_response(vector<double> & mm_eff, bool legacy){
    //gives detector response to muon, returns list of which planes registered hit
    
    int n_mm = 0;
    vector<int> oct_hitmask(NPLANES*NPCB_PER_PLANE,0);
    for ( int j=0; j < NPLANES*NPCB_PER_PLANE; j++){
        // For legacy support explicitly set the other oct hit values to -999
        if( legacy && j >= NPLANES){
            oct_hitmask[j] = 0;
            break;
        }
        double RAND = ran->Uniform(0.,1.);
        if (RAND < mm_eff[j]){
            oct_hitmask[j] = 1;
            n_mm++;
        }
    }
    return oct_hitmask;
}

#endif
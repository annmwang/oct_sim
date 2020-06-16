/************** 

SIMULATION PARTICLE UTILITIES

Original code by: Ann Wang and Alexander Tuna
Commented / Modified by: Anthony Badea (June 2020)

**************/

#ifndef ParticleUtilities_HH
#define ParticleUtilities_HH

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
tuple<double,double> cosmic_angle(int angcos, double angx, double angy){
  double thx,thy;
  double deg2rad = TMath::Pi()/180;
  if (angcos){
    thx = (angx > 0) ? cosmic_dist->GetRandom(-angx*deg2rad, angx*deg2rad) : 0;
    thy = (angy > 0) ? cosmic_dist->GetRandom(-angy*deg2rad, angy*deg2rad) : 0;
  }
  else{
    thx = ran->Uniform(-angx, angx) * deg2rad;
    thy = ran->Uniform(-angy, angy) * deg2rad;
  }
  return make_tuple(thx,thy);
}


// Input:
// Output:
tuple<double,double,double,double> generate_muon(vector<double> & xpos, vector<double> & ypos, vector<double> & zpos, string chamber, int angcos, double angx, double angy, bool trapflag){

  double x = 9e9;
  double y = 9e9;

  if (trapflag) {
    while (!fiducial(x, y, chamber)){
      x = ran->Uniform(mu_xlow,mu_xhigh);
      y = ran->Uniform(mu_ylow,mu_yhigh);
    }
  }
  else {
    x = ran->Uniform(mu_xlow,mu_xhigh);
    y = ran->Uniform(mu_ylow,mu_yhigh);
  }
  
  double thx, thy;

  std::tie(thx,thy) = cosmic_angle(angcos, angx, angy);

  double avgz = 0.5*(zpos[0]+zpos[NPLANES-1]);
  double x_b, y_b;
  // double z, x_b, y_b;
  for ( int j = 0; j < NPLANES; j++){
    // z = zpos[j];
    x_b = TMath::Tan(thx)*(zpos[j]-avgz)+x;
    y_b = TMath::Tan(thy)*(zpos[j]-avgz)+y;
    xpos[j] = x_b;
    ypos[j] = y_b;
  }    
  return make_tuple(x,y,thx,thy);
}



// Input:
// Output:
vector<Hit*> generate_bkg(int start_bc, const GeoOctuplet& geometry, int bkgrate, string chamber){

  vector<Hit*> bkghits;

  int noise_window = bc_wind * 5;
  int start_noise = start_bc - bc_wind * 2; // this takes into account overwriting tp hits for a plane+road
  //int start_noise = start_bc - bc_wind;

  // int end_noise = start_noise + noise_window - 1;
  // int end_noise = start_bc + bc_wind * 2 -1;

  //assume uniform distribution of background - correct for noise
  double time_window = noise_window * 25*pow(10,-9);
  double bkg_prob = bkgrate * time_window;
  for ( int j = 0; j < NPLANES; j++){
    //int nbkg = expbkg;
    for ( int k = 0; k < NSTRIPS; k++){
      double prob = ran->Uniform(0,1.);
      if (bkgrate == -1)
        bkg_prob = predicted_rate(k, chamber) * time_window;
      if (prob < bkg_prob){
        Hit* newhit = nullptr;
        newhit = new Hit(j, start_noise+ran->Integer(noise_window), k, true, geometry);
        bkghits.push_back(newhit);
      }
    }
  }
  return bkghits;
}

#endif
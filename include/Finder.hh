/************** 
SIMULATION FINDER FUNCTION
Original code by: Ann Wang and Alexander Tuna
Commented / Modified by: Anthony Badea (June 2020)
**************/

#ifndef Finder_HH
#define Finder_HH

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
// ROOT include
#include <TH1F.h>
#include <TRandom3.h>
#include <TF1.h>
// Header includes
#include <Hit.hh>
#include <GeoOctuplet.hh>
#include <Road.hh>

// Input:
// Output: 
tuple<int, vector < slope_t> > finder(vector<Hit*> hits, int mu_firstbc, vector<Road*> roads, bool saveHits, bool ideal_vmm, bool ideal_addc, bool ideal_tp, int evt){

  // applies the MMTP finder to a series of hits and roads
  // returns slope structs for roads which found a coincidence and have at least 1 real muon hit
  int ntrigs = 0;
  int bc_start = 999999;
  int bc_end = -1;
  unsigned int ibc = 0;

  vector<slope_t> slopes = {};

  if (hits.size() == 0)
    return make_tuple(ntrigs, slopes);

  std::sort(hits.begin(), hits.end(), compare_age);
  bc_start = hits.front()->Age();
  bc_end   = hits.back()->Age();
  bc_start = bc_start - bc_wind*2;
  bc_end   = bc_end   + bc_wind*2;

  // // silly hack to look only in one window                                                                        
  // bc_start = hits.front()->Age();
  // bc_end   = mu_firstbc;
  // bc_end   = mu_firstbc + 16;

  // setup the roads
  for (unsigned int i = 0; i < roads.size(); i++)
    roads[i]->Reset();
  vector<Hit*> hits_now = {};
  vector<int> vmm_same  = {};
  vector< pair<int, double> > addc_same = {};
  vector<int> to_erase = {};
  int n_vmm  = NSTRIPS/64;
  int n_addc = NSTRIPS/2048;

  // each road makes independent triggers, evaluated on each BC
  for (int bc = bc_start; bc < bc_end; bc++){

    hits_now.clear();

    for (unsigned int j = ibc; j < hits.size(); j++){
      if (hits[j]->Age() == bc){
        hits_now.push_back(hits[j]);
      }
      else if (hits[j]->Age() > bc){
        ibc = j;
        break;
      }
    }

    //implement vmm ART-like signal
    for (int ib = 0; ib < NPLANES; ib++){
      for (int j = 0; j < n_vmm; j++){
        vmm_same.clear();
        // save indices of all elements in vmm j
        for (unsigned int k = 0; k < hits_now.size(); k++){
          if (hits_now[k]->MMFE8Index() != ib)
            continue;
          if (hits_now[k]->VMM() == j){
            vmm_same.push_back(k);
          }
        }
        // if 2+ hits in same vmm, erase all except 1 randomly
        if (vmm_same.size() > 1){
          int the_chosen_one = ran->Integer((int)(vmm_same.size()));
          
          if (ideal_vmm)
            for (unsigned int k = 0; k < vmm_same.size(); k++)
              if (hits_now[vmm_same[k]]->IsReal()){
                the_chosen_one = k;
                break;
              }
          
          for (int k = vmm_same.size()-1; k > -1; k--)
            if (k != the_chosen_one)
              hits_now.erase(hits_now.begin()+vmm_same[k]);
        }
      }

      // implement ADDC-like handling
      for (int ia = 0; ia < n_addc; ia++){

        // collect all hits on one ADDC
        addc_same.clear();
        for (unsigned int k = 0; k < hits_now.size(); k++)
          if (hits_now[k]->MMFE8Index() == ib && hits_now[k]->ADDC() == ia)
            addc_same.push_back( std::make_pair(k, hits_now[k]->Channel()) );
        
        if ((int)(addc_same.size()) <= ADDC_BUFFER)
          continue;

        // priority encode the hits by channel number; remember hits 8+
        to_erase.clear();
        std::sort(addc_same.begin(), addc_same.end(), compare_second);
        for (int it = ADDC_BUFFER; it < (int)(addc_same.size()); it++)
          to_erase.push_back(addc_same[it].first);

        // reverse and erase
        std::sort(to_erase.rbegin(), to_erase.rend()); 
        for (auto k: to_erase)
          if (ideal_addc && hits_now[k]->IsReal())
            continue;
          else
            hits_now.erase(hits_now.begin() + k);
        
      }
    }

    for (unsigned int i = 0; i < roads.size(); i++){

      roads[i]->Increment_Age(bc_wind);
      roads[i]->Add_Hits(hits_now, XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV, ideal_tp);

      //if (roads[i]->Coincidence(bc_wind) && bc == (mu_firstbc - 1)){
      if (roads[i]->Coincidence(bc_wind) && bc >= (mu_firstbc - 1)){
      //if (roads[i]->Coincidence(bc_wind)){
        if (db){
          cout << "---------------------------" << endl;
          cout << "FOUND COINCIDENCE @ BC " << bc << endl;
          cout << "Road (i,count): ("<< roads[i]->iRoad() <<", " << roads[i]->Count()<<")" << endl;
          cout << "---------------------------" << endl;
          for (unsigned int k = 0; k < roads[i]->Hits().size(); k++){
            printf("Hit (board, BC, strip): (%d,%d,%4.4f)", roads[i]->Hits()[k].MMFE8Index(),roads[i]->Hits()[k].Age(),roads[i]->Hits()[k].Channel());
            cout << endl;
          }
        }
        ntrigs++;

        int nmuonhits = roads[i]->NMuon();
        double xcenter = 0;
        double ycenter = 0;
        std::tie(xcenter, ycenter) = roads[i]->Center(XROAD, NSTRIPS_UP_XX, NSTRIPS_DN_XX, NSTRIPS_UP_UV, NSTRIPS_DN_UV);

//         if (nmuonhits < 1)
//           continue;

        slope_t m_slope;

        m_slope.count = roads[i]->Count();
        m_slope.iroad = roads[i]->iRoad();
        m_slope.iroadu = roads[i]->iRoadu();
        m_slope.iroadv = roads[i]->iRoadv();
        m_slope.imuonhits = nmuonhits;
        m_slope.uvbkg = roads[i]->UV_bkg();
        m_slope.xbkg = roads[i]->X_bkg();
        m_slope.xmuon = roads[i]->X_muon();
        m_slope.age = bc-mu_firstbc;
        m_slope.mxl = roads[i]->Mxl();
        m_slope.xavg = roads[i]->AvgXofX();
        m_slope.yavg = -B*( roads[i]->AvgXofU() - roads[i]->AvgXofV() + (roads[i]->AvgZofV()-roads[i]->AvgZofU())*roads[i]->Mxl() ) / 2 + yhigh;
        m_slope.xcenter = xcenter;
        m_slope.ycenter = ycenter;
        if (saveHits)
          m_slope.slopehits = roads[i]->Hits();
        slopes.push_back(m_slope);
      }
    }
  }
  return make_tuple(ntrigs,slopes);
}

#endif
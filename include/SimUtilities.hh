/************** 

SIMULATION UTILITIES

Original code by: Ann Wang and Alexander Tuna
Commented / Modified by: Anthony Badea (June 2020)

**************/


#ifndef SimUtilities_HH
#define SimUtilities_HH

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
//	- ints to decide whether to kill randomly one PCB of the entire detector, of just the X planes, or of just the UV planes
//	- detector hit vector
//	- number of planes
//	- number of PCBs per plane
// Output: no ouput, operates on the detector hit vector 
void kill_random(int killran, // bool if you want to kill one plane PCB randomly
				 int killxran, // bool if you want to kill one X plane PCB randomly
				 int killuvran, // bool if you want to kill one UV plane PCB randomly
				 int NPLANES, // number of planes on the detector
			     int NPCB_PER_PLANE, // number of PCBs per plane
			     TRandom3 *ran, // random number generator from main method
			     std::vector<int> oct_hitmask // detector hit vector
				 )
{
	// Number of PCBs on the entire chamber
	int nPCB = NPCB_PER_PLANE*NPLANES;

	// if the length of the hit vector isn't equal to the expected number pcb's then the user has made an error. Go check oct_response(...).
    if( nPCB != oct_hitmask.size()){
    	std::cout<<"ERROR THE OCT_HITMASK IS OF LENGTH "<<oct_hitmask.size()<<" RATHER THAN "<<nPCB<<std::endl;
    	std::cout<<"GO CHECK OCT_RESPONSE()"<<std::endl;
    	exit(0);
    }

    // Randomly kill one PCB (killran), one x plane PCB (killxran), one uv plane PCB (killuvran)
    if (killran)
      oct_hitmask[ran->Integer(nPCB)] = 0;
    if (killxran){
    	// randomly pick up one x plane
    	int rand_plane = vector<int> {0, 1, 6, 7}[ran->Integer(4)];
    	// randomly pick up one of the PCBs
    	int rand_pcb = ran->Integer(NPCB_PER_PLANE);
      	oct_hitmask[rand_plane*rand_pcb] = 0;
    }
    if (killuvran){
    	// randomly pick up one of the uv planes
    	int rand_plane = vector<int> {2, 3, 4, 5}[ran->Integer(4)];
    	// randomly pick up one of the PCBs
    	int rand_pcb = ran->Integer(NPCB_PER_PLANE);
      	oct_hitmask[rand_plane*rand_pcb] = 0;
    }
}

/******************************************************** 64 PCB Mode ********************************************************/
// Input: number of planes, number of PCBs per plane, and the detector hit vector
// Output: 
//	- art bunch crossing index (art_time/bunch crossing time) as determined by a hit on a Plane
//	- vector of Hits based on registered hits on the Planes
//	- number of hits on x1, u, v, x2 planes as determined by hits in the PCBs
//	- fills the x strip resolution histogram
std::tuple< std::vector<int>, std::vector<Hit*>, int, int, int, int > get_hits_64PCBs(int NPLANES, // number of planes on the detector
																			  int NPCB_PER_PLANE, // number of PCBs per plane
																			  GeoOctuplet *GEOMETRY, // geometry of the octuplet
																			  TH1F *h_xres_strip, // x plane resolution histogram
																			  TRandom3 *ran, // random number generator from main method
																			  int sig_art, // art time resolution from set_chamber (in ns)
																			  int m_sig_art_x, // art time resolution (in ns)
																			  int bc_length, // time between bunch crossing (in ns)
																			  vector<double> & xpos, // x positions of generated muons
																			  vector<double> & ypos, // y positions of generated muons
																			  bool smear_art, // True to smear the art strip
																			  bool funcsmear_art, // True to use custom smear function
																			  TF1 *func, // custom smear function
																			  bool bkgonly, // True then do not add hit to Hits vector
																			  std::vector<int> oct_hitmask // detector hit vector
																			  )
{

    vector<int> art_bc(NPLANES, -1.);
    
    vector<Hit*> hits;

    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;
    
    double art_time;
    
    double strip, strip_smear;

    // loop over the PCBs

    // locations in oct_hitmask of the various planes
    // combine the first two x planes into x1
    int x1PCBs = 1*NPCB_PER_PLANE;
    int u1PCBs = 2*NPCB_PER_PLANE;
 	int v1PCBs = 3*NPCB_PER_PLANE;
    int u2PCBs = 4*NPCB_PER_PLANE;
    int v2PCBs = 5*NPCB_PER_PLANE;
    // combine the last two x planes into x2
    int x2PCBs = 6*NPCB_PER_PLANE;

    // Number of PCBs on the entire chamber
	int nPCB = NPCB_PER_PLANE*NPLANES;

    for ( int j = 0; j < nPCB; j++){
    	// Determine the plane index
      	int j_plane = (int) round(j/NPLANES);

  		if (oct_hitmask[j] == 1){
  			// Increment the number of hits per plane
	        if (j <= x1PCBs)
	          	n_x1++;
	        else if (j >= u1PCBs && j < v1PCBs)
	          	n_u++;
	      	else if (j >= v1PCBs && j < u2PCBs)
	      	 	n_v++;
	      	else if (j >= u2PCBs && j < v2PCBs)
	      		n_u++;
	      	else if (j >= v2PCBs && j < x2PCBs)
	      		n_v++;
	        else if (j >= x2PCBs)
	          	n_x2++;

	       	// Randomly calculate an art time based on the ART resolution in ns
     		art_time = ran->Gaus(400.,(double)(sig_art));

     		// There is a bunch crossing every 25 ns. Calculate where in that time the art happened
	      	art_bc[j_plane] = (int)floor(art_time/bc_length);
	      	
	      	// Prepare for a new hit
	      	Hit* newhit = nullptr;

	      	// Extracts the channel of the entire layer NOT the PCB
	      	strip = GEOMETRY->Get(j_plane).channel_from_pos(xpos[j_plane],ypos[j_plane]);

	      	// Smear the art strip position
	      	// Excerpt from: https://cds.cern.ch/record/2302523/files/ATL-COM-MUON-2018-003.pdf?
	      	// The spatial position of the muon hit is smeared with a gaussian with a σ of 1 strip as measured with 
	      	// cosmic events [6] and shown in Fig. 2. The arrival timing of the ART hits due to muon tracks is smeared 
	      	// with a gaussian with a σ of 32 ns to emulate the ART time distribution reported in Ref. [6].
	      	if (smear_art){
	        	strip_smear = round(ran->Gaus(strip,m_sig_art_x));
	      	}
	      	else if (funcsmear_art){
	      	  strip_smear = round( strip + func->GetRandom(-10, 10)/0.4 );
	     	 }
	      	else{
	      	  strip_smear = strip;
	      	}

	      	// sanity check
	      	if (j <= x1PCBs || j >= x2PCBs){
	      		h_xres_strip->Fill(xpos[j_plane] - (GEOMETRY->Get(j_plane).Origin().X() + GEOMETRY->Get(j_plane).LocalXatYbegin(strip_smear)));
	      	}

	      	// Create a new hit and push it to the ongoing list
	     	newhit = new Hit(j_plane, art_bc[j_plane], strip_smear, false, *GEOMETRY);
	     	if (!bkgonly)
				hits.push_back(newhit);
      	}
    }

    return std::make_tuple(art_bc, hits, n_x1, n_u, n_v, n_x2);
}



/************************************** LEGACY SUPPORT FOR PREVIOUS VERSION **************************************/
// Legacy: 
//	- Only uses 8 efficiencies, one for each layer, to calculated the number of hits. 
//	- The 8 efficiencies that are read are the first 8 entries of oct_hitmask
// Input: number of planes, number of PCBs per plane, and the detector hit vector
// Output: 
//	- art bunch crossing index (art_time/bunch crossing time) as determined by a hit on a Plane
//	- vector of Hits based on registered hits on the Planes
//	- number of hits on x1, u, v, x2 planes as determined by hits in the PCBs
//	- fills the x strip resolution histogram
std::tuple< std::vector<int>, std::vector<Hit*>, int, int, int, int > get_hits_LEGACY(int NPLANES, // number of planes on the detector
																			  		  GeoOctuplet *GEOMETRY, // geometry of the octuplet
																			  		  TH1F *h_xres_strip, // x plane resolution histogram
																			  		  TRandom3 *ran, // random number generator from main method
																			  		  int sig_art, // art time resolution from set_chamber (in ns)
																			  		  int m_sig_art_x, // art time resolution (in ns)
																			  		  int bc_length, // time between bunch crossing (in ns)
																					  vector<double> & xpos, // x positions of generated muons
																					  vector<double> & ypos, // y positions of generated muons
																					  bool smear_art, // True to smear the art strip
																					  bool funcsmear_art, // True to use custom smear function
																					  TF1 *func, // custom smear function
																					  bool bkgonly, // True then do not add hit to Hits vector
																					  std::vector<int> oct_hitmask // detector hit vector
																			  		)
{

    vector<int> art_bc(NPLANES, -1.);
    
    vector<Hit*> hits;

    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;
    
    double art_time;
    
    double strip, strip_smear;
    for ( int j = 0; j < NPLANES; j++){
      if (oct_hitmask[j] == 1){
        if (j < 2)
          n_x1++;
        else if (j > 5)
          n_x2++;
        else if (j == 2|| j ==4)
          n_u++;
        else
          n_v++;
      art_time = ran->Gaus(400.,(double)(sig_art));
      art_bc[j] = (int)floor(art_time/25.);
      Hit* newhit = nullptr;

      strip = GEOMETRY->Get(j).channel_from_pos(xpos[j],ypos[j]);
      if (smear_art){
        strip_smear = round(ran->Gaus(strip,m_sig_art_x));
      }
      else if (funcsmear_art){
        strip_smear = round( strip + func->GetRandom(-10, 10)/0.4 );
      }
      else{
        strip_smear = strip;
      }

      // sanity check
      if (j < 2 || j > 5)
        h_xres_strip->Fill(xpos[j] - (GEOMETRY->Get(j).Origin().X() + GEOMETRY->Get(j).LocalXatYbegin(strip_smear)));

      newhit = new Hit(j, art_bc[j], strip_smear, false, *GEOMETRY);
      //newhit = new Hit(j, art_bc[j], xpos[j], ypos[j], false, *GEOMETRY);
      if (!bkgonly)
	hits.push_back(newhit);
      }
    }

    return std::make_tuple(art_bc, hits, n_x1, n_u, n_v, n_x2);
}

/************************************************ DECIDE WHICH VERSION TO USE ************************************************/
// Input: number of planes, number of PCBs per plane, and the detector hit vector
// Output: 
//	- art bunch crossing index (art_time/bunch crossing time) as determined by a hit on a Plane
//	- vector of Hits based on registered hits on the Planes
//	- number of hits on x1, u, v, x2 planes as determined by hits in the PCBs
//	- fills the x strip resolution histogram
std::tuple< std::vector<int>, std::vector<Hit*>, int, int, int, int > get_hits(int NPLANES, // number of planes on the detector
																			  int NPCB_PER_PLANE, // number of PCBs per plane
																			  GeoOctuplet *GEOMETRY, // geometry of the octuplet
																			  TH1F *h_xres_strip, // x plane resolution histogram
																			  TRandom3 *ran, // random number generator from main method
																			  int sig_art, // art time resolution from set_chamber (in ns)
																			  int m_sig_art_x, // art time resolution (in ns)
																			  int bc_length, // time between bunch crossing (in ns)
																			  vector<double> & xpos, // x positions of generated muons
																			  vector<double> & ypos, // y positions of generated muons
																			  bool smear_art, // True to smear the art strip
																			  bool funcsmear_art, // True to use custom smear function
																			  TF1 *func, // custom smear function
																			  bool bkgonly, // True then do not add hit to Hits vector
																			  std::vector<int> oct_hitmask, // detector hit vector
																			  bool legacy // Legacy mode crosscheck
																			  )
{

    vector<int> art_bc(NPLANES, -1.);
    vector<Hit*> hits;
    int n_u = 0;
    int n_v = 0;
    int n_x1 = 0;
    int n_x2 = 0;
    
    if (legacy){
    	/*
    	printf("*****************************************************************\n");
    	printf("********************* LEGACY MODE TURNED ON *********************\n");
    	printf("*****************************************************************\n");*/
	    std::tie(art_bc,hits, n_x1, n_u, n_v, n_x2) = get_hits_LEGACY(NPLANES, 
                                                       				  GEOMETRY, 
				                                                      h_xres_strip, 
				                                                      ran, 
				                                                      sig_art,
				                                                      m_sig_art_x,
				                                                      bc_length,
				                                                      xpos,
				                                                      ypos,
				                                                      smear_art,
				                                                      funcsmear_art,
				                                                      func,
				                                                      bkgonly,
				                                                      oct_hitmask);
	    return std::make_tuple(art_bc, hits, n_x1, n_u, n_v, n_x2);
    }
    else{
    	/*
    	printf("*****************************************************************\n");
    	printf("********************* 64 PCB MODE TURNED ON *********************\n");
    	printf("*****************************************************************\n");*/
	    std::tie(art_bc,hits, n_x1, n_u, n_v, n_x2) = get_hits_64PCBs(NPLANES, 
	    															  NPCB_PER_PLANE,
                                                       				  GEOMETRY, 
				                                                      h_xres_strip, 
				                                                      ran, 
				                                                      sig_art,
				                                                      m_sig_art_x,
				                                                      bc_length,
				                                                      xpos,
				                                                      ypos,
				                                                      smear_art,
				                                                      funcsmear_art,
				                                                      func,
				                                                      bkgonly,
				                                                      oct_hitmask);
	    return std::make_tuple(art_bc, hits, n_x1, n_u, n_v, n_x2);
    }
    
    return std::make_tuple(art_bc, hits, n_x1, n_u, n_v, n_x2);
}



#endif
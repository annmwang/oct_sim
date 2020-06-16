#ifndef SimParameters_HH
#define SimParameters_HH

class SimParameters {

public:
	SimParameters();

	// Setup the parameters for the study 
	int nevents = -1; // number of events to generate
	int bkgrate = 0; // Hz per strip

	int m_xroad = 8; // size of x road in strips
	int m_NSTRIPS = -1; // number of x strips

	// ART = Address in Real Time (output of VMM)
	int m_bcwind = 8; // fixed time window (in bunch crossings) during which the algorithm collects ART hits
	int m_sig_art = 32; // art time resolution (in nanoseconds)

	int killran   = 0; // bool if you want to kill one plane randomly
	int killxran  = 0; // bool if you want to kill one X plane randomly 
	int killuvran = 0; // bool if you want to kill one U or V plane randomly 

	int m_sig_art_x = 1; // ART position resolution (in strips). used to smear ART position

	vector<double> mm_eff = {1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.,
	                       1., 1., 1., 1., 1., 1., 1., 1.}; // efficiency of each PCB Left/Right of the MM

    double chamber_eff = -1.; // overall efficiency of the chamber

	// Angle that the cosmic ray muon flies 
	// refer to method cosmic_angle above for details
	double angx = 0; // angular window in x for a muon to be created
	double angy = 0; // angular window in y for a muon to be created
	int angcos  = 0; // determines which distribution the muon track x,y is drawn from 
	// angcos = 1 --> cosmic_dist defined L41: cos^2(x) distribution from [-0.3,0.3] IF angx and angy are greater than 0. ELSE both x and y track locations are zero
	// angcos = 0 --> x and y track angles (in radians) are drawn from uniform distribution between [-angx,angx] and [-angy,angy]

	string histograms = "histograms"; // directory for output histograms

	// coincidence params
	int m_xthr = 2; // required total number of hits on all x channels combined required for a trigger. Used in create_roads function which uses Road.hh in include folder
	int m_uvthr = 2; // required total number of hits on all u and v channels combined required for a trigger. Used in create_roads function which uses Road.hh in include folder

	bool bkgflag = false; // decides if background should be generated
	bool pltflag = false; // decides if event displays should be plotted
	bool uvrflag = false; // decides if ??? used in set_chamber
	bool trapflag = false; // decides if ??? used in create_roads
	bool ideal_tp   = false; // decides if ??? used in Road.hh
	bool ideal_vmm  = false; // decides if ??? used in finder function
	bool ideal_addc = false; // decides if ??? used in finder function
	bool write_tree = false; // decides if an output TTree is produced
	bool bkgonly = false; // decides if only the background should be produced
	bool smear_art = false; // decides if the arrival time of the ART hits due to muon tracks is smeared with a gaussian with a σ of 32 ns to emulate the ART time distribution
	bool funcsmear_art = false; // ONLY used if smear_art is false. Uses a custom smearing function rather than a gaussian. 

	TF1* func = 0;

	char outputFileName[400];
	char chamberType[400];
	
}
#endif
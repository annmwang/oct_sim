#ifndef SimConstants_HH
#define SimConstants_HH

TRandom3 *ran = new TRandom3(time(NULL));
TF1 *cosmic_dist = new TF1("cosmic_dist", "cos(x)*cos(x)", -0.3, 0.3);

bool db = false; // debug output flag

// SOME CONSTANTS

int NPLANES = 8;
int NPCB_PER_PLANE = 8;
int NSTRIPS;
int ADDC_BUFFER = 8;
double xlow, xhigh, ylow, yhigh; // chamber dimensions
double mu_xlow, mu_xhigh, mu_ylow, mu_yhigh; // active chamber area to decouple edge effects

int XROAD, UVFACTOR;

int NSTRIPS_UP_UV, NSTRIPS_DN_UV;
int NSTRIPS_UP_XX, NSTRIPS_DN_XX;

int bc_wind;
int sig_art;

// Length of time between bunch crossing (in ns)
double bc_length = 25.0; 

double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));

// colors
string pink = "\033[38;5;205m";
string green = "\033[38;5;84m";
string blue = "\033[38;5;27m";
string ending = "\033[0m";
string warning = "\033[38;5;227;48;5;232m";

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

bool b_out = false;
bool ch_type = false;

// Input: 
// Output:
int read_parameters_from_user(int argc, char* argv[]){

	if ( argc < 3 ){
		std::cout << "Error at Input: please specify number of events to generate "<< std::endl;
		std::cout << "Example:   ./sim -n 100 -ch <chamber type> -o output.root" << std::endl;
		std::cout << "Example:   ./sim -n 100 -ch <chamber type> -b <bkg rate in kHz/strip> -o output.root" << std::endl;
		std::cout << "Example:   ./sim -n 100 -ch <chamber type> -b <bkg rate in Hz/strip> -p <make event displays> -o output.root" << std::endl;
		std::cout << "Other options include: -w <bc_wind> -sig <art res (ns)>" << std::endl;
		std::cout << "If art res = 0, then we do bkg only" << std::endl;
		return 0;
	}

	// Fill using the input from the user
  	for (int i=1; i<argc; i++){
	    if (strncmp(argv[i],"-n",2)==0){
	      nevents = atoi(argv[i+1]);
	    }
	    if (strncmp(argv[i],"-o",2)==0){
	      sscanf(argv[i+1],"%s", outputFileName);
	      b_out = true;
	    }
	    if (strncmp(argv[i],"-x",2)==0){
	      m_xroad = atoi(argv[i+1]);
	    }
	    if (strncmp(argv[i],"-w",2)==0){
	      m_bcwind = atoi(argv[i+1]);
	    }
	    if (strncmp(argv[i],"-thrx",5)==0){
	      m_xthr = atoi(argv[i+1]);
	    }
	    if (strncmp(argv[i],"-thruv",6)==0){
	      m_uvthr = atoi(argv[i+1]);
	    }
	    if (strncmp(argv[i],"--trap",6)==0){
	      trapflag = true;
	    }
	    if (strncmp(argv[i],"-ch",3)==0){
	      sscanf(argv[i+1],"%s", chamberType);
	      ch_type = true;
	    }
	    if (strncmp(argv[i],"-sig",4)==0){
	      m_sig_art = atoi(argv[i+1]);
	      if (m_sig_art == 0)
	        bkgonly = true;
	    }
	    if (strncmp(argv[i],"-b",2)==0){
	      bkgrate = atoi(argv[i+1]);
	      bkgflag = true;
	    }
	    if (strncmp(argv[i],"-p",2)==0){
	      pltflag = true;
	    }
	    if (strncmp(argv[i],"-uvr",4)==0){
	      uvrflag = true;
	    }
	    if (strncmp(argv[i],"-hdir",4)==0){
	      histograms = argv[i+1];
	    }
	    if (strncmp(argv[i],"-e",2)==0){
	      std::cout<<argv[i+1]<<std::endl;
	      printf("%s",argv[i+1]);
	      std::cout<<"FILLING mm_EFF"<<std::endl;
	      std::vector<double> v;
	      size_t pos = 0;
	      std::string s = argv[i+1];
	      std::string delimiter = ",";
	      std::string token;
	      while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		v.push_back(std::stod(token));
		s.erase(0, pos + delimiter.length());
	      }
	      v.push_back(std::stod(s));
	      for (unsigned int i=0; i<mm_eff.size();i++){
		mm_eff[i] = v[i];
	      }
	    }
	    if (strncmp(argv[i],"-angx",5)==0){
	      angx = fabs( atof(argv[i+1]) );
	    }
	    if (strncmp(argv[i],"-angy",5)==0){
	      angy = fabs( atof(argv[i+1]) );
	    }
	    if (strncmp(argv[i],"-angcos",7)==0){
	      angcos = 1;
	    }
	    if (strncmp(argv[i],"-killran",8)==0){
	      killran = true;
	    }
	    if (strncmp(argv[i],"-killxran",9)==0){
	      killxran = true;
	    }
	    if (strncmp(argv[i],"-killuvran",10)==0){
	      killuvran = true;
	    }
	    if (strncmp(argv[i],"-ideal-vmm", 10)==0){
	      ideal_vmm = true;
	    }
	    if (strncmp(argv[i],"-ideal-addc", 11)==0){
	      ideal_addc = true;
	    }
	    if (strncmp(argv[i],"-ideal-tp", 9)==0){
	      ideal_tp = true;
	    }
	    if (strncmp(argv[i],"-seed", 5)==0){
	      ran->SetSeed( atoi(argv[i+1]) );
	    }
	    if (strncmp(argv[i],"-tree", 5)==0){
	      write_tree = true;
	    }
	    if (strncmp(argv[i],"-strips", 7)==0){
	      m_NSTRIPS = atoi(argv[i+1]);
	    }
	    if (strncmp(argv[i],"-smear",6)==0){
	      smear_art = true;
	    }
	    if (strncmp(argv[i],"-smearstrips",12)==0){
	      m_sig_art_x = atoi(argv[i+1]);
	      smear_art = true;
	    }
	    if (strncmp(argv[i],"-funcsmear",10)==0){
	      funcsmear_art = true;
	      // func defined in mm
	      func = new TF1("gaus3", "gaus(0)+gaus(3)+gaus(6)", -10, 10);
	      func->SetNpx(1000);
	      func->SetParameter(0, 1826.11631);
	      func->SetParameter(1,    0.00000);
	      func->SetParameter(2,    0.21855);
	      func->SetParameter(3,  185.87369);
	      func->SetParameter(4,    0.00000);
	      func->SetParameter(5,    0.70467);
	      func->SetParameter(6,   10.68744);
	      func->SetParameter(7,    0.00000);
	      func->SetParameter(8,    3.70079);
	    }
  	}

  	return 1;
}


// Input: 
// Output:
int check_good_params(){
	if (!b_out){
		std::cout << "Error at Input: please specify output file (-o flag)" << std::endl;
		return 0;
	}

	if (!ch_type){
		std::cout << "Error at Input: please specify chamber type (-ch flag, options: large, small, oct)" << std::endl;
		return 0;
	}

	if (nevents == -1){
		std::cout << "Didn't set the number of generated events! Exiting." << std::endl;
		return 0;
	}

	return 1;
}

// Input:
// Output:
int check_good_chamber(){
	if (NSTRIPS % XROAD != 0) {
		std::cout << "Number of strips not divisible by the road size!" << std::endl;
		return 0;
	}

	if ( ( (mu_xlow || mu_xhigh) < xlow) || ( (mu_xlow || mu_xhigh) > xhigh) || (mu_xlow > mu_xhigh) ){
		std::cout << "Muon active area is outside the chamber area!" << std::endl;
		return 0;
	} 

	if (smear_art && funcsmear_art){
		std::cout << "Cant smear with gaussian and functional form at the same time!" << std::endl;
		return 0;
	}

	return 1;
}


// Input:
// Output:
void print_parameters(){

	std::cout << std::endl;
	std::cout << blue << "--------------" << ending << std::endl;
	std::cout << blue << "OCT SIM ✪ ‿ ✪ " << ending << std::endl;
	std::cout << blue << "--------------" << ending << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	printf("\r >> plot flag: %s", pltflag ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> bkgonly flag: %s", bkgonly ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> smear art position (gaussian): %s", smear_art ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> smear art position (functional): %s", funcsmear_art ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> Number of Events: %d", nevents);
	std::cout << std::endl;
	printf("\r >> x-road size (in strips): %d, +/- neighbor roads (uv): %d", XROAD, UVFACTOR);
	std::cout << std::endl;
	printf("\r >> art res (in ns): %d", m_sig_art);
	std::cout << std::endl;
	std::cout << "\r >> Using BCID window: " << bc_wind << std::endl;
	printf("\r >> Background rate of %d Hz per strip",bkgrate);
	std::cout << std::endl;
	printf("\r >> Assuming chamber size: (%4.1f,%4.1f) in mm",xhigh-xlow, yhigh-ylow);
	std::cout << std::endl;
	printf("\r >> Assuming muon active area: (%4.1f,%4.1f) in mm",mu_xhigh-mu_xlow, mu_yhigh-mu_ylow);
	std::cout << std::endl;
	printf("\r >> Using UV roads: %s", (uvrflag) ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> Using trapezoidal geometry: %s", (trapflag) ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> Using thresholds (x, uv): (%d, %d)", m_xthr, m_uvthr);
	std::cout << std::endl;
	printf("\r >> Generate muons with cosmic distribution: %s", (angcos) ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> Generate muons with angle (x) from %f to %f", -angx, angx);
	std::cout << std::endl;
	printf("\r >> Generate muons with angle (y) from %f to %f", -angy, angy);
	std::cout << std::endl;
	printf("\r >> Killing one plane randomly: %s", (killran) ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> Killing one X plane randomly: %s", (killxran) ? "true" : "false");
	std::cout << std::endl;
	printf("\r >> Killing one UV plane randomly: %s", (killuvran) ? "true" : "false");
	std::cout << std::endl;
	for (unsigned int i = 0; i < mm_eff.size(); i++){
		printf("\r >> MM efficiency, chamber %i: %f", i, mm_eff[i]);
		std::cout << std::endl;
	}
	printf("\r >> Seed for TRandom3: %d", ran->GetSeed());
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}




#endif
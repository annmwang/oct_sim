ROOTCFLAGS    = $(shell root-config --cflags)
ROOTGLIBS     = $(shell root-config --glibs)

CXX            = g++
CXXFLAGS       = -fPIC -Wall -O3 -g -std=c++11 -Wc++17-extensions 
CXXFLAGS       += $(filter-out -stdlib=libc++ -pthread , $(ROOTCFLAGS))
GLIBS          = $(filter-out -stdlib=libc++ -pthread , $(ROOTGLIBS))
RPATH 		   = -rpath /Users/anthonybadea/builddir/lib

INCLUDEDIR       = ./include/
INCLUDEDIR		 = $(PWD)/include/
SRCDIR           = ./src/
CXX             += -I$(INCLUDEDIR) -I.
OUTOBJ	         = ./obj/
BINDIR			 = ./bin/

BB5              = ../bb5_analysis
BB5_INCLUDEDIR   = $(BB5)/include
CXX             += -I$(BB5) -I.
CXX             += -I$(BB5_INCLUDEDIR) -I.

CC_FILES := $(wildcard src/*.cc)
HH_FILES := $(wildcard include/*.hh)
OBJ_FILES := $(addprefix $(OUTOBJ),$(notdir $(CC_FILES:.cc=.o)))
DICT_FILES := $(wildcard include/*.pcm)

MKDIR_BIN=mkdir -p $(PWD)/bin

all: mkdirBin VectorDict sim testGBT sim_PCBEff_Modular validate_changes tpcosmics
cosmics: mkdirBin VectorDict tpcosmics

mkdirBin:
	$(MKDIR_BIN)

VectorDict: $(INCLUDEDIR)VectorDict.hh
	rootcint -f $(INCLUDEDIR)VectorDict.cxx -c $(CXXFLAGS) -p $ $<
	touch $(INCLUDEDIR)VectorDict.cxx

sim:  $(SRCDIR)sim.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) $(RPATH) -o $(BINDIR)sim $ $< $(GLIBS) 
	touch $(BINDIR)sim

sim_PCBEff_Modular:  $(SRCDIR)sim_PCBEff_Modular.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) $(RPATH) -o $(BINDIR)sim_PCBEff_Modular $ $< $(GLIBS) 
	touch $(BINDIR)sim_PCBEff_Modular

validate_changes:  $(SRCDIR)validate_changes.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) $(RPATH) -o $(BINDIR)validate_changes $ $< $(GLIBS) 
	touch $(BINDIR)validate_changes

testGBT:  $(SRCDIR)testGBT.C $(OBJ_FILES) $(HH_FILES) 
	$(CXX) $(CXXFLAGS) $(RPATH) -o $(BINDIR)testGBT $ $< $(GLIBS) 
	touch $(BINDIR)testGBT

tpcosmics:  $(SRCDIR)tpcosmics.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) $(RPATH) -o $(BINDIR)tpcosmics $ $< $(GLIBS) 
	touch $(BINDIR)tpcosmics

$(OUTOBJ)%.o: src/%.cc include/%.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f $(OUTOBJ)*.o
	rm -rf *.dSYM
	rm -f sim
	rm -f testGBT 
	rm -f VectorDict.cxx
	rm -f VectorDict_rdict.pcm

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTGLIBS     = $(shell root-config --glibs)

CXX            = g++
CXXFLAGS       = -fPIC -Wall -O3 -g
CXXFLAGS       += $(filter-out -stdlib=libc++ -pthread , $(ROOTCFLAGS))
GLIBS          = $(filter-out -stdlib=libc++ -pthread , $(ROOTGLIBS))

INCLUDEDIR       = ./include/
SRCDIR           = ./src/
CXX             += -I$(INCLUDEDIR) -I.
OUTOBJ	         = ./obj/

CC_FILES := $(wildcard src/*.cc)
HH_FILES := $(wildcard include/*.hh)
OBJ_FILES := $(addprefix $(OUTOBJ),$(notdir $(CC_FILES:.cc=.o)))
DICT_FILES := $(wildcard include/*.pcm)

all: VectorDict.cxx sim testGBT sim_PCBEff_Modular validate_changes

VectorDict: $(INCLUDEDIR)VectorDict.hh
	rootcint -f VectorDict.cxx -c $(CXXFLAGS) -p $ $<
	touch VectorDict.cxx

sim:  $(SRCDIR)sim.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) -o sim $ $< $(GLIBS) 
	touch sim

sim_PCBEff_Modular:  $(SRCDIR)sim_PCBEff_Modular.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) -o sim_PCBEff_Modular $ $< $(GLIBS) 
	touch sim_PCBEff_Modular

validate_changes:  $(SRCDIR)validate_changes.C $(OBJ_FILES) $(HH_FILES) $(DICT_FILES)
	$(CXX) $(CXXFLAGS) -o validate_changes $ $< $(GLIBS) 
	touch validate_changes

testGBT:  $(SRCDIR)testGBT.C $(OBJ_FILES) $(HH_FILES) 
	$(CXX) $(CXXFLAGS) -o testGBT $ $< $(GLIBS) 
	touch testGBT

$(OUTOBJ)%.o: src/%.cc include/%.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f $(OUTOBJ)*.o
	rm -rf *.dSYM
	rm -f sim
	rm -f testGBT 
	rm -f VectorDict.cxx
	rm -f VectorDict_rdict.pcm
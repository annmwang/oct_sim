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

all: sim testGBT

sim:  $(SRCDIR)sim.C $(OBJ_FILES) $(HH_FILES) 
	$(CXX) $(CXXFLAGS) -o sim $ $< $(GLIBS) 
	touch sim

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

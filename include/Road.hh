///
///  \file   Road.hh
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Sept
///


#ifndef Road_HH
#define Road_HH

#include "include/Hit.hh"
#include <numeric>

class Road {

public:
  Road();
  Road(int iroad, const GeoOctuplet& geometry);
  ~Road();

  int iRoad();
  int Count();
  void Reset();
  bool Contains(const Hit& hit, int roadsize, int uvfactor);
  bool Contains_Neighbors(const Hit& hit, int roadsize, int uvfactor);
  void Add_Hits(std::vector<Hit*> & hits, int roadsize, int uvfactor);
  void Increment_Age(int wind);
  bool Coincidence(int wind);
  bool Horiz_ok();
  bool Stereo_ok();
  bool Mature(int wind);
  double Mxl();
private:
  int m_iroad;
  std::vector<Hit> m_hits;

  const GeoOctuplet* m_geometry;
  bool m_trig;
};

inline Road::Road(){
  m_iroad = -1;
  m_geometry = nullptr;
  m_trig = false;
}

inline Road::Road(int iroad, const GeoOctuplet& geometry){
  m_iroad = iroad;
  m_geometry = &geometry;
  m_trig = false;
}

inline Road::~Road() {}

inline int Road::iRoad(){
  return m_iroad;
}

inline int Road::Count(){
  return m_hits.size();
}

inline void Road::Reset(){
  m_hits.clear();
}

inline bool Road::Contains(const Hit& hit, int roadsize, int uvfactor) {
  int addstrip; 
  double slow, shigh;
  if (hit.MMFE8Index() > 1 && hit.MMFE8Index() < 6){
    addstrip = uvfactor*roadsize-roadsize;
    slow = roadsize*m_iroad-addstrip/2.;
    shigh = roadsize*(m_iroad+1)+addstrip/2.;
  }
  else{
    slow = roadsize*m_iroad;
    shigh = roadsize*(m_iroad+1);
  }
  if (hit.Channel()>=slow && hit.Channel() <= shigh)
    return true;
  else
    return false;
}

inline bool Road::Contains_Neighbors(const Hit& hit, int roadsize, int uvfactor) {
  int addstrip; 
  double slow, shigh;
  if (hit.MMFE8Index() > 1 && hit.MMFE8Index() < 6){
    addstrip = uvfactor*roadsize-roadsize;
    slow = roadsize*(m_iroad-1)-addstrip/2.;
    shigh = roadsize*(m_iroad+2)+addstrip/2.;
  }
  else{
    slow = roadsize*(m_iroad-1);
    shigh = roadsize*(m_iroad+2);
  }
  if (hit.Channel()>=slow && hit.Channel() <= shigh)
    return true;
  else
    return false;
}

inline void Road::Add_Hits(std::vector<Hit*> & hits, int roadsize, int uvfactor){
  for (int i = 0; i < hits.size(); i++){
    int bo = hits[i]->MMFE8Index();
    bool has_hit = false;
    if (Contains_Neighbors(*hits[i], roadsize, uvfactor)){
      // check if hit exists on the plane already
      for (int j = 0; j < m_hits.size(); j++){
        if (m_hits[j].MMFE8Index() == bo){
          has_hit = true;
          break;
        }
      }
      if (has_hit)
        continue;
      m_hits.push_back(*hits[i]);
      m_hits.back().SetAge(0);
    }
  }
}
void Road::Increment_Age(int wind){
  for (int j = 0; j < m_hits.size(); j++){
    m_hits[j].SetAge(m_hits[j].Age()+1);
    if (m_hits[j].Age() > wind)
      m_hits.erase(m_hits.begin()+j);
  }
}

bool Road::Coincidence(int wind){
  return Horiz_ok() && Stereo_ok() && Mature(wind);
}

bool Road::Horiz_ok(){
  int nx1 = 0;
  int nx2 = 0;
  for (int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() < 2)
      nx1++;
    if (m_hits[i].MMFE8Index() > 5)
      nx2++;
  }
  if (nx1 > 0 && nx2 > 0)
    return true;
  return false;
}

bool Road::Stereo_ok(){
  int nu = 0;
  int nv = 0;
  for (int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() == 2 || m_hits[i].MMFE8Index() == 4)
      nu++;
    if (m_hits[i].MMFE8Index() == 3 || m_hits[i].MMFE8Index() == 5)
      nv++;
  }
  if (nu > 0 && nv > 0)
    return true;
  return false;
}

bool Road::Mature(int wind){
  for (int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].Age() == wind)
      return true;
  }
  return false;
}

double Road::Mxl(){
  std::vector<double> xs;
  std::vector<double> zs;
  for (int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo < 2 || bo > 5){
      double vmm_ch = m_hits[i].Channel()* 0.4;
      xs.push_back(vmm_ch);
      zs.push_back(m_geometry->Get(bo).Origin().Z());
    }
  }
  double mxl = 0.;
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  double sum_sq_z = std::inner_product(zs.begin(), zs.end(), zs.begin(), 0.0);
  for (int i = 0; i < xs.size(); i++){
    mxl += xs[i]*( (zs[i]-avg_z) / (sum_sq_z - zs.size()*pow(avg_z,2)));
  }
  return mxl;
}

#endif




///
///  \file   GeoOctuplet.hh
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Sept
///


#ifndef GeoOctuplet_HH
#define GeoOctuplet_HH

#include <map>


#include "include/GeoPlane.hh"

class GeoOctuplet {

public:
  GeoOctuplet();
  GeoOctuplet(int RunNumber);
  GeoOctuplet(const GeoOctuplet& oct);
  ~GeoOctuplet();

  int GetNPlanes() const;
  GeoPlane const& Get(int index) const;
  GeoPlane const& operator [] (int index) const;

private:
  void Init();

  std::vector<GeoPlane*> m_planes;

  mutable std::map<int,int> m_MMFE82Index;
  mutable std::map<int,int> m_Index2MMFE8;

};

inline GeoOctuplet::GeoOctuplet(){
  Init();
}

inline GeoOctuplet::GeoOctuplet(int RunNumber){
  Init();
}

inline GeoOctuplet::GeoOctuplet(const GeoOctuplet& oct){
  int N = oct.GetNPlanes();
  for(int i = 0; i < N; i++)
    m_planes.push_back(new GeoPlane(oct[i]));
}

inline GeoOctuplet::~GeoOctuplet(){
  int N = GetNPlanes();
  for(int i = 0; i < N; i++)
    delete m_planes[i];
}

inline void GeoOctuplet::Init(){
  int i = 0;
  TVector3 origin;

  // plane 0
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100., 0.);
  origin.SetXYZ(102.3, 100., -2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(0.);
  m_planes[i]->SetSignChannel(-1);

  i++;
  // plane 1
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100., 11.2);
  origin.SetXYZ(102.3, 100., 11.2+2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(0.);
  m_planes[i]->SetSignChannel(1);

  i++;
  // plane 2
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100.+17.9, 32.4);
  origin.SetXYZ(102.3, 100.+17.9, 32.4-2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(-0.0261799);
  m_planes[i]->SetSignChannel(1);

  i++;
  // plane 3
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100.+17.9, 43.6);
  origin.SetXYZ(102.3, 100.+17.9, 43.6+2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(0.0261799);
  m_planes[i]->SetSignChannel(-1);

  i++;
  // plane 4
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100.+17.9, 113.6);
  origin.SetXYZ(102.3, 100.+17.9, 113.6-2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(-0.0261799);
  m_planes[i]->SetSignChannel(1);

  i++;
  // plane 5
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100.+17.9, 124.8);
  origin.SetXYZ(102.3, 100.+17.9, 124.8+2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(0.0261799);
  m_planes[i]->SetSignChannel(-1);

  i++;
  // plane 6
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100., 146.0);
  origin.SetXYZ(102.3, 100., 146.0-2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(0.);
  m_planes[i]->SetSignChannel(-1);

  i++;
  // plane 7
  m_planes.push_back(new GeoPlane());
  //origin.SetXYZ(102.3, 100., 157.2);
  origin.SetXYZ(102.3, 100., 157.2+2.7);
  m_planes[i]->SetOrigin(origin);
  m_planes[i]->SetStripAlpha(0.);
  m_planes[i]->SetSignChannel(1);
}

inline int GeoOctuplet::GetNPlanes() const {
  return int(m_planes.size());
}

inline GeoPlane const& GeoOctuplet::Get(int index) const{
  return *m_planes[index];
}

inline GeoPlane const& GeoOctuplet::operator [] (int index) const{
  return Get(index);
}


#endif




///
///  \file   GeoPlane.hh
///
///  \author Christopher Rogan
///          (crogan@cern.ch)
///
///  \date   2016 Sept
///


#ifndef GeoPlane_HH
#define GeoPlane_HH

#include "TVector3.h"

class GeoPlane {

public:
  GeoPlane();
  GeoPlane(const GeoPlane& plane);
  ~GeoPlane();

  TVector3 const& Origin() const;
  TVector3 const& nX() const;
  TVector3 const& nY() const;
  TVector3 const& nZ() const;
  double StripAlpha() const;

  
  void SetOrigin(const TVector3& p);
  void SetStripAlpha(double alpha);

  double channel_from_pos(double xpos, double ypos) const;

private:
  TVector3 m_Origin;

  TVector3 m_nX;
  TVector3 m_nY;
  TVector3 m_nZ;

  double m_Alpha;


};

inline GeoPlane::GeoPlane(){
  m_Origin.SetXYZ(0.,0.,0.);
  m_nX.SetXYZ(1.,0.,0.);
  m_nY.SetXYZ(0.,1.,0.);
  m_nZ.SetXYZ(0.,0.,1.);
  m_Alpha = 0.;
}

inline GeoPlane::GeoPlane(const GeoPlane& plane){
  m_Origin = plane.Origin();
  m_nX = plane.nX();
  m_nY = plane.nY();
  m_nZ = plane.nZ();
  m_Alpha = plane.StripAlpha();
}

inline GeoPlane::~GeoPlane() {}

inline TVector3 const& GeoPlane::Origin() const {
  return m_Origin;
}

inline TVector3 const& GeoPlane::nX() const {
  return m_nX;
}

inline TVector3 const& GeoPlane::nY() const {
  return m_nY;
}

inline TVector3 const& GeoPlane::nZ() const {
  return m_nZ;
}

inline double GeoPlane::StripAlpha() const {
  return m_Alpha;
}

inline void GeoPlane::SetOrigin(const TVector3& p){
  m_Origin = p;
}

inline void GeoPlane::SetStripAlpha(double alpha){
  m_Alpha = alpha;
}

inline double GeoPlane::channel_from_pos(double xpos, double ypos) const{
  return  ((xpos - m_Origin.X() - tan(m_Alpha)*( ypos - m_Origin.Y() )) / (0.4)) + 256.5;
}

#endif




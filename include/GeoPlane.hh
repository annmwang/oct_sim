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
#include "TMath.h"

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
  double xlen() const;
  double ylen() const;


  double LocalXatYbegin(double channel) const;
  double LocalXatYend(double channel) const;
  
  void SetOrigin(const TVector3& p);
  void SetStripAlpha(double alpha);
  void SetDim(double xlen, double ylen);

  double channel_from_pos(double xpos, double ypos) const;

private:
  TVector3 m_Origin;

  TVector3 m_nX;
  TVector3 m_nY;
  TVector3 m_nZ;

  double m_Alpha;

  double m_xlen;
  double m_ylen;

};

inline GeoPlane::GeoPlane(){
  m_Origin.SetXYZ(0.,0.,0.);
  m_nX.SetXYZ(1.,0.,0.);
  m_nY.SetXYZ(0.,1.,0.);
  m_nZ.SetXYZ(0.,0.,1.);
  m_Alpha = 0.;
  m_xlen = 0.;
  m_ylen = 0.;
}

inline GeoPlane::GeoPlane(const GeoPlane& plane){
  m_Origin = plane.Origin();
  m_nX = plane.nX();
  m_nY = plane.nY();
  m_nZ = plane.nZ();
  m_Alpha = plane.StripAlpha();
  m_xlen = plane.xlen();
  m_ylen = plane.ylen();
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

inline double GeoPlane::xlen() const {
  return m_xlen;
}

inline double GeoPlane::ylen() const {
  return m_ylen;
}

inline void GeoPlane::SetOrigin(const TVector3& p){
  m_Origin = p;
}

inline void GeoPlane::SetStripAlpha(double alpha){
  m_Alpha = alpha;
}

inline void GeoPlane::SetDim(double xlen, double ylen){
  m_xlen = xlen;
  m_ylen = ylen;
}

inline double GeoPlane::channel_from_pos(double xpos, double ypos) const{
  double ch_mid = (m_xlen+0.2)/0.4/2+0.5;
  double channel = ((xpos - m_Origin.X()- TMath::Tan(m_Alpha) * (m_Origin.Y()+m_ylen/2.-ypos)) / (0.4)) + ch_mid;
  channel = round(channel);
  return channel;
}

inline double GeoPlane::LocalXatYbegin(double channel) const {
  double ch_mid = (m_xlen+0.2)/0.4/2+0.5;
  return (channel-ch_mid)*0.4 + TMath::Tan(m_Alpha)*m_ylen;
}

inline double GeoPlane::LocalXatYend(double channel) const {
  double ch_mid = (m_xlen+0.2)/0.4/2+0.5;
  return (channel-ch_mid)*0.4;
}

#endif




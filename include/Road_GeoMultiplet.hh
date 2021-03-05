///
///  \file   Road_GeoMultiplet.hh
///
///  \author Ann Wang
///          (anwang@cern.ch)
///
///  \date   2017 Aug
///


#ifndef Road_HH
#define Road_HH

#include "include/Hit_GeoMultiplet.hh"
#include <numeric>

class Road {

public:
  Road();
  Road(int iroad, const GeoMultiplet& geometry, int xthr, int uvthr);
  Road(const GeoMultiplet* geometry, int xthr, int uvthr, int iroadx, int iroadu = -1, int iroadv = -1);
  ~Road();

  int iRoad();
  int iRoadx();
  int iRoadu();
  int iRoadv();
  int Count();
  void Reset();
  int Offset(int ib);
  bool Contains(const Hit& hit, int roadsize, int uvfactor);
  bool Contains_Neighbors(const Hit& hit, int roadsize, int uvfactor);
  bool Contains_Neighbors(const Hit& hit, int roadsize, int nstr_up_xx, int nstr_dn_xx, int nstr_up_uv, int nstr_dn_uv);
  void Add_Hits(std::vector<Hit*> & hits, int roadsize, int uvfactor);
  void Add_Hits(std::vector<Hit*> & hits, int roadsize, int nstr_up_xx, int nstr_dn_xx, int nstr_up_uv, int nstr_dn_uv, bool ideal_tp);
  void Increment_Age(int wind);
  bool Coincidence(int wind);
  int NMuon();
  bool Horiz_ok();
  bool Stereo_ok();
  int UV_bkg();
  int UV_muon();
  int X_bkg();
  int X_muon();
  int NX();
  int NUV();
  bool Mature(int wind);
  double Mxl();
  double Xpos(double ch, int ib);
  double AvgXofX();
  double AvgXofU();
  double AvgXofV();
  double AvgZofX();
  double AvgZofU();
  double AvgZofV();
  double AvgYfromUV();
  double AvgYfromUV_BestPair();
  double YfromUV(Hit uhit, Hit vhit);
  std::vector<Hit> Hits();
  std::tuple<double, double> CornerXY(int corner, int roadsize, int nstr_up_xx, int nstr_dn_xx, int nstr_up_uv, int nstr_dn_uv);
  std::tuple<double, double> Center(int roadsize, int nstr_up_xx, int nstr_dn_xx, int nstr_up_uv, int nstr_dn_uv);

private:
  int m_iroad;
  int m_iroadx;
  int m_iroadu;
  int m_iroadv;
  std::vector<Hit> m_hits;

  int m_xthr;
  int m_uvthr;

  const GeoMultiplet* m_geometry;
  bool m_trig;
};

inline Road::Road(){
  m_iroad = -1;
  m_geometry = nullptr;
  m_trig = false;
}

inline Road::Road(int iroad, const GeoMultiplet& geometry, int xthr, int uvthr){
  m_iroad = iroad;
  m_geometry = &geometry;
  m_trig = false;
  m_xthr = xthr;
  m_uvthr = uvthr;
}

inline Road::Road(const GeoMultiplet* geometry, int xthr, int uvthr, int iroadx, int iroadu, int iroadv){
  m_iroad  = iroadx;
  m_iroadx = iroadx;
  m_iroadu = (iroadu != -1) ? iroadu : iroadx;
  m_iroadv = (iroadv != -1) ? iroadv : iroadx;
  m_geometry = geometry;
  m_trig = false;
  m_xthr = xthr;
  m_uvthr = uvthr;

  if (iroadu == -1 && iroadv != -1)
    std::cout << "WARNING: iroadu = -1 but iroadv aint" << std::endl;
  if (iroadu != -1 && iroadv == -1)
    std::cout << "WARNING: iroadv = -1 but iroadu aint" << std::endl;
}

inline Road::~Road() {}

inline int Road::iRoad(){
  return m_iroad;
}

inline int Road::iRoadx(){
  return m_iroadx;
}

inline int Road::iRoadu(){
  return m_iroadu;
}

inline int Road::iRoadv(){
  return m_iroadv;
}

inline int Road::Count(){
  return m_hits.size();
}

inline void Road::Reset(){
  m_hits.clear();
}

inline int Road::Offset(int ib){
  int offsets[3];
  // if (m_geometry->Get(0).ylen() == 200){
  //   offsets[0] = 0;
  //   offsets[1] = -6; 
  //   offsets[2] = 7;
  // }
  // if (m_geometry->Get(0).ylen() == 500){
  //   offsets[0] = 0;
  //   offsets[1] = -16; 
  //   offsets[2] = 17;
  // }
  // if (m_geometry->Get(0).ylen() == 1821){
  //   offsets[0] = 0;
  //   offsets[1] = -59; 
  //   offsets[2] = 60;
  // }
  // if (m_geometry->Get(0).ylen() == 2200){
  //   offsets[0] = 0;
  //   offsets[1] = -72; 
  //   offsets[2] = 72;
  // }

  // just inserted to bypass the compilation error
  offsets[0] = 0;
  offsets[1] = 0; 
  offsets[2] = 0;
  // remove when you figure out how to use .ylen() with GeoLayer.hh

  if (ib == 2 || ib == 4)
    //    return -6;
    return offsets[1];
  else if (ib == 3 || ib == 5)
    //    return 7;
    return offsets[2];
  else
    return offsets[0];
}

inline bool Road::Contains(const Hit& hit, int roadsize, int uvfactor) {
  double slow, shigh;
  slow = roadsize*m_iroad;
  shigh = roadsize*(m_iroad+1);
  int strip = hit.Channel();
  strip += Offset(hit.MMFE8Index());
  if (strip >= slow && strip < shigh)
    return true;
  else
    return false;
}

inline bool Road::Contains_Neighbors(const Hit& hit, int roadsize, int uvfactor) {
  double slow, shigh;

  if (hit.MMFE8Index() > 1 && hit.MMFE8Index() < 6){
    slow = roadsize*(m_iroad-uvfactor - 1);
    shigh = roadsize*(m_iroad+uvfactor+1 + 1);
  }
  else{
    slow = roadsize*(m_iroad-1);
    shigh = roadsize*(m_iroad+2);
  }
  int strip = hit.Channel();
  strip += Offset(hit.MMFE8Index());
  // std::cout << "strip from iboard, layer: " << strip << ", "<< hit.MMFE8Index() << ", " << std::endl; 
  // std::cout << "looking @ iRoad " << iRoad() <<" ["<<slow <<", " << shigh << "]"<< std::endl;
  if (strip >= slow && strip < shigh)
    return true;
  else
    return false;
}

inline bool Road::Contains_Neighbors(const Hit& hit, int roadsize,
                                     int nstr_up_xx, int nstr_dn_xx,
                                     int nstr_up_uv, int nstr_dn_uv) {

  int iroad = 0;
  if      (hit.isX()) iroad = m_iroadx;
  else if (hit.isU()) iroad = m_iroadu;
  else if (hit.isV()) iroad = m_iroadv;
  else {
    std::cout << "WARNING: Hit is weird." << std::endl;
    return false;
  }

  int slow  = roadsize*(iroad);
  int shigh = roadsize*(iroad+1);

  if (hit.isX()){
    slow  -= nstr_dn_xx;
    shigh += nstr_up_xx;
  }
  else {
    slow  -= nstr_dn_uv;
    shigh += nstr_up_uv;
  }

  int strip = hit.Channel();
  strip += Offset(hit.MMFE8Index());

  return (strip >= slow && strip < shigh);
}

inline void Road::Add_Hits(std::vector<Hit*> & hits, int roadsize, int uvfactor){
  for (unsigned int i = 0; i < hits.size(); i++){
    int bo = hits[i]->MMFE8Index();
    bool has_hit = false;
    if (Contains_Neighbors(*hits[i], roadsize, uvfactor)){
      // check if hit exists on the plane already
      for (unsigned int j = 0; j < m_hits.size(); j++){
        if (m_hits[j].MMFE8Index() == bo){
          has_hit = true;
          // std::cout << "already has hit on plane: " << bo << std::endl;
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

inline void Road::Add_Hits(std::vector<Hit*> & hits, int roadsize, 
                           int nstr_up_xx, int nstr_dn_xx, 
                           int nstr_up_uv, int nstr_dn_uv,
                           bool ideal_tp){
  for (auto hit_i: hits){
    int bo = hit_i->MMFE8Index();
    bool has_hit = false;
    if (Contains_Neighbors(*hit_i, roadsize, nstr_up_xx, nstr_dn_xx, nstr_up_uv, nstr_dn_uv)){
      for (auto hit_j: m_hits){
        if (hit_j.MMFE8Index() == bo){
          has_hit = true;
          break;
        }
      }

      if (ideal_tp && hit_i->IsReal()){
        int erase_me = -1;
        for (unsigned int j = 0; j < m_hits.size(); j++){
          if (m_hits[j].MMFE8Index() == bo && m_hits[j].IsNoise()){
            erase_me = j;
            has_hit = false;
            break;
          }
        }
        if (erase_me >= 0)
          m_hits.erase(m_hits.begin() + erase_me);
      }

      if (has_hit)
        continue;
      m_hits.push_back(*hit_i);
      m_hits.back().SetAge(0);
    }
  }
}

void Road::Increment_Age(int wind){
  int nlost = 0;
  std::vector<int> old_ihits;
  for (unsigned int j = 0; j < m_hits.size(); j++){
    m_hits[j].SetAge(m_hits[j].Age()+1);
    if (m_hits[j].Age() > (wind-1)){
      old_ihits.push_back(j);
      nlost++;
    }
  }
  for (int j = old_ihits.size()-1; j > -1; j--){
    m_hits.erase(m_hits.begin()+j);
  }
}

bool Road::Coincidence(int wind){
  //return Horiz_ok() && Stereo_ok();
  return Horiz_ok() && Stereo_ok() && Mature(wind);
}

int Road::NMuon(){
  int nreal = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].IsNoise() == false)
      nreal++;
  }
  return nreal;
}

bool Road::Horiz_ok(){
  int nx1 = 0;
  int nx2 = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() < 2)
      nx1++;
    if (m_hits[i].MMFE8Index() > 5)
      nx2++;
  }
  if (nx1 > 0 && nx2 > 0 && (nx1+nx2) >= m_xthr)
    return true;
  return false;
}

bool Road::Stereo_ok(){
  int nu = 0;
  int nv = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() == 2 || m_hits[i].MMFE8Index() == 4)
      nu++;
    if (m_hits[i].MMFE8Index() == 3 || m_hits[i].MMFE8Index() == 5)
      nv++;
  }
  if (m_uvthr == 0)
    return true;
  if (nu > 0 && nv > 0 && (nu+nv) >= m_uvthr)
    return true;
  return false;
}

int Road::UV_bkg(){
  int nuv_bkg = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() == 2 || m_hits[i].MMFE8Index() == 4)
      if (m_hits[i].IsNoise() == true)
        nuv_bkg++;
    if (m_hits[i].MMFE8Index() == 3 || m_hits[i].MMFE8Index() == 5)
      if (m_hits[i].IsNoise() == true)
        nuv_bkg++;
  }
  return nuv_bkg;
}

int Road::UV_muon(){
  int nuv_muon = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() == 2 || m_hits[i].MMFE8Index() == 4)
      if (m_hits[i].IsNoise() == false)
        nuv_muon++;
    if (m_hits[i].MMFE8Index() == 3 || m_hits[i].MMFE8Index() == 5)
      if (m_hits[i].IsNoise() == false)
        nuv_muon++;
  }
  return nuv_muon;
}

int Road::X_bkg(){
  int nx_bkg = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() < 2 || m_hits[i].MMFE8Index() > 5)
      if (m_hits[i].IsNoise() == true)
        nx_bkg++;
  }
  return nx_bkg;
}

int Road::X_muon(){
  int nx_muon = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    if (m_hits[i].MMFE8Index() < 2 || m_hits[i].MMFE8Index() > 5)
      if (m_hits[i].IsNoise() == false)
        nx_muon++;
  }
  return nx_muon;
}
int Road::NX(){
  int n = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++)
    if (m_hits[i].MMFE8Index() == 0 || m_hits[i].MMFE8Index() == 1 ||
        m_hits[i].MMFE8Index() == 6 || m_hits[i].MMFE8Index() == 7)
      n++;
  return n;
}

int Road::NUV(){
  int n = 0;
  for (unsigned int i = 0; i < m_hits.size(); i++)
    if (m_hits[i].MMFE8Index() == 2 || m_hits[i].MMFE8Index() == 4 ||
        m_hits[i].MMFE8Index() == 3 || m_hits[i].MMFE8Index() == 5)
      n++;
  return n;
}

bool Road::Mature(int wind){
  for (unsigned int i = 0; i < m_hits.size(); i++){
    //if ( (m_hits[i].Age() == (wind-1)) && ( m_hits[i].MMFE8Index() < 2 || m_hits[i].MMFE8Index() > 5 ) )
    //  return true;
    if (m_hits[i].Age() == (wind-1))
      return true;  
  }
  return false;
}

double Road::Mxl(){
  std::vector<double> xs;
  std::vector<double> zs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo < 2 || bo > 5){
      double vmm_ch = m_hits[i].Channel()* 0.4;
      xs.push_back(vmm_ch);
      zs.push_back(m_geometry->Get(bo).Origin().Z() + 2.7*pow(-1,bo));
    }
  }
  double mxl = 0.;
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  double sum_sq_z = std::inner_product(zs.begin(), zs.end(), zs.begin(), 0.0);
  for (unsigned int i = 0; i < xs.size(); i++){
    mxl += xs[i]*( (zs[i]-avg_z) / (sum_sq_z - zs.size()*pow(avg_z,2)));
  }
  return mxl;
}

double Road::Xpos(double ch, int ib){
  // double xpos = m_geometry->Get(ib).LocalXatYend(ch)+m_geometry->Get(ib).Origin().X(); // use with GeoOctuplet.hh/GeoPlane.hh
  double xpos = m_geometry->Get(ib).LocalXatYcenter(ch)+m_geometry->Get(ib).Origin().X(); // use with GeoMultiplet.hh/GeoLayer.hh
  return xpos;
}

double Road::AvgXofX(){
  // avg x over x boards

  std::vector<double> xs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo < 2 || bo > 5){
      double vmm_ch = Xpos(m_hits[i].Channel(),bo);
      xs.push_back(vmm_ch);
    }
  }
  double avg_x = std::accumulate(xs.begin(), xs.end(), 0.0)/(double)xs.size();
  return avg_x;
}

double Road::AvgXofU(){
  // avg x over u boards

  std::vector<double> xs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo == 2 || bo == 4){
      double vmm_ch = Xpos(m_hits[i].Channel(),bo);
      xs.push_back(vmm_ch);
    }
  }
  double avg_x = std::accumulate(xs.begin(), xs.end(), 0.0)/(double)xs.size();
  return avg_x;
}

double Road::AvgXofV(){
  // avg x over v boards

  std::vector<double> xs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo == 3 || bo == 5){
      double vmm_ch = Xpos(m_hits[i].Channel(),bo);
      xs.push_back(vmm_ch);
    }
  }
  double avg_x = std::accumulate(xs.begin(), xs.end(), 0.0)/(double)xs.size();
  return avg_x;
}

double Road::AvgZofX(){
  // avg z over x boards

  std::vector<double> xs;
  std::vector<double> zs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo < 2 || bo > 5){
      double vmm_ch = m_hits[i].Channel()* 0.4;
      xs.push_back(vmm_ch);
      zs.push_back(m_geometry->Get(bo).Origin().Z());
    }
  }
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  return avg_z;
}

double Road::AvgZofU(){
  // avg z over u boards

  std::vector<double> xs;
  std::vector<double> zs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo == 2 || bo == 4){
      double vmm_ch = m_hits[i].Channel()* 0.4;
      xs.push_back(vmm_ch);
      zs.push_back(m_geometry->Get(bo).Origin().Z());
    }
  }
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  return avg_z;
}

double Road::AvgZofV(){
  // avg z over v boards

  std::vector<double> xs;
  std::vector<double> zs;
  for (unsigned int i = 0; i < m_hits.size(); i++){
    int bo = m_hits[i].MMFE8Index();
    if (bo == 3 || bo == 5){
      double vmm_ch = m_hits[i].Channel()* 0.4;
      xs.push_back(vmm_ch);
      zs.push_back(m_geometry->Get(bo).Origin().Z());
    }
  }
  double avg_z = std::accumulate(zs.begin(), zs.end(), 0.0)/(double)zs.size();
  return avg_z;
}

double Road::AvgYfromUV(){
  double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));
  return -B*( AvgXofU() - AvgXofV() + (AvgZofV()-AvgZofU())*Mxl() ) / 2;
}

double Road::YfromUV(Hit uhit, Hit vhit){
  double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));
  double xpos_u, xpos_v, zpos_u, zpos_v;
  xpos_u = Xpos(uhit.Channel(), uhit.MMFE8Index());
  xpos_v = Xpos(vhit.Channel(), vhit.MMFE8Index());
  zpos_u = m_geometry->Get(uhit.MMFE8Index()).Origin().Z();
  zpos_v = m_geometry->Get(vhit.MMFE8Index()).Origin().Z();
  return -B*( xpos_u - xpos_v + (zpos_v-zpos_u)*Mxl() ) / 2;
}

double Road::AvgYfromUV_BestPair(){

  double xpos_u, xpos_v, xpos_uv, diff, best_diff, best_y;
  best_diff = 99999.9;
  best_y = 0.0;

  for (auto uhit: m_hits){
    if (! uhit.isU())
      continue;
    for (auto vhit: m_hits){
      if (! vhit.isV())
        continue;

      xpos_u = Xpos(uhit.Channel(), uhit.MMFE8Index());
      xpos_v = Xpos(vhit.Channel(), vhit.MMFE8Index());
      xpos_uv = (xpos_u + xpos_v) / 2.0;
      diff = std::fabs(xpos_uv - AvgXofX());

      if (diff < best_diff){
        best_diff = diff;
        best_y    = YfromUV(uhit, vhit);
      }
    }
  }
  return best_y;
}

std::vector<Hit> Road::Hits(){
  return m_hits;
}

std::tuple<double, double> Road::Center(int roadsize, int nstr_up_xx, int nstr_dn_xx, int nstr_up_uv, int nstr_dn_uv) {

  int corners = 4;
  double x_corner = 0.0;
  double y_corner = 0.0;
  double x_total  = 0.0;
  double y_total  = 0.0;

  for (int corner = 0; corner < corners; corner++){
    std::tie(x_corner, y_corner) = CornerXY(corner, roadsize, nstr_up_xx, nstr_dn_xx, nstr_up_uv, nstr_dn_uv);
    x_total += x_corner;
    y_total += y_corner;
  }

  double x_avg = x_total / (double)(corners);
  double y_avg = y_total / (double)(corners);

  return std::make_tuple(x_avg, y_avg);
}

std::tuple<double, double> Road::CornerXY(int corner, int roadsize, int nstr_up_xx, int nstr_dn_xx, int nstr_up_uv, int nstr_dn_uv){

  int uboard = 2;
  int vboard = 3;

  // find the min and max strip of U,V road
  double ustrip_min = roadsize*(m_iroadu)   - nstr_dn_uv - Offset(uboard);
  double ustrip_max = roadsize*(m_iroadu+1) + nstr_up_uv - Offset(uboard) - 1;
  double vstrip_min = roadsize*(m_iroadv)   - nstr_dn_uv - Offset(vboard);
  double vstrip_max = roadsize*(m_iroadv+1) + nstr_up_uv - Offset(vboard) - 1;

  // get their x-positions
  double x_ustrip_min = Xpos(ustrip_min, uboard);
  double x_ustrip_max = Xpos(ustrip_max, uboard);
  double x_vstrip_min = Xpos(vstrip_min, vboard);
  double x_vstrip_max = Xpos(vstrip_max, vboard);

  // derive the corner coordinates
  double x_0 = (x_ustrip_min + x_vstrip_min) / 2.0;
  double x_1 = (x_ustrip_min + x_vstrip_max) / 2.0;
  double x_2 = (x_ustrip_max + x_vstrip_min) / 2.0;
  double x_3 = (x_ustrip_max + x_vstrip_max) / 2.0;

  double B = (1/TMath::Tan(1.5/180.*TMath::Pi()));
  double y_offset = 2200; // m_geometry->Get(0).ylen(); use this with GeoPlane/GeoOctuplet. Hard coded as 2200 until figure out how to use GeoMultiplet
  double y_0 = -B * (x_ustrip_min - x_vstrip_min) / 2.0 + y_offset;
  double y_1 = -B * (x_ustrip_min - x_vstrip_max) / 2.0 + y_offset;
  double y_2 = -B * (x_ustrip_max - x_vstrip_min) / 2.0 + y_offset;
  double y_3 = -B * (x_ustrip_max - x_vstrip_max) / 2.0 + y_offset;

  // get the requested corner
  if (corner == 0) return std::make_tuple(x_0, y_0);
  if (corner == 1) return std::make_tuple(x_1, y_1);
  if (corner == 2) return std::make_tuple(x_2, y_2);
  if (corner == 3) return std::make_tuple(x_3, y_3);

  std::cerr << "Bad corner! Needs to be 0, 1, 2, or 3. You gave: " << corner << std::endl;
  return std::make_tuple(0.0, 0.0);
}

#endif




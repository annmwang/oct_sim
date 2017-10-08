///
///  \file   Hit.hh
///
///  \author Ann Miao Wang
///
///  \date   2017 May
///


#ifndef Hit_HH
#define Hit_HH

#include "include/GeoOctuplet.hh"

class Hit {

public:
  Hit();
  Hit(int ib, int age, double xpos, double ypos, bool is_noise, const GeoOctuplet& geometry);
  Hit(int ib, int age, double strip, bool is_noise, const GeoOctuplet& geometry);
  ~Hit();

  int MMFE8Index() const;
  double Channel() const;
  int VMM() const;
  int Age();
  int isX() const;
  int isU() const;
  int isV() const;
  double x_pos() const;
  double y_pos() const;
  double x_pos_at_end() const;
  bool IsNoise() const;

  void SetMMFE8Index(int ib);
  void SetPos(double xpos, double ypos, int ib);
  void SetAge(int age);
  
private:
  int m_MMFE8index;
  int m_Age;
  double m_x_pos;
  double m_y_pos;
  double m_x_pos_at_end;
  double m_strip;
  int  m_vmm;
  bool m_IsNoise;

  const GeoOctuplet* m_geometry;
};

inline Hit::Hit(){
  m_MMFE8index = -1;
  m_Age = -1;
  m_x_pos = -1;
  m_y_pos = -1;
  m_x_pos_at_end = -1;
  m_IsNoise = false;
  m_geometry = nullptr;
  m_strip = -1;
  m_vmm = -1;
}

inline Hit::Hit(int ib, int age, double xpos, double ypos, bool is_noise, const GeoOctuplet& geometry){
  m_MMFE8index = ib;
  m_Age = age;
  m_x_pos = xpos;
  m_y_pos = ypos;
  m_IsNoise = is_noise;
  m_geometry = &geometry;
  //  std::cout << "for board: " << ib << std::endl;
  m_strip = m_geometry->Get(ib).channel_from_pos(xpos,ypos);
  m_x_pos_at_end = m_geometry->Get(ib).LocalXatYend(m_strip)+m_geometry->Get(ib).Origin().X();
  m_vmm = m_strip/64;
}

inline Hit::Hit(int ib, int age, double strip, bool is_noise, const GeoOctuplet& geometry){
  m_MMFE8index = ib;
  m_Age = age;
  m_y_pos = -1.;
  m_x_pos = -1.;
  m_IsNoise = is_noise;
  m_geometry = &geometry;
  m_strip = strip;
  m_x_pos_at_end = m_geometry->Get(ib).LocalXatYend(m_strip)+m_geometry->Get(ib).Origin().X();
  m_vmm = m_strip/64;
}

inline Hit::~Hit(){

}

inline int Hit::MMFE8Index() const {
  return m_MMFE8index;
}

inline double Hit::Channel() const{
  return m_strip;
}

inline int Hit::VMM() const{
  return m_vmm;
}

inline int Hit::Age() {
  return m_Age;
}

inline int Hit::isX() const {
  return (MMFE8Index() == 0 ||
          MMFE8Index() == 1 ||
          MMFE8Index() == 6 ||
          MMFE8Index() == 7);
}

inline int Hit::isU() const {
  return (MMFE8Index() == 2 ||
          MMFE8Index() == 4);
}

inline int Hit::isV() const {
  return (MMFE8Index() == 3 ||
          MMFE8Index() == 5);
}

inline double Hit::x_pos() const{
  return m_x_pos;
}

inline double Hit::y_pos() const{
  return m_y_pos;
}

inline double Hit::x_pos_at_end() const{
  return m_x_pos_at_end;
}

inline bool Hit::IsNoise() const{
  return m_IsNoise;
}

inline void Hit::SetPos(double xpos, double ypos, int ib){
  m_x_pos = xpos;
  m_y_pos = ypos;
  m_strip =  m_geometry->Get(ib).channel_from_pos(m_x_pos,m_y_pos);
  m_x_pos_at_end = m_geometry->Get(ib).LocalXatYend(m_strip)+m_geometry->Get(ib).Origin().X();
  m_MMFE8index = ib;
}

inline void Hit::SetAge(int age){
  m_Age = age;
}

inline void Hit::SetMMFE8Index(int ib) {
  m_MMFE8index = ib;
}

#endif


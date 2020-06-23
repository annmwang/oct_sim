#ifndef VANGOGHPALETTE_H
#define VANGOGHPALETTE_H

#include <vector>
#include "TColor.h"

class vanGoghPalette{
 public:
  vanGoghPalette();
  ~vanGoghPalette(){};

  std::vector<Int_t> kirchColors;
  Int_t nCol;

  Int_t getNColor();
  Int_t getColor(unsigned int colPos);
};

vanGoghPalette::vanGoghPalette()
{
  TColor kirchCol;
  kirchColors.push_back(kirchCol.GetColor(240,197,40)); //GOLD
  kirchColors.push_back(kirchCol.GetColor(71, 97, 130)); //BLUE
  kirchColors.push_back(kirchCol.GetColor(175,50,20)); //RED
  kirchColors.push_back(kirchCol.GetColor(183,118,34)); //ORANGE
  kirchColors.push_back(kirchCol.GetColor(143, 145, 132)); //GREEN
  kirchColors.push_back(kirchCol.GetColor(145, 103, 65)); //BROWN
  kirchColors.push_back(kirchCol.GetColor(229, 193,145)); //PEACH
  nCol = kirchColors.size();

  return;
}

Int_t vanGoghPalette::getNColor(){return nCol;}
Int_t vanGoghPalette::getColor(unsigned int colPos){return kirchColors.at(colPos);}

#endif
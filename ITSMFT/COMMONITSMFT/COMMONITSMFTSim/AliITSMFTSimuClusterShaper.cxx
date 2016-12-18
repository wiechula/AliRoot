/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include <iostream>
#include <TBits.h>
#include <TRandom.h>

#include "AliITSMFTSimuClusterShaper.h"

ClassImp(AliITSMFTSimuClusterShaper)

//______________________________________________________________________
AliITSMFTSimuClusterShaper::AliITSMFTSimuClusterShaper() :
fCShape(0)
{}


//______________________________________________________________________
AliITSMFTSimuClusterShaper::AliITSMFTSimuClusterShaper(const UInt_t &cs) {
  fNpixOn = cs;
  UInt_t nRows = 0;
  UInt_t nCols = 0;
  while (nRows*nCols < cs) {
    nRows += 1;
    nCols += 1;
  }
  fCShape = new AliITSMFTClusterShape(nRows, nCols, fNpixOn);
}


//______________________________________________________________________
AliITSMFTSimuClusterShaper::~AliITSMFTSimuClusterShaper() {
  delete fCShape;
}


//______________________________________________________________________
void AliITSMFTSimuClusterShaper::FillClusterRandomly() {
  Int_t matrixSize = fCShape->GetNRows()*fCShape->GetNCols();

  // generate UNIQUE random numbers
  UInt_t i = 0;
  TBits *bits = new TBits(fNpixOn);
  while (i < fNpixOn) {
    UInt_t j = gRandom->Integer(matrixSize); // [0, matrixSize-1]
    if (bits->TestBitNumber(j)) continue;
    bits->SetBitNumber(j);
    i++;
  }

  Int_t bit = 0;
  for (i = 0; i < fNpixOn; ++i) {
    UInt_t j = bits->FirstSetBit(bit);
    fCShape->SetShapeValue(i, j);
    bit = j+1;
  }
  delete bits;
}


//______________________________________________________________________
void AliITSMFTSimuClusterShaper::AddNoisePixel() {
  Int_t matrixSize = fCShape->GetNRows()*fCShape->GetNCols();
  UInt_t j = gRandom->Integer(matrixSize); // [0, matrixSize-1]
  while (fCShape->HasElement(j)) {
    j = gRandom->Integer(matrixSize);
  }
  //fCShape->SetShapeValue(i, j);
}

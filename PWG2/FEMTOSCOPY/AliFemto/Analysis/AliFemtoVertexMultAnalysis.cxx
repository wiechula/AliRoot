/***************************************************************************
 *
 * $Id$
 *
 * Author: Frank Laue, Ohio State, laue@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: AliFemtoMaker package
 *      This is the Class for Analysis objects.  Each of the simultaneous
 *      Analyses running should have one of these instantiated.  They link
 *      into the Manager in an Analysis Collection.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2007/03/07 10:14:49  mchojnacki
 * First version on CVS
 *
 * Revision 1.2  2005/06/28 23:12:24  chajecki
 * UncorrectedNumberOfNegativePrimaries() -> UncorrectedNumberOfPrimaries()
 *
 * For data taken in Y2 and later the centrality definition bases
 * on UncorrectedNumberOfPrimaries() while for Y1(AuAu@130)
 * it based on UncorrectedNumberOfNegativePrimaries().
 * But in many places of HBT code the number of negative primaries
 * was used as a multiplicity for all productions.
 * This has been fixed.
 *
 * Revision 1.1  2001/11/11 18:34:14  laue
 * AliFemtoPicoEventCollectionVectorHideAway: updated for 3d grid
 * AliFemtoVertexMultAnalysis: new
 *
 *
 **************************************************************************/

#include "Analysis/AliFemtoVertexMultAnalysis.h"
#include "Infrastructure/AliFemtoParticleCollection.h"
#include "Base/AliFemtoTrackCut.h"
#include "Base/AliFemtoV0Cut.h"
#include "Base/AliFemtoKinkCut.h"
#include "Infrastructure/AliFemtoPicoEventCollectionVector.h"
#include "Infrastructure/AliFemtoPicoEventCollectionVectorHideAway.h"


#ifdef __ROOT__ 
ClassImp(AliFemtoVertexMultAnalysis)
#endif

extern void FillHbtParticleCollection(AliFemtoParticleCut*         partCut,
				     AliFemtoEvent*               hbtEvent,
				     AliFemtoParticleCollection*  partCollection);


//____________________________
AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(unsigned int binsVertex, double minVertex, double maxVertex,
						 unsigned int binsMult, double minMult, double maxMult) 
  : fVertexZBins(binsVertex), fMultBins(binsMult) {
  //  mControlSwitch     = 0;
  fEventCut          = 0;
  fFirstParticleCut  = 0;
  fSecondParticleCut = 0;
  fPairCut           = 0;
  fCorrFctnCollection= 0;
  fCorrFctnCollection = new AliFemtoCorrFctnCollection;
  fVertexZ[0] = minVertex;
  fVertexZ[1] = maxVertex;
  fUnderFlowVertexZ = 0; 
  fOverFlowVertexZ = 0; 
  fMult[0] = minMult;
  fMult[1] = maxMult;
  fUnderFlowMult = 0; 
  fOverFlowMult = 0; 
  if (fMixingBuffer) delete fMixingBuffer;
  fPicoEventCollectionVectorHideAway = new AliFemtoPicoEventCollectionVectorHideAway(fVertexZBins,fVertexZ[0],fVertexZ[1],
										  fMultBins,fMult[0],fMult[1]);
};
//____________________________

AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) : AliFemtoAnalysis() {
  //AliFemtoVertexMultAnalysis();
  fEventCut          = 0;
  fFirstParticleCut  = 0;
  fSecondParticleCut = 0;
  fPairCut           = 0;
  fCorrFctnCollection= 0;
  fCorrFctnCollection = new AliFemtoCorrFctnCollection;
  fVertexZBins = a.fVertexZBins; 
  fVertexZ[0] = a.fVertexZ[0]; 
  fVertexZ[1] = a.fVertexZ[1];
  fUnderFlowVertexZ = 0; 
  fOverFlowVertexZ = 0; 
  fMultBins = a.fMultBins; 
  fMult[0] = a.fMult[0]; 
  fMult[1] = a.fMult[1];
  fUnderFlowMult = 0; 
  fOverFlowMult = 0; 
  if (fMixingBuffer) delete fMixingBuffer;
  fPicoEventCollectionVectorHideAway = new AliFemtoPicoEventCollectionVectorHideAway(fVertexZBins,fVertexZ[0],fVertexZ[1],
										  fMultBins,fMult[0],fMult[1]);

  // find the right event cut
  fEventCut = a.fEventCut->Clone();
  // find the right first particle cut
  fFirstParticleCut = a.fFirstParticleCut->Clone();
  // find the right second particle cut
  if (a.fFirstParticleCut==a.fSecondParticleCut) 
    SetSecondParticleCut(fFirstParticleCut); // identical particle hbt
  else
  fSecondParticleCut = a.fSecondParticleCut->Clone();

  fPairCut = a.fPairCut->Clone();
  
  if ( fEventCut ) {
      SetEventCut(fEventCut); // this will set the myAnalysis pointer inside the cut
      cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - event cut set " << endl;
  }
  if ( fFirstParticleCut ) {
      SetFirstParticleCut(fFirstParticleCut); // this will set the myAnalysis pointer inside the cut
      cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - first particle cut set " << endl;
  }
  if ( fSecondParticleCut ) {
      SetSecondParticleCut(fSecondParticleCut); // this will set the myAnalysis pointer inside the cut
      cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - second particle cut set " << endl;
  }  if ( fPairCut ) {
      SetPairCut(fPairCut); // this will set the myAnalysis pointer inside the cut
      cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - pair cut set " << endl;
  }

  AliFemtoCorrFctnIterator iter;
  for (iter=a.fCorrFctnCollection->begin(); iter!=a.fCorrFctnCollection->end();iter++){
    cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - looking for correlation functions " << endl;
    AliFemtoCorrFctn* fctn = (*iter)->Clone();
    if (fctn) AddCorrFctn(fctn);
    else cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - correlation function not found " << endl;
  }

  fNumEventsToMix = a.fNumEventsToMix;

  cout << " AliFemtoVertexMultAnalysis::AliFemtoVertexMultAnalysis(const AliFemtoVertexMultAnalysis& a) - analysis copied " << endl;

}
//____________________________
AliFemtoVertexMultAnalysis::~AliFemtoVertexMultAnalysis(){
  // now delete every PicoEvent in the EventMixingBuffer and then the Buffer itself
  delete fPicoEventCollectionVectorHideAway;
}

//____________________________
AliFemtoString AliFemtoVertexMultAnalysis::Report()
{
  cout << "AliFemtoVertexMultAnalysis - constructing Report..."<<endl;
  char Ctemp[200];
  AliFemtoString temp = "-----------\nHbt AliFemtoVertexMultAnalysis Report:\n";
  sprintf(Ctemp,"Events are mixed in %d VertexZ bins in the range %E cm to %E cm.\n",fVertexZBins,fVertexZ[0],fVertexZ[1]);
  temp += Ctemp;
  sprintf(Ctemp,"Events underflowing: %d\n",fUnderFlowVertexZ);
  temp += Ctemp;
  sprintf(Ctemp,"Events overflowing: %d\n",fOverFlowVertexZ);
  temp += Ctemp;
  sprintf(Ctemp,"Events are mixed in %d Mult bins in the range %E cm to %E cm.\n",fMultBins,fMult[0],fMult[1]);
  temp += Ctemp;
  sprintf(Ctemp,"Events underflowing: %d\n",fUnderFlowMult);
  temp += Ctemp;
  sprintf(Ctemp,"Events overflowing: %d\n",fOverFlowMult);
  temp += Ctemp;
  sprintf(Ctemp,"Now adding AliFemtoAnalysis(base) Report\n");
  temp += Ctemp;
  temp += AliFemtoAnalysis::Report();
  AliFemtoString returnThis=temp;
  return returnThis;
}
//_________________________
void AliFemtoVertexMultAnalysis::ProcessEvent(const AliFemtoEvent* hbtEvent) {
  cout << " AliFemtoVertexMultAnalysis::ProcessEvent(const AliFemtoEvent* hbtEvent) " << endl;
  // get right mixing buffer
  double vertexZ = hbtEvent->PrimVertPos().z();
  double mult = hbtEvent->UncorrectedNumberOfPrimaries();
  fMixingBuffer = fPicoEventCollectionVectorHideAway->PicoEventCollection(vertexZ,mult); 
  if (!fMixingBuffer) {
    if ( vertexZ < fVertexZ[0] ) fUnderFlowVertexZ++;
    if ( vertexZ > fVertexZ[1] ) fOverFlowVertexZ++;
    if ( mult < fMult[0] ) fUnderFlowMult++;
    if ( mult > fMult[1] ) fOverFlowMult++;
    return;
  }
  // call ProcessEvent() from AliFemtoAnalysis-base
  AliFemtoAnalysis::ProcessEvent(hbtEvent);
}

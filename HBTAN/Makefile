############################### HBTAnalysis Makefile ##################################

# Include machine specific definitions

include $(ALICE_ROOT)/conf/GeneralDef
include $(ALICE_ROOT)/conf/MachineDef.$(ALICE_TARGET)


OPT = -g
PACKAGE = HBTAnalysis

# C++ sources

SRCS          = AliHBTAnalysis.cxx    AliHBTPair.cxx  AliHBTReaderPPprod.cxx \
                AliHBTReader.cxx      AliHBTReaderTPC.cxx \
                AliHBTCorrelFctn.cxx  AliHBTPairCut.cxx \
                AliHBTEvent.cxx       AliHBTParticle.cxx       AliHBTRun.cxx \
                AliHBTFunction.cxx    AliHBTMonitorFunction.cxx   AliHBTParticleCut.cxx  \
                AliHBTReaderITSv1.cxx AliHBTReaderITSv2.cxx    AliHBTReaderKineTree.cxx \
                AliHBTTwoTrackEffFctn.cxx       AliHBTReaderInternal.cxx\
                AliHBTQResolutionFctns.cxx      AliHBTQDistributionFctns.cxx    \
                AliHBTLLWeights.cxx             AliHBTLLWeightFctn.cxx\
                AliHBTMonDistributionFctns.cxx     AliHBTMonResolutionFctns.cxx \
                AliHBTLLWeightsPID.cxx AliHBTLLWeightTheorFctn.cxx

# Fortran sources

FSRCS	       = fsiini.F fsiw.F led_bldata.F ltran12.F

# C++ Headers

HDRS          = $(SRCS:.cxx=.h) HBTAnalysisLinkDef.h

# Library dictionary

DICT          = HBTAnalysisCint.cxx
DICTH         = $(DICT:.cxx=.h)
DICTO         = $(patsubst %.cxx,tgt_$(ALICE_TARGET)/%.o,$(DICT))

# FORTRAN Objectrs

FOBJS         = $(patsubst %.F,tgt_$(ALICE_TARGET)/%.o,$(FSRCS))

# C Objects

COBJS         = $(patsubst %.c,tgt_$(ALICE_TARGET)/%.o,$(CSRCS))

# C++ Objects

OBJS          = $(patsubst %.cxx,tgt_$(ALICE_TARGET)/%.o,$(SRCS)) $(DICTO)

# C++ compilation flags

INCLUDES      = -I$(ALICE_ROOT)/TPC/ -I$(ALICE_ROOT)/CONTAINERS/ -I$(ALICE_ROOT)/ITS/

CXXFLAGS      = $(CXXOPTS) -I$(ROOTSYS)/include -I. -I$(ALICE_ROOT)/include/ $(INCLUDES)
 
# FORTRAN compilation flags

FFLAGS      = $(FOPT) -I$(ALICE_ROOT)/GEANT321

##### TARGETS #####
 
# Target

SLIBRARY	= $(LIBDIR)/libHBTAN.$(SL)
ALIBRARY	= $(LIBDIR)/libHBTAN.a

default:	$(SLIBRARY)


$(LIBDIR)/libHBTAN.$(SL):  $(OBJS) $(FOBJS)

$(DICT):  $(HDRS)

DEPINC += -I$(ALICE_ROOT)/GEANT321

depend:				$(SRCS) $(FSRCS)

TOCLEAN		= $(OBJS) $(FOBJS) *Cint.cxx *Cint.h

CHECKS        = $(patsubst %.cxx,check/%.viol,$(SRCS))

############################### General Macros ################################

include $(ALICE_ROOT)/conf/GeneralMacros

############################ Dependencies #####################################

-include tgt_$(ALICE_TARGET)/Make-depend 



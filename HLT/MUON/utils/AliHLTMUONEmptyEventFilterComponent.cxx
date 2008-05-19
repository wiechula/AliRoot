/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Artur Szostak <artursz@iafrica.com>                                  *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///
/// @file   AliHLTMUONEmptyEventFilterComponent.cxx
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   2007-12-12
/// @brief  Implementation of the empty event filter component.
///
/// This component is used to forward events for where there is at least one
/// non-empty dHLT data block.
///

#include "AliHLTMUONEmptyEventFilterComponent.h"
#include "AliHLTMUONConstants.h"
#include "AliHLTLogging.h"
#include "AliHLTSystem.h"
#include "AliHLTDefinitions.h"
#include <cstdlib>
#include <cerrno>
#include <cassert>

ClassImp(AliHLTMUONEmptyEventFilterComponent)


AliHLTMUONEmptyEventFilterComponent::AliHLTMUONEmptyEventFilterComponent() :
	AliHLTMUONProcessor(),
	fSendOnEmpty(false)
{
	///
	/// Default constructor.
	///
}


AliHLTMUONEmptyEventFilterComponent::~AliHLTMUONEmptyEventFilterComponent()
{
	///
	/// Default destructor.
	///
}

const char* AliHLTMUONEmptyEventFilterComponent::GetComponentID()
{
	///
	/// Inherited from AliHLTComponent. Returns the component ID.
	///
	
	return AliHLTMUONConstants::EmptyEventFilterComponentId();
}


void AliHLTMUONEmptyEventFilterComponent::GetInputDataTypes( std::vector<AliHLTComponentDataType>& list)
{
	///
	/// Inherited from AliHLTProcessor. Returns the list of expected input data types.
	/// At the moment this list is "any data type" since it is not known before
	/// hand what kind of input blocks we will get.
	
	assert( list.empty() );
	list.push_back( kAliHLTAnyDataType );
}


AliHLTComponentDataType AliHLTMUONEmptyEventFilterComponent::GetOutputDataType()
{
	///
	/// Inherited from AliHLTComponent. Returns the output data type of
	/// "any data type" with MUON origin.
	
	return kAliHLTAnyDataType | kAliHLTDataOriginMUON;
}


void AliHLTMUONEmptyEventFilterComponent::GetOutputDataSize(
		unsigned long& constBase, double& inputMultiplier
	)
{
	///
	/// Inherited from AliHLTComponent.
	/// Returns an estimate of the expected output data size.
	
	// Both of these are zero because we will only ever pass on input data blocks
	// and never generate data in this component.
	constBase = 0;
	inputMultiplier = 0;
}


AliHLTComponent* AliHLTMUONEmptyEventFilterComponent::Spawn()
{
	///
	/// Inherited from AliHLTComponent. Creates a new object instance.
	///
	
	return new AliHLTMUONEmptyEventFilterComponent;
}


int AliHLTMUONEmptyEventFilterComponent::DoInit(int argc, const char** argv)
{
	///
	/// Inherited from AliHLTComponent.
	/// Parses the command line parameters and initialises the component.
	///

	fSendOnEmpty = false;  // Set to the default value.

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-sendempty") == 0)
		{
			fSendOnEmpty = true;
			HLTInfo("Turning on anti-filtering. Will be passing all data on empty dHLT results.");
			continue;
		}
		
		HLTError("Unknown option '%s'.", argv[i]);
		return EINVAL;
	}

	return 0;
}


int AliHLTMUONEmptyEventFilterComponent::DoDeinit()
{
	///
	/// Inherited from AliHLTComponent. Performs a cleanup of the component.
	///
  
	return 0;
}


int AliHLTMUONEmptyEventFilterComponent::DoEvent(
		const AliHLTComponentEventData& evtData,
		const AliHLTComponentBlockData* blocks,
		AliHLTComponentTriggerData& /*trigData*/,
		AliHLTUInt8_t* /*outputPtr*/,
		AliHLTUInt32_t& size,
		std::vector<AliHLTComponentBlockData>& outputBlocks
	)
{
	/// Inherited from AliHLTProcessor. Processes the new event data.
	/// Here we go through the list of input data blocks looking for blocks
	/// containing dHLT results. If all of these blocks are empty then we
	/// mark the event for filtering.
	/// What we actually do with the whole event will depend on the fSendOnEmpty
	/// flag. If it is set to false (the default) then we will copy all the
	/// input data blocks to output if the dHLT results were NOT empty.
	/// If fSendOnEmpty is true then we will copy all the input data blocks
	/// to the output if the dHLT results ARE empty.
	
	HLTDebug("Processing event %llu with %u input data blocks.",
		evtData.fEventID, evtData.fBlockCnt
	);

	bool emptyEvent = true;

	for (AliHLTUInt32_t n = 0; n < evtData.fBlockCnt; n++)
	{
		HLTDebug("Handling block: %u, with fDataType = '%s', fPtr = %p and fSize = %u bytes.",
			i, DataType2Text(blocks[n].fDataType).c_str(), blocks[n].fPtr, blocks[n].fSize
		);

		if (blocks[n].fDataType == AliHLTMUONConstants::TriggerRecordsBlockDataType())
		{
			AliHLTMUONTriggerRecordsBlockReader inblock(blocks[n].fPtr, blocks[n].fSize);
			if (not BlockStructureOk(inblock)) continue;
			if (inblock.Nentries() != 0) emptyEvent = false;
		}
		else if (blocks[n].fDataType == AliHLTMUONConstants::RecHitsBlockDataType())
		{
			AliHLTMUONRecHitsBlockReader inblock(blocks[n].fPtr, blocks[n].fSize);
			if (not BlockStructureOk(inblock)) continue;
			if (inblock.Nentries() != 0) emptyEvent = false;
		}
		else if (blocks[n].fDataType == AliHLTMUONConstants::MansoTracksBlockDataType())
		{
			AliHLTMUONMansoTracksBlockReader inblock(blocks[n].fPtr, blocks[n].fSize);
			if (not BlockStructureOk(inblock)) continue;
			if (inblock.Nentries() != 0) emptyEvent = false;
		}
		else if (blocks[n].fDataType == AliHLTMUONConstants::SinglesDecisionBlockDataType())
		{
			AliHLTMUONSinglesDecisionBlockReader inblock(blocks[n].fPtr, blocks[n].fSize);
			if (not BlockStructureOk(inblock)) continue;
			if (inblock.Nentries() != 0) emptyEvent = false;
		}
		else if (blocks[n].fDataType == AliHLTMUONConstants::PairsDecisionBlockDataType())
		{
			AliHLTMUONPairsDecisionBlockReader inblock(blocks[n].fPtr, blocks[n].fSize);
			if (not BlockStructureOk(inblock)) continue;
			if (inblock.Nentries() != 0) emptyEvent = false;
		}
	}

	// If we are filtering or required to send only empty events then
	// copy all the input blocks to the output.
	if (emptyEvent and fSendOnEmpty or not emptyEvent and not fSendOnEmpty)
	{
		for (AliHLTUInt32_t n = 0; n < evtData.fBlockCnt; n++)
		{
			outputBlocks.push_back(blocks[n]);
		}
	}

	// Finally we set the total size of output memory we consumed which is
	// zero since we just copied the input descriptors to output if anything.
	size = 0;
	return 0;
}


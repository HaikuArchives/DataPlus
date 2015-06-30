/* ------------------------------------------------------------------ 

	Title: DPOldDataFile

	Description:  Class for reading and writing DataPlus files
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 March, 2001
			Created
		
------------------------------------------------------------------ */ 
#include "DPOldDataFile.h"

#include <InterfaceKit.h>

const uint32 version_2				= 'DFv2';

// File structure for data sets
const off_t		defSize_versKey	= sizeof(uint32);
const off_t		defSize_numSets	= sizeof(uint32);

// Offsets to the data in the file
const off_t		indx_versKey		= 0;	// First item is the key
const off_t		indx_numSets		= indx_versKey + defSize_versKey;
const off_t		indx_recs			= indx_numSets + sizeof(off_t);
const off_t		indx_data			= indx_recs + sizeof(off_t)*max_data_sets;

// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPOldDataFile::DPOldDataFile(const char* filePath):BFile(filePath, B_READ_WRITE | B_CREATE_FILE)
{
	mNumSets = 0;
	mVersion = 0;
	for(int n = 0; n < max_data_sets; n++) {
		mRecIndx[n] = -1;
	}
}		
// -------------------------------------------------------------------
DPOldDataFile::DPOldDataFile(const entry_ref* theRef):BFile(theRef, B_READ_WRITE | B_CREATE_FILE)
{
	mNumSets = 0;
	mVersion = 0;
	for(int n = 0; n < max_data_sets; n++) {
		mRecIndx[n] = -1;
	}
}		
// -------------------------------------------------------------------
DPOldDataFile::~DPOldDataFile()
{
}
		
// -------------------------------------------------------------------
// *  Public methods             
// -------------------------------------------------------------------
status_t
DPOldDataFile::AddData(const DPData* theData, bool deep)
{
	if(mNumSets >= max_data_sets) {
		return B_ERROR;
	}
	mNumSets++;
	off_t posn;
	ssize_t bytes;
	posn = Seek(indx_versKey,SEEK_SET);
	bytes = Write(&mVersion,sizeof(mVersion));
	posn = Seek(indx_numSets,SEEK_SET);
	bytes = Write(&mNumSets,sizeof(mNumSets));
	int16 index = mNumSets - 1;
	
	// Append data to end of file and save the starting position
	off_t start;
	GetSize(&start);
	// Save starting posn in index and in file
	mRecIndx[index] = start;
	posn = Seek(indx_recs + sizeof(off_t)*index,SEEK_SET);
	bytes = Write(&start,sizeof(start));	// Store starting location
	// Move to end of file
	posn = Seek(start,SEEK_SET);
	theData->Archive(this,deep);
	return B_OK;
}		
// -------------------------------------------------------------------
void
DPOldDataFile::Clear()
{
	mNumSets = 0;
	mVersion = version_2;
	off_t posn;
	ssize_t bytes;
	posn = Seek(indx_versKey,SEEK_SET);
	bytes = Write(&mVersion,sizeof(mVersion));
	posn = Seek(indx_numSets,SEEK_SET);
	bytes = Write(&mNumSets,sizeof(mNumSets));
	posn = Seek(indx_recs,SEEK_SET);
	for(int n = 0; n < max_data_sets; n++) {
		mRecIndx[n] = -1;
		bytes = Write(&mRecIndx[n],sizeof(off_t));
	}
	SetSize(indx_data);
}
// -------------------------------------------------------------------
DPData*
DPOldDataFile::GetSet(int16 setNum)
{
	if(setNum >= 0 && setNum < mNumSets) {
		if(Seek(mRecIndx[setNum],SEEK_SET) != B_ERROR) {
			return new DPData(this);
		}
	}
	return NULL;
}
// -------------------------------------------------------------------
status_t
DPOldDataFile::InitCheck()
{
	status_t status = BFile::InitCheck();
	if(status == B_OK) {
		off_t posn;
		ssize_t bytes;
		posn = Seek(indx_versKey,SEEK_SET);
		bytes = Read(&mVersion,sizeof(mVersion));
		if(mVersion == version_2) {
			ReadData();
			status = B_OK;
		} else {
			status = file_version_error;
		}
	}
	return status;
}
// -------------------------------------------------------------------
void
DPOldDataFile::ShowFileAlert(status_t error)
{
	if(error == file_version_error) {
		BAlert* theAlert = new BAlert("alert","File Version Not Compatable","OK");
		theAlert->Go();
		
	} else if(error == file_init_error) {
		BAlert* theAlert = new BAlert("alert","File Can't Be Initialized","OK");
		theAlert->Go();
	}
}
// -------------------------------------------------------------------
// *  Protected methods             
// -------------------------------------------------------------------
void
DPOldDataFile::ReadData()
{
	off_t posn;
	ssize_t bytes;
	posn = Seek(indx_numSets,SEEK_SET);
	bytes = Read(&mNumSets,sizeof(mNumSets));
	posn = Seek(indx_recs,SEEK_SET);
	for(int n = 0; n < mNumSets; n++) {
		bytes = Read(&mRecIndx[n],sizeof(off_t));
	}
}
// -------------------------------------------------------------------

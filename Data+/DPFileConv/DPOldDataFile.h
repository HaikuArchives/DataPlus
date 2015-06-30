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
#ifndef _DPDataFile
#define _DPDataFile

#include "DPData.h"
#include "DPGlobals.h"
#include <SupportKit.h>
#include <StorageKit.h>

const status_t file_version_error	= -2;
const status_t file_init_error		= -3;

class DPOldDataFile : public BFile {

public:
		
		DPOldDataFile(const char* filePath);
		
		DPOldDataFile(const entry_ref* theRef);
		
	virtual
		~DPOldDataFile();
		
	status_t	// Adds a set of data to be saved
		AddData(const DPData* theData, bool deep);
		
	void
		Clear();
		
	DPData*
		GetSet(int16 setNum);

	virtual status_t
		InitCheck();

	int16
		NumSets() {return mNumSets;}
				
	void
		ShowFileAlert(status_t error); 
		
	protected :	// Methods
	
	void
		ReadData();
	
	protected:	// Data
		int16 	mNumSets;
		uint32	mVersion;
		off_t 	mRecIndx[max_data_sets];
};
#endif


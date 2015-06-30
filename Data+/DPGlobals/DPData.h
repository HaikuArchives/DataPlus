/* ------------------------------------------------------------------ 

	Title: DPData

	Description:  Data structures and handling for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 March, 2001
			Added file archiving
		16 August, 1999
			Created 
		
------------------------------------------------------------------ */ 

#ifndef _DPData
#define _DPData

#include "DPDataRec.h"
#include "DPDataType.h"

#include <SupportKit.h>

// String constants for archiving
const char data_type_str[8]	= "dptypes";
const char data_rec_str[8]		= "dprecs";

class DPData : public BArchivable {

	public :
		enum {
			fld_code = 'DPda',
			max_set_name = 31
		};
		
	static BArchivable*
		Instantiate(BMessage* theArchive);
	
		DPData();
		
		DPData(BMessage* theArchive);
		
		DPData(BDataIO* theDataObj);
		
		DPData(const DPData &theData);
		
	virtual
		~DPData();
		
	void
		AddField(DPDataType* theFld);
		
	void
		AddRecord(DPDataRec* theRec);
		
	status_t
		Archive(BMessage* theArchive, bool deep = true) const;
		
	status_t
		Archive(BDataIO* theDataObj, bool deep = true) const;
		
	void
		ClearStatus(EDataStatus dStatus);
		
	DPDataRec*
		CurrentRec() const {return mCurrRec;}
		
	DPDataRec*
		DataRecord(int32 recNum) const {return (DPDataRec*)mData->ItemAt(recNum);}
		
	DPDataType*
		FieldType(int16 fldNum) const {return (DPDataType*)mFieldTypes->ItemAt(fldNum);}
		
	bool	// Find a record by its key
		FindRecord(record_key recKey);	
		
	bool	// Find a record by its name and index
		FindRecord(const char* recName, int16 index = 0);
		
	bool	// Find a record by its sequence
		FindRecord(int32 recNum);
		
	bool
		HasMarkedData() const {return mHasMarkedData;}
	bool
		HasSuppressedData() const {return mHasSuppressedData;}
	bool
		HasDeletedData() const {return mHasDeletedData;}
		
	record_key
		Key() const {return mKey;}
		
	void
		MakeTitleString(BString* theString, bool fileTitleToo) const;
		
	void
		MarkData(BMessage* theMesg);
		
	int32
		NumFields() const {return mFieldTypes->CountItems();}
		
	int32
		NumRecords() const {return mData->CountItems();}
		
	void
		SetSetName(const char* name) {::strncpy(mSetName,name,max_set_name);}
		
	const char*
		SetName() const {return mSetName;}
		
	void
		SetXYZFields(int16 xFld, int16 yFld, int16 zFld);
		
	void
		SortRecords(int (*compareFunc)(const void *, const void *)) {mData->SortItems(compareFunc);}
		
	int16
		XField() const {return mXField;}
	int16
		YField() const {return mYField;}
	int16
		ZField() const {return mZField;}
	
	// All these operate off of the 'current' record	
	bool
		XValue(float &val) const;
	bool
		YValue(float &val) const;
	bool
		ZValue(float &val) const;
		
	protected :	// Methods
	void
		Initialize();
		
	protected :	// Data
		char		mSetName[max_set_name + 1];
		record_key	mKey;
		record_key	mMaxKey;
		record_key	mMinKey;
		DPDataRec*	mCurrRec;
		BList*		mFieldTypes;
		BList*		mData;
		int16		mXField;
		int16		mYField;
		int16		mZField;
		int16		mNumFields;
		bool		mHasMarkedData;
		bool		mHasSuppressedData;
		bool		mHasDeletedData;
};

#endif

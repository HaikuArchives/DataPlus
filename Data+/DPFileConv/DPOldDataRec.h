/* ------------------------------------------------------------------ 

	Title: DPOldDataRec

	Description:  The data records for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 August, 1999
			Modified as a BArchivable
		16 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPOldDataRec
#define _DPOldDataRec

#include <SupportKit.h>
#include "DPData.h"


class DPOldDataRec : public BArchivable {

	public :
		enum {
			fld_code = 'DPDr',
			max_fld_name = 15
		};
	
	static record_key
		MakeKey();
		
	static BArchivable*
		Instantiate(BMessage* theArchive);
		
		DPOldDataRec(BMessage* theArchive);
		
		DPOldDataRec(DPOldDataRec* theRec);
		
		DPOldDataRec(DPOldDataRec* theRec, int16 xFld, int16 yFld, int16 zFld);
		
		DPOldDataRec(int16 numFlds);
		
	virtual
		~DPOldDataRec();
		
	status_t
		Archive(BMessage* theArchive, bool deep = true) const;
		
	void
		AddField();
		
	bool
		CanUseFld(int16 fldNum);
		
	EDataStatus
		DataStatus() const {return mStatus;}
		
	record_key
		Key() {return mKey;}
		
	rgb_color
		MarkColor() {return mMarkColor;}
		
	int16
		NumFields() {return mNumFlds;}
		
	const char*
		RecordName() {return (const char*)mRecName;}
		
	void
		SetDataStatus(EDataStatus theStatus) {mStatus = theStatus;}
		
	void
		SetFieldValue(int16 fldNum, float val);
		
	void
		SetFieldValue(int16 fldNum, int16 val);
		
	void
		SetMarkColor(rgb_color color) {mMarkColor = color;}
		
	void
		SetRecordName(const char* rName);

	void
		SetUse(int16 fldNum, bool use);
		
	bool
		ValidField(int16 fldNum);
		
	bool
		Value(int16 fldNum, bigtime_t &val) const;

	bool
		Value(int16 fldNum, float &val) const;
		
	bool
		Value(int16 fldNum, int16 &val) const;

	union UFldData {
		bigtime_t	timeVal;
		float			realVal;
		int16			intVal;	
		char			charVal;
		bool			boolVal;
	};

	protected :	// Data
		record_key	mKey;
		rgb_color	mMarkColor;
		char 			mRecName[max_fld_name + 1];
		int16			mNumFlds;
		UFldData*	mData;
		EDataStatus	mStatus;
		bool*			mUseFld;
		bool*			mFldValid;
};

#endif

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

#include "DPOldDataRec.h"
#include "DPGlobals.h"
#include "WColorControl.h"
#include "U_Conversions.h"


const char str_rec_key[8] 		= "drkey";
const char str_rec_name[8]		= "drname";
const char str_rec_numflds[8]	= "drnumf";
const char str_rec_data[8]		= "drdata";
const char str_rec_valid[8]	= "drvalid";
const char str_rec_status[8]	= "drstat";
const char str_rec_mkColor[8]	= "drmkcol";

// -------------------------------------------------------------------
// *  Static Methods             
// -------------------------------------------------------------------
// Ensure a unique key based on the system clock.
// **** This should eventually set up to be thread-safe ****
record_key
DPOldDataRec::MakeKey()
{
	record_key newKey = ::real_time_clock_usecs();
	while(newKey == 0) {
		snooze(5);
		newKey = ::real_time_clock_usecs();
	}
	return newKey;
}

BArchivable*
DPOldDataRec::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPDataRec")) {
		return new DPOldDataRec(theArchive); 
	} else {
		return NULL;
	}
}
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPOldDataRec::DPOldDataRec(int16 numFlds)
{
	mKey = MakeKey();
	mRecName[0] = 0;
	mNumFlds = numFlds;
	mData = new UFldData[numFlds];
	mUseFld = new bool[numFlds];
	mFldValid = new bool[numFlds];
	for(int n = 0; n < numFlds; n++) {
		mData[n].realVal = 0.0;
		mUseFld[n] = false;
		mFldValid[n] = false;
	}
	mStatus = data_normal;
	mMarkColor = color_red;
}

// Creation from a BMessage
DPOldDataRec::DPOldDataRec(BMessage* theArchive)
{
	const char* rName;
	theArchive->FindInt64(str_rec_key,&mKey);
	mRecName[0] = 0;
	theArchive->FindString(str_rec_name,&rName);
	if(rName != NULL) {
		::strcpy(mRecName,rName);
	} 
	theArchive->FindInt16(str_rec_numflds,&mNumFlds);
	mData = new UFldData[mNumFlds];
	mUseFld = new bool[mNumFlds];
	mFldValid = new bool[mNumFlds];
	
	int16 sval;
	if(theArchive->FindInt16(str_rec_status,&sval) == B_OK) {
		mStatus = (EDataStatus)sval;
	} else {
		mStatus = data_normal;
	}
	
	int32 cVal;
	if(theArchive->FindInt32(str_rec_mkColor,&cVal) == B_OK) {
		mMarkColor = ValueToColor(cVal);
	} else {
		mMarkColor = color_red;
	}
	
	ssize_t bytes;
	const void* theFld;
	for(int n = 0; n < mNumFlds; n++) {
		theArchive->FindData(str_rec_data,B_ANY_TYPE,n,&theFld,&bytes);
		::memcpy(&mData[n],theFld,sizeof(UFldData));
		theArchive->FindBool(str_rec_valid,n,&mFldValid[n]);
		mUseFld[n] = mFldValid[n];
	}
}

// Deep copy constructor
DPOldDataRec::DPOldDataRec(DPOldDataRec* theRec)
{
	mKey = theRec->mKey;
	mStatus = theRec->mStatus;
	mMarkColor = theRec->mMarkColor;
	mStatus = theRec->mStatus;
	mNumFlds = theRec->mNumFlds;
	mData = new UFldData[mNumFlds];
	mUseFld = new bool[mNumFlds];
	mFldValid = new bool[mNumFlds];
	for(int n = 0; n < mNumFlds; n++) {
		mData[n] = theRec->mData[n];
		mUseFld[n] = theRec->mUseFld[n];
		mFldValid[n] = theRec->mFldValid[n];
	}
	::strcpy(mRecName,theRec->mRecName);
}

// Shallow copy constructor		
DPOldDataRec::DPOldDataRec(DPOldDataRec* theRec, int16 xFld, int16 yFld, int16 zFld)
{
	mKey = theRec->mKey;
	mStatus = theRec->mStatus;
	mMarkColor = color_red;
	::strcpy(mRecName,theRec->mRecName);
	
	mNumFlds = 0;
	if(xFld >= 0) mNumFlds++ ;
	if(yFld >= 0) mNumFlds++ ;
	if(zFld >= 0) mNumFlds++ ;
	mData = new UFldData[mNumFlds];
	mUseFld = new bool[mNumFlds];
	mFldValid = new bool[mNumFlds];
	
	// Add any valid fields in x,y,z order
	int16 fNum = -1;
	if(xFld >= 0) {
		fNum++;
		mData[fNum] = theRec->mData[xFld];
		mFldValid[fNum] = mUseFld[fNum] = theRec->mFldValid[xFld];
	}
	if(yFld >= 0) {
		fNum++;
		mData[fNum] = theRec->mData[yFld];
		mFldValid[fNum] = mUseFld[fNum] = theRec->mFldValid[yFld];
	}
	if(zFld >= 0) {
		fNum++;
		mData[fNum] = theRec->mData[zFld];
		mFldValid[fNum] = mUseFld[fNum] = theRec->mFldValid[zFld];
	}
}

DPOldDataRec::~DPOldDataRec()
{
	delete mData;
	delete mUseFld;
	delete mFldValid;
}		
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPOldDataRec::AddField()
{
	mNumFlds++;
	UFldData* oldData = mData;
	mData = new UFldData[mNumFlds];
	bool* oldUse = mUseFld;
	mUseFld = new bool[mNumFlds];
	bool* oldValid = mFldValid;
	mFldValid = new bool[mNumFlds];
	for(int16 fld = 0; fld < (mNumFlds - 1); fld++) {
		mData[fld] = oldData[fld];
		mUseFld[fld] = oldUse[fld];
		mFldValid[fld] = oldValid[fld];
	}
	delete oldData;
	delete oldUse;
	delete oldData;
	SetFieldValue(mNumFlds - 1,(float)0.0);
	mFldValid[mNumFlds - 1] = false;
	mUseFld[mNumFlds - 1] = false;
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
status_t
DPOldDataRec::Archive(BMessage* theArchive, bool deep) const
{
	theArchive->AddString("class","DPDataRec");
	theArchive->AddInt64(str_rec_key,mKey);
	theArchive->AddString(str_rec_name,mRecName);
	theArchive->AddInt16(str_rec_numflds,mNumFlds);
	theArchive->AddInt16(str_rec_status,mStatus);
	theArchive->AddInt32(str_rec_mkColor,ColorToValue(mMarkColor));
	for(int n = 0; n < mNumFlds; n++) {
		theArchive->AddData(str_rec_data,B_ANY_TYPE,&mData[n],sizeof(UFldData));
		theArchive->AddBool(str_rec_valid,mFldValid[n]);
	}
	return B_OK;
}
// -------------------------------------------------------------------
bool
DPOldDataRec::CanUseFld(int16 fldNum)
{
	return fldNum >= 0 && fldNum < mNumFlds && mUseFld[fldNum];
}		
// -------------------------------------------------------------------
void
DPOldDataRec::SetFieldValue(int16 fldNum, float val)
{
	if(fldNum >= 0 && fldNum < mNumFlds) {
		mData[fldNum].realVal = val;
		mFldValid[fldNum] = true;
		mUseFld[fldNum] = true;
	}
}		
// -------------------------------------------------------------------
void
DPOldDataRec::SetFieldValue(int16 fldNum, int16 val)
{
	if(fldNum >= 0 && fldNum < mNumFlds && mFldValid[fldNum]) {
		mData[fldNum].intVal = val;
		mFldValid[fldNum] = true;
		mUseFld[fldNum]	 = true;
	}
}
// -------------------------------------------------------------------
void
DPOldDataRec::SetUse(int16 fldNum, bool use)
{
	if(fldNum >= 0 && fldNum < mNumFlds && mFldValid[fldNum]) {
		mUseFld[fldNum] = use;
	}
}
// -------------------------------------------------------------------
void
DPOldDataRec::SetRecordName(const char* rName)
{
	::strncpy(mRecName,rName,max_fld_name);	//******************
}
// -------------------------------------------------------------------
bool
DPOldDataRec::ValidField(int16 fldNum)
{
	if(fldNum >= 0 && fldNum < mNumFlds) {
		return mFldValid[fldNum];
	}
	return false;
}		
// -------------------------------------------------------------------
bool
DPOldDataRec::Value(int16 fldNum, float &val) const
{
	if(fldNum >= 0 && fldNum < mNumFlds) {
		val = mData[fldNum].realVal;
		return mUseFld[fldNum];
	}
	val = 0.0;
	return false;
}	
// -------------------------------------------------------------------
bool
DPOldDataRec::Value(int16 fldNum, int16 &val) const
{
	if(fldNum >= 0 && fldNum < mNumFlds) {
		val = mData[fldNum].intVal;
		return mUseFld[fldNum];
	}
	val = 0;
	return false;
}	
// -------------------------------------------------------------------

/* ------------------------------------------------------------------ 

	Title: DPDataRec

	Description:  The data records for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 March, 2001
			Added file archiving and changed data structure
		26 August, 1999
			Modified as a BArchivable
		16 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DPDataRec.h"
#include "DPGlobals.h"
#include "WColorControl.h"
#include "U_Conversions.h"
#include "UStringFmt_Utils.h"

record_key lastKey = 0;
const int32	key_sem	= 'Ksem';

const char str_rec_data[8] 		= "drdata";
const char str_rec_fields[8]	= "drdflds";

// -------------------------------------------------------------------
// *  Static Methods             
// -------------------------------------------------------------------
// Ensure a unique key based on the system clock.
// Since this static, we need a semaphore..
record_key
DPDataRec::MakeKey()
{
	acquire_sem(key_sem);
	record_key newKey = ::real_time_clock_usecs();
	while(newKey == lastKey) {
		snooze(5);
		newKey = ::real_time_clock_usecs();
	}
	lastKey = newKey;
	release_sem(key_sem);
	return newKey;
}

BArchivable*
DPDataRec::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPDataRec")) {
		return new DPDataRec(theArchive); 
	} else {
		return NULL;
	}
}
DPDataRec*	// Static creator from file
DPDataRec::Instantiate(BDataIO* theDataObj)
{
	DPDataRec* theRec = new DPDataRec(theDataObj);
	if(!theRec->InitCheck()) {
		delete theRec;
		return NULL;
	}
	return theRec;
}
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPDataRec::DPDataRec(int16 numFlds)
{
	Initialize();
	mData.numFlds = numFlds;
	mData.valid = (mData.numFlds > 0);
	if(mData.valid) {
		mData.data= new SRecFld[numFlds];
		for(int n = 0; n < numFlds; n++) {
			mData.data[n].fldData.realVal = 0.0;
			mData.data[n].fldValid = false;
			mData.data[n].useFld = false;
		}
	} else {
		mData.data = NULL;
	}
	mData.status = data_normal;
	mData.markColor = color_red;
}
// -------------------------------------------------------------------
// Creation from a BMessage
DPDataRec::DPDataRec(BMessage* theArchive)
{
	Initialize();

	ssize_t bytes;
	const void* theData;
	theArchive->FindData(str_rec_data,B_ANY_TYPE,&theData,&bytes);
	if(bytes == sizeof(mData)) {
		::memcpy(&mData,theData,sizeof(SRecData));
	}
	mData.valid = (mData.numFlds > 0);
	if(mData.valid) {
		mData.data= new SRecFld[mData.numFlds];
		
		for(int n = 0; n < mData.numFlds; n++) {
			theArchive->FindData(str_rec_fields,B_ANY_TYPE,n,&theData,&bytes);
			if(bytes == sizeof(SRecFld)) {
				::memcpy(&mData.data[n],theData,sizeof(SRecFld));
			} else {
				mData.data[n].fldData.realVal = 0.0;
				mData.data[n].fldValid = false;
				mData.data[n].useFld = false;
			}
		}
	}
}
// -------------------------------------------------------------------
// Creation from a BDataObject
DPDataRec::DPDataRec(BDataIO* theDataObj)
{
	Initialize();
	uint32 signature;
	ssize_t bytesRead = theDataObj->Read(&signature, sizeof(signature));
	if(signature == fld_code) {	// If a valid field...
		bytesRead = theDataObj->Read(&mData,sizeof(mData));
		mData.valid = (mData.numFlds > 0);
		if(mData.valid) {
			ssize_t size = sizeof(SRecFld)*mData.numFlds;
			mData.data= new SRecFld[mData.numFlds];
			bytesRead = theDataObj->Read(&mData.data[0],size);
		} else {
			mData.data = NULL;
			BFile* theFile = cast_as(theDataObj,BFile);
			off_t posn = theFile->Position();
			theFile->Seek(posn - sizeof(signature),SEEK_SET);
		}
	}
}
// -------------------------------------------------------------------
// Deep copy constructor
DPDataRec::DPDataRec(const DPDataRec &theRec)
{
	Initialize();
	const SRecData* theData = theRec.GetData();
	mData.key = theData->key;
	mData.markColor = theData->markColor;
	::memcpy(mData.recName,theData->recName,max_fld_name + 1);
	mData.status = theData->status;
	mData.numFlds = theData->numFlds;
	
	mData.valid = (mData.numFlds > 0);
	if(mData.valid) {
		mData.data= new SRecFld[mData.numFlds];
		for(int n = 0; n < mData.numFlds; n++) {
			mData.data[n] = theData->data[n];
		}
	}
}
// -------------------------------------------------------------------
// Shallow copy constructor		
DPDataRec::DPDataRec(const DPDataRec &theRec, int16 xFld, int16 yFld, int16 zFld)
{
	Initialize();
	mData.key = theRec.mData.key;
	mData.status = theRec.mData.status;
	mData.markColor = color_red;
	::strcpy(mData.recName,theRec.mData.recName);
	
	if(xFld >= 0) mData.numFlds++ ;
	if(yFld >= 0) mData.numFlds++ ;
	if(zFld >= 0) mData.numFlds++ ;
	mData.valid = (mData.numFlds > 0);
	if(mData.valid) {
		mData.data= new SRecFld[mData.numFlds];
		// Add any valid fields in x,y,z order
		int16 fNum = 0;
		if(xFld >= 0) {
			mData.data[fNum++] = theRec.mData.data[xFld];
		}
		if(yFld >= 0) {
			mData.data[fNum++] = theRec.mData.data[yFld];
		}
		if(zFld >= 0) {
			mData.data[fNum] = theRec.mData.data[zFld];
		}
	}
}
// -------------------------------------------------------------------
DPDataRec::~DPDataRec()
{
	delete mData.data;
}		
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPDataRec::AddField()
{
	mData.numFlds++;
	SRecFld* oldData = mData.data;
	mData.data= new SRecFld[mData.numFlds];
	for(int16 fld = 0; fld < (mData.numFlds - 1); fld++) {
		mData.data[fld] = oldData[fld];
	}
	delete oldData;
	mData.data[mData.numFlds - 1].fldData.realVal = 0.0;
	mData.data[mData.numFlds - 1].fldValid = false;
	mData.data[mData.numFlds - 1].useFld = false;
}
// -------------------------------------------------------------------
status_t
DPDataRec::Archive(BMessage* theArchive, bool deep) const
{
	theArchive->AddString("class","DPDataRec");
	theArchive->AddData(str_rec_data,B_ANY_TYPE,&mData,sizeof(SRecData));
	for(int n = 0; n < mData.numFlds; n++) {
		theArchive->AddData(str_rec_fields,B_ANY_TYPE,&mData.data[n],sizeof(SRecFld));
	}
	return B_OK;
}
// -------------------------------------------------------------------
status_t
DPDataRec::Archive(BMessage* theArchive, int16 xFld, int16 yFld, int16 zFld) const
{
	theArchive->AddString("class","DPDataRec");
	SRecData dataCopy = mData;
	dataCopy.numFlds = 0;
	if(xFld >= 0 && xFld < mData.numFlds) {
		theArchive->AddData(str_rec_fields,B_ANY_TYPE,&mData.data[xFld],sizeof(SRecFld));
		dataCopy.numFlds++;
	}
	if(yFld >= 0 && yFld < mData.numFlds) {
		theArchive->AddData(str_rec_fields,B_ANY_TYPE,&mData.data[yFld],sizeof(SRecFld));
		dataCopy.numFlds++;
	}
	if(zFld >= 0 && zFld < mData.numFlds) {
		theArchive->AddData(str_rec_fields,B_ANY_TYPE,&mData.data[zFld],sizeof(SRecFld));
		dataCopy.numFlds++;
	}
	theArchive->AddData(str_rec_data,B_ANY_TYPE,&dataCopy,sizeof(SRecData));
	return B_OK;
}
// -------------------------------------------------------------------
status_t
DPDataRec::Archive(BDataIO* theDataObj) const
{
	uint32 signature = fld_code;
	ssize_t bytesWritten = theDataObj->Write(&signature, sizeof(signature));
	bytesWritten = theDataObj->Write(&mData,sizeof(mData));
	if(mData.valid) {
		bytesWritten = theDataObj->Write(&mData.data[0],sizeof(SRecFld)*mData.numFlds);
	}
	return B_OK;
}
// -------------------------------------------------------------------
status_t
DPDataRec::Archive(BDataIO* theDataObj, int16 xFld, int16 yFld, int16 zFld) const
{
	uint32 signature = fld_code;
	ssize_t bytesWritten = theDataObj->Write(&signature, sizeof(signature));
	SRecData theData = mData;	// Make a data copy
	theData.numFlds = 0;
	if(xFld >= 0 && xFld < mData.numFlds) theData.numFlds++;
	if(yFld >= 0 && yFld < mData.numFlds) theData.numFlds++;
	if(zFld >= 0 && zFld < mData.numFlds) theData.numFlds++;
	bytesWritten = theDataObj->Write(&theData,sizeof(theData));
	if(xFld >= 0 && xFld < mData.numFlds) {
		bytesWritten = theDataObj->Write(&mData.data[xFld],sizeof(SRecFld));
	}
	if(yFld >= 0 && yFld < mData.numFlds) {
		bytesWritten = theDataObj->Write(&mData.data[yFld],sizeof(SRecFld));
	}
	if(zFld >= 0 && zFld < mData.numFlds) {
		bytesWritten = theDataObj->Write(&mData.data[zFld],sizeof(SRecFld));
	}
	return B_OK;
}
// -------------------------------------------------------------------
bool
DPDataRec::CanUseFld(int16 fldNum) const
{
	return fldNum >= 0 && fldNum < mData.numFlds && mData.data[fldNum].useFld;
}
// -------------------------------------------------------------------
/*
const SRecFld*
DPDataRec::GetDataField(int16 fldNum)
{
	if(fldNum >= 0 && fldNum < mData.numFlds) {
		return (const SRecFld*)&mData.data[fldNum];
	} else {
		return NULL;
	}
}
*/
// -------------------------------------------------------------------
void
DPDataRec::SetFieldValue(int16 fldNum, double val)
{
	if(fldNum >= 0 && fldNum < mData.numFlds) {
		mData.data[fldNum].fldData.doubleVal = val;
		mData.data[fldNum].fldValid = true;
		mData.data[fldNum].useFld = true;
	}
}
// -------------------------------------------------------------------
void
DPDataRec::SetFieldValue(int16 fldNum, float val)
{
	if(fldNum >= 0 && fldNum < mData.numFlds) {
		mData.data[fldNum].fldData.realVal = val;
		mData.data[fldNum].fldValid = true;
		mData.data[fldNum].useFld = true;
	}
}		
// -------------------------------------------------------------------
void
DPDataRec::SetFieldValue(int16 fldNum, int32 val)
{
	if(fldNum >= 0 && fldNum < mData.numFlds) {
		mData.data[fldNum].fldData.longVal = val;
		mData.data[fldNum].fldValid = true;
		mData.data[fldNum].useFld = true;
	}
}		
// -------------------------------------------------------------------
void
DPDataRec::SetFieldValue(int16 fldNum, int16 val)
{
	if(fldNum >= 0 && fldNum < mData.numFlds && mData.data[fldNum].fldValid) {
		mData.data[fldNum].fldData.intVal = val;
		mData.data[fldNum].fldValid = true;
		mData.data[fldNum].useFld = true;
	}
}
// -------------------------------------------------------------------
void
DPDataRec::SetUse(int16 fldNum, bool use)
{
	if(fldNum >= 0 && fldNum < mData.numFlds && mData.data[fldNum].fldValid) {
		mData.data[fldNum].useFld = use;
	}
}
// -------------------------------------------------------------------
void
DPDataRec::SetRecordName(const char* rName)
{
	if(::strlen(rName) < max_fld_name) {
		::strcpy(mData.recName,rName);	
	} else {
		char* temp = new char[::strlen(rName) + 1];
		::strcpy(temp,rName);
		::CenterTruncate(temp,max_fld_name);
		::strcpy(mData.recName,temp);
		delete temp;
	}
}
// -------------------------------------------------------------------
bool
DPDataRec::ValidField(int16 fldNum) const
{
	if(fldNum >= 0 && fldNum < mData.numFlds) {
		return mData.data[fldNum].fldValid;
	}
	return false;
}		
// -------------------------------------------------------------------
bool
DPDataRec::Value(int16 fldNum, double &val) const
{
	if(ValidField(fldNum)) {
		val = mData.data[fldNum].fldData.doubleVal;
		return true;
	}
	val = 0.0;
	return false;
}	
// -------------------------------------------------------------------
bool
DPDataRec::Value(int16 fldNum, float &val) const
{
	if(ValidField(fldNum)) {
		val = mData.data[fldNum].fldData.realVal;
		return true;
	}
	val = 0.0;
	return false;
}	
// -------------------------------------------------------------------
bool
DPDataRec::Value(int16 fldNum, int32 &val) const
{
	if(ValidField(fldNum)) {
		val = mData.data[fldNum].fldData.longVal;
		return true;
	}
	val = 0;
	return false;
}	
// -------------------------------------------------------------------
bool
DPDataRec::Value(int16 fldNum, int16 &val) const
{
	if(ValidField(fldNum)) {
		val = mData.data[fldNum].fldData.intVal;
		return true;
	}
	val = 0;
	return false;
}	
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDataRec::Initialize()
{
	mData.key = MakeKey();
	mData.recName[0] = 0;
	mData.numFlds = 0;
	mData.data= NULL;
	mData.valid = false;
}
// -------------------------------------------------------------------

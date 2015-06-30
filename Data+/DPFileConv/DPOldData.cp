/* ------------------------------------------------------------------ 

	Title: DPOldData

	Description:  Data structures and handling for Data+

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		16 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DPOldData.h"
#include "DPOldDataRec.h"
#include "DPGlobals.h"
#include "U_Conversions.h"

#include <InterfaceKit.h>

const char set_key_str[8]		= "dpkey";
const char fld_sel_str[8]		= "dpflds";
const char data_sub_str[8]		= "dpsubs";
const char set_name_str[8]		= "dpname";
const char field_sel_str[8]	= "dpfsel";

// -------------------------------------------------------------------
// *  Static Methods             
// -------------------------------------------------------------------
BArchivable*
DPOldData::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPData")) {
		return new DPOldData(theArchive); 
	} else {
		return NULL;
	}
}
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPOldData::DPOldData()
{
	mCurrRec = NULL;
	::strcpy(mSetName,"Untitled Set");
	mKey = DPOldDataRec::MakeKey();
	mMinKey = 0;
	mMaxKey = 0;
	mXField = -1;
	mYField = -1;
	mZField = -1;
	mData = new BList(1000);
	mFieldTypes = new BList(32);
	mNumFields = 0;
	mHasMarkedData = false;
	mHasSuppressedData = false;
	mHasDeletedData = false;
}

DPOldData::DPOldData(BMessage* theArchive)
{
	mCurrRec = NULL;
	mMinKey = 0;
	mMaxKey = 0;
	mXField = -1;
	mYField = -1;
	mZField = -1;
	mData = new BList(1000);
	mFieldTypes = new BList(32);
	mNumFields = 0;
	mHasMarkedData = false;
	mHasSuppressedData = false;
	mHasDeletedData = false;

	// Find or make the data key
	if(theArchive->FindInt64(set_key_str,&mKey) != B_OK) {
		mKey = DPOldDataRec::MakeKey();
	}	
	
	// Find the set name
	const char* name = theArchive->FindString(set_name_str);
	if(name != NULL) {
		::strcpy(mSetName,name);
	} else {
		::strcpy(mSetName,"Untitled Set");
	}

	// Collect the field types
	int32 fldIndx = 0;
	status_t status;
	do {
		BMessage* theMesg = new BMessage();
		status = theArchive->FindMessage(data_type_str,fldIndx++,theMesg);
		if(status == B_OK) {
			BArchivable* theObj = DPOldDataType::Instantiate(theMesg);
			if(theObj != NULL) {
				DPOldDataType* oldType = (DPOldDataType*)theObj;
				AddField(oldType);
			}
		}
		delete theMesg;
	} while (status == B_OK) ;
	
	// Collect the data records
	int32 index = 0;
	do {
		BMessage* theMesg = new BMessage();
		status = theArchive->FindMessage(data_rec_str,index++,theMesg);
		if(status == B_OK) {
			BArchivable* theObj = DPOldDataRec::Instantiate(theMesg);
			DPOldDataRec* oldRec = (DPOldDataRec*)theObj;
			if(oldRec != NULL) {
				AddRecord(oldRec);
				switch(oldRec->DataStatus()) {
					case data_marked :
						mHasMarkedData = true;
						break;
					case data_suppress :
						mHasSuppressedData = true;
						break;
					case data_delete :
						mHasDeletedData = true;
						break;
					default :
						;	// No default case
				}
			}
		}
		delete theMesg;
	} while (status == B_OK) ;
	
	// If this is a data subset, we set the field designators
	bool isWhole = theArchive->FindBool(data_sub_str);
	if(!isWhole) {
		mXField = 0;
		if(mNumFields > 1) mYField = 1 ;
		if(mNumFields > 2) mZField = 2 ;
		
	} else {	// Find the archived field selections
		int16 iVal;
		int16 fIndx = 0;
		if(theArchive->FindInt16(field_sel_str,fIndx++,&iVal) == B_OK) {
			mXField = iVal;
		}
		if(theArchive->FindInt16(field_sel_str,fIndx++,&iVal) == B_OK) {
			mYField = iVal;
		}
		if(theArchive->FindInt16(field_sel_str,fIndx++,&iVal) == B_OK) {
			mZField = iVal;
		}
	}
}
		
DPOldData::~DPOldData()
{
	for(int32 fld = 0; fld < mData->CountItems(); fld++) {
		DPOldDataRec* theRec = (DPOldDataRec*)mData->ItemAt(fld);
		delete theRec;
	}
	delete mData;
	for(int32 fld = 0; fld < mFieldTypes->CountItems(); fld++) {
		DPOldDataType* theFld = (DPOldDataType*)mFieldTypes->ItemAt(fld);
		delete theFld;
	}
	delete mFieldTypes;
}		
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPOldData::AddField(DPOldDataType* theFld)
{
	mFieldTypes->AddItem(theFld); 
	mNumFields++;
	// Add space for the field in any existing records
	for(int32 recNum = 0; recNum < mData->CountItems(); recNum++) {
		DPOldDataRec* theRec = (DPOldDataRec*)mData->ItemAt(recNum);
		if(theRec != NULL) {
			theRec->AddField();
		}
	}
}
// -------------------------------------------------------------------
void
DPOldData::AddRecord(DPOldDataRec* theRec)
{
	if(mMaxKey == 0) {	// Starting condition
		mMaxKey = theRec->Key();
		mMinKey = theRec->Key();
	
	} else if(theRec->Key() > mMaxKey) {
		mMaxKey = theRec->Key();
		
	} else if(theRec->Key() < mMinKey) {
		mMinKey = theRec->Key();
	}
	mData->AddItem(theRec);
	mCurrRec = theRec;
}
// -------------------------------------------------------------------
// NOTE!! If not 'deep', only the selected fields are archived
status_t
DPOldData::Archive(BMessage* theArchive, bool deep) const
{
	// Only add fields sequentially
	int16 shallowFlds = 0;
	if(mXField >= 0) {
		shallowFlds++ ;
		if(mYField >= 0) {
			shallowFlds++ ;
			if(mZField >= 0) {
				shallowFlds++ ;
			}
		}
	}
	if(!deep && shallowFlds == 0) {
		char aStr[64];
		::strcpy(aStr,"No Data Fields Were Selected For ");
		::strcat(aStr,mSetName);
		BAlert* theAlert = new BAlert("archAlert",aStr,"OK");
		theAlert->Go();
		return B_ERROR;
	}
	
	theArchive->AddString("class","DPData");
	// Add the key
	theArchive->AddInt64(set_key_str,mKey);
	// Add the name
	theArchive->AddString(set_name_str,mSetName);
	// Keep track of whether this is a subset
	theArchive->AddBool(data_sub_str,deep);
	
	// Add the field types as a set of messages
	if(deep) {	// Add all fields
		for(int32 fld = 0; fld < mFieldTypes->CountItems(); fld++) {
			DPOldDataType* theFld = (DPOldDataType*)mFieldTypes->ItemAt(fld);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPOldDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
			// Archive the fields, in order
			theArchive->AddInt16(field_sel_str,mXField);
			theArchive->AddInt16(field_sel_str,mYField);
			theArchive->AddInt16(field_sel_str,mZField);
		}
	// The x,y,z fields must be added in order if we are a subset
	} else {	// If a copy of only the xyz fields
		if(mXField >= 0) {
			DPOldDataType* theFld = (DPOldDataType*)mFieldTypes->ItemAt(mXField);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPOldDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
		}
		if(mYField >= 0) {
			DPOldDataType* theFld = (DPOldDataType*)mFieldTypes->ItemAt(mYField);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPOldDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
		}
		if(mZField >= 0) {
			DPOldDataType* theFld = (DPOldDataType*)mFieldTypes->ItemAt(mZField);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPOldDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
		}
	}
	
	// Add the records
	for(int32 rec = 0; rec < mData->CountItems(); rec++) {
		DPOldDataRec* theRec = (DPOldDataRec*)mData->ItemAt(rec);
		if(theRec != NULL) {
			BMessage* recMesg = new BMessage(DPOldDataRec::fld_code);
			// If deep, archive everything
			if(deep) {
				theRec->Archive(recMesg,deep);
				
			} else {
				// Make a second rec with only selected fields added
				DPOldDataRec* copyRec = new DPOldDataRec(theRec,mXField,mYField,mZField);
				copyRec->Archive(recMesg,deep);
				delete copyRec;
			}
			theArchive->AddMessage(data_rec_str,recMesg);
			delete recMesg;
		}
	}
	return B_OK;
}
// -------------------------------------------------------------------
void
DPOldData::ClearStatus(EDataStatus dStatus)
{
	for(int32 indx = 0; indx < mData->CountItems(); indx++) {
		mCurrRec = (DPOldDataRec*)mData->ItemAt(indx);
		if(mCurrRec->DataStatus() == dStatus) {
			mCurrRec->SetDataStatus(data_normal);
		}
	}
	switch(dStatus) {
		case data_marked :
			mHasMarkedData = false;
			break;
			
		case data_suppress :
			mHasSuppressedData = false;
			break;
			
		case data_delete :
			mHasDeletedData = false;
			break;
			
		default :
			;	// No default case
	}
}
// -------------------------------------------------------------------
// Find a record by its key
bool	
DPOldData::FindRecord(record_key recKey)
{
	if(recKey >= mMinKey && recKey <= mMaxKey) {
		for(int32 indx = 0; indx < mData->CountItems(); indx++) {
			mCurrRec = (DPOldDataRec*)mData->ItemAt(indx);
			if(mCurrRec->Key() == recKey) {
				return true;
			}
		}
	}
	mCurrRec = NULL;
	return false;
}
// -------------------------------------------------------------------
bool
DPOldData::FindRecord(const char* recName, int16 index) 
{
	for(int32 indx = 0; indx < mData->CountItems(); indx++) {
		mCurrRec = (DPOldDataRec*)mData->ItemAt(indx);
		if(::strcmp(recName,mCurrRec->RecordName()) == 0 && index-- == 0) {
			return true;
		}
	}
	mCurrRec = NULL;
	return false;
}
// -------------------------------------------------------------------
bool	// Find a record by its sequence
DPOldData::FindRecord(int32 recNum)
{
	if(recNum >= 0) {
		mCurrRec = (DPOldDataRec*)mData->ItemAt(recNum);
	} else {
		mCurrRec = NULL;
	}
	return mCurrRec != NULL;
}
// -------------------------------------------------------------------
void
DPOldData::MakeTitleString(BString* theString,bool fileTitleToo) const
{
	if(fileTitleToo) {
		theString->Append(SetName());
		if(mXField >= 0 || mYField >= 0) {
			theString->Append(":");
		}
	}
	if(mXField >= 0) {
		theString->Append(FieldType(mXField)->FieldName());
		theString->Append("/");
	}
	if(mYField >= 0) {
		theString->Append(FieldType(mYField)->FieldName());
	}
	theString->Truncate(32);
}
// -------------------------------------------------------------------
void
DPOldData::MarkData(BMessage* theMesg)
{
	int16 dataStatus = theMesg->FindInt16(status_name);
	int32 cVal;
	theMesg->FindInt32(mark_col_name,&cVal);
	rgb_color mkColor = ::ValueToColor(cVal);
	type_code theType;
	int32 numKeys;
	int16 indx;
	int64	key;
	theMesg->GetInfo(keys_names,&theType,&numKeys);
	if(numKeys > 0) {
		for(indx = 0; indx < numKeys; indx++) {
			theMesg->FindInt64(keys_names,indx,&key);
			if(FindRecord(key)) {
				CurrentRec()->SetDataStatus((EDataStatus)dataStatus);
				switch(dataStatus) {
					case data_marked :
						mHasMarkedData = true;
						break;
						
					case data_suppress :
						mHasSuppressedData = true;
						break;
						
					case data_delete :
						mHasDeletedData = true;
						break;
						
					default :
						;	// No default case
				}
				CurrentRec()->SetMarkColor(mkColor);
			}
		}
	}
}
// -------------------------------------------------------------------
void
DPOldData::SetXYZFields(int16 xFld, int16 yFld, int16 zFld)
{
	int32 numFlds = mFieldTypes->CountItems();
	// Ensure that no two fields are the same
	if(xFld >= 0 && xFld == yFld) {
		yFld = -1;
	}
	if(yFld >= 0 && yFld == zFld) {
		zFld = -1;
	}
	if(xFld >= 0 && xFld < numFlds) {
		mXField = xFld;
	} else {
		mXField = -1;
	}

	if(yFld >= 0 && yFld < numFlds) {
		mYField = yFld;
	} else {
		mYField = -1;
	}
	
	if(zFld >= 0 && zFld < numFlds) {
		mZField = zFld;
	} else {
		mZField = -1;
	}
}
// -------------------------------------------------------------------
bool
DPOldData::XValue(float &val) const
{
	if(mCurrRec != NULL && mXField >= 0) {
		return mCurrRec->Value(mXField,val);
	} else {
		val = 0.0;
		return false;
	}
}
// -------------------------------------------------------------------
bool
DPOldData::YValue(float &val) const
{
	if(mCurrRec != NULL && mYField >= 0) {
		return mCurrRec->Value(mYField,val);
	} else {
		val = 0.0;
		return false;
	}
}
// -------------------------------------------------------------------
bool
DPOldData::ZValue(float &val) const
{
	if(mCurrRec != NULL && mZField >= 0) {
		return mCurrRec->Value(mZField,val);
	} else {
		val = 0.0;
		return false;
	}
}
// -------------------------------------------------------------------

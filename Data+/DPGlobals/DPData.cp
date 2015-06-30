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

#include "DPData.h"
#include "DPDataRec.h"
#include "DPGlobals.h"
#include "U_Conversions.h"

#include <InterfaceKit.h>

const char set_key_str[8]		= "dpkey";
const char fld_sel_str[8]		= "dpflds";
const char data_sub_str[8]		= "dpsubs";
const char set_name_str[8]		= "dpname";
const char field_sel_str[8]		= "dpfsel";

// -------------------------------------------------------------------
// *  Static Methods             
// -------------------------------------------------------------------
BArchivable*
DPData::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPData")) {
		return new DPData(theArchive); 
	} else {
		return NULL;
	}
}
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPData::DPData()
{
	Initialize();
}
// -------------------------------------------------------------------
// Copy constructor
DPData::DPData(const DPData &theData)
{
	Initialize();
	mKey = theData.mKey;
	::strcpy(mSetName,theData.mSetName);
	mXField = theData.mXField;
	mYField = theData.mYField;
	mZField = theData.mZField;
	mHasMarkedData = theData.mHasMarkedData;
	mHasSuppressedData = theData.mHasSuppressedData;
	mHasDeletedData = theData.mHasDeletedData;
	for(int32 fld = 0; fld < theData.NumFields(); fld++) {
		DPDataType* theType = new DPDataType(*theData.FieldType(fld));
		AddField(theType);
	}
	for(int32 rec = 0; rec < theData.NumRecords(); rec++) {
		DPDataRec* theRec = new DPDataRec(*theData.DataRecord(rec));
		if(theRec != NULL) {
			AddRecord(theRec);
		}
	}
}
// -------------------------------------------------------------------
DPData::DPData(BMessage* theArchive)
{
	Initialize();

	// Find or make the data key
	if(theArchive->FindInt64(set_key_str,&mKey) != B_OK) {
		mKey = DPDataRec::MakeKey();
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
			BArchivable* theObj = instantiate_object(theMesg);
			if(theObj != NULL) {
				DPDataType* theType = cast_as(theObj,DPDataType);
				if(theType != NULL) {
					AddField(theType);
				}
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
			BArchivable* theObj = DPDataRec::Instantiate(theMesg);
			DPDataRec* theRec = cast_as(theObj,DPDataRec);
			if(theRec != NULL) {
				AddRecord(theRec);
				switch(theRec->DataStatus()) {
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
// -------------------------------------------------------------------
DPData::DPData(BDataIO* theDataObj)
{
	Initialize();
	uint32 signature;
	ssize_t bytesRead = theDataObj->Read(&signature,sizeof(signature));
	if(signature == fld_code) {
		bytesRead = theDataObj->Read(&mKey,sizeof(mKey));
		bytesRead = theDataObj->Read(&mSetName,max_set_name + 1);
		// Get the field designators
		bytesRead = theDataObj->Read(&mXField,sizeof(mXField));
		bytesRead = theDataObj->Read(&mYField,sizeof(mYField));
		bytesRead = theDataObj->Read(&mZField,sizeof(mZField));
		
		// Collect the field types
		DPDataType* theType = DPDataType::Instantiate(theDataObj);
		while(theType != NULL) {
			AddField(theType);
			theType = DPDataType::Instantiate(theDataObj);
		}		
	
		// Collect the data records
		DPDataRec* theRec = DPDataRec::Instantiate(theDataObj);
		while(theRec != NULL) {
			if(theRec->InitCheck()) {
				AddRecord(theRec);
				switch(theRec->DataStatus()) {
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
		theRec = DPDataRec::Instantiate(theDataObj);
		}	// Of while(
	}
}
// -------------------------------------------------------------------
DPData::~DPData()
{
	for(int32 rec = 0; rec < mData->CountItems(); rec++) {
		DPDataRec* theRec = (DPDataRec*)mData->ItemAt(rec);
		delete theRec;
	}
	delete mData;
	for(int32 fld = 0; fld < mFieldTypes->CountItems(); fld++) {
		DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(fld);
		delete theFld;
	}
	delete mFieldTypes;
}		
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPData::AddField(DPDataType* theFld)
{
	mFieldTypes->AddItem(theFld); 
	mNumFields++;
	// Add space for the field in any existing records
	for(int32 recNum = 0; recNum < mData->CountItems(); recNum++) {
		DPDataRec* theRec = (DPDataRec*)mData->ItemAt(recNum);
		if(theRec != NULL) {
			theRec->AddField();
		}
	}
}
// -------------------------------------------------------------------
void
DPData::AddRecord(DPDataRec* theRec)
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
DPData::Archive(BMessage* theArchive, bool deep) const
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
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(fld);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPDataType::fld_code);
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
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(mXField);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
		}
		if(mYField >= 0) {
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(mYField);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
		}
		if(mZField >= 0) {
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(mZField);
			if(theFld != NULL) {
				BMessage* fldMesg = new BMessage(DPDataType::fld_code);
				theFld->Archive(fldMesg);
				theArchive->AddMessage(data_type_str,fldMesg);
				delete fldMesg;
			}
		}
	}
	
	// Add the records
	for(int32 rec = 0; rec < mData->CountItems(); rec++) {
		DPDataRec* theRec = (DPDataRec*)mData->ItemAt(rec);
		if(theRec != NULL) {
			BMessage* recMesg = new BMessage(DPDataRec::fld_code);
			// If deep, archive everything
			if(deep) {
				theRec->Archive(recMesg,deep);
				
			} else {
				theRec->Archive(recMesg,mXField,mYField,mZField);
			}
			theArchive->AddMessage(data_rec_str,recMesg);
			delete recMesg;
		}
	}
	return B_OK;
}
// -------------------------------------------------------------------
status_t
DPData::Archive(BDataIO* theDataObj, bool deep) const
{
	// Only add fields sequentially
	int16 xFld = -1;
	int16 yFld = -1;
	int16 zFld = -1;
	int16 shallowFlds = 0;
	
	if(deep) {
		xFld = mXField;
		yFld = mYField;
		zFld = mZField;
		
	} else {	// Find out how many fields to be added
		if(mXField >= 0) {
			xFld = shallowFlds;
			shallowFlds++ ;
			if(mYField >= 0) {
				yFld = shallowFlds;
				shallowFlds++ ;
				if(mZField >= 0) {
					zFld = shallowFlds;
					shallowFlds++ ;
				}
			}
		}
		
		if(shallowFlds == 0) {
			char aStr[64];
			::strcpy(aStr,"No Data Fields Were Selected For ");
			::strcat(aStr,mSetName);
			BAlert* theAlert = new BAlert("archAlert",aStr,"OK");
			theAlert->Go();
			return B_ERROR;
		}
	}
	
	uint32 signature = fld_code;
	ssize_t bytesWritten = theDataObj->Write(&signature,sizeof(signature));
	bytesWritten = theDataObj->Write(&mKey,sizeof(mKey));
	bytesWritten = theDataObj->Write(&mSetName,max_set_name + 1);
	
	// Write the field designators
	bytesWritten = theDataObj->Write(&xFld,sizeof(xFld));
	bytesWritten = theDataObj->Write(&yFld,sizeof(yFld));
	bytesWritten = theDataObj->Write(&zFld,sizeof(zFld));

	// Add the field types
	if(deep) {	// Add all fields
		for(int32 fld = 0; fld < mFieldTypes->CountItems(); fld++) {
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(fld);
			if(theFld != NULL) {
				theFld->Archive(theDataObj);
			}
		}
	// The x,y,z fields must be added in order if we are a subset
	} else {	// If a copy of only the xyz fields
		if(mXField >= 0) {
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(mXField);
			if(theFld != NULL) {
				theFld->Archive(theDataObj);
			}
		}
		if(mYField >= 0) {
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(mYField);
			if(theFld != NULL) {
				theFld->Archive(theDataObj);
			}
		}
		if(mZField >= 0) {
			DPDataType* theFld = (DPDataType*)mFieldTypes->ItemAt(mZField);
			if(theFld != NULL) {
				theFld->Archive(theDataObj);
			}
		}
	}	// Of else deep
	
	// Add the records
	for(int32 rec = 0; rec < mData->CountItems(); rec++) {
		DPDataRec* theRec = (DPDataRec*)mData->ItemAt(rec);
		if(theRec != NULL) {
			if(deep) {
				theRec->Archive(theDataObj);
				
			} else {
				theRec->Archive(theDataObj,mXField,mYField,mZField);
			}
		}
	}
	return B_OK;
}
// -------------------------------------------------------------------
void
DPData::ClearStatus(EDataStatus dStatus)
{
	for(int32 indx = 0; indx < mData->CountItems(); indx++) {
		mCurrRec = (DPDataRec*)mData->ItemAt(indx);
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
DPData::FindRecord(record_key recKey)
{
	if(recKey >= mMinKey && recKey <= mMaxKey) {
		for(int32 indx = 0; indx < mData->CountItems(); indx++) {
			mCurrRec = (DPDataRec*)mData->ItemAt(indx);
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
DPData::FindRecord(const char* recName, int16 index) 
{
	for(int32 indx = 0; indx < mData->CountItems(); indx++) {
		mCurrRec = (DPDataRec*)mData->ItemAt(indx);
		if(::strcmp(recName,mCurrRec->RecordName()) == 0 && index-- == 0) {
			return true;
		}
	}
	mCurrRec = NULL;
	return false;
}
// -------------------------------------------------------------------
bool	// Find a record by its sequence
DPData::FindRecord(int32 recNum)
{
	if(recNum >= 0) {
		mCurrRec = (DPDataRec*)mData->ItemAt(recNum);
	} else {
		mCurrRec = NULL;
	}
	return mCurrRec != NULL;
}
// -------------------------------------------------------------------
void
DPData::MakeTitleString(BString* theString,bool fileTitleToo) const
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
DPData::MarkData(BMessage* theMesg)
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
DPData::SetXYZFields(int16 xFld, int16 yFld, int16 zFld)
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
DPData::XValue(float &val) const
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
DPData::YValue(float &val) const
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
DPData::ZValue(float &val) const
{
	if(mCurrRec != NULL && mZField >= 0) {
		return mCurrRec->Value(mZField,val);
	} else {
		val = 0.0;
		return false;
	}
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
void
DPData::Initialize()
{
	mCurrRec = NULL;
	::strcpy(mSetName,"Untitled Set");
	mKey = DPDataRec::MakeKey();
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
// -------------------------------------------------------------------

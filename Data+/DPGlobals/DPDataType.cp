/* ------------------------------------------------------------------ 

	Title: DPDataType

	Description:  Field definitions and methods for DPDataFld

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		28 March, 2001
			Modified data structure and added file archiving
		26 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DPDataType.h"
#include "DPDataRec.h"
#include <AppKit.h>
#include "UStringFmt_Utils.h"

const char dp_data_str[8]		= "dptdata";
const char dp_flds_str[8]		= "dptenum";	// Enumerated types
const char dp_vals_str[8]		= "dpteval";	// Enumerated values

// -------------------------------------------------------------------
//	* Static Methods
// -------------------------------------------------------------------
BArchivable*	// Static creator
DPDataType::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPDataType")) {
		return new DPDataType(theArchive); 
	} else {
		return NULL;
	}
}

DPDataType*	// Static creator from file
DPDataType::Instantiate(BDataIO* theDataObj)
{
	DPDataType* theType = new DPDataType(theDataObj);
	if(theType->DataType() == type_invalid) {
		delete theType;
		return NULL;
	}
	return theType;
	
}
// -------------------------------------------------------------------
//	* Constructor & Destructor
// -------------------------------------------------------------------
DPDataType::DPDataType(BMessage* theArchive)
{
	Initialize();
	const void* theData;
	ssize_t bytes;
	theArchive->FindData(dp_data_str,B_ANY_TYPE,&theData,&bytes);
	if(bytes == sizeof(STypeData)) {
		::memcpy(&mData,theData,sizeof(STypeData));
	} else {
		Initialize();
	}

	if(mData.fldName[0] == 0) {
		::strcpy(mData.fldName,"Unknown");
	}
	if(mData.type == DP_ENUM_TYPE) {
		// Get enum data
		;
	}
}
// -------------------------------------------------------------------
DPDataType::DPDataType(BDataIO* theDataObj)
{
	Initialize();
	uint32 signature;
	ssize_t readBytes = theDataObj->Read(&signature,sizeof(signature));
	if(signature == fld_code) {	// If a valid type
		readBytes = theDataObj->Read(&mData,sizeof(mData));
		if(mData.type == DP_ENUM_TYPE) {
			// Get enum data
			;
		}
	} else {
		BFile* theFile = cast_as(theDataObj,BFile);
		off_t posn = theFile->Position();
		posn = theFile->Seek(posn - sizeof(signature),SEEK_SET);
	}
}
// -------------------------------------------------------------------
DPDataType::DPDataType(const DPDataType &theType)
{
	mData.enumStr = theType.mData.enumStr;
	mData.enumVal = NULL;
	mData.realPrec = theType.mData.realPrec;
	mData.realFmt = theType.mData.realFmt;
	mData.type = theType.mData.type;
	::strcpy(mData.fldName,theType.mData.fldName);
}
// -------------------------------------------------------------------
DPDataType::DPDataType(type_code dataType)
{
	Initialize();
	mData.type = dataType;
}
// -------------------------------------------------------------------
DPDataType::~DPDataType()
{
	if(mData.type == DP_ENUM_TYPE) {
		// Delete the fields
		;
	}
	delete mData.enumStr;
	delete mData.enumVal;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
void
DPDataType::AddEnum(const char* itemStr, int32 val) const
{
	// For now, enumerated types are not supported

}
// -------------------------------------------------------------------
status_t
DPDataType::Archive(BMessage* theArchive,bool deep) const
{
	theArchive->AddString("class","DPDataType");
	theArchive->AddData(dp_data_str,B_ANY_TYPE,&mData,sizeof(mData));
	if(mData.type == DP_ENUM_TYPE) {
		// add enum data
		;
	}
	return B_OK;
}
// -------------------------------------------------------------------
status_t
DPDataType::Archive(BDataIO* theDataObj,bool deep) const
{
	uint32 signature = fld_code;
	ssize_t writtenBytes = theDataObj->Write(&signature,sizeof(signature));
	writtenBytes = theDataObj->Write(&mData,sizeof(mData));
	if(mData.type == DP_ENUM_TYPE) {
		// Get enum data...
		;
	}
	return B_OK;
}
// -------------------------------------------------------------------
const char*
DPDataType::DataString(const DPDataRec* theRec, int16 fldNum)
{
	float rVal;
	double dVal;
	
	mRsltStr[0] = 0;
	
	switch (mData.type) {
	
		case B_FLOAT_TYPE :
			if(theRec->Value(fldNum,rVal)) {
				::FormatReal(rVal,mData.realFmt,mData.realPrec,mRsltStr);
			}
			break;
		
		case B_DOUBLE_TYPE :
			if(theRec->Value(fldNum,dVal)) {
				::FormatReal(dVal,mData.realFmt,mData.realPrec,mRsltStr);
			}
			break;
		
		default :
			::strcpy(mRsltStr,"!Supported");
	}
	return (const char*)mRsltStr;
}
// -------------------------------------------------------------------
void
DPDataType::SetFieldName(const char* title)
{
	if(::strlen(title) > max_fld_name) {
		// Use central truncation
		char* tTitle = new char[::strlen(title) + 1];
		::strcpy(tTitle,title);
		::CenterTruncate(tTitle,max_fld_name - 1);
		::strcpy(mData.fldName,tTitle);
		delete tTitle;
	} else {
		::strcpy(mData.fldName,title);
	}
}
// -------------------------------------------------------------------
void
DPDataType::SetRealFormat(ERealFormat format, int16 prec)
{
	mData.realFmt = format;
	mData.realPrec = prec;
}
// -------------------------------------------------------------------
//	* Protected METHODS
// -------------------------------------------------------------------
void
DPDataType::Initialize()
{
	mData.fldName[0] = 0;
	mData.type = type_invalid;
	mData.enumStr = NULL;
	mData.enumVal = NULL;
	mData.realPrec = 4;
	mData.realFmt = realFmt_general;
}
// -------------------------------------------------------------------

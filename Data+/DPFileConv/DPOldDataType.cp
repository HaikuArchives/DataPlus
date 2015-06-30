/* ------------------------------------------------------------------ 

	Title: DPOldDataType

	Description:  Field definitions and methods for DPOldDataFld

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#include "DPOldDataType.h"
#include "DPOldDataRec.h"
#include <AppKit.h>
#include "UStringFmt_Utils.h"

const char dp_type_str[8]		= "dpdtype";
const char dp_title_str[8]		= "dpdtitl";
const char dp_flds_str[8]		= "dpdenum";
const char dp_vals_str[8]		= "dpdeval";
const char dp_prec_str[8]		= "dpprec";
const char dp_rtype_str[8]		= "dprtype";

// -------------------------------------------------------------------
//	* Static Methods
// -------------------------------------------------------------------
BArchivable*	// Static creator
DPOldDataType::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPDataType")) {
		return new DPOldDataType(theArchive); 
	} else {
		return NULL;
	}
}
// -------------------------------------------------------------------
//	* Constructor & Destructor
// -------------------------------------------------------------------
DPOldDataType::DPOldDataType(BMessage* theArchive)
{
	mEnumStr = NULL;
	mEnumVal = NULL;
	mRealPrec = 3;
	mRealFmt = realFmt_float;

	theArchive->FindInt32(dp_type_str,(int32*)&mType);
	const char* fName;
	fName = theArchive->FindString(dp_title_str);
	if(fName != NULL) {
		::strcpy(mFldName,fName);
	} else {
		::strcpy(mFldName,"Unknown");
	}
	mRealPrec = theArchive->FindInt16(dp_prec_str);
	int16 rType = theArchive->FindInt16(dp_rtype_str);
	mRealFmt = (ERealFormat)rType;
	if(mType == DP_ENUM_TYPE) {
		// Get enum data
		;
	}
}

DPOldDataType::DPOldDataType(DPOldDataType &theType)
{
	mEnumStr = theType.mEnumStr;
	mEnumVal = NULL;
	mRealPrec = theType.mRealPrec;
	mRealFmt = theType.mRealFmt;
	mType = theType.mType;
	::strcpy(mFldName,theType.mFldName);
}

DPOldDataType::DPOldDataType(type_code dataType)
{
	mEnumStr = NULL;
	mEnumVal = NULL;
	mRealPrec = 3;
	mRealFmt = realFmt_float;
	mType = dataType;
}
		
DPOldDataType::~DPOldDataType()
{
	if(mType == DP_ENUM_TYPE) {
		// Delete the fields
		;
	}
	delete mEnumStr;
	delete mEnumVal;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
void
DPOldDataType::AddEnum(const char* itemStr, int32 val) const
{
	// For now, enumerated types are not supported

}
// -------------------------------------------------------------------
status_t
DPOldDataType::Archive(BMessage* theArchive,bool deep) const
{
	theArchive->AddString("class","DPDataType");
	theArchive->AddInt32(dp_type_str,mType);
	theArchive->AddString(dp_title_str,mFldName);
	theArchive->AddInt16(dp_prec_str,mRealPrec);
	theArchive->AddInt16(dp_rtype_str,mRealFmt);
	if(mType == DP_ENUM_TYPE) {
		// add enum data
		;
	}
		
	return B_OK;
}
// -------------------------------------------------------------------
const char*
DPOldDataType::DataString(const DPOldDataRec* theRec, int16 fldNum)
{
	float rVal;
	
	switch (mType) {
	
		case B_FLOAT_TYPE :
			if(theRec->Value(fldNum,rVal)) {
				::FormatReal(rVal,mRealFmt,mRealPrec,mRsltStr);
			} else {
				::strcpy(mRsltStr,"InvalFld");	
			}
			break;
		
		default :
			::strcpy(mRsltStr,"Not Supported");
	}
	return (const char*)mRsltStr;
}
// -------------------------------------------------------------------
void
DPOldDataType::SetFieldName(const char* title)
{
	if(::strlen(title) > max_fld_name) {
		// Use central truncation
		char* tTitle = new char[::strlen(title) + 1];
		::strcpy(tTitle,title);
		::CenterTruncate(tTitle,max_fld_name - 1);
		::strcpy(mFldName,tTitle);
		delete tTitle;
	} else {
		::strcpy(mFldName,title);
	}
}
// -------------------------------------------------------------------
void
DPOldDataType::SetRealFormat(ERealFormat format, int16 prec)
{
	mRealFmt = format;
	mRealPrec = prec;
}
// -------------------------------------------------------------------

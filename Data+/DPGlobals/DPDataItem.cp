/* ------------------------------------------------------------------ 

	Title: DPDataItem
	
	Description:  A class for showing and listing the data

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		07 October, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DPDataItem.h"

#include "UStringFmt_Utils.h"

const char	dpitem_key[8]	= "dpkey";
const char	dpitem_name[8]	= "dpname";
const char	dpitem_xval[8]	= "dpitemx";
const char	dpitem_yval[8]	= "dpitemy";

const float name_tab = 5;
const float x_tab = 110;
const float y_tab = 140;
	

// -------------------------------------------------------------------
// *  Static Methods           
// -------------------------------------------------------------------
BArchivable*
DPDataItem::Instantiate(BMessage* theArchive)
{
	if ( validate_instantiation(theArchive, "DPDataItem")) {
		return new DPDataItem(theArchive); 
	} else {
		return NULL;
	}
}
// -------------------------------------------------------------------
// *  Constructors & Destructor             
// -------------------------------------------------------------------
DPDataItem::DPDataItem(DPData* theSet, int32 theRecord)
{
	mXVal[0] = 0;
	mYVal[0] = 0;
	char tName[255];
	DPDataRec* theRec = theSet->DataRecord(theRecord);
	mFHeight.ascent = 0;
	mKey = theRec->Key();
	::strcpy(tName,theRec->RecordName());
	::CenterTruncate(tName,max_name_len);
//	::strncpy(mRecName,theRec->RecordName(),max_name_len);
//	int len = ::strlen(theRec->RecordName());
//	len = len > max_name_len ? max_name_len : len;
//	mRecName[len] = 0;
	::strcpy(mRecName,tName);
	DPDataType* xType = theSet->FieldType(theSet->XField());
	if(xType != NULL) {
		::strcpy(tName,xType->DataString(theRec,theSet->XField()));
		::CenterTruncate(tName,max_num_len);
		::strcpy(mXVal,tName);
//		::strncpy(mXVal,xType->DataString(theRec,theSet->XField()),max_num_len);
//		len = ::strlen(xType->DataString(theRec,theSet->XField()));
//		len = len > max_name_len ? max_name_len : len;
//		mXVal[len] = 0;
	} 
		
	DPDataType* yType = theSet->FieldType(theSet->YField());
	if(yType != NULL) {
		::strcpy(tName,yType->DataString(theRec,theSet->YField()));
		::CenterTruncate(tName,max_num_len);
		::strcpy(mYVal,tName);
//		::strncpy(mYVal,yType->DataString(theRec,theSet->YField()),max_num_len);
//		len = ::strlen(yType->DataString(theRec,theSet->YField()));
//		len = len > max_name_len ? max_name_len : len;
//		mYVal[len] = 0;
	}
}

DPDataItem::DPDataItem(BMessage* theMesg):BListItem(theMesg)
{
	const char* tStr;
	mFHeight.ascent = 0;
	theMesg->FindInt64(dpitem_key,&mKey);
	if(theMesg->FindString(dpitem_name,&tStr) == B_OK) {
		::strcpy(mRecName,tStr);
	}
	if(theMesg->FindString(dpitem_xval,&tStr) == B_OK) {
		::strcpy(mXVal,tStr);
	}
	if(theMesg->FindString(dpitem_yval,&tStr) ==B_OK) {
		::strcpy(mYVal,tStr);
	}
}
		
DPDataItem::~DPDataItem()
{
}
// -------------------------------------------------------------------
// *  Public Methods             
// -------------------------------------------------------------------
status_t
DPDataItem::Archive(BMessage* theArchive, bool deep) const
{
	BListItem::Archive(theArchive,deep);
	theArchive->AddString("class","DPDataItem");
	theArchive->AddInt64(dpitem_key,mKey);
	theArchive->AddString(dpitem_name,mRecName);
	theArchive->AddString(dpitem_xval,mXVal);
	theArchive->AddString(dpitem_yval,mYVal);
	return B_OK;
}
// -------------------------------------------------------------------
float
DPDataItem::Width()
{
	return y_tab + 30;
}
// -------------------------------------------------------------------
// *  Protected Methods             
// -------------------------------------------------------------------
void
DPDataItem::DrawItem(BView* owner, BRect itemRect, bool drawEverything)
{
	if(mFHeight.ascent == 0) {
		BFont theFont;
		owner->GetFont(&theFont);
		theFont.GetHeight(&mFHeight);
	}
	
	float vPosn = itemRect.bottom - (mFHeight.descent + 1);
	owner->MovePenTo(name_tab + itemRect.left,vPosn);
	owner->SetHighColor(255,0,0);	// Red
	owner->DrawString(mRecName);
	owner->SetHighColor(0,0,0);	// Black
	owner->MovePenTo(x_tab + itemRect.left,vPosn);
	owner->DrawString(" ");
	owner->DrawString(mXVal);
	owner->MovePenTo(y_tab + itemRect.left,vPosn);
	owner->DrawString(" ");
	owner->DrawString(mYVal);
}
// -------------------------------------------------------------------

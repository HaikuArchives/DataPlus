/* ------------------------------------------------------------------ 

	Title: DPTextFile

	Description:  Collects data from a text file
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		18 August, 1999
			Created
		
------------------------------------------------------------------ */ 
#include "DPTextFile.h"

#include <InterfaceKit.h>
#include "DPGlobals.h"
#include <stdio.h>
#include <ctype.h>

const char _CR 	= 015;
const char _LF 	= 012;
const char _TAB	= 011;

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPTextFile::DPTextFile(entry_ref* theRef)
{
	mFile = new BFile(theRef,B_READ_ONLY);
	mOwnFile = true;
	for(int n = 0; n < max_items; n++) {
		mItemNames[n] = NULL;
	}
	::strncpy(mFileName,theRef->name,31);
	MakeTextWindow();
}
		
DPTextFile::DPTextFile(BFile* theFile)
{
	mOwnFile = false;
	mFile = theFile;
	for(int n = 0; n < max_items; n++) {
		mItemNames[n] = NULL;
	}
	mFileName[0] = 0;
	MakeTextWindow();
}
		
DPTextFile::DPTextFile(BClipboard* theClip)
{
	const char path[32]= "/boot/home/dpcltmp";
	BEntry* tmpEntry = new BEntry(path);
	if(tmpEntry->InitCheck() == B_OK) {
		mFile = new BFile(tmpEntry,B_READ_WRITE | B_CREATE_FILE);
		tmpEntry->Remove();
		if(mFile->InitCheck() == B_OK) {
			const char *text; 
			int32 textLen; 
			BMessage* clipMesg = NULL; 
			if(theClip->Lock()) {
				clipMesg = theClip->Data();
				if(clipMesg != NULL) {
					clipMesg->FindData("text/plain", B_MIME_TYPE,(const void **)&text, &textLen); 
					if(textLen > 6) {
						mFile->Write(text,textLen);
					} else {
						delete mFile;
						mFile = NULL;
					}
				}
				theClip->Unlock();
			}

		} else {	// If we can't create a file
			delete mFile;
			mFile = NULL;
		}
	}
	mOwnFile = true;
	for(int n = 0; n < max_items; n++) {
		mItemNames[n] = NULL;
	}
	::strcpy(mFileName,"Clipboard");
	MakeTextWindow();
}
		
DPTextFile::DPTextFile(BMessage* theMesg)
{
	const char path[32]= "/boot/home/dpcltmp";
	BEntry* tmpEntry = new BEntry(path);
	if(tmpEntry->InitCheck() == B_OK) {
		mFile = new BFile(tmpEntry,B_READ_WRITE | B_CREATE_FILE);
		tmpEntry->Remove();
		const char *data; 
		ssize_t dataLen;
		if(mFile->InitCheck() == B_OK && theMesg->FindData(text_mime_type,B_MIME_TYPE,(const void**)&data,&dataLen) == B_OK) { 
				mFile->Write(data,dataLen);
		} else {	// If we can't create a file or find the data
			delete mFile;
			mFile = NULL;
		}
	}
	mOwnFile = true;
	for(int n = 0; n < max_items; n++) {
		mItemNames[n] = NULL;
	}
	::strcpy(mFileName,"Clipboard");
	MakeTextWindow();
}

DPTextFile::~DPTextFile()
{
	if(mOwnFile) {
		delete mFile;
	}
	
	for(int n = 0; n < max_items; n++) {
		delete mItemNames[n];
	}
	
//	delete mTextWindow;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
DPData*
DPTextFile::ParseFile()
{
	// See if file exists
	if(mFile == NULL || mFile->InitCheck() != B_OK) {
		FileWarning("File Cannot Be Opened");
		return NULL;
	}
	// Check for correct type of file
	if(!GoodFileType()) {
		FileWarning("Incorrect File Type");
		return NULL;
	}
	// Begin by getting first line from file and getting the # of items.
	mLocation = 0;
	mFile->Seek(mLocation,SEEK_SET);
	if(GetNextLine()) {
		CountItems();
		if(mNumItems == 0) {
			FileWarning("No Items Could Ne Located");
			return NULL;
		}
	} else {
		FileWarning("No Lines Could Be Located");
		return NULL;
	}	
		
		/* 
		By now, we have some items. The next step is to see if the first
		items should be treated as column headers or just numbers. They are
		considered numbers if at least one of them is a valid number.
	*/ 
	bool isNumber = false;
	for(int n = 0; n < mNumItems; n++) {
		if(isdigit(mItemNames[n]->ByteAt(0))) {
			isNumber = true;
		}
	}
	/*
		If they are numbers, the column headers are given "Col#1", "Col#2",
		etc. The file is then reset so that the first line can be read in 
		as numbers.
	*/
	if(isNumber) {
		mLocation = 0;	// ** Reset file location
		int16 colNum = 1;
		char dStr[8];
		for(int n = 0; n < mNumItems; n++) {
			sprintf(dStr,"%d",colNum++);
			mItemNames[n]->SetTo("Col#").Append(dStr);
		}
	}	
	mTextWindow->Append("The Following Headers Were");
	if(isNumber) {
		mTextWindow->SingleLine(" Created:");
	} else {
		mTextWindow->SingleLine(" Found:");
	}
	for(int n = 0; n < mNumItems; n++) {
		mTextWindow->Append(mItemNames[n]->String());
		mTextWindow->Append("  ");
	}
	mTextWindow->NewLine();
	
	// Look at the next line and see if there is a field that looks
	// like a datum title. Use the first non-# field, if any.
	if(!isNumber) {
		// Get first actual data line, if needed
		if(!GetNextLine()) {
			FileWarning("No Data Lines In File");
			return NULL;
		}
	}
	mItemEnd = -1;	// IMPORTANT Reset
	mTitleItem = -1;
	int itemNum = 0;
	char xferStr[64];
	while(NextItem()) {
		int16 len = mItemEnd - mItemBegin;
		if(len >= 64) len = 63;
		::strncpy(xferStr,&mCurrLine[mItemBegin],len);
		xferStr[len] = 0;	// Terminate
		if(!IsValidNumber(xferStr)) {	// If not a number
			mTitleItem = itemNum;
			break;
		}
		itemNum++;
	}
	
	// Now, read in the lines of data and create the data fields.
	// We should be on the first data line at this point
	int numFlds = mNumItems;
	// Adjust the number of fields for the presence of a title field
	if(mTitleItem >= 0) {
		numFlds--;
	}
	if(numFlds <= 0) {
		mTextWindow->SingleLine("No Numeric Fields In Data Record");
		return NULL;
	}
	
	DPData* theData = new DPData();
	// Add the record field names. Do not add the title field name, if any
	for(int n = 0; n < mNumItems; n++) {
		if(n != mTitleItem) {
			DPDataType* theType = new DPDataType(B_FLOAT_TYPE);
			theType->SetFieldName(mItemNames[n]->String());
			theData->AddField(theType);
		}	
	}
	// Now, we can load the data
	do {
		DPDataRec* theRec = new DPDataRec(numFlds);
		if(LoadFieldRecord(theRec)) {
			theData->AddRecord(theRec);
		} else {
			delete theRec;
		}
	} 	while(GetNextLine()) ;
	
	char dStr[8];
	::sprintf(dStr,"%ld",theData->NumRecords());
	mTextWindow->NewLine();
	mTextWindow->Append(dStr);
	mTextWindow->SingleLine(" Data Records Were Found.");
	if(theData->NumRecords() == 0) {
		delete theData;
		return NULL;
	}
	return theData;
}
// -------------------------------------------------------------------
//	* PROTECTED METHODS
// -------------------------------------------------------------------
void
DPTextFile::CountItems()
{
	int16 n;
	mNumItems = 0;
	// Determine the field separation method
	mSepType = sep_white;
	for(n = 0; n < ::strlen(mCurrLine); n++) {
		if(mCurrLine[n] == _TAB) mSepType = sep_tabs ;
	}
	if(mSepType == sep_white) {
		for(n = 0; n < ::strlen(mCurrLine); n++) {
			if(mCurrLine[n] == ',') mSepType = sep_commas ;
		}
	}
	
	// Delete the existing field labels
	for(n = 0; n < max_items; n++) {
		delete mItemNames[n];
		mItemNames[n] = NULL;
	}
	// Count the first line fields
	char xferStr[max_title_len];
	bool finished = false;
	mItemEnd = -1;
	while(! finished && NextItem()) {
		finished = IsLineTerm(mCurrLine[mItemEnd]);
		int16 len = mItemEnd - mItemBegin;
		if(len >= max_title_len) len = max_title_len - 1;
		::strncpy(xferStr,&mCurrLine[mItemBegin],len);
		xferStr[len] = 0;	// Terminate
		if(mNumItems < max_items) {
			if(finished && IsWhiteSpace(xferStr)) {
			;	// Ignore trailing 'white space'
			} else {
				mItemNames[mNumItems++] = new BString(xferStr);
			}
		}
	}
}
// -------------------------------------------------------------------
void
DPTextFile::FileWarning(const char* warning)
{
	mTextWindow->SingleLine(warning);
}			
// -------------------------------------------------------------------
bool
DPTextFile::GetLine()
{
	size_t bytesRead = mFile->ReadAt(mLocation,mCurrLine,max_line);
	if(bytesRead < 3) {
		return false;	// EXIT HERE if no more data
	}
	// Look for EOL characters
	int indx = 0;
	while((indx < bytesRead) && !IsLineTerm(mCurrLine[indx])) {
		indx++;
	}
	if(indx == max_line) {
		FileWarning("File Line Is Too Long");
		return false;	// EXIT HERE if line overrun
	}
	// Terminate string and move past any redundant line terminators
	mCurrLine[indx++] = 0;
	while((indx < max_line) && IsLineTerm(mCurrLine[indx])) {
		indx++;
	}
	// Adjust file location for next read
	mLocation += indx;
	return true;
}
// -------------------------------------------------------------------
bool
DPTextFile::GetNextLine()
{
	int16 numBlanks = 5;
	do {
		bool haveLine = GetLine();
		if(!haveLine) {
			return false;
		}
		if(::strlen(mCurrLine) < 2) {
			numBlanks--;
		} else {
			return true;
		}
	} while(numBlanks > 0) ;
	return false;
}
// -------------------------------------------------------------------
bool
DPTextFile::GoodFileType()
{
	return true;
}
// -------------------------------------------------------------------
bool
DPTextFile::IsLineTerm(char ch)
{
	return (ch == _CR) || (ch == _LF);
}
// -------------------------------------------------------------------
bool
DPTextFile::IsSepChar(char ch)
{
	if(mSepType == sep_tabs && ch == _TAB) return true ;
	if(mSepType == sep_commas && ch == ',') return true ;
	if(mSepType == sep_white && ch == ' ') return true ;
	return IsLineTerm(ch);
}
// -------------------------------------------------------------------
bool
DPTextFile::IsValidNumber(const char* testStr)
{
	// irst, see if sscanf can read it
	float val;
	if(::sscanf(testStr,"%f",&val) == 0) {
		return false;
	}
	
	// Look for funny characters..
	int dpcnt = 0;
	for(int n = 0; n < ::strlen(testStr); n++) {
		char ch = testStr[n];
		if(! isdigit(ch)) {	// See if a valid non-number
			if(ch == '.') {
				dpcnt++;
				if(dpcnt > 1) {
					return false;
				}
			} else if(ch != 'e' && ch != 'E' && ch != '-' && ch != '+') {
				return false;
			}
		}
	}
	return true;
}
// -------------------------------------------------------------------
bool
DPTextFile::IsWhiteSpace(const char* item)
{
	for(int n = 0; n < ::strlen(item); n++) {
		if(item[n] != ' ') {
			return false;
		}
	}
	return true;
}
// -------------------------------------------------------------------
bool
DPTextFile::LoadFieldRecord(DPDataRec* theRec)
{
	char xferStr[max_title_len];
	int16 itemNum = 0;
	int16 fldNum = 0;
	mItemEnd = -1;	// Reset item extractor
	while(NextItem()) {
		if(itemNum > mNumItems) {
			FileWarning("Too many Items On This Line:");
			FileWarning(mCurrLine);
			return (fldNum > 0);
		}
		int16 len = mItemEnd - mItemBegin;
		if(len >= max_title_len) len = max_title_len - 1;
		::strncpy(xferStr,&mCurrLine[mItemBegin],len);
		xferStr[len] = 0;	// Terminate
		if(itemNum == mTitleItem) {
			theRec->SetRecordName(xferStr);
		} else {
			float val;
			if(::sscanf(xferStr,"%f",&val) > 0) {
				theRec->SetFieldValue(fldNum,val);
			}
			fldNum++;	// Advance even if a blank field
		}
		itemNum++;
	}
	if(fldNum == 0) {
		FileWarning("No Data Fields On This Line:");
		FileWarning(mCurrLine);
	}
	return (fldNum > 0);	// Must have at least one valid data field
}
// -------------------------------------------------------------------
void
DPTextFile::MakeTextWindow()
{
	// Overlap the main window on the lower left
	BWindow* appWindow = be_app->WindowAt(0);
	BRect	windRect = appWindow->Frame();
	windRect.right = windRect.left + 250;
	windRect.top = windRect.bottom - 100;
	mTextWindow = new DPTextWindow(windRect);
	mTextWindow->Append("Translating File: ");
	mTextWindow->SingleLine(mFileName);
	mTextWindow->NewLine();	
}
// -------------------------------------------------------------------
bool
DPTextFile::NextItem()
{
	int16 len = ::strlen(mCurrLine);
	mItemEnd++;	// Move past last separator
	// If we use white space as a separator, eat any sep chars
	if(mSepType == sep_white) {
		while(mItemEnd < len && IsSepChar(mCurrLine[mItemEnd])) {
			mItemEnd++;
		}
	}
	if(mItemEnd >= len) {
		return false;
	}
	mItemBegin = mItemEnd;	
	while(mItemEnd < len && !IsSepChar(mCurrLine[mItemEnd])) {
		mItemEnd++;
	}
	return true;
}
// -------------------------------------------------------------------

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

#ifndef _DPTextFile
#define _DPTextFile

#include "DPData.h"
#include "DPTextWindow.h"

#include <StorageKit.h>
#include <AppKit.h>

class DPTextFile {

	public:
		DPTextFile(entry_ref* theRef);
		
		DPTextFile(BFile* theFile);
		
		DPTextFile(BClipboard* theClip);
		
		DPTextFile(BMessage* theMesg);
		
	virtual
		~DPTextFile();
		
	DPData*
		ParseFile();
		
	protected : // Methods
	
	void
		CountItems();
		
	void
		FileWarning(const char* warning);
			
	bool
		GetLine();
		
	bool
		GetNextLine();
		
	bool
		GoodFileType();
		
	bool
		IsLineTerm(char ch);
	
	bool
		IsSepChar(char ch);
		
	bool
		IsValidNumber(const char* testStr);
		
	bool
		IsWhiteSpace(const char* item);
		
	bool
		LoadFieldRecord(DPDataRec* theRec);
		
	void
		MakeTextWindow();
		
	bool
		NextItem();
	
	protected : // Data
	
	char				mFileName[32];
	BFile*			mFile;
	DPTextWindow*	mTextWindow;
	off_t				mLocation;
	bool				mOwnFile;
	
	enum ESepType {
		sep_tabs,	// Uses tabs for field separation
		sep_commas,	// Uses commas
		sep_white	// Uses white space
	};
	
	enum {
		max_items = 32,		// Max # of columns
		max_line = max_items*16,	// Max length of a single line in the file
		max_title_len = 32
	};
	
	char		mCurrLine[max_line];	// The line being worked on
	BString*	mItemNames[max_items];	// List of item names
	int16		mLineIndx;	// Index into current line
	int16		mNumItems;	// # of items on a line
	int16		mTitleItem;	// Fld# of title item, if any
	int16		mItemBegin;	// Indices to items in 'mCurrLine'
	int16		mItemEnd;	
	ESepType	mSepType;
	
};
	

#endif

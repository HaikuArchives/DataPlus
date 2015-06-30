/* ------------------------------------------------------------------ 

	Title: DPDataConversion

	Description:  File version conversion for Data+
	
	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		04 April, 2001
			Created
		
------------------------------------------------------------------ */ 
#include <AppKit.h>
#include "WU_AppUtils.h"
#include <StorageKit.h>
#include <InterfaceKit.h>
#include "DPGlobals.h"
#include "DPData.h"
#include "DPOldData.h"
#include "DPDataType.h"
#include "DPDataFile.h"

#include "URealFmt_Utils.h"

class DPDataConversion : public BApplication {

	public:
		DPDataConversion();
		
	virtual
		~DPDataConversion();
		
	void
		ConvertData(const entry_ref* theRef);
				
	void
		ConvertFile(const entry_ref* theRef);

	DPDataRec*
		ConvertRec(DPOldDataRec* oldRec);
			
	DPData*
		ConvertSet(DPOldData* oldType);
		
	DPDataType*
		ConvertType(DPOldDataType* oldType);
		
	virtual void
		RefsReceived(BMessage* theMesg);
		
	protected:	// Data
	
	BList*	mData;
	
		
};

// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS
// -------------------------------------------------------------------
DPDataConversion::DPDataConversion() : BApplication("application/x_DataPlus.FileConv")
{
	mData = new BList();

}

DPDataConversion::~DPDataConversion()
{
	for(int32 n = 0; n < mData->CountItems(); n++) {
		DPOldData* theData = (DPOldData*)mData->ItemAt(n);
		delete theData;
	}
	delete mData;
}
// -------------------------------------------------------------------
//	* PUBLIC METHODS
// -------------------------------------------------------------------
void
DPDataConversion::ConvertData(const entry_ref* theRef)
{
	char oldName[B_FILE_NAME_LENGTH + 1];
	char newName[B_FILE_NAME_LENGTH + 1];
	status_t status = B_ERROR;
	
	// If there is data save it in a new file with the old name
	if(mData->CountItems() > 0) {
		BEntry theEntry(theRef);
		if(theEntry.InitCheck() == B_OK) {
			theEntry.GetName(oldName);
			::strcpy(newName,oldName);
			::strcat(newName,".old");
			status = theEntry.Rename(newName);
			if(status == B_OK) {
				DPDataFile theFile(theRef);
				theFile.Clear();
				if(theFile.InitCheck() == B_OK) {
					for( int32 set = 0; set < mData->CountItems(); set++) {
						DPOldData* theOldData = (DPOldData*)mData->ItemAt(set);
						DPData* theNewData = ConvertSet(theOldData);
						theFile.AddData(theNewData,true);
						delete theNewData;
					}
					// Set the MIME type and this application as preferred
					BNodeInfo theInfo;
					theInfo.SetTo(&theFile);
					theInfo.SetType(native_mime_type);
					theInfo.SetPreferredApp(data_plus_sig);
					BMimeType theMime(native_mime_type);
					if(!theMime.IsInstalled()) {
						theMime.Install();
					}	
				}
			} // Of status == B_OK
		}	// Of theEntry.InitCheck()
		// Clear the entries
		for(int32 n = 0; n < mData->CountItems(); n++) {
			DPOldData* theData = (DPOldData*)mData->ItemAt(n);
			delete theData;
		}
		mData->MakeEmpty();
	} // Of if CountItems
}
// ------------------------------------------------------------------ 
void
DPDataConversion::ConvertFile(const entry_ref* theRef)
{
	BFile* theFile = new BFile(theRef,B_READ_WRITE);
	if(theFile->InitCheck() == B_OK) {
		BMessage* archive = new BMessage();
		if(archive->Unflatten(theFile) == B_OK) {
			// Get and add the data sets
			int32 sIndx = 0;
			DPOldData* theData;
			do {
				BMessage* setMesg = new BMessage();
				archive->FindMessage(data_set_str,sIndx++,setMesg);
				BArchivable* theObj = DPOldData::Instantiate(setMesg);
				theData = (DPOldData*)theObj;
				delete setMesg;
				if(theData != NULL) {
					mData->AddItem(theData);
				}
			} while (theData != NULL) ;
		}
		delete archive;

	} else {
		BAlert* theAlert = new BAlert("OFile","Can't Open The File","OK");
		theAlert->Go();
	}
	ConvertData(theRef);
	delete theFile;
}
// ------------------------------------------------------------------ 
DPData*
DPDataConversion::ConvertSet(DPOldData* theOldData)
{
	DPData* theData = new DPData();
	// Extract and convert the field information
	for(int16 fld = 0; fld < theOldData->NumFields(); fld++) {
		DPOldDataType* oldType = theOldData->FieldType(fld);
		DPDataType* newType = ConvertType(oldType);
		theData->AddField(newType);
	}

	for(int32 rec = 0; rec < theOldData->NumRecords(); rec++) {
		DPOldDataRec* oldRec = theOldData->DataRecord(rec);
		DPDataRec* newRec = ConvertRec(oldRec);
		theData->AddRecord(newRec);
	}
	
	return theData;
}
// ------------------------------------------------------------------ 
DPDataType*
DPDataConversion::ConvertType(DPOldDataType* oldType)
{
	DPDataType* newType = new DPDataType(oldType->DataType());
	newType->SetFieldName(oldType->FieldName());
	
	ERealFormat theFmt;
	int16 prec;
	oldType->GetRealFormat(theFmt,prec);
	newType->SetRealFormat(theFmt,prec);
	return newType;
}
// ------------------------------------------------------------------ 
DPDataRec*
DPDataConversion::ConvertRec(DPOldDataRec* oldRec)
{
	DPDataRec* newRec = new DPDataRec(oldRec->NumFields());
	const char* recName = oldRec->RecordName();
	if(recName != NULL) {
		newRec->SetRecordName(recName);
	}
	newRec->SetMarkColor(oldRec->MarkColor());
	newRec->SetDataStatus(oldRec->DataStatus());
	newRec->SetKey(oldRec->Key());
	for(int16 fld = 0; fld < oldRec->NumFields(); fld++) {
		// So far, all the old types are float...
		float val;
		oldRec->SetUse(fld,true);
		if(oldRec->Value(fld,val)) {
			newRec->SetFieldValue(fld,val);
		}
	}
	return newRec;
}
// ------------------------------------------------------------------ 
void
DPDataConversion::RefsReceived(BMessage* theMesg)
{
	entry_ref theRef;
	int32 refIndx = 0;
	status_t status = B_OK;
	char typeStr[64];
	while(status == B_OK) {
		status = theMesg->FindRef("refs",refIndx++,&theRef);
		if(status == B_OK) {
			BNode theNode(&theRef);
			if(theNode.InitCheck() == B_OK) {
				BNodeInfo theInfo(&theNode);
				if(theInfo.InitCheck() == B_OK && theInfo.GetType(typeStr) == B_OK) {
					// Check MIME type for translator, if required
					BMimeType mType(typeStr);
					if(mType.Type() != NULL) {
						if(::strcmp(mType.Type(),old_mime_type) == 0) {
							// Convert only if an old native file type
							ConvertFile(&theRef);
						} else {
							BAlert* theAlert = new BAlert("theAlert","Not A Recognized FileType","OK");
							theAlert->Go();
						}
					}
				}
			}
		}
	} 
	PostMessage(B_QUIT_REQUESTED);
}
// -------------------------------------------------------------------
//	* MAIN Program
// -------------------------------------------------------------------
int
main()
{
	try {
		DPDataConversion theApp;

		theApp.Run();
	}
	
	catch(const char* theMesg) {
	}
		

	return(0);
}

/* ------------------------------------------------------------------ 

	Title: DPDataType

	Description:  Field definitions and methods for DPDataFld

	Author :
		Stephen Wardlaw, M.D.
		Medical Research & Development
		Highrock
		Lyme, CT 06371

	Edit History:	
		26 March, 2001
			Added BDataIO archiving and construction
		26 August, 1999
			Created
		
------------------------------------------------------------------ */ 

#ifndef _DPDataType
#define _DPDataType

#include <SupportKit.h>
#include "URealFmt_Utils.h"
#include "DPDataRec.h"

const type_code DP_ENUM_TYPE = 'Denu';	// Enumerated data
const type_code type_invalid = 0;	// Not a valid type

class DPDataRec;	// Forward declaration

class DPDataType : public BArchivable {

	public :
		enum {
			fld_code = 'DPTy',
			max_fld_name = 15,
			max_enum_name = 12,
			max_result_len = 12
		};
		
	static BArchivable*	// Static creator from message
		Instantiate(BMessage* theArchive);
		
	static DPDataType*	// Static creator from file
		Instantiate(BDataIO* theDataObj);

		DPDataType(BMessage* theArchive);
		
		DPDataType(BDataIO* theDataObj);

		DPDataType(const DPDataType &theType);

		DPDataType(type_code dataType);
		
	virtual
		~DPDataType();
		
	void
		AddEnum(const char* itemStr, int32 val) const;
		
	virtual status_t
		Archive(BMessage* theArchive,bool deep = true) const;
		
	status_t
		Archive(BDataIO* theDataObj,bool deep = true) const;
		
	const char*
		DataString(const DPDataRec*, int16 fldNum);
		
	type_code
		DataType() {return mData.type;}
		
	const char*
		FieldName() const {return (const char*)mData.fldName;}
		
	void
		GetRealFormat(ERealFormat &fmt, int16 &precision) const {fmt = mData.realFmt; precision = mData.realPrec;}
		
	void
		SetFieldName(const char* title);
		
	void
		SetRealFormat(ERealFormat format, int16 prec);

	protected :	// Methods
	void
		Initialize();
	
	protected :	// Data
	
	struct STypeData {
		char 		fldName[max_fld_name + 1];
		type_code	type;
		BList*		enumStr;
		BList*		enumVal;		
		int16		realPrec;
		ERealFormat	realFmt;
		char		bytePad;
		void*		extra1;	
	};
	STypeData		mData;
	char 			mRsltStr[max_result_len + 1];

};

#endif

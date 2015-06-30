/* ------------------------------------------------------------------ 

	Title: DPDistData

	Description:  A wrapper class for distribution statistics
	Author :
		Stephen Wardlaw, M.D.
		MDInventions
		Highrock
		Lyme, CT 06371

	Edit History:	
		21 April, 2000
			Created
		
------------------------------------------------------------------ */
#include "DPDistData.h"

#include <AppKit.h>
#include <stdlib.h>
#include "DPDistFit.h"
#include "UStringFmt_Utils.h"
#include "WColorControl.h"
#include "U_Conversions.h"

// NOTE!!!  There is a 'kludge' here, although not an egregious one. 
// The 'marked' status of a data point in the working data is signified
// by the setting of the alpha value of the mark color. 
// A non-zero value means the data point was marked.

// -------------------------------------------------------------------
//	* Methods for DistGroup
// -------------------------------------------------------------------
DistGroup::DistGroup(float begin, float end)
{
	mBegin = begin;
	mEnd = end;
	mTotalPoints = 0;
}
		
DistGroup::~DistGroup()
{
	for(int32 n = 0; n < mMarkList.CountItems(); n++) {
		SDistSet* theSet = (SDistSet*)mMarkList.ItemAt(n);
		delete theSet;
	}
}
// -------------------------------------------------------------------
void
DistGroup::AddMarked(rgb_color markColor)
{
	mTotalPoints++;
	for(int32 n = 0; n < mMarkList.CountItems(); n++) {
		SDistSet* theSet = (SDistSet*)mMarkList.ItemAt(n);
		if(ColorToValue(theSet->color) == ColorToValue(markColor)) {
			theSet->number++;
			return;
		}
	}
	
	// Add new color group if one not present
	SDistSet* theSet = new SDistSet;
	theSet->number = 1;
	theSet->color = markColor;
	mMarkList.AddItem((void*)theSet);
}
// -------------------------------------------------------------------
// Sorts the sets by color
void
DistGroup::SortColors()
{
	mMarkList.SortItems(&Compare);
}
// -------------------------------------------------------------------
bool
DistGroup::SetInfo(int32 index, int32 &number, rgb_color &color) const
{
	SDistSet* theSet = (SDistSet*)mMarkList.ItemAt(index);
	if(theSet != NULL) {
		number = theSet->number;
		color = theSet->color;
		return true;
	} else {
		number = 0;
		color = color_black;
	}
	
	return false;
}
// -------------------------------------------------------------------
int	// Compare colors
DistGroup::Compare(const void* ptr1, const void* ptr2)
{
	SDistSet* set1 = *(SDistSet**)ptr1;
	SDistSet* set2 = *(SDistSet**)ptr2;
	
	int64 color1 = ColorToValue(set1->color);
	int64 color2 = ColorToValue(set2->color);
	if(color1 > color2) {
		return 1;
		
	} else if(color1 < color2) {
		return -1;
	}
	
	return 0;	
}
// -------------------------------------------------------------------
//	* CONSTRUCTORS and DESTRUCTORS for DPDistData
// -------------------------------------------------------------------
DPDistData::DPDistData(DPData* theData)
{
	mData = theData;
	mWorkData = NULL;
	mDistFit = NULL;
	mMaxGroup = 0;
	mNumPoints = 0;
	mDistType = distType_distribution;
	mDistColor.red = 0;
	mDistColor.green = 0;
	mDistColor.blue = 0;
	mDist.SetTitle(mData->SetName());
	// CalcDistribution();
}
		

DPDistData::~DPDistData()
{
	delete mData;
	delete mWorkData;
	delete mDistFit;
	for(int32 n = 0; n < mGroups.CountItems(); n++) {
		delete (DistGroup*)mGroups.ItemAt(n);
	}
	// The windows delete themselves...
}
// -------------------------------------------------------------------
//	* Public Methods
// -------------------------------------------------------------------
void
DPDistData::CalcDistribution()
{
	// Create a working array of valid data points
	mNumPoints = 0;
	mDistType = distType_distribution;
	delete mWorkData;
	mWorkData = NULL;
	for(int32 n = 0; n < mData->NumRecords(); n++) {
		if(mData->FindRecord(n) && mData->CurrentRec()->DataStatus() <= data_marked) {
			mNumPoints++;
		}
	}
	if(mNumPoints < 3) {
		return;	// *** EXIT HERE if not enough data
	}
	
	// Create a working array
	mWorkData = new SDistData[mNumPoints];
	int32 index = 0;
	for(int32 n = 0; n < mData->NumRecords(); n++) {
		if(mData->FindRecord(n)) {

			switch(mData->CurrentRec()->DataStatus()) {
			
				case data_normal :
					mData->CurrentRec()->Value(0,mWorkData[index].value);
					mWorkData[index++].statusColor = color_black;
					break;
					
				case data_marked :
					mData->CurrentRec()->Value(0,mWorkData[index].value);
					mWorkData[index].statusColor = mData->CurrentRec()->MarkColor();
					mWorkData[index++].statusColor.alpha = 1;	// NOTE! Means marked
					break;
					
				default :
					;	// No data is added
			}
		}
	}
	
	// Sort the array
	qsort(mWorkData,mNumPoints,sizeof(SDistData),CompareValues);
	// Create a data fit class
	delete mDistFit;
	mDistFit = new DPDistFit(this);
	mDist.SetData(mWorkData,mNumPoints);
}
// -------------------------------------------------------------------
void
DPDistData::CreateGroups(float min, float width)
{
	// Delete any old group info
	for(int32 n = 0; n < mGroups.CountItems(); n++) {
		delete (DistGroup*)mGroups.ItemAt(n);
	}
	mGroups.MakeEmpty();
	if(mWorkData == NULL) {
		return;	// *** EXIT HERE if no data
	}
	// Make the groups
	mMaxGroup = 0;
	float end = min + width;
	int32 index = 0;
	float mode = 0;
	while(index < mNumPoints) {
		DistGroup* theGroup = new DistGroup(end - width, end);
		while(index < mNumPoints && mWorkData[index].value <= end) {
			theGroup->AddMarked(mWorkData[index++].statusColor);
		}
		if(theGroup->Points() > mMaxGroup) {
			mMaxGroup = theGroup->Points();
			mode = end - width/2.0;
		}
		theGroup->SortColors();
		mGroups.AddItem((void*)theGroup);
		end += width;
	}
	mDist.SetMode(mode);
	
	// Create the fit function
	delete mDistFit;
	mDistFit = new DPDistFit(this);
}
// -------------------------------------------------------------------
float
DPDistData::Max()
{
	if(mWorkData != NULL) {
		return mWorkData[mNumPoints - 1].value;
	}
	return 0;
}
// -------------------------------------------------------------------
int32
DPDistData::MaxGroupSize(float width)
{
	// Data must be sorted!
	if(mWorkData == NULL) {
		return 0;	// *** EXIT HERE if no data
	}
	
	uint32 gSize = 0;
	uint32 maxGroup = 0;
	float endG = Min() + width;
	int32 indx = 0;
	while(indx < mNumPoints) {
		if(mWorkData[indx].value <= endG) {
			gSize++;
		} else {
			if(gSize > maxGroup) {
				maxGroup = gSize;
				gSize = 0;
			}
			while(endG < mWorkData[indx].value) {
				endG += width;
			}
		}
		indx++;
	}
	if(gSize > maxGroup) {
		maxGroup = gSize;
	}
	return maxGroup;
}
// -------------------------------------------------------------------
void
DPDistData::MessageReceived(BMessage* theMesg)
{
	switch (theMesg->what) {
		
		default :
			BHandler::MessageReceived(theMesg);
	}
}
// -------------------------------------------------------------------
float 
DPDistData::Min()
{
	if(mWorkData != NULL) {
		return mWorkData[0].value;
	}
	return 0;
}
// ------------------------------------------------------------------
bool
DPDistData::Percentile(int32 index, float &val)
{
	if(mWorkData != NULL && index >= 0 && index < mNumPoints) {
		val = (float)(index + 1)*100.0/(float)mNumPoints;
		return true;
	}	
	val = 0;
	return false;
}
// ------------------------------------------------------------------
bool
DPDistData::Value(int32 index, float &val, bool &marked)
{
	if(mWorkData != NULL && index >= 0 && index < mNumPoints) {
		val = mWorkData[index].value;
		marked = mWorkData[index].statusColor.alpha > 0;
		return true;
	}	
	val = 0;
	marked = false;
	return false;
}
// ------------------------------------------------------------------
//	* PROTECTED METHODS
// ------------------------------------------------------------------
int	// Compare data values
DPDistData::CompareValues(const void* sPtr1, const void* sPtr2)
{
	SDistData* val1 = (SDistData*)sPtr1;
	SDistData* val2 = (SDistData*)sPtr2;
	
	if(val1->value > val2->value) {
		return 1;
		
	} else if(val1->value == val2->value) {
		return 0;
	}
	
	return -1;	// If 1 < 2
}
// ------------------------------------------------------------------

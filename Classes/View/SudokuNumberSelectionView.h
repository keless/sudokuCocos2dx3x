#pragma once

#include <CCLayer.h>
#include <ZZ\ZZUtils.h>
#include "GameDefines.h"
#include "Model\SudokuSheetModel.h"

using namespace cocos2d;

/*
The SudokuNumberSelectionView contains the UI state of which number is 
	currently selected to write, and which numbers have been completed

generated events:
  "ui"("numberSelected") { "val":int }  //note: val is one-based index
*/

class SudokuNumberSelectionView : public Layer
{
public:
	SudokuNumberSelectionView();
	~SudokuNumberSelectionView();

	void reset(SudokuSheetModel* model);
	void numberExhausted( int number );
	void setSelection( int number );
};


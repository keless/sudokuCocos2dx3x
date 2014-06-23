#pragma once

#include <ZZ\ZZUtils.h>
#include "GameDefines.h"

/*
 SudokuInkSelectionView - simulates a radio btn group to toggle between ink and pencil modes

 generated events:
  "ui"("chooseInk")
  "ui"("choosePencil")
*/

class SudokuInkSelectionView : public Layer
{
	LayerColor* m_btnInk;
	LayerColor* m_btnPencil;

public:
	SudokuInkSelectionView();
	~SudokuInkSelectionView();

	void setBtnInk( bool on );
};


#pragma once

#include <CCNode.h>
#include <CCLayer.h>
#include <CCRenderer.h>

using namespace cocos2d;

#include "Model\SudokuSheetModel.h"

#include "GameDefines.h"

/*
 A SudokuGrid is a 3x3 group of blocks, each of which is a 3x3 group of cells

 generated events:
  "ui"("cellTouched") { "blockX":int, "blockY":int, "cellX":int, "cellY":int }

*/

class SudokuGridCellView;

class SudokuGridView :
	public Layer
{
	CustomCommand _customDrawCommand;

	SudokuSheetModel * m_pModel;

public:
	SudokuGridView(void);
	~SudokuGridView(void);


	void draw(Renderer *renderer, const kmMat4& transform, bool transformUpdated);
	void onDrawPrimitives(const kmMat4 &transform, bool transformUpdated);
    
	void setModel( SudokuSheetModel* model );

	void updateView(int highlightNumber);
	void updateSpecificCell(int cellX, int cellY, int blockX, int blockY, int highlightNumber);

	void setVisible( bool visible );

protected:

	Node* makeCell(int cellX, int cellY, int blockX, int blockY);
	Node* makeBlock(int blockX, int blockY);

	SudokuGridCellView* getCellInBlock( int cellX, int cellY, int blockX, int blockY );
};


class SudokuGridCellView : public ZZ::TouchableNode
{
	Label* m_lblInk;
	Label* m_lblPencils[TOTAL_VALUES];

public:
	static SudokuGridCellView *SudokuGridCellView::create(const std::string evt, const std::string bus);
	SudokuGridCellView(const std::string evt, const std::string bus);
	~SudokuGridCellView();

	void updateWithModel(const SudokuCellModel& model, int selectedNumber );
	void setInk( int val, bool correct, bool highlighted );
	void setPencil( int val, bool on, bool highlighted );

	void setInkHighlight( Color3B color );
};
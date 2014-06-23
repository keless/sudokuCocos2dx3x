#include "SudokuNumberSelectionView.h"

#include "SudokuGridView.h"
#include "GameDefines.h"

SudokuNumberSelectionView::SudokuNumberSelectionView() 
{
	ignoreAnchorPointForPosition(false);
	setAnchorPoint(Point(0.5f,0.0f));

	float margin = 1;
	for(int i=0; i< TOTAL_VALUES; i++)
	{
		SudokuGridCellView* number = SudokuGridCellView::create("numberSelected", "ui");
		number->setTag(i);
		number->setInk( i+1, true, false );

		Json::Value data;
		data["val"] = i + 1;
		number->setData(data);

		float offsetHalf = (TOTAL_VALUES * (number->getContentSize().width + margin))/2;
		
		number->setPositionX( (i * (number->getContentSize().width + margin)) - offsetHalf );

		addChild(number);
	}
}


SudokuNumberSelectionView::~SudokuNumberSelectionView()
{
}

void SudokuNumberSelectionView::setSelection( int number )
{
	number--; //shift into zero-based

	for( int i=0; i<TOTAL_VALUES; i++)
	{
		SudokuGridCellView* cell = dynamic_cast<SudokuGridCellView*>(getChildByTag(i));

		cell->setInkHighlight( (number == i) ? Color3B::RED : Color3B::GRAY );
	}
}

void SudokuNumberSelectionView::reset( SudokuSheetModel* model )
{
	for( int i=0; i<TOTAL_VALUES; i++)
	{
		SudokuGridCellView* cell = dynamic_cast<SudokuGridCellView*>(getChildByTag(i));
		cell->setVisible( model->isValueExhausted(i+1) );
	}
}

void SudokuNumberSelectionView::numberExhausted( int number )
{
	number--; //shift into zero-based

	SudokuGridCellView* cell = dynamic_cast<SudokuGridCellView*>(getChildByTag(number));
	cell->setVisible(false);
}
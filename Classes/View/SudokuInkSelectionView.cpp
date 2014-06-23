#include "SudokuInkSelectionView.h"


SudokuInkSelectionView::SudokuInkSelectionView()
{
	ignoreAnchorPointForPosition(false);
	setAnchorPoint(Point(0.5f,1.0f));


	//create Ink and Pencil buttons
	Node* btnInk = ZZ::CreateSimpleButton("Ink", "chooseInk", "ui");
	Node* btnPencil = ZZ::CreateSimpleButton("Pencil", "choosePencil", "ui");

	Size area = btnPencil->getContentSize();
	area.width *= 3;
	setContentSize(area);

	LayerColor* bg = LayerColor::create(Color4B(255,255,255,255));
	bg->ignoreAnchorPointForPosition(false);
	bg->setAnchorPoint(Point(0,0));
	bg->setContentSize(area);
	addChild(bg);

	btnPencil->setAnchorPoint(Point(1.0f,0.0f));
	btnPencil->setPositionX(area.width );

	addChild(btnInk);
	addChild(btnPencil);

	m_btnInk = (LayerColor*)btnInk->getChildren().at(0);
	m_btnPencil = (LayerColor*)btnPencil->getChildren().at(0);
	
}


SudokuInkSelectionView::~SudokuInkSelectionView()
{
}

void SudokuInkSelectionView::setBtnInk( bool on )
{
	if( on )
	{
		m_btnInk->setColor(Color3B::GREEN);
		m_btnPencil->setColor(Color3B::GRAY);
	}
	else 
	{
		m_btnInk->setColor(Color3B::GRAY);
		m_btnPencil->setColor(Color3B::GREEN);
	}
}
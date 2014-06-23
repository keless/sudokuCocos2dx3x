#include "SudokuGridView.h"

#include <CCDrawingPrimitives.h>
#include <CCEventListenerTouch.h>
#include <CCDirector.h>



SudokuGridView::SudokuGridView(void)
{
	ignoreAnchorPointForPosition(false);

	m_pModel = NULL;

	int blockId = 0;
	for( int y=0; y< SUDOKU_GRID_BLOCKS_WIDE; y++)
	{
		for( int x=0; x< SUDOKU_GRID_BLOCKS_WIDE; x++)
		{
			Node* block = makeBlock(x, y);
			block->setPosition( x * (block->getContentSize().width + SUDOKU_GRID_BLOCK_MARGIN), 
								y * (block->getContentSize().height + SUDOKU_GRID_BLOCK_MARGIN) );
			block->setTag(blockId);

			addChild(block);
			blockId++;
		}
	}

	Node* blockOne = getChildren().at(0);
	Size contentSize = blockOne->getContentSize() * SUDOKU_GRID_BLOCKS_WIDE;
	contentSize.width += SUDOKU_GRID_BLOCK_MARGIN * (SUDOKU_GRID_BLOCKS_WIDE -1);
	contentSize.height += SUDOKU_GRID_BLOCK_MARGIN * (SUDOKU_GRID_BLOCKS_WIDE -1);
	setContentSize(contentSize);
}

Node* SudokuGridView::makeCell(int cellX, int cellY, int blockX, int blockY)
{
	SudokuGridCellView * cell = SudokuGridCellView::create("cellTouched", "ui");

		Json::Value data;
		data["blockX"] = blockX;
		data["blockY"] = blockY;
		data["cellX"] = cellX;
		data["cellY"] = cellY;
		cell->setData(data);

	return cell;
}

Node* SudokuGridView::makeBlock(int blockX, int blockY)
{
	Node* block = Node::create();
	block->ignoreAnchorPointForPosition(false);
	block->setAnchorPoint(Point(0,0));
	block->setContentSize(Size( SUDOKU_GRID_BLOCK_SIZE * (SUDOKU_GRID_CELL_SIZE + SUDOKU_GRID_CELL_MARGIN), 
								SUDOKU_GRID_BLOCK_SIZE * (SUDOKU_GRID_CELL_SIZE + SUDOKU_GRID_CELL_MARGIN) ) );

	int cellId = 0;
	for( int cellY=0; cellY< SUDOKU_GRID_BLOCK_SIZE; cellY++)
	{
		for( int cellX=0; cellX< SUDOKU_GRID_BLOCK_SIZE; cellX++)
		{
			Node* cell = makeCell(cellX, cellY, blockX, blockY);
			cell->setPosition(	cellX * (SUDOKU_GRID_CELL_SIZE + SUDOKU_GRID_CELL_MARGIN), 
								cellY * (SUDOKU_GRID_CELL_SIZE + SUDOKU_GRID_CELL_MARGIN));
			cell->setTag( cellId );
			block->addChild(cell);
			log("add cell id %d", cellId);
			cellId++;
		}
	}
	return block;
}

SudokuGridView::~SudokuGridView(void)
{
}

void SudokuGridView::setModel( SudokuSheetModel* pModel )
{
	m_pModel = pModel;
}

void SudokuGridView::updateView(int highlightNumber)
{
	if(!m_pModel) return;

	for( int blockY=0; blockY< SUDOKU_GRID_BLOCKS_WIDE; blockY++)
	{
		for( int blockX=0; blockX< SUDOKU_GRID_BLOCKS_WIDE; blockX++)
		{
			for( int cellY=0; cellY< SUDOKU_GRID_BLOCK_SIZE; cellY++)
			{
				for( int cellX=0; cellX< SUDOKU_GRID_BLOCK_SIZE; cellX++)
				{
					updateSpecificCell(cellX, cellY, blockX, blockY, highlightNumber);
				}
			}
		}
	}
}

void SudokuGridView::updateSpecificCell(int cellX, int cellY, int blockX, int blockY, int highlightNumber)
{
	if(!m_pModel) return;

	SudokuCellModel cellModel = m_pModel->getCellInBlock(cellX, cellY, blockX, blockY);
	SudokuGridCellView* cellView = this->getCellInBlock(cellX, cellY, blockX, blockY);

	cellView->updateWithModel(cellModel, highlightNumber);
}

SudokuGridCellView* SudokuGridView::getCellInBlock( int cellX, int cellY, int blockX, int blockY )
{
	int cellIdx = CALC_2DARR_IDX(cellX, cellY, SUDOKU_GRID_BLOCK_SIZE);
	int blockIdx = CALC_2DARR_IDX(blockX, blockY, SUDOKU_GRID_BLOCKS_WIDE);

	Node* block = getChildByTag( blockIdx );
	if( block == NULL ) return NULL;

	SudokuGridCellView* cell = dynamic_cast<SudokuGridCellView*>( block->getChildByTag( cellIdx ) );
	return cell;
}

void SudokuGridView::draw(Renderer *renderer, const kmMat4& transform, bool transformUpdated)
{
	

    _customDrawCommand.init(_globalZOrder + 1);
    _customDrawCommand.func = CC_CALLBACK_0(SudokuGridView::onDrawPrimitives, this, transform, transformUpdated);
    renderer->addCommand(&_customDrawCommand);

	Node::draw(renderer, transform, transformUpdated);
}

void SudokuGridView::onDrawPrimitives(const kmMat4 &transform, bool transformUpdated)
{
	if(!isVisible()) return;

	kmGLPushMatrix();
    kmGLLoadMatrix(&transform);

    //add your primitive drawing code here
	DrawPrimitives::setDrawColor4B(0xFF, 0, 0, 0xFF);
	DrawPrimitives::drawRect(Point(0,0), Point(getContentSize().width,getContentSize().height)  );
}

void SudokuGridView::setVisible( bool visible )
{
	Layer::setVisible(visible);

	//toggle visibility of cell views so that they dont absorb touches when grid is invisible

	for( int i=0; i< getChildrenCount(); i++)
	{
		auto cells = _children.at(i)->getChildren();
		for( int j=0; j < cells.size(); j++)
		{
			cells.at(j)->setVisible(visible);
		}
	}
}

//static
SudokuGridCellView *SudokuGridCellView::create(const std::string evt, const std::string bus)
{
    SudokuGridCellView *ret = new SudokuGridCellView(evt, bus);
	ret->autorelease();
	return ret;
}

SudokuGridCellView::SudokuGridCellView( const std::string evt, const std::string bus ) 
	: ZZ::TouchableNode(evt, bus)
{

	ignoreAnchorPointForPosition(false);
	setAnchorPoint(Point(0,0));

	LayerColor * bg = LayerColor::create(Color4B(0xFF,0xFF,0xFF,0xFF), SUDOKU_GRID_CELL_SIZE, SUDOKU_GRID_CELL_SIZE);
	bg->ignoreAnchorPointForPosition(false);
	bg->setAnchorPoint(Point(0,0));
	addChild(bg);

	Size area = bg->getContentSize();

	m_lblInk = Label::createWithSystemFont(std::to_string(9), "Arial", 24);
	m_lblInk->setColor(Color3B::RED);
	m_lblInk->setPosition( Point( area.width/2, area.height/2 ) );

	addChild(m_lblInk);
	//TODO: m_lblInk->setVisible(false);

	for( int i=0; i< TOTAL_VALUES; i++)
	{
		int x = i % 3;
		int y = (i-x) / 3;

		m_lblPencils[i] = Label::createWithSystemFont( std::to_string(i+1), "Arial", 12, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
		m_lblPencils[i]->setColor(Color3B::BLUE);
		//m_lblPencils[i]->setAnchorPoint(Point(0.5f,0.5f));
		m_lblPencils[i]->setPosition( Point( (x+1) * (area.width/4), (y+1) * (area.width/4) ) );

		addChild(m_lblPencils[i]);

		m_lblPencils[i]->setVisible(false);
	}

	setContentSize(bg->getContentSize());
}

SudokuGridCellView::~SudokuGridCellView()
{
}

void SudokuGridCellView::updateWithModel(const SudokuCellModel& model, int selectedNumber  )
{
	
	if( model.inkValue != 0 )
	{
		//set ink
		bool correctInk = model.inkValue == model.value;
		setInk( model.inkValue, correctInk, model.inkValue == selectedNumber );
	}
	else 
	{
		int selectedIdx = selectedNumber -1;
		//check pencils
		for( int i=0; i<TOTAL_VALUES; i++)
		{
			setPencil( i, model.pencilValues[i], i == selectedIdx );
		}
	}
}

void SudokuGridCellView::setInk( int val, bool correct, bool highlighted )
{
	m_lblInk->setString(std::to_string(val));

	if( highlighted )
	{
		m_lblInk->setColor(Color3B::GREEN);
	}
	else if( correct ) 
	{
		m_lblInk->setColor(Color3B::GRAY);
	}
	else 
	{
		m_lblInk->setColor(Color3B::RED);
	}

	m_lblInk->setVisible(true);


	for( int i=0; i<TOTAL_VALUES; i++)
	{
		m_lblPencils[i]->setVisible(false);
	}
}

void SudokuGridCellView::setPencil( int val, bool on, bool highlighted )
{
	if( val < 0 || val >= TOTAL_VALUES ) return;

	m_lblInk->setVisible(false);

	m_lblPencils[val]->setVisible(on);
	if(on)
	{
		if( highlighted ) 
		{
			m_lblPencils[val]->setColor(Color3B::GREEN);
		}
		else
		{
			m_lblPencils[val]->setColor(Color3B::BLUE);
		}
	}
}

void SudokuGridCellView::setInkHighlight( Color3B color )
{
	m_lblInk->setColor(color);
}

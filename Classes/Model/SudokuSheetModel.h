#pragma once

#include <ZZ\ZZUtils.h>
#include "GameDefines.h"


/*
 A SudokuSheetModel is a 3x3 group of blocks, each of which is a 3x3 group of cells
	
	generated events:
	SACEvent
*/

struct SudokuCellModel;
class SudokuBlockModel;

#define JSON_MODEL_SECTION_START "start"
#define JSON_MODEL_SECTION_SOLUTION "solution"
#define JSON_MODEL_SECTION_PENCILS "pencils"

class SudokuSheetModel
{
	SudokuBlockModel* m_blocks[NUM_BLOCKS];

public:
	SudokuSheetModel();
	~SudokuSheetModel();

	bool loadFromJson( JsonValueRef puzzleJson );

	//TODO: implement this
	Json::Value serializeState();

	SudokuCellModel getCellInBlock( int cellX, int cellY, int blockX, int blockY );

	bool writeToCell( int cellX, int cellY, int blockX, int blockY, int value, bool inInk );
	bool eraseInCell( int cellX, int cellY, int blockX, int blockY, int value, bool inInk );

	bool doesRowHaveValue( int cellY, int blockY, int value );
	bool doesColHaveValue( int cellX, int blockX, int value );
	bool doesBlockHaveValue( int blockX, int blockY, int value );

	bool isValueExhausted( int value );
	bool isPuzzleComplete();

	void erasePencilValueInBlock( int blockX, int blockY, int value );
	void erasePencilValueInColumn( int cellX, int blockX, int value );
	void erasePencilValueInRow( int cellY, int blockY, int value );
	void erasePencilValueInSheet( int value );
};

enum eSudokuBlockStatus
{
	BLOCK_STATE_COMPLETED,
	BLOCK_STATE_ERROR,
	BLOCK_STATE_IN_PROGRESS
};

struct SudokuCellModel
{

	int value;
	int inkValue;
	bool pencilValues[TOTAL_VALUES];

	SudokuCellModel();
};

class SudokuBlockModel
{
	SudokuCellModel m_cells[NUM_CELLS_PER_BLOCK];

public:

	SudokuBlockModel();

	eSudokuBlockStatus getStatus();

	

	friend class SudokuSheetModel;
};


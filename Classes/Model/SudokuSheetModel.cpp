#include "SudokuSheetModel.h"

#include "Controller\SudokuActionCommand.h"

SudokuSheetModel::SudokuSheetModel( )
{
	for( int i=0; i < NUM_BLOCKS; i++)
	{
		m_blocks[i] = new SudokuBlockModel( );
	}
}


SudokuSheetModel::~SudokuSheetModel(void)
{
	for( int i=0; i < NUM_BLOCKS; i++)
	{
		delete m_blocks[i];
	}
}




bool SudokuSheetModel::loadFromJson( JsonValueRef puzzleJson )
{
	if( !puzzleJson.isMember(JSON_MODEL_SECTION_START) || !puzzleJson.isMember(JSON_MODEL_SECTION_SOLUTION) )
	{
		//missing neccesary sections
		log("error: puzzle json missing neccesary sections");
		return false;
	}

	JsonValueRef start = puzzleJson[JSON_MODEL_SECTION_START];
	if(!start.isArray()) return false;

	JsonValueRef solution = puzzleJson[JSON_MODEL_SECTION_SOLUTION];
	if(!solution.isArray()) return false;

	JsonValueRef pencils = puzzleJson[JSON_MODEL_SECTION_PENCILS];

	int idx = 0;

	for( int blockY = (BLOCK_STRIDE-1); blockY >= 0; blockY--)
	{
		for( int cellY = (CELL_STRIDE-1); cellY >= 0; cellY--)
		{
			for( int blockX = 0; blockX < BLOCK_STRIDE; blockX++)
			{
				for( int cellX = 0; cellX < CELL_STRIDE; cellX++)
				{
					int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
					int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

					int ink = start[idx].asInt();
					int val = solution[idx].asInt();

					if( ink != 0 && ink != val )
					{
						log("error: a starting val not equal to actual solution");
						return false;
					}

					m_blocks[blockIdx]->m_cells[cellIdx].inkValue = ink;
					m_blocks[blockIdx]->m_cells[cellIdx].value = val;

					
					if( pencils.size() > 0 )
					{
						unsigned int pencilField = pencils[idx].asUInt();

						//pencil field is a bit field, where b000001 = "1", b0000010 = "2", b0000100 = "3" and so on
						for( int i=0; i< TOTAL_VALUES; i++)
						{
							int test =  (1 << i);
							m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[i] = ( (pencilField & (1 << i)) != 0 );
						}
					}
					else 
					{
						for( int i=0; i< TOTAL_VALUES; i++)
						{
							m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[i] = false;
						}
					}

					idx++;
				}
			}
		}
	}

	return true;
}

Json::Value SudokuSheetModel::serializeState()
{
	Json::Value game, start, solution, pencils;

	int idx = 0;
	for( int blockY = (BLOCK_STRIDE-1); blockY >= 0; blockY--)
	{
		for( int cellY = (CELL_STRIDE-1); cellY >= 0; cellY--)
		{
			for( int blockX = 0; blockX < BLOCK_STRIDE; blockX++)
			{
				for( int cellX = 0; cellX < CELL_STRIDE; cellX++)
				{
					int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
					int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

					int ink = m_blocks[blockIdx]->m_cells[cellIdx].inkValue;
					int val = m_blocks[blockIdx]->m_cells[cellIdx].value;

					start.append( ink );
					solution.append(val);

					//pencil field is a bit field, where b000001 = "1", b0000010 = "2", b0000100 = "3" and so on
					unsigned int pencilField = 0;
					for( int i=0; i< TOTAL_VALUES; i++)
					{
						int test = ( 1 << i );
						if( m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[i] )
						{
							pencilField |= ( 1 << i );
							log( " add pencilField: %x", ( 1 << i ));
						}
					}
					log( "store pencilField: %x", pencilField);

					pencils.append(pencilField);

					idx++;
				}
			}
		}
	}

	game[JSON_MODEL_SECTION_START] = start;
	game[JSON_MODEL_SECTION_SOLUTION] = solution;
	game[JSON_MODEL_SECTION_PENCILS] = pencils;

	return game;
}

SudokuCellModel SudokuSheetModel::getCellInBlock( int cellX, int cellY, int blockX, int blockY )
{
	int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
	int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);
	

	return m_blocks[blockIdx]->m_cells[cellIdx];
}

bool SudokuSheetModel::writeToCell( int cellX, int cellY, int blockX, int blockY, int value, bool inInk )
{
	int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
	int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

	if( inInk && m_blocks[blockIdx]->m_cells[cellIdx].inkValue != 0 )
	{
		log("error: trying to write ink over ink");
		return false;
	}

	if( !inInk && m_blocks[blockIdx]->m_cells[cellIdx].inkValue != 0 )
	{
		log("error: trying to write pencil over ink");
		return false;
	}

	if( value == 0 || value > TOTAL_VALUES ) 
	{
		log("error: value out of bounds");
		return false;
	}

	bool gameOverLose = false;
	bool gameOverWin = false;
	bool valueExhausted = false;
	bool pencilErased = false;
	if( inInk ) 
	{
		m_blocks[blockIdx]->m_cells[cellIdx].inkValue = value;

		
		if(m_blocks[blockIdx]->m_cells[cellIdx].value != value )
		{
			log("player lost");
			gameOverLose = true;
		}
		else if(isValueExhausted(value))
		{
			log("value exhausted");
			if( isPuzzleComplete() )
			{
				log("puzzle completed");
				gameOverWin = true;
			}
			else 
			{
				erasePencilValueInSheet(value);
				valueExhausted = true;
			}
		}
		else 
		{
			erasePencilValueInBlock(blockX, blockY, value);
			erasePencilValueInColumn(cellX, blockX, value);
			erasePencilValueInRow(cellY, blockY, value);
		}
	}
	else 
	{
		int valueIdx = value - 1;
		m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx] = !m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx];

		if(!m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx]) pencilErased = true;
	}

	
	//todo: send pencil erased instead of write if erased detected
	SACEvent commandEvent( inInk ? SAC_WRITE_INK : SAC_WRITE_PENCIL );
	commandEvent.cmd.data["blockX"] = blockX;
	commandEvent.cmd.data["blockY"] = blockY;
	commandEvent.cmd.data["cellX"] = cellX;
	commandEvent.cmd.data["cellY"] = cellY;
	commandEvent.cmd.data["val"] = value;
	ZZ::EventBus::game()->dispatch(&commandEvent);

	if( gameOverLose )
	{
		SACEvent commandEvent( SAC_GAME_OVER_LOSE );
		ZZ::EventBus::game()->dispatch(&commandEvent);
	}
	else if( gameOverWin )
	{
		SACEvent commandEvent( SAC_GAME_OVER_WIN );
		ZZ::EventBus::game()->dispatch(&commandEvent);
	}
	else if( valueExhausted )
	{
		SACEvent commandEvent( SAC_EXHAUSTED_NUMBER );
		commandEvent.cmd.data["val"] = value;
		ZZ::EventBus::game()->dispatch(&commandEvent);
	}

	return true;
}

void SudokuSheetModel::erasePencilValueInBlock( int blockX, int blockY, int value )
{
	int valueIdx = value - 1;
	for( int cellY = 0; cellY < SUDOKU_GRID_BLOCK_SIZE; cellY++)
	{
		for( int cellX = 0; cellX < SUDOKU_GRID_BLOCK_SIZE; cellX++)
		{
			int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
			int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

			m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx] = false;
		}
	}

}
void SudokuSheetModel::erasePencilValueInColumn( int cellX, int blockX, int value )
{
	int valueIdx = value - 1;
	for( int blockY = 0; blockY < SUDOKU_GRID_BLOCKS_WIDE; blockY++)
	{
		for( int cellY = 0; cellY < SUDOKU_GRID_BLOCK_SIZE; cellY++)
		{
			int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
			int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

			m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx] = false;
		}
	}
}
void SudokuSheetModel::erasePencilValueInRow( int cellY, int blockY, int value )
{
	int valueIdx = value - 1;
	for( int blockX = 0; blockX < SUDOKU_GRID_BLOCKS_WIDE; blockX++)
	{
		for( int cellX = 0; cellX < SUDOKU_GRID_BLOCK_SIZE; cellX++)
		{
			int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
			int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

			m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx] = false;
		}
	}
}
void SudokuSheetModel::erasePencilValueInSheet( int value )
{
	int valueIdx = value - 1;

	for( int blockY=0; blockY< SUDOKU_GRID_BLOCKS_WIDE; blockY++)
	{
		for( int blockX=0; blockX< SUDOKU_GRID_BLOCKS_WIDE; blockX++)
		{
			for( int cellY=0; cellY< SUDOKU_GRID_BLOCK_SIZE; cellY++)
			{
				for( int cellX=0; cellX< SUDOKU_GRID_BLOCK_SIZE; cellX++)
				{
					int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
					int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

					m_blocks[blockIdx]->m_cells[cellIdx].pencilValues[valueIdx] = false;
				}
			}
		}
	}
}

bool SudokuSheetModel::doesRowHaveValue( int cellY, int blockY, int value )
{
	for( int blockX = 0; blockX < SUDOKU_GRID_BLOCKS_WIDE; blockX++)
	{
		for( int cellX = 0; cellX < SUDOKU_GRID_BLOCK_SIZE; cellX++)
		{
			int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
			int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

			if( m_blocks[blockIdx]->m_cells[cellIdx].inkValue == value ) 
			{
				return true;	
			}
		}
	}

	return false;
}

bool SudokuSheetModel::doesColHaveValue( int cellX, int blockX, int value )
{
	for( int blockY = 0; blockY < SUDOKU_GRID_BLOCKS_WIDE; blockY++)
	{
		for( int cellY = 0; cellY < SUDOKU_GRID_BLOCK_SIZE; cellY++)
		{
			int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
			int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

			if( m_blocks[blockIdx]->m_cells[cellIdx].inkValue == value ) 
			{
				return true;	
			}
		}
	}

	return false;
}

bool SudokuSheetModel::doesBlockHaveValue( int blockX, int blockY, int value )
{
	for( int cellY = 0; cellY < SUDOKU_GRID_BLOCK_SIZE; cellY++)
	{
		for( int cellX = 0; cellX < SUDOKU_GRID_BLOCK_SIZE; cellX++)
		{
			int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
			int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

			if( m_blocks[blockIdx]->m_cells[cellIdx].inkValue == value ) 
			{
				return true;	
			}
		}
	}

	return false;
}

bool SudokuSheetModel::isPuzzleComplete()
{
	for( int i=0; i< NUM_BLOCKS; i++)
	{
		if(m_blocks[i]->getStatus() != BLOCK_STATE_COMPLETED )
		{
			return false;
		}
	}

	return true;
}

bool SudokuSheetModel::isValueExhausted( int value )
{
	if( value <= 0 || value > TOTAL_VALUES ) return false;

	int count = 0;

	for( int blockY=0; blockY< SUDOKU_GRID_BLOCKS_WIDE; blockY++)
	{
		for( int blockX=0; blockX< SUDOKU_GRID_BLOCKS_WIDE; blockX++)
		{
			for( int cellY=0; cellY< SUDOKU_GRID_BLOCK_SIZE; cellY++)
			{
				for( int cellX=0; cellX< SUDOKU_GRID_BLOCK_SIZE; cellX++)
				{
					int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
					int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

					if( m_blocks[blockIdx]->m_cells[cellIdx].inkValue == value ) 
					{
						count++;
					}
				}
			}
		}
	}

	return (count == NUM_BLOCKS);
}

bool SudokuSheetModel::eraseInCell( int cellX, int cellY, int blockX, int blockY, int value, bool inInk )
{
	int cellIdx = CALC_2DARR_IDX(cellX, cellY, CELL_STRIDE);
	int blockIdx = CALC_2DARR_IDX(blockX, blockY, BLOCK_STRIDE);

	//TODO: implement this

	return false;
}

SudokuBlockModel::SudokuBlockModel( )
{
	for( int i=0; i < NUM_CELLS_PER_BLOCK; i++ )
	{
		m_cells[i].value = 0;
	}
}


//todo: unit test this
eSudokuBlockStatus SudokuBlockModel::getStatus()
{
	bool exists[TOTAL_VALUES];
	for( int i=0; i< TOTAL_VALUES; i++)
	{
		exists[i] = false;
	}

	bool outOfPlace = false;
	int numFound = 0;
	for( int i=0; i< NUM_CELLS_PER_BLOCK; i++)
	{
		int cellValue = m_cells[i].inkValue; //0 if unset, or range 1-9
		if( cellValue == 0 || cellValue > TOTAL_VALUES ) continue; //cell not marked

		if( cellValue != m_cells[i].value ) 
		{
			outOfPlace = true;
		}

		if( exists[ (cellValue-1) ] ) {
			//found duplicate, so block is in error
			return BLOCK_STATE_ERROR;
		}

		exists[ (cellValue-1) ] = true;
		numFound++;
	}

	if( numFound == TOTAL_VALUES && !outOfPlace )
	{
		return BLOCK_STATE_COMPLETED;
	}

	return BLOCK_STATE_IN_PROGRESS;
}

SudokuCellModel::SudokuCellModel()
{
	value = 0;
	inkValue = 0;
	for( int i=0; i< TOTAL_VALUES; i++)
	{
		pencilValues[i] = false;
	}
}
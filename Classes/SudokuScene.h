#ifndef __SUDOKU_SCENE_H__
#define __SUDOKU_SCENE_H__

#include "cocos2d.h"

#include "Model\SudokuSheetModel.h"
#include "View\SudokuGridView.h"
#include "View\SudokuInkSelectionView.h"
#include "View\SudokuNumberSelectionView.h"
#include "Controller\SudokuActionCommand.h"

enum eGameState
{
	MainMenu,
	GamePlay,
	GamePaused,
	GameOver
};

class SudokuScene : public cocos2d::Layer
{
	eGameState m_state;

	SudokuNumberSelectionView* m_numberSelections;
	SudokuInkSelectionView* m_inkSelection;
	SudokuGridView* m_gridView;
	SudokuSheetModel* m_sheetModel;
	
	//note: if we care about performance -- since SAC holds a JSON::Value, resizing this array could take a while if history is large
	std::vector< SudokuActionCommand > m_history;

	int		m_numSelected;
	bool	m_inkMode; //true if ink, false if pencil

	~SudokuScene();
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
	bool onTouchBegan(Touch *pTouch, Event *pEvent);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event);

	void onNumberSelected( cocos2d::Ref* e );
	void onCellSelected( cocos2d::Ref* e );
	void onSudokuActionCommand( cocos2d::Ref* e );
	void onSelectInkMode( cocos2d::Ref* e );
	void onSelectPencilMode( cocos2d::Ref* e );

	void saveCurrentGame();
	void loadSavedGame();

	void enterStateGamePlay( bool newGame );
	void enterStateGamePaused();
	void enterStateMainMenu();
	void enterStateGameOver( bool playerWon );

    // implement the "static create()" method manually
    CREATE_FUNC(SudokuScene);
};

#endif // __HELLOWORLD_SCENE_H__

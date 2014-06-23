#include "SudokuScene.h"

USING_NS_CC;

#include "ZZ\ZZUtils.h"

#define SAVE_GAME_FILE_NAME "saveGame.sudoku"

Scene* SudokuScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SudokuScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

SudokuScene::~SudokuScene()
{
	if( m_state == GamePlay || m_state == GamePaused )
	{
		saveCurrentGame();
	}

	ZZ::EventBus::get("ui")->remListener("numberSelected", this, callfuncO_selector(SudokuScene::onNumberSelected));
	ZZ::EventBus::get("ui")->remListener("cellTouched", this, callfuncO_selector(SudokuScene::onCellSelected));
	ZZ::EventBus::get("ui")->remListener("chooseInk", this, callfuncO_selector(SudokuScene::onSelectInkMode));
	ZZ::EventBus::get("ui")->remListener("choosePencil", this, callfuncO_selector(SudokuScene::onSelectPencilMode));
	ZZ::EventBus::game()->remListener("SACEvent", this, callfuncO_selector(SudokuScene::onSudokuActionCommand));

	if( m_sheetModel != NULL ) 
	{
		delete m_sheetModel;
	}
}

// on "init" you need to initialize your instance
bool SudokuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	m_state = MainMenu;

	m_sheetModel = NULL;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(SudokuScene::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);


	auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(SudokuScene::onTouchBegan, this);
    //touchListener->onTouchMoved = CC_CALLBACK_2(SudokuScene::onTouchMoved, this);
    //touchListener->onTouchEnded = CC_CALLBACK_2(SudokuScene::onTouchEnded, this);
    //touchListener->onTouchCancelled = CC_CALLBACK_2(SudokuScene::onTouchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this); //shouldnt this get removed?
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(SudokuScene::onKeyPressed, this);
	//keyboardListener->onKeyReleased = CC_CALLBACK_2(OurScene::keyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this); 
    
	/*
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);
	*/

    // add "SudokuScene" splash screen"
	/*
    auto sprite = Sprite::create("HelloWorld.png");
    sprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(sprite, 0);
	*/

	//Set up views
	m_gridView = new SudokuGridView();
	m_gridView->setAnchorPoint(Point(0.5f, 0.5f));
	m_gridView->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	addChild(m_gridView);

	m_numberSelections = new SudokuNumberSelectionView();
	m_numberSelections->setPositionX(visibleSize.width/2 + origin.x);
	addChild(m_numberSelections);

	m_inkMode = true;
	m_numSelected = 0;

	//TODO: set up ink/pencil selection UI
	m_inkSelection = new SudokuInkSelectionView();
	m_inkSelection->setPositionY( visibleSize.height + origin.y );
	m_inkSelection->setPositionX( visibleSize.width/2 + origin.x );
	addChild(m_inkSelection);

	m_inkSelection->setBtnInk( m_inkMode );

	//Set up model
	m_sheetModel = NULL;

	/* moved to on enter
	Json::Value puzzleJson = ZZ::ReadFileToJson("puzzles/puzzle.json");
	m_sheetModel = new SudokuSheetModel();
	m_sheetModel->loadFromJson(puzzleJson);
	m_gridView->setModel(m_sheetModel);
	*/

	
	
    
	ZZ::EventBus::get("ui")->addListener("numberSelected", this, callfuncO_selector(SudokuScene::onNumberSelected));
	ZZ::EventBus::get("ui")->addListener("cellTouched", this, callfuncO_selector(SudokuScene::onCellSelected));
	ZZ::EventBus::get("ui")->addListener("chooseInk", this, callfuncO_selector(SudokuScene::onSelectInkMode));
	ZZ::EventBus::get("ui")->addListener("choosePencil", this, callfuncO_selector(SudokuScene::onSelectPencilMode));
	ZZ::EventBus::game()->addListener("SACEvent", this, callfuncO_selector(SudokuScene::onSudokuActionCommand));
	//TODO: remove on destructor

	enterStateMainMenu();

    return true;
}


void SudokuScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

	switch( m_state )
	{
	case GamePaused:
		enterStateGamePlay(false);
		break;

	case GamePlay:
		enterStateGamePaused();
		break;

	case MainMenu:
	case GameOver:
	default:
		Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif
	}
}

void SudokuScene::saveCurrentGame()
{
	if( m_state == GamePlay || m_state == GamePaused )
	{
		Json::Value jsonGame = m_sheetModel->serializeState();
		ZZ::WriteJsonToFile(jsonGame, SAVE_GAME_FILE_NAME);
	}
	else 
	{
		log("no current game available to save");
	}
}

void SudokuScene::loadSavedGame()
{
	Json::Value jsonGame = ZZ::ReadFileToJson(SAVE_GAME_FILE_NAME);
	if( jsonGame.isObject() )
	{
		m_sheetModel->loadFromJson( jsonGame );
		ZZ::RemoveFile( SAVE_GAME_FILE_NAME );
	}
	else 
	{
		log("error: unable to load saved game");
	}
}

void SudokuScene::enterStateGamePlay( bool newGame )
{
	Node* lbl = this->getChildByTag(999991);
	if(lbl) lbl->removeFromParent();

	if(newGame)
	{

		if( m_sheetModel != NULL ) delete m_sheetModel;

		m_sheetModel = new SudokuSheetModel();

		Json::Value puzzleJson;

		//Set up model
		if( ZZ::IsFile( SAVE_GAME_FILE_NAME, true ) )
		{
			loadSavedGame();
		}
		else 
		{
			puzzleJson = ZZ::ReadFileToJson("puzzles/puzzle.json");
			m_sheetModel->loadFromJson(puzzleJson);
		}

		m_gridView->setModel(m_sheetModel);
		m_gridView->updateView(m_numSelected);

		m_inkMode = true;
		m_numSelected = 0;

		m_inkSelection->setBtnInk(m_inkMode);
		m_numberSelections->setSelection(m_numSelected);
		m_numberSelections->reset(m_sheetModel);
	}

	m_numberSelections->setVisible(true);
	m_inkSelection->setVisible(true);

	m_gridView->setVisible(true);

	m_state = GamePlay;
}

void SudokuScene::enterStateGamePaused()
{
	m_numberSelections->setVisible(false);
	m_inkSelection->setVisible(false);
	m_gridView->setVisible(false);

	Label* lbl = Label::createWithSystemFont("Game Paused", "Arial", 32, Size(400,140), TextHAlignment::CENTER, TextVAlignment::CENTER);
	lbl->setTag(999991);
	lbl->setPosition(Point(getContentSize().width/2, getContentSize().height/2));
	addChild(lbl);
	m_state = GamePaused;
}

void SudokuScene::enterStateMainMenu()
{
	Node* lbl = this->getChildByTag(999991);
	if(lbl) lbl->removeFromParent();

	m_numberSelections->setVisible(false);
	m_inkSelection->setVisible(false);
	m_gridView->setVisible(false);

	std::string text;
	if( ZZ::IsFile( SAVE_GAME_FILE_NAME, true ) )
	{
		text = "Click to Continue";
	} else {
		text = "Click to Begin";
	}

	lbl = Label::createWithSystemFont(text, "Arial", 32, Size(400,140), TextHAlignment::CENTER, TextVAlignment::CENTER);
	lbl->setTag(999991);
	lbl->setPosition(Point(getContentSize().width/2, getContentSize().height/2));
	addChild(lbl);
	m_state = MainMenu;
}

void SudokuScene::enterStateGameOver( bool playerWon )
{
	std::string text;
	Color3B color;
	if( playerWon )
	{
		text = "YOU WON!";
		color = Color3B::YELLOW;
	}
	else 
	{
		text = "GAME OVER";
		color = Color3B::RED;
	}

	Label* lbl = Label::createWithSystemFont(text, "Arial", 32, Size(400,140), TextHAlignment::CENTER, TextVAlignment::CENTER);
	lbl->setColor(color);
	lbl->enableOutline(Color4B::BLACK);
	lbl->setTag(999991);
	lbl->setPosition(Point(getContentSize().width/2, getContentSize().height/2));
	addChild(lbl);
	m_state = GameOver;
}

bool SudokuScene::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	switch( m_state )
	{
	case MainMenu:
		enterStateGamePlay(true);
		break;
	case GamePaused:
		enterStateGamePlay(false);
		break;
	case GamePlay:
		return false;
		break;
	case GameOver:
		enterStateMainMenu();
		break;
	}

	return true;
}

void SudokuScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event)
{
	ZZ::JsonEvent evt("");
	switch( keyCode )
	{
		case EventKeyboard::KeyCode::KEY_SPACE:
			if(m_inkMode) {
				onSelectPencilMode(NULL);
			} else {
				onSelectInkMode(NULL);
			}
		break;

		case EventKeyboard::KeyCode::KEY_1:
			evt.json["val"] = 1;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_2:
			evt.json["val"] = 2;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_3:
			evt.json["val"] = 3;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_4:
			evt.json["val"] = 4;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_5:
			evt.json["val"] = 5;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_6:
			evt.json["val"] = 6;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_7:
			evt.json["val"] = 7;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_8:
			evt.json["val"] = 8;
			onNumberSelected( &evt );
		break;
		case EventKeyboard::KeyCode::KEY_9:
			evt.json["val"] = 9;
			onNumberSelected( &evt );
		break;
	}


}

void SudokuScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event)
{

}


void SudokuScene::onNumberSelected( Ref* e )
{
	if( m_state != GamePlay ) 
	{
		onTouchBegan(NULL, NULL);
		return;
	}

	ZZ::JsonEvent* evt = dynamic_cast<ZZ::JsonEvent*>(e);
	if(!evt) return;

	int number = evt->json["val"].asInt();

	log("number selected: %d", number );

	m_numSelected = number;

	//update numberSelectionView

	m_numberSelections->setSelection( number );
	m_gridView->updateView(m_numSelected);

}

void SudokuScene::onCellSelected( Ref* e )
{
	if( m_state != GamePlay ) 
	{
		onTouchBegan(NULL, NULL);
		return;
	}

	ZZ::JsonEvent* evt = dynamic_cast<ZZ::JsonEvent*>(e);
	if(!evt) return;

	int blockX = evt->json["blockX"].asInt();
	int blockY = evt->json["blockY"].asInt();
	int cellX = evt->json["cellX"].asInt();
	int cellY = evt->json["cellY"].asInt();

	log("cell selected: %d,%d of block %d,%d", cellX, cellY, blockX, blockY );

	//submit change to model, we'll receive a SACEvent if it succeeds and update UI then
	m_sheetModel->writeToCell( cellX, cellY, blockX, blockY, m_numSelected, m_inkMode );
}

void SudokuScene::onSudokuActionCommand( cocos2d::Ref* e )
{
	if( m_state != GamePlay ) return;

	SACEvent* evt = dynamic_cast<SACEvent*>(e);
	if(!evt) return;

	int cellX, cellY, blockX, blockY, value;
	JsonValueRef data = evt->cmd.data;

	switch( evt->cmd.type )
	{
	case SAC_GAME_OVER_WIN:
		enterStateGameOver(true);
		break;

	case SAC_GAME_OVER_LOSE:
		enterStateGameOver(false);
		break;

	case SAC_WRITE_INK:
		//a whole lot of changes could have happened, update the entire view
		m_gridView->updateView(m_numSelected);
		break;

	case SAC_EXHAUSTED_NUMBER:
		value = data["val"].asInt();
		m_numberSelections->numberExhausted( value );
		m_numSelected = 0;
		break;

	case SAC_ERASE_PENCIL:
	case SAC_WRITE_PENCIL:
		//just update the one cellE
		blockX = data["blockX"].asInt();
		blockY = data["blockY"].asInt();
		cellX = data["cellX"].asInt();
		cellY = data["cellY"].asInt();
		value = data["val"].asInt();

		m_gridView->updateSpecificCell(cellX, cellY, blockX, blockY, m_numSelected);

		if(m_sheetModel->isValueExhausted(value))
		{
			//value series completed, update UI, check win
		}
		break;
	}
}

void SudokuScene::onSelectInkMode( cocos2d::Ref* e )
{
	if( m_state != GamePlay ) return;

	m_inkMode = true;
	m_inkSelection->setBtnInk( m_inkMode );
}

void SudokuScene::onSelectPencilMode( cocos2d::Ref* e )
{
	if( m_state != GamePlay ) return;

	m_inkMode = false;
	m_inkSelection->setBtnInk( m_inkMode );
}

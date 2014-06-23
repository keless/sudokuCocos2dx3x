#include "ZZUtils.h"

#include <fstream>

namespace ZZ {

bool IsFile(  const std::string& fileName, bool checkWritablePath ) 
{
	bool found = FileUtils::getInstance()->isFileExist(fileName.c_str());
	if(!found && checkWritablePath)
	{
		std::string path = FileUtils::getInstance()->fullPathForFilename(fileName.c_str());
		std::string writable = FileUtils::getInstance()->getWritablePath();
		path = writable + path;

		found = FileUtils::getInstance()->isFileExist(path.c_str());
	}

	return found;
}

Json::Value ReadFileToJson(  const std::string& fileName )
{
	std::string path = FileUtils::getInstance()->fullPathForFilename(fileName.c_str());

	if(!IsFile(fileName)) {
		//try reading from the writable directory instead
		
		std::string writable = FileUtils::getInstance()->getWritablePath();
		path = writable + path;
	}
	
	//bool bRet = false;
	Data data = FileUtils::getInstance()->getDataFromFile(path);
	ssize_t size = data.getSize();
    char* pBuffer = (char*)data.getBytes();
    if (pBuffer == NULL || size <= 0)
    {
		log("ERROR: unable to open file, or no bytes, at %s", path.c_str());
		return Json::Value();
    }
	std::string strJson(pBuffer, size);

	Json::Value root;
	Json::Reader reader;
	reader.parse(strJson, root);

    return root;

}

bool WriteJsonToFile( const Json::Value& json,  const std::string& fileName )
{
	std::string writable = FileUtils::getInstance()->getWritablePath();
	writable += fileName;
	
	std::string path = FileUtils::getInstance()->fullPathForFilename(writable.c_str());

	Json::FastWriter writer;
	std::string strJson = writer.write( json );

	std::ofstream file; //(path);
	file.open( path.c_str(), std::ios::out | std::ios::binary );
	file << strJson;
	file.close();

	return true;
}

bool RemoveFile( const std::string& fileName )
{
	std::string path = FileUtils::getInstance()->fullPathForFilename(fileName.c_str());
	if(!FileUtils::getInstance()->isFileExist(fileName.c_str()))
	{
		
		std::string writable = FileUtils::getInstance()->getWritablePath();
		path = writable + path;

	}

	return remove( path.c_str() ) == 0;
}

Layer* CreateSimplePopup( std::string msg, std::string btnLabel, std::string okEvt, std::string bus )
{
	Size area = Size(300,250);

	LayerGradient* pop = LayerGradient::create(Color4B(200,200,200,255), Color4B(150,150,150,255), Point(0.3f,1));
	pop->setContentSize(area);
	pop->ignoreAnchorPointForPosition(false);
	pop->setAnchorPoint(Point(0.5,0.5));

	area.height -= 50;
	Label* label = Label::createWithSystemFont(msg.c_str(), "Arial", 24, area, TextHAlignment::CENTER, TextVAlignment::TOP);
	label->setAnchorPoint(Point(0,0));
	label->setPositionY(50);
	pop->addChild(label);

	TouchableNode* btn = CreateSimpleButton( btnLabel, okEvt, bus );
	btn->setKill(pop);
	btn->setAnchorPoint(Point(0.5,0));
	btn->setPositionX( area.width/2);

	pop->addChild(btn);

	return pop;
}

Layer* CreateSimpleEditBox( std::string msg, std::string defaultTxt, std::string btnLabel, std::string okEvt, std::string bus )
{
	Size area = Size(300,250);

	LayerGradient* pop = LayerGradient::create(Color4B(200,200,200,255), Color4B(150,150,150,255), Point(0.3f,1));
	pop->setContentSize(area);
	pop->ignoreAnchorPointForPosition(false);
	pop->setAnchorPoint(Point(0.5,0.5));

	area.height -= 50;
	Label* label = Label::createWithSystemFont(msg.c_str(), "Arial", 24, area, TextHAlignment::CENTER, TextVAlignment::TOP);
	label->setAnchorPoint(Point(0,0));
	label->setPositionY(50);
	pop->addChild(label);


	TextField* tf = TextField::create(defaultTxt);
	//TextFieldTTF* tf = TextFieldTTF::textFieldWithPlaceHolder(defaultTxt.c_str(), "Arial", 28);
	tf->setAnchorPoint(Point(0.5,0));
	tf->setPosition(Point(area.width/2, 50));
	pop->addChild(tf);

	TouchableNode* btn = CreateSimpleButton( btnLabel, okEvt, bus );
	btn->setKill(pop);
	btn->setStringGrab(tf);
	btn->setAnchorPoint(Point(0.5,0));
	btn->setPositionX( area.width/2);

	pop->addChild(btn);

	return pop;
}

TouchableNode* CreateSimpleButton( std::string msg, std::string evt, std::string bus )
{
	Size area = Size(100, 40);

	LayerColor* btn = LayerColor::create(Color4B(50,150,50,255));
	btn->ignoreAnchorPointForPosition(false);
	btn->setContentSize(area);
	btn->setAnchorPoint(Point(0,0));
	Label* btnLabel = Label::createWithSystemFont(msg.c_str(), "Arial", 20, Size(120,40), TextHAlignment::CENTER, TextVAlignment::CENTER);
	btnLabel->setAnchorPoint(Point(0.5f,0));
	btnLabel->setPositionX( btn->getContentSize().width/2 );
	btn->addChild(btnLabel);

	TouchableNode* sBtn = new TouchableNode(evt, bus);
	sBtn->addChild(btn);
	sBtn->setAnchorPoint(Point(0,0));
	sBtn->setPositionY(5);
	sBtn->setContentSize(area);


	return sBtn;
}

Label* CreateSimpleLabel( std::string label )
{

	Label* lblName = Label::createWithSystemFont(label.c_str(), "Arial", 20, Size(120,40), TextHAlignment::CENTER, TextVAlignment::CENTER);
	return lblName;
}

Node* CreateSimpleNamedLabel( std::string labelName, std::string labelText )
{
	Node* area = Node::create();
	area->setContentSize(Size(240,40));

	Label* lblName = Label::createWithSystemFont(labelName.c_str(), "Arial", 20, Size(120,40), TextHAlignment::CENTER, TextVAlignment::CENTER);

	Label* lblText = Label::createWithSystemFont(labelText.c_str(), "Arial", 20, Size(120,40), TextHAlignment::CENTER, TextVAlignment::CENTER);
	lblText->setPositionX(120);


	area->addChild(lblName);
	area->addChild(lblText);

	return area;
}

bool rHasInvisibleParent( Node* node )
{
	Node* parent = node->getParent();

	if( parent == NULL ) return false;

	if( !parent->isVisible() ) return true;

	return rHasInvisibleParent(parent);
}


}
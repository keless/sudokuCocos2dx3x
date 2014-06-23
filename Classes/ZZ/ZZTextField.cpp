#include "ZZTextField.h"
#include "ZZUtils.h"

namespace ZZ {

TextField::TextField(void)
{
	m_tf = NULL;
	m_inEditMode = false;
}


TextField::~TextField(void)
{
}


//static 
TextField* TextField::create( std::string defaultText, std::string fontName, int size, const Size& dimensions, TextHAlignment alignment )
{
	ZZ::TextField* tf = new ZZ::TextField();
	if(!tf->init( defaultText, fontName, size, dimensions, alignment ) )
	{
		delete tf;
		return NULL;
	}
	return tf;
}

//virtual 
bool TextField::init( std::string defaultText, std::string fontName, int size, const Size& dimensions, TextHAlignment alignment )
{
	m_defaultText = defaultText; 
	m_tf = TextFieldTTF::textFieldWithPlaceHolder(defaultText.c_str(), dimensions, alignment, fontName.c_str(), size);
	addChild(m_tf);

	m_outline = DrawNode::create();
	
	m_outline->drawSegment(Point(0,0), Point(dimensions.width,0), 2, Color4F(1,1,1,1));
	m_outline->drawSegment(Point(0,0), Point(0,dimensions.height), 2, Color4F(1,1,1,1));
	m_outline->drawSegment(Point(dimensions.width,0), Point(dimensions.width,dimensions.height), 2, Color4F(1,1,1,1));
	m_outline->drawSegment(Point(0,dimensions.height), Point(dimensions.width,dimensions.height), 2, Color4F(1,1,1,1));
	m_outline->setAnchorPoint(Point(0,0));
	m_outline->setPosition(-dimensions.width/2, -dimensions.height/2);
	
	addChild(m_outline);
	m_outline->setVisible(false);

	m_inEditMode = false;

	ignoreAnchorPointForPosition(false);
	//setTouchEnabled(true);
	//setTouchMode( kTouchesOneByOne );

	return true;
}

//virtual 
void TextField::setEventBus( std::string bus )
{
	m_evtBus = bus;
}

//virtual
void TextField::setString( const std::string &label)
{
	m_tf->setString(label);
}
//virtual 
const std::string& TextField::getString() const 
{
	return m_tf->getString();
}

//virtual 
bool TextField::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	Point p = pTouch->getLocation();
	//todo: convert to local coords

	Point local = convertToNodeSpace(p);

	for( int i=0; i< _children.size(); i++)
	{
		Node* child = (Node*) _children.at(i);
		if( child->getBoundingBox().containsPoint( local ) ) {
			m_touchStarted = true;
			return true;
		}
	}

	return m_inEditMode; //always return true for TF if in edit mode-- we want to consume ALL touches (inside and out)
}
//virtual 
void TextField::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	Point p = pTouch->getLocation();
	//todo: convert to local coords

	Point local = convertToNodeSpace(p);

	bool movedInside = false;
	
	for( int i=0; i< _children.size(); i++)
	{
		Node* child = (Node*) _children.at(i);
		if( child->getBoundingBox().containsPoint( local ) ) {
			movedInside = true;
			break;
		}
	}
}
//virtual 
void TextField::onTouchEnded(Touch *pTouch, Event *pEvent)
{
	if(!isVisible() ) return;
	if(rHasInvisibleParent(this)) return;

	Point p = pTouch->getLocation();
	//todo: convert to local coords

	Point local = convertToNodeSpace(p);

	bool clickedInside = false;

	for( int i=0; i< _children.size(); i++)
	{
		Node* child = (Node*) _children.at(i);
		if( child->getBoundingBox().containsPoint( local ) ) {
			//touched inside

			//handle touch event
			clickedInside = true;
			break;
		}
	}

	if( clickedInside ) {
		CCLog("start text entry");

		if( m_defaultText.compare( m_tf->getString() ) == 0 )
		{
			m_tf->setString(""); //clear default string on entry
		}
		
		m_inEditMode = true;
		m_tf->attachWithIME();

		if( m_evtBus.size() > 0 ) {
			m_outline->setVisible(true);
			JsonEvent* evt = new JsonEvent("zztfStart");
			evt->setUserData(this);
			EventBus::get(m_evtBus.c_str())->dispatch(evt->type, evt);
		}

	}else {
		CCLog("quit text entry");

		m_inEditMode = false;
		m_tf->detachWithIME();

		if( m_evtBus.size() > 0 ) {
			m_outline->setVisible(false);
			JsonEvent* evt = new JsonEvent("zztfEnd");
			evt->setUserData(this);
			EventBus::get(m_evtBus.c_str())->dispatch(evt->type, evt);
		}
	}
	m_touchStarted = false;
}

//virtual 
void TextField::onTouchCancelled(Touch *pTouch, Event *pEvent)
{
	m_touchStarted = false;
}

}
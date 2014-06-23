#include "ZZTouchableNode.h"
#include "ZZUtils.h"


namespace ZZ {

TouchableNode::TouchableNode( const std::string evt, const std::string bus, bool includeSelf) : m_evt(evt), m_bus(bus) 
{
	//setTouchMode(konTouchesOneByOne);
	//setTouchEnabled(true);
	ignoreAnchorPointForPosition(false);
	setAnchorPoint(Point(0.5,0.5));
	m_touchStarted = false;
	m_pKillOnTouch = NULL;
	m_pGrabStringOnTouch = NULL;
	m_includeSelf = includeSelf;

	auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
        
    touchListener->onTouchBegan = CC_CALLBACK_2(TouchableNode::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(TouchableNode::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(TouchableNode::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(TouchableNode::onTouchCancelled, this);
        
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this); //shouldnt this get removed?

}

/*
void TouchableNode::addChild(CCNode *child, int zOrder, int tag)
{
    CCNode::addChild(child, zOrder, tag);

    CCRect childArea = child->getBoundingBox();
	CCRect area = this->getBoundingBox();

	//todo: resize to fit children?

}
*/

//virtual 
bool TouchableNode::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	if(!isVisible()) return false;

	Point p = pTouch->getLocation();

	Point local = convertToNodeSpace(p);

	if( m_includeSelf && getBoundingBox().containsPoint(local) )
	{
		m_touchStarted = true;
		return true;
	}

	for( int i=0; i< _children.size(); i++)
	{
		Node* child = (Node*) _children.at(i);
		if( child->getBoundingBox().containsPoint( local ) ) {
			m_touchStarted = true;
			return true;
		}
	}

	//todo: what if the node itself was touched, instead of just children

	return false;
}
//virtual 
void TouchableNode::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	Point p = pTouch->getLocation();

	Point local = convertToNodeSpace(p);

	bool movedInside = false;

	if( m_includeSelf && getBoundingBox().containsPoint(local) )
	{
		movedInside = true;
		return;
	}

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
void TouchableNode::onTouched()
{
	//handle touch event
	CCLog("send %s:%s", m_bus.c_str(),m_evt.c_str());
	JsonEvent* evt = new JsonEvent(m_evt);
	evt->json = m_data;

	if( m_pGrabStringOnTouch != NULL ) 
	{
		const std::string str = m_pGrabStringOnTouch->getString();
		evt->json["string"] = str;
	}

	if( m_evt.size() > 0  ) {
		EventBus::get(m_bus.c_str())->dispatch(m_evt, evt );
	}

	if( m_pKillOnTouch != NULL ) {
		m_pKillOnTouch->removeFromParentAndCleanup(true);
	}
}

//virtual 
void TouchableNode::onTouchEnded(Touch *pTouch, Event *pEvent)
{
	if(!isVisible()) return;

	if( rHasInvisibleParent(this) ) return; //recursive search up to root to see if any parents are invisible

	Point p = pTouch->getLocation();

	Point local = convertToNodeSpace(p);

	if( m_includeSelf && getBoundingBox().containsPoint(local) )
	{
		onTouched();
		m_touchStarted = false;
		return;
	}

	for( int i=0; i< _children.size(); i++)
	{
		Node* child = (Node*) _children.at(i);
		if( child->getBoundingBox().containsPoint( local ) ) {
			//touched inside

			onTouched();
			m_touchStarted = false;
			return;
		}
	}

	m_touchStarted = false;
}

//virtual 
void TouchableNode::onTouchCancelled(Touch *pTouch, Event *pEvent)
{
	m_touchStarted = false;
}

}
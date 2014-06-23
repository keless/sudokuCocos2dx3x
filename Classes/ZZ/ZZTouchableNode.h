#ifndef _ZZTOUCHABLE_H_
#define _ZZTOUCHABLE_H_

/**************

TouchableNode is a CCLayer that will fire off a JsonEvent to 
	the EventBus when any of its children are touched.

if 'evt' is "", no event is sent
if 'setKill' is called, when the touch event is fired, the target node will  
	call removeFromParentAndCleanup(true)
if 'StringGrab' is called, when the touch event is fired, the JsonEvent will 
	include a 'string' parameter which holds the value of the CCLabelProtocol

***************/

#include "ZZDefines.h"
#include "ZZEventBus.h"

namespace ZZ {

class TouchableNode : public Layer
{
protected:
	std::string m_evt;
	std::string m_bus;
	Json::Value m_data;

	bool m_touchStarted;
	bool m_includeSelf;

	Node* m_pKillOnTouch;
	LabelProtocol* m_pGrabStringOnTouch;

public:
	TouchableNode( const std::string evt, const std::string bus = "game", bool includeSelf = false);
	virtual ~TouchableNode(void) {}

	void setKill( Node* killThis ) { m_pKillOnTouch = killThis; }
	void setStringGrab( LabelProtocol* label ) { m_pGrabStringOnTouch = label; }

	void setData( JsonValueRef data) { m_data = data; }
	Json::Value& getData() { return m_data; }

	virtual void onTouched();

	virtual bool onTouchBegan(Touch *pTouch, Event *pEvent);
    virtual void onTouchMoved(Touch *pTouch, Event *pEvent);
    virtual void onTouchEnded(Touch *pTouch, Event *pEvent);
    virtual void onTouchCancelled(Touch *pTouch, Event *pEvent);

	//virtual void addChild(CCNode *child, int zOrder, int tag);
};

}

#endif
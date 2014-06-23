#ifndef _ZZTEXTFIELD_H_
#define _ZZTEXTFIELD_H_

#include <cocos2d.h>
using namespace cocos2d;

#include "ZZEventBus.h"

/*************
wraps the CCTextFieldTTF class in a more easy to use ZZ-friendly class

if you setEventBus, it will send signals "zztfStart" and "zztfEnd" to it on IME begin and end
 the JsonEvent will have a pUserData pointing at the TextField instance that spawned the event

**************/

namespace ZZ {

class TextField : public Layer, public LabelProtocol
{
protected:
	TextFieldTTF* m_tf;
	
	
	DrawNode* m_outline;

	std::string m_defaultText;
	bool m_touchStarted;
	bool m_inEditMode;

	std::string m_evtBus;

public:
	TextField(void);
	virtual ~TextField(void);

	static TextField* create( std::string defaultText = "", std::string fontName = "Arial", int size = 24, const Size& dimensions = Size(200,60), TextHAlignment alignment = TextHAlignment::LEFT );

	virtual bool init( std::string defaultText, std::string fontName, int size, const Size& dimensions, TextHAlignment alignment  = TextHAlignment::LEFT );

	virtual void setEventBus( std::string bus );
	
	virtual void setString(const std::string &label);
    virtual const std::string& getString() const;

	virtual bool onTouchBegan(Touch *pTouch, Event *pEvent);
    virtual void onTouchMoved(Touch *pTouch, Event *pEvent);
    virtual void onTouchEnded(Touch *pTouch, Event *pEvent);
    virtual void onTouchCancelled(Touch *pTouch, Event *pEvent);
};

}

#endif
#ifndef _ZZPROGRESSBAR_H_
#define _ZZPROGRESSBAR_H_

#include <cocos2d.h>

using namespace cocos2d;

namespace ZZ {

class ProgressBar : public Node
{
	LayerColor* m_bg;
	LayerColor* m_bar;
	float m_margin;

public:
	ProgressBar(void);
	virtual ~ProgressBar(void);

	void setColor( const Color3B& fg, const Color3B& bg );

	virtual bool init(  const Rect &area );

	static ProgressBar* create( const Rect &area );

	void setProgress( float pct );
	void setMargin( float margin );
};

}

#endif

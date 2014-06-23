#include "ZZProgressBar.h"

namespace ZZ {

ProgressBar::ProgressBar(void) : m_bg(NULL), m_bar(NULL), m_margin(0)
{
}


ProgressBar::~ProgressBar(void)
{
}

//virtual 
bool ProgressBar::init( const Rect &area )
{
	setContentSize( Size( area.size.width, area.size.height ) );

	m_bg = LayerColor::create( Color4B(255,0,0,255), area.size.width, area.size.height );
	m_bar = LayerColor::create( Color4B(0,255,0,255), area.size.width, area.size.height );
	m_bar->setAnchorPoint(Point(0.0f, 0.5f));
	//m_bar->setPositionX( area.size.width * -0.5f );
	//m_bar->setPositionY( 30 );

	addChild(m_bg);
	addChild(m_bar);

	this->setPosition(area.origin.x, area.origin.y);

	return true;
}

//.static 
ProgressBar * ProgressBar::create( const Rect &area )
{
	ProgressBar* bar = new ProgressBar();
	if(!bar->init(area))
	{
		delete bar;
		return NULL;
	}
	bar->autorelease();
	return bar;
}

void ProgressBar::setColor( const Color3B& fg, const Color3B& bg )
{
	m_bg->setColor( bg );
	m_bar->setColor(fg);
}

void ProgressBar::setProgress( float pct )
{
	m_bar->setScaleX( pct );
}

void ProgressBar::setMargin( float margin )
{
	Size area = m_bg->getContentSize();
	area.height -= margin * 2;
	area.width -= margin * 2;
	m_margin = margin;
	m_bar->setContentSize(area);
	m_bar->setPosition(Point(margin, margin )); //after resizing, cause position to recalculate
}

}

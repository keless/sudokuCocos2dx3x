/******

ZZUtils  by Paul Zirkle





*******/


#ifndef _ZZUTILS_H_
#define _ZZUTILS_H_


#include <cocos2d.h>

using namespace cocos2d;

#include "json.h"
#include "ZZProgressBar.h"
#include "ZZEventBus.h"
#include "ZZRadialLayer.h"
#include "ZZTouchableNode.h"
#include "ZZTextField.h"
#include "ZZRadioGroup.h"
#include "ZZJsonManager.h"
#include "ZZAnimatedSprite.h"

namespace ZZ {


bool IsFile( const std::string& filename, bool checkWritablePath = false );
Json::Value ReadFileToJson( const std::string& fileName );
bool WriteJsonToFile( const Json::Value& json, const std::string& fileName );
bool RemoveFile( const std::string& fileName );
bool CheckFileExists( const std::string& fileName );
Layer* CreateSimplePopup( std::string msg, std::string btnLabel, std::string okEvt, std::string bus = "game" );
Layer* CreateSimpleEditBox( std::string msg, std::string defaultTxt, std::string btnLabel, std::string okEvt, std::string bus = "game" );
TouchableNode* CreateSimpleButton( std::string msg, std::string evt, std::string bus = "game" );
Label* CreateSimpleLabel( std::string label );
Node* CreateSimpleNamedLabel( std::string labelName, std::string labelText );

bool rHasInvisibleParent( Node* node );

#define CALC_2DARR_IDX( x, y, stride ) (y * stride + x)

}

#endif

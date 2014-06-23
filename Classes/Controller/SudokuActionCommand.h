#pragma once
#include <CCRef.h>

#include "ZZ/ZZUtils.h"

using namespace cocos2d;

//NOTE: commands represent valid state changes to the model
//			as opposed to attempted user input

enum eSACommands
{
	SAC_WRITE_INK,
	SAC_WRITE_PENCIL,
	SAC_ERASE_PENCIL,

	SAC_EXHAUSTED_NUMBER,

	SAC_GAME_OVER_WIN,
	SAC_GAME_OVER_LOSE
};

struct SudokuActionCommand 
{
	eSACommands type;
	Json::Value data;
};


class SACEvent : public ZZ::BaseEvent
{
public:
	SudokuActionCommand cmd;

	SACEvent( eSACommands commandType ) : BaseEvent("SACEvent")
	{
		cmd.type = commandType;
	}
};


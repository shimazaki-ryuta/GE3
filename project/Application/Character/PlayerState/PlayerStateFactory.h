#pragma once
#include "BasePlayerState.h"
#include <string>
class PlayerStateFactory
{
public:
	~PlayerStateFactory() {};

	BasePlayerState* CreateState(const std::string& stateName);

private:

};

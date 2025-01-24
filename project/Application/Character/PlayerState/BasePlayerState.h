#pragma once
class Player;
class BasePlayerState
{
public:
	BasePlayerState() {  };
	virtual~BasePlayerState() {};

	virtual void Initialize() {};
	virtual void Update() {};

	void SetPlayer(Player* player) { player_ = player; };

protected:
	Player* player_ = nullptr;
};

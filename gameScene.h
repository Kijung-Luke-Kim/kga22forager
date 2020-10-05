#pragma once
#include "gameNode.h"
#include "ForagerPlayer.h"
#include "inGameMenu.h"
class gameScene :public gameNode
{
private:
	ForagerPlayer* _player;
	inGameMenu* _Meun;


private:
	bool inven_open;
public:
	HRESULT init();
	void release();
	void update();
	void render();

};

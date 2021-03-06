#pragma once
#include "unit.h"
enum MUSTATE
{
	MIDLE,
	CRY
};

class muBoss : public enemy
{
private:
	MUSTATE _state5;
	int fireCount;
	int crashCount;
	int crashPointCount;
	int searchCount;
	int _difficultyLevel;
	int _spawnCount;
	bool _canFire;
	bool _readyToCrash;

public:

	HRESULT init(int p_level);
	void update() override;
	void render(HDC hdc) override;
	
	void muuAnimation();
	void hurt(int damage, bool onlyEffect = false) override;
	void dead() override;

	void muBossExplode();


	
};


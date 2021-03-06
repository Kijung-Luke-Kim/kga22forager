#pragma once
#include "inventory_slot.h"
#include "targetingBox.h"
class quick_slot
{
private:
	vector<inventory_slot*> _quick;
	targetingBox* _targetBox;
	int targetnum;
	bool _isQuickSlotChanged;
public:
	
	HRESULT init();
	void release();
	void update();
	void render(HDC hdc);
	void quick_slot_update();
	void changeQuickSlot(int num);
	void settargetNum(int i) {
		targetnum = i;
	}

	void quick_slot_target_Move();
	inventory_slot* GetQuickSlotNumber() { return _quick[targetnum]; };
	void Item_Minus(string key,int count);

	bool GetIsQuickSlotChanged() { return _isQuickSlotChanged; };
	void Done() { _isQuickSlotChanged = false; };
};


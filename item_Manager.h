#pragma once
#include "inventory_slot.h"
#include "singletonBase.h"
#include "unit.h"

class quick_slot;
class item_Manager : public singletonBase<item_Manager>
{
private:
	quick_slot * _quick;
	vector<inventory_slot*> _item_push;
	vector<inventory_slot*> _equip;
	int *money;
private:
	bool isItemCheck(string key);
	int itemempty();
	int itemfind(string key);

	void inventorysection();
public:
	void setvInventory_info(vector<inventory_slot*> inven) {
		_item_push = inven;
	};
	
	void set_equip_info(vector<inventory_slot*> equip) {
		_equip = equip;
	};

	vector<inventory_slot*> get_equip_info() {
		return _equip ;
	};

	vector<inventory_slot*> getvInventory_info() {
		return _item_push;
	}

	void setMoney(int* m) {
		money = m;
	}

	int getMoney() {
		return *money;
	}
	void setMoney(int m) {
		*money = m;
	}
public:
	
	int equip_count();
	void vItem_push(string key);
	void vequip_push(string key);
	bool isequipCheck(string key);
	void vItem_count_zoro();
	int item_count(string key);
public:
	bool Item_industry_check(string key);
	void _Item_industry_decrease(string key);
	bool Item_count_Minus(string key, int count);
	bool Item_count_Minus(string key, int count, string key2, int count2);
	bool Item_count_Minus(string key, int count, string key2, int count2, string key3, int count3);
	bool Item_count_Minus(string key, int count, string key2, int count2, string key3, int count3, string key4, int count4);


public:
	void quick_slot_info(quick_slot* _q) {
		_quick = _q;
	}

};


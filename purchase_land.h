#pragma once

class purchase_land 
{
private:
	bool isCheck;
public:
	HRESULT init();
	void release();
	void update();
	void render(HDC hdc);
	void setIsCheck(bool check) {
		isCheck = check;
	}
};

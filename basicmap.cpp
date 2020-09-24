#include "stdafx.h"
#include "basicmap.h"

HRESULT basicmap::init()
{
	_player = new ForagerPlayer;
	_statManager = new ForagerStatManager;

	_player->init();
	_statManager->init();

	_player->setPMLink(this);

	//�κ��丮
	_inventory = new inventory;
	_inventory->init();
	_inventory->setIMLink(this);

	//����
	watertile = IMAGEMANAGER->addImage("watertile", "Images/�̹���/Ÿ��/img_tile_water.bmp", TILESIZE, TILESIZE);
	plaintile = IMAGEMANAGER->addFrameImage("plaintile", "Images/�̹���/Ÿ��/img_tile_plain.bmp", 224, 56, 4, 1);
	plainedge = IMAGEMANAGER->addFrameImage("plainedge", "Images/�̹���/Ÿ��/img_tile_plainEdge.bmp", 224, 112, 4, 2, true, RGB(255, 0, 255));
	wave = IMAGEMANAGER->addImage("wave", "Images/�̹���/Ÿ��/img_tile_wave.bmp", 56, 56);
	underwater = IMAGEMANAGER->addImage("underwater", "Images/�̹���/Ÿ��/img_tile_bottomGround.bmp", 56, 56, true, RGB(255, 0, 255));
	_rcCam = RectMake(0, 0, WINSIZEX, WINSIZEY);
	_rcPlayer = _player->getPlayerRect();

	_count = wavetick = 0;

	//�ڿ�
	berry = IMAGEMANAGER->addFrameImage("berry", "Images/�̹���/������Ʈ/resource/img_object_berry.bmp", 112, 56, 2, 1, true, RGB(255, 0, 255));
	rock = IMAGEMANAGER->addFrameImage("rock", "Images/�̹���/������Ʈ/resource/img_object_rock.bmp", 112, 56, 2, 1, true, RGB(255, 0, 255));
	tree = IMAGEMANAGER->addFrameImage("tree", "Images/�̹���/������Ʈ/resource/img_object_tree.bmp", 280, 168, 5, 1, true, RGB(255, 0, 255));

	//�ǹ�
	steelwork = IMAGEMANAGER->addFrameImage("steelwork", "Images/�̹���/������Ʈ/building/img_object_steelwork.bmp", 336/2, 168/2, 3, 1, true, RGB(255,0,255));

	this->mapSetup();
	_playerPos = getPlayerPos();

	// Ÿ���� �ڽ� ����
	_targetingBox = new targetingBox;
	_targetingBox->init();

	return S_OK;
}

void basicmap::release()
{
	_player->release();
	_statManager->release();
	_inventory->release();
}

void basicmap::update()
{
	_rcPlayer = _player->getPlayerRect();
	//Ÿ�� ����
	this->setTile();
	// ī�޶� �̵�
	this->cameraMove();
	this->cameraFocus();

	_count++;
	if (_count % 10 == 0) {
		wavetick++;
	}
	
	if (_count % RESGENTIME == 0) {
		this->setRandomTile();
	}
	if (wavetick > 10) {
		wavetick = 0;
	}

	//�÷��̾� ������Ʈ
	_player->update();
	_statManager->update();
	_inventory->update();

	//���콺 Ÿ����
	for (int i = 0; i < TILEY*MAPY; i++) {
		bool stop = false;
		for (int j = 0; j < MAPTILEX; j++) {
			if (PtInRect(&_vTiles[i*MAPTILEY + j].rc, _ptMouse)) {
				_targetingBox->RemoveTarget();
				_targetingBox->SetTarget(_vTiles[i*MAPTILEY + j].rc);
				if (_vTiles[i*MAPTILEY + j].objHp > 0) {
					_vTiles[i*MAPTILEY + j].objHp -= 1;
				}
				else {
					_vTiles[i*MAPTILEY + j].terrainHp -= 1;
				}
				stop = true;
				break;
			}
		}
		if (stop) break;
	}
	_targetingBox->update();
}

void basicmap::render()
{
	//�ٴ� ����
	for (int i = 0; i < MAPTILEY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			RECT temp;
			if (!IntersectRect(&temp, &_rcCam, &_vTiles[i*MAPTILEY + j].rc)) continue;
			if (_vTiles[i*MAPTILEY + j].terrain == watertile) {
				_vTiles[i*MAPTILEY + j].terrain->render(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top);
			}
		}
	}
	//���� ����
	for (int i = 0; i < MAPTILEY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			RECT temp;
			if (!IntersectRect(&temp, &_rcCam, &_vTiles[i*MAPTILEY + j].rc)) continue;
			if (_vTiles[i*MAPTILEY + j].terrain != watertile) {
				_vTiles[i*MAPTILEY + j].terrain->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, _vTiles[i*MAPTILEY + j].terrainFrameX, _vTiles[i*MAPTILEY + j].terrainFrameY);
				//textOut(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, to_string(_vTiles[i*MAPTILEY + j].rc.left).c_str());
				//���� ������
				if (j - 1 >= 0 && _vTiles[(i)*MAPTILEY + (j - 1)].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 0, 1);
				}
				//���� ������
				if (i - 1 >= 0 && j + 1 < MAPTILEX && _vTiles[(i - 1)*MAPTILEY +(j + 1)].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 1, 1);
				}
				//���� ������
				if (i + 1 < MAPTILEY && j + 1 < MAPTILEX && _vTiles[(i + 1)*MAPTILEY +(j + 1)].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 2, 1);
				}
				//���� ������
				if (i + 1 < MAPTILEY && j - 1 >= 0 && _vTiles[(i + 1)*MAPTILEY +(j - 1)].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 3, 1);
				}
				//��
				if (j-1 >= 0 && _vTiles[i*MAPTILEY +(j - 1)].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 0, 0);
				}
				//��
				if (j + 1 < MAPTILEX && _vTiles[i*MAPTILEY +(j + 1)].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 1, 0);
				}
				//��
				if (i - 1 >= 0 && _vTiles[(i - 1)*MAPTILEY +j].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 2, 0);
				}
				//��
				if (i + 1 < MAPTILEY && _vTiles[(i + 1)*MAPTILEY +j].terrain == watertile) {
					plainedge->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.top, 3, 0);
					underwater->render(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.bottom);
					wave->alphaRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.bottom+20+wavetick, 100);

				}

			}
		}
	}
	//�÷��̾� ��ġ ��ǥ ��Ʈ
	Rectangle(getMemDC(), _vTiles[_playerPos].rc);
	//�÷��̾�
	_player->render();
	_statManager->render();

	//������Ʈ ����
	for (int i = 0; i < MAPTILEY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			RECT temp;
			if (!IntersectRect(&temp, &_rcCam, &_vTiles[i*MAPTILEY + j].rc)) continue;
			if (_vTiles[i*MAPTILEY + j].objHp > 0) {
				//if (_vTiles[i*MAPTILEY + j].object == steelwork) {
				//	if (_vTiles[i*MAPTILEY + j + 1].object == steelwork &&
				//		_vTiles[(i+1)*MAPTILEY + j].object == steelwork &&
				//		_vTiles[(i + 1)*MAPTILEY + j+1].object == steelwork) {
				//		steelwork->frameRender(getMemDC(), 
				//			_vTiles[(i-1)*MAPTILEY + j].rc.left, 
				//			_vTiles[(i-1)*MAPTILEY + j].rc.top, 
				//			_vTiles[i*MAPTILEY + j].objFrameX,  
				//			_vTiles[i*MAPTILEY + j].objFrameY);
				//	}
				//}
				//else {
				//	_vTiles[i*MAPTILEY + j].object->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.bottom - _vTiles[i*MAPTILEY + j].object->getFrameHeight(), _vTiles[i*MAPTILEY + j].objFrameX, _vTiles[i*MAPTILEY + j].objFrameY);
				//}
				_vTiles[i*MAPTILEY + j].object->frameRender(getMemDC(), _vTiles[i*MAPTILEY + j].rc.left, _vTiles[i*MAPTILEY + j].rc.bottom - _vTiles[i*MAPTILEY + j].object->getFrameHeight(), _vTiles[i*MAPTILEY + j].objFrameX, _vTiles[i*MAPTILEY + j].objFrameY);
				//Rectangle(getMemDC(), _vTiles[i*MAPTILEY + j].rc);
			}
		}
	}
	// Ŀ�� ����
	IMAGEMANAGER->findImage("TitleCursor")->render(getMemDC(), _ptMouse.x, _ptMouse.y);
	_targetingBox->render(getMemDC());

	//int playerCenterX = _rcPlayer.left + (_rcPlayer.right - _rcPlayer.left) / 2;
	//int playerCenterY = _rcPlayer.top + (_rcPlayer.bottom - _rcPlayer.top) / 2;
	//textOut(getMemDC(), 0, 0, (to_string(CAMRANGE)+" "+ to_string(getDistance(playerCenterX - CAMSPEED, playerCenterY, WINSIZEX / 2, WINSIZEY / 2))).c_str());
	//textOut(getMemDC(), 0, 0, ("�÷��̾� ��ǥ : "+ to_string(_playerPos)).c_str());
	//Rectangle(getMemDC(), _rcPlayer);

	_inventory->render();
}

void basicmap::mapSetup()
{
	for (int i = 0; i < MAPTILEY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			tile _tile;
			_tile.rc = RectMake(WINSIZEX / 2 - (MAPTILEX*TILESIZE / 2.0f) + j * TILESIZE, WINSIZEY / 2 - (MAPTILEY*TILESIZE / 2.0f) + i * TILESIZE, TILESIZE, TILESIZE);
			_tile.level = TERRAIN;
			_tile.terrain = watertile;
			_tile.terrainFrameX = 0;
			_tile.terrainFrameY = 0;
			_tile.terrainHp = _tile.objHp = 0;
			_tile.object = tree;
			_tile.objHp = 0;
			_tile.objFrameX = 0;
			_tile.objFrameY = 0;

			_vTiles.push_back(_tile);
		}
	}
	for (int i = 0; i < TILEY; i++) {
		for (int j = 0; j < TILEX; j++) {
			_vTiles[(3 * MAPTILEY*TILEY + i * MAPTILEY) + 3 * TILEX + j].terrain = plaintile;
			_vTiles[(3 * MAPTILEY*TILEY + i * MAPTILEY) + 3 * TILEX + j].terrainFrameX = RANDOM->range(4);
			_vTiles[(3 * MAPTILEY*TILEY + i * MAPTILEY) + 3 * TILEX + j].terrainHp = PLAINHP;
		}
	}
}

void basicmap::setTile()
{
	//���� ü�� 0 -> �ٴ� Ÿ�Ϸ� �ٲ�
	for (int i = 0; i < TILEY*MAPY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			if (_vTiles[i*MAPTILEY + j].terrainHp <= 0) {
				_vTiles[i*MAPTILEY + j].terrain = watertile;
				_vTiles[i*MAPTILEY + j].terrainFrameX = 0;
				_vTiles[i*MAPTILEY + j].terrainHp = 0;
			}
		}
	}
	if (INPUT->GetKey(VK_LBUTTON)) {
		if (_inventory->getBuildingStatus()) {
			//POINT _ptBuilding = { _ptMouse.x - 1, _ptMouse.y + IMAGEMANAGER->findImage("�뱤��")->getHeight() / 2 };
			for (int i = 0; i < TILEY*MAPY; i++) {
				bool stop = false;
				for (int j = 0; j < MAPTILEX; j++) {
					if (PtInRect(&_vTiles[i*MAPTILEY + j].rc, _ptMouse)) {
						//Ÿ�� 4��
						//if (_vTiles[i*MAPTILEY + j].objHp == 0 &&
						//	_vTiles[i*MAPTILEY + j].terrain == IMAGEMANAGER->findImage("plaintile") &&
						//	_vTiles[i*MAPTILEY + j + 1].objHp == 0 &&
						//	_vTiles[i*MAPTILEY + j + 1].terrain == IMAGEMANAGER->findImage("plaintile") &&
						//	_vTiles[(i - 1)*MAPTILEY + j].objHp == 0 &&
						//	_vTiles[(i - 1) *MAPTILEY + j].terrain == IMAGEMANAGER->findImage("plaintile") &&
						//	_vTiles[(i - 1)*MAPTILEY + j + 1].objHp == 0 &&
						//	_vTiles[(i - 1) * MAPTILEY + j + 1].terrain == IMAGEMANAGER->findImage("plaintile")) {
						//	_vTiles[i*MAPTILEY + j].object = steelwork;
						//	_vTiles[i*MAPTILEY + j].objHp = 10;
						//	_vTiles[i*MAPTILEY + j].objFrameX = 0;
						//	_vTiles[i*MAPTILEY + j].objFrameY = 0;
						//	_vTiles[i*MAPTILEY + j + 1].object = _vTiles[i*MAPTILEY + j].object;
						//	_vTiles[i*MAPTILEY + j + 1].objHp = _vTiles[i*MAPTILEY + j].objHp;
						//	_vTiles[i*MAPTILEY + j + 1].objFrameX = _vTiles[i*MAPTILEY + j].objFrameX;
						//	_vTiles[i*MAPTILEY + j + 1].objFrameY = _vTiles[i*MAPTILEY + j].objFrameY;
						//	_vTiles[(i - 1)*MAPTILEY + j].object = _vTiles[i*MAPTILEY + j].object;
						//	_vTiles[(i - 1)*MAPTILEY + j].objHp = _vTiles[i*MAPTILEY + j].objHp;
						//	_vTiles[(i - 1)*MAPTILEY + j].objFrameX = _vTiles[i*MAPTILEY + j].objFrameX;
						//	_vTiles[(i - 1)*MAPTILEY + j].objFrameY = _vTiles[i*MAPTILEY + j].objFrameY;
						//	_vTiles[(i - 1)*MAPTILEY + j + 1].object = _vTiles[i*MAPTILEY + j].object;
						//	_vTiles[(i - 1)*MAPTILEY + j + 1].objHp = _vTiles[i*MAPTILEY + j].objHp;
						//	_vTiles[(i - 1)*MAPTILEY + j + 1].objFrameX = _vTiles[i*MAPTILEY + j].objFrameX;
						//	_vTiles[(i - 1)*MAPTILEY + j + 1].objFrameY = _vTiles[i*MAPTILEY + j].objFrameY;
						//	_inventory->setBuildingStatus(false);
						//}
						if (_vTiles[i*MAPTILEY + j].objHp == 0 && 
							_vTiles[i*MAPTILEY + j].terrain == IMAGEMANAGER->findImage("plaintile")) {
								_vTiles[i*MAPTILEY + j].object = steelwork;
								_vTiles[i*MAPTILEY + j].objHp = 500;
								_vTiles[i*MAPTILEY + j].objFrameX = 0;
								_vTiles[i*MAPTILEY + j].objFrameY = 0;
								_inventory->setBuildingStatus(false);
						}
						stop = true;
						break;
					}
				}
				if (stop) break;
			}
		}
		for (int i = 0; i < TILEY*MAPY; i++) {
			bool stop = false;
			for (int j = 0; j < MAPTILEX; j++) {
				if (PtInRect(&_vTiles[i*MAPTILEY + j].rc, _ptMouse)) {
					_targetingBox->RemoveTarget();
					_targetingBox->SetTarget(_vTiles[i*MAPTILEY + j].rc);
					if (_vTiles[i*MAPTILEY + j].objHp > 0) {
						_vTiles[i*MAPTILEY + j].objHp -= 1;
					}
					else {
						_vTiles[i*MAPTILEY + j].terrainHp -= 1;
					}					
					stop = true;
					break;
				}
			}
			if (stop) break;
		}
	}
	if (INPUT->GetKey(VK_RBUTTON)) {		
		for (int i = 0; i < MAPTILEY; i++) {
			bool stop = false;
			for (int j = 0; j < MAPTILEX; j++) {
				if (PtInRect(&_vTiles[i*MAPTILEY + j].rc, _ptMouse)) {
					_vTiles[i*MAPTILEY + j].terrain = plaintile;
					_vTiles[i*MAPTILEY + j].terrainFrameX = RANDOM->range(4);
					_vTiles[i*MAPTILEY + j].terrainHp = PLAINHP;
					stop = true;
					break;
				}
			}
			if (stop) break;
		}
	}
}

void basicmap::cameraMove()
{
	int playerCenterX = _rcPlayer.left + (_rcPlayer.right - _rcPlayer.left) / 2;
	int playerCenterY = _rcPlayer.top + (_rcPlayer.bottom - _rcPlayer.top) / 2;
	if (_ptMouse.x > (WINSIZEX / 2) + 100 &&
		_vTiles.back().rc.right > WINSIZEX &&
		getDistance(playerCenterX - CAMSPEED, playerCenterY, WINSIZEX/2, WINSIZEY/2) < CAMRANGE) {
		for (int i = 0; i < MAPTILEY; i++) {
			for (int j = 0; j < MAPTILEX; j++) {
				_vTiles[i*MAPTILEY + j].rc.left -= CAMSPEED;
				_vTiles[i*MAPTILEY + j].rc.right -= CAMSPEED;
			}
		}
		//_rcPlayer.left -= CAMSPEED;
		//_rcPlayer.right -= CAMSPEED;
		RECT playerPos = _player->getPlayerRect();
		playerPos.left -= CAMSPEED;
		playerPos.right -= CAMSPEED;
		_player->setPlayerRect(playerPos);
	}
	if (_ptMouse.x < (WINSIZEX / 2) - 100 &&
		_vTiles[0].rc.left < 0 &&
		getDistance(playerCenterX + CAMSPEED, playerCenterY, WINSIZEX / 2, WINSIZEY / 2) < CAMRANGE) {
		for (int i = 0; i < MAPTILEY; i++) {
			for (int j = 0; j < MAPTILEX; j++) {
				_vTiles[i*MAPTILEY + j].rc.left += CAMSPEED;
				_vTiles[i*MAPTILEY + j].rc.right += CAMSPEED;
			}
		}
		//_rcPlayer.left += CAMSPEED;
		//_rcPlayer.right += CAMSPEED;
		RECT playerPos = _player->getPlayerRect();
		playerPos.left += CAMSPEED;
		playerPos.right += CAMSPEED;
		_player->setPlayerRect(playerPos);
	}
	if (_ptMouse.y > WINSIZEY / 2 + 100 &&
		_vTiles.back().rc.bottom > WINSIZEY &&
		getDistance(playerCenterX, playerCenterY - CAMSPEED, WINSIZEX / 2, WINSIZEY / 2) < CAMRANGE) {
		for (int i = 0; i < MAPTILEY; i++) {
			for (int j = 0; j < MAPTILEX; j++) {
				_vTiles[i*MAPTILEY + j].rc.top -= CAMSPEED;
				_vTiles[i*MAPTILEY + j].rc.bottom -= CAMSPEED;
			}
		}
		//_rcPlayer.top -= CAMSPEED;
		//_rcPlayer.bottom -= CAMSPEED;
		RECT playerPos = _player->getPlayerRect();
		playerPos.top -= CAMSPEED;
		playerPos.bottom -= CAMSPEED;
		_player->setPlayerRect(playerPos);
	}
	if (_ptMouse.y < WINSIZEY / 2 - 100 &&
		_vTiles[0].rc.top < 0 &&
		getDistance(playerCenterX, playerCenterY + CAMSPEED, WINSIZEX / 2, WINSIZEY / 2) < CAMRANGE) {
		for (int i = 0; i < MAPTILEY; i++) {
			for (int j = 0; j < MAPTILEX; j++) {
				_vTiles[i*MAPTILEY + j].rc.top += CAMSPEED;
				_vTiles[i*MAPTILEY + j].rc.bottom += CAMSPEED;
			}
		}
		//_rcPlayer.top += CAMSPEED;
		//_rcPlayer.bottom += CAMSPEED;

		RECT playerPos = _player->getPlayerRect();
		playerPos.top += CAMSPEED;
		playerPos.bottom += CAMSPEED;
		_player->setPlayerRect(playerPos);
	}
}

void basicmap::cameraFocus()
{
	//if (_ptmouse.x < winsizex / 2 - 100 ||
	//	_ptmouse.x > winsizex / 2 + 100 ||
	//	_ptmouse.y < winsizey / 2 - 100 ||
	//	_ptmouse.y > winsizey / 2 + 100) return;
	int playerCenterX = _rcPlayer.left + (_rcPlayer.right - _rcPlayer.left) / 2;
	int playerCenterY = _rcPlayer.top + (_rcPlayer.bottom - _rcPlayer.top) / 2;
	if (playerCenterX != WINSIZEX/2) {
		for (int i = 0; i < MAPTILEY; i++) {
			for (int j = 0; j < MAPTILEX; j++) {
				_vTiles[i*MAPTILEY + j].rc.left += WINSIZEX/2 - playerCenterX < 0 ? -1 : 1;
				_vTiles[i*MAPTILEY + j].rc.right += WINSIZEX / 2 - playerCenterX < 0 ? -1 : 1;
			}
		}
		RECT playerPos = _player->getPlayerRect();
		playerPos.left += WINSIZEX / 2 - playerCenterX < 0 ? -1 : 1;
		playerPos.right += WINSIZEX / 2 - playerCenterX < 0 ? -1 : 1;
		_player->setPlayerRect(playerPos);
	}
	if (playerCenterY != WINSIZEY/2) {
		for (int i = 0; i < MAPTILEY; i++) {
			for (int j = 0; j < MAPTILEX; j++) {
				_vTiles[i*MAPTILEY + j].rc.top += WINSIZEY / 2 - playerCenterY < 0 ? -1 : 1;
				_vTiles[i*MAPTILEY + j].rc.bottom += WINSIZEY / 2 - playerCenterY < 0 ? -1 : 1;
			}
		}
		RECT playerPos = _player->getPlayerRect();
		playerPos.top += WINSIZEY / 2 - playerCenterY < 0 ? -1 : 1;
		playerPos.bottom += WINSIZEY / 2 - playerCenterY < 0 ? -1 : 1;
		_player->setPlayerRect(playerPos);
	}
}

void basicmap::setRandomTile()
{
	if (this->getResRatio() > RESRATIOLIMIT) return;
	while (true) {
		int i = RANDOM->range(TILEY*MAPY);
		int j = RANDOM->range(MAPTILEX);
		//���� �÷��̾ ���ִ� Ÿ���� ��ŵ
		if (i + j == getPlayerPos()) continue;
		if (_vTiles[i*MAPTILEY + j].terrain == plaintile &&
			_vTiles[i*MAPTILEY + j].objHp == 0) {
			switch (RANDOM->range(NUMOBJECTS)) {
			case 0:
				_vTiles[i*MAPTILEY + j].object = tree;
				break;

			case 1:
				_vTiles[i*MAPTILEY + j].object = berry;
				break;

			case 2:
				_vTiles[i*MAPTILEY + j].object = rock;
				break;
			}
			_vTiles[i*MAPTILEY + j].objFrameX = 0;
			_vTiles[i*MAPTILEY + j].objFrameY = 0;
			_vTiles[i*MAPTILEY + j].objHp = 50;
			break;
		}
	}
}

float basicmap::getResRatio()
{
	float nResTile = 0;
	float nPlainTile = 0;
	for (int i = 0; i < MAPTILEY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			if (_vTiles[i*MAPTILEY + j].terrain == plaintile &&
				_vTiles[i*MAPTILEY + j].objHp == 0) {
				nPlainTile++;
			}
			else if (_vTiles[i*MAPTILEY + j].terrain == plaintile &&
				_vTiles[i*MAPTILEY + j].objHp > 0) {
				nResTile++;
			}
		}
	}
	return nResTile / float(nPlainTile);
}


// ���� 1ȸ ����
int basicmap::getPlayerPos()
{
	int playerCenterX = _rcPlayer.left + (_rcPlayer.right - _rcPlayer.left) / 2;
	int playerCenterY = _rcPlayer.bottom;
	_ptPlayerPos = { playerCenterX, playerCenterY };


	for (int i = 0; i < MAPTILEY; i++) {
		for (int j = 0; j < MAPTILEX; j++) {
			if (PtInRect(&_vTiles[i*MAPTILEY + j].rc, _ptPlayerPos)) {
				return (i*MAPTILEY + j);
			}
		}
	}
}

// �̵� ���� ���� üũ
bool basicmap::checkCanMove(int index)
{
	// �̵� ����
	if (_vTiles[_playerPos + index].terrain != watertile &&
		_vTiles[_playerPos + index].objHp <= 0) {


		return false;
	}
		
	RECT t_bound = RectMakeCenter(_ptPlayerPos.x, _ptPlayerPos.y, 20, 10);
	RECT t_temp;
	if (!IntersectRect(&t_temp, &t_bound, &_vTiles[_playerPos + index].rc)) {
		return false;
	}

	// �̵� �Ұ�
	return true;
}

// �÷��̾� ��ǥ ����.
void basicmap::setPlayerPosTile()
{
	int playerCenterX = _rcPlayer.left + (_rcPlayer.right - _rcPlayer.left) / 2;
	int playerCenterY = _rcPlayer.bottom;
	_ptPlayerPos = { playerCenterX, playerCenterY };

	// �÷��̾� ��ǥ ���� �����¿� Ÿ���߿� 
	// ���� ��� �ִ� Ÿ�Ϸ� ��ǥ ����
	if (PtInRect(&_vTiles[_playerPos].rc, _ptPlayerPos))
		_playerPos = _playerPos;


	else if (PtInRect(&_vTiles[_playerPos + 1].rc, _ptPlayerPos)) 
		_playerPos += 1;
	else if (PtInRect(&_vTiles[_playerPos - 1].rc, _ptPlayerPos)) 
		_playerPos -= 1;
	else if (PtInRect(&_vTiles[_playerPos + MAPTILEX].rc, _ptPlayerPos)) 
		_playerPos += MAPTILEX;
	else if (PtInRect(&_vTiles[_playerPos - MAPTILEX].rc, _ptPlayerPos))
		_playerPos -= MAPTILEX;


	else if (PtInRect(&_vTiles[_playerPos - MAPTILEX + 1].rc, _ptPlayerPos))
		_playerPos += MAPTILEX + 1;
	else if (PtInRect(&_vTiles[_playerPos - MAPTILEX - 1].rc, _ptPlayerPos))
		_playerPos -= MAPTILEX - 1;
	else if (PtInRect(&_vTiles[_playerPos + MAPTILEX + 1].rc, _ptPlayerPos))
		_playerPos += MAPTILEX + 1;
	else if (PtInRect(&_vTiles[_playerPos + MAPTILEX - 1].rc, _ptPlayerPos))
		_playerPos -= MAPTILEX - 1;

	else 
		_playerPos = getPlayerPos();
	
}
#ifndef __GAME_BOARD_H__
#define __GAME_BOARD_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class BoardData
{
public:
	Vec2 size = Vec2(7, 23);
	int flag_left;
	int totalMine = 40;
	bool endGame = false;
	bool youLose = false;

	char boardData[20][100];
	vector<Vec2> getArround(Vec2 position);
	vector<Vec2> touchAt(Vec2 touchAtMinePosition);
	bool hasMineAt(int, int);
	bool setFlag(int, int);
	bool removeFlag(int, int);
	bool hasFlagAt(int, int);
	bool isWin();
	void randomNow();
	bool isExpaned(int, int);
private:
	char boardData2[20][100];
	bool isCelInBoard(Vec2);
};

#endif __GAME_BOARD_H__

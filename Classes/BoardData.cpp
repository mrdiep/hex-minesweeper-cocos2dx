#include "BoardData.h"
#include <iostream>

void BoardData::randomNow() {
	int addedMine = 0;
	for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 100; y++) {
			boardData[x][y] = 0;
		}
	}

	srand(time(0));
	flag_left = totalMine;
	while (addedMine < totalMine) {
		int x = rand() % (int)size.x;
		int y = rand() % (int)size.y;
		if (!isCelInBoard(Vec2(x, y))) continue;

		if (boardData[x][y] == 0) {
			boardData[x][y] = -1;
			addedMine++;
		}
	}

	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			vector<Vec2> listArround = getArround(Vec2(x, y));

			int t = 0;
			CCLOG("-------------At x=%d, y=%d", x, y);
			for (int i = 0; i < listArround.size();i++) {
				Vec2 nei = listArround.at(i);

				if (boardData[(int)nei.x][(int)nei.y] == -1) {
					t++;
					CCLOG("NB x=%d, y=%d", (int)nei.x, (int)nei.y);
				}
			}
			if (boardData[x][y] != -1) {
				boardData[x][y] = t;
			}
		}
	}
}
bool BoardData::isCelInBoard(Vec2 position) {
	int x = position.x;
	int y = position.y;

	// hàng lẻ, ít hơn 1 ô
	if (y % 2 == 1 && x >= size.x - 1) return false;

	if (x < 0 || y < 0) return false;

	if (x >= size.x) return false;
	if(y >= size.y) return false;

	return true;
}
vector<Vec2> BoardData::getArround(Vec2 position) {
	int x = position.x;
	int y = position.y;

	vector<Vec2> list;

	if (y % 2 == 0) {
		list.push_back(Vec2(x - 1, y + 1));
		list.push_back(Vec2(x, y + 1));

		list.push_back(Vec2(x, y + 2));
		list.push_back(Vec2(x, y - 2));

		list.push_back(Vec2(x - 1, y - 1));
		list.push_back(Vec2(x, y - 1));
	}
	else if (y % 2 == 1) {
		list.push_back(Vec2(x, y + 1));
		list.push_back(Vec2(x + 1, y + 1));

		list.push_back(Vec2(x, y + 2));
		list.push_back(Vec2(x, y - 2));

		list.push_back(Vec2(x, y - 1));
		list.push_back(Vec2(x + 1, y - 1));
	}

	vector<Vec2> validArround;
	for (int i = 0; i < list.size(); i++) {
		Vec2 tCell = list.at(i);
		if (!isCelInBoard(tCell)) continue;
		validArround.push_back(tCell);
	}

	return validArround;
}

bool BoardData::hasMineAt(int x, int y) {
	return boardData[x][y] == -1;
}

bool BoardData::hasFlagAt(int x, int y) {
	return boardData2[x][y] == -3;
}

bool BoardData::setFlag(int x, int y) {
	if (flag_left == 0) return false;
	if (boardData2[x][y] != -2 && boardData2[x][y] != -3) {
		boardData2[x][y] = -3;
		flag_left--;
		return true;
	}

	return false;
}

bool BoardData::removeFlag(int x, int y) {
	if (boardData2[x][y] == -3) {
		boardData2[x][y] = 0;
		flag_left++;
		return true;
	}

	return false;
}

bool BoardData::isWin() {
	//algorithm: all is traved and flag is setted
	for (int x = 0; x < size.x;x++) {
		for (int y = 0;y < size.y;y++) {
			if (!isCelInBoard(Vec2(x, y))) continue;
			if (boardData2[x][y] == 0) {
				return false;
			}
		}
	}

	return true;
}

bool BoardData::isExpaned(int x, int y) {
	return boardData2[x][y] == -2;
}

vector<Vec2> BoardData::touchAt(Vec2 touchAtMinePosition) {
	vector<Vec2> openCells;
	std::queue<Vec2> q;

	if (boardData2[(int)touchAtMinePosition.x][(int)touchAtMinePosition.y] == -2) {
		return openCells;
	}

	q.push(touchAtMinePosition);
	openCells.push_back(touchAtMinePosition);
	boardData2[(int)touchAtMinePosition.x][(int)touchAtMinePosition.y] = -2;

	if (boardData[(int)touchAtMinePosition.x][(int)touchAtMinePosition.y] > 0) {
		return openCells;
	}

	set<int> travelled;
	do {
		Vec2 t = q.front();
		q.pop();
		char currentValue = boardData[(int)t.x][(int)t.y];
		vector<Vec2> arround = getArround(t);
		for (int i = 0;i < arround.size();i++) {
			Vec2 neigboard = arround[i];

			if (!isCelInBoard(neigboard)) continue;

			char neiValue = boardData[(int)neigboard.x][(int)neigboard.y];

			if (neiValue < 0) continue;
			if (travelled.count(neigboard.x * 100 + neigboard.y) != 0) continue;

			if (boardData[(int)neigboard.x][(int)neigboard.y] == 0) {
				boardData[(int)neigboard.x][(int)neigboard.y] = -2;
			}

			travelled.insert(neigboard.x * 100 + neigboard.y);
			openCells.push_back(neigboard);
			if (boardData2[(int)neigboard.x][(int)neigboard.y] == -3) {
				flag_left++; //wrong pos flag
			}
			boardData2[(int)neigboard.x][(int)neigboard.y] = -2;
			
			if (neiValue == 0) { // dont expand the border
				q.push(neigboard);
			}
		}
	} while (!q.empty());

	return openCells;
}
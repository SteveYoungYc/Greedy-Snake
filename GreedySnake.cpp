#include "stdafx.h"
#include <Windows.h>
#include <graphics.h>      // 引用图形库头文件
#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <queue>
#include <vector>
#include <math.h>
//#include "Snake.h"
using namespace std;

#define PI           3.14159265358979323846
#define KEY_A		 0x41
#define KEY_D		 0x44
#define KEY_S		 0x53
#define KEY_W		 0x57

bool isGameRunning = true;
bool lastState = true;
int row_min = 10, row_max = 630;
int colunm_min = 10, colunm_max = 470;
int width = 6;
COLORREF BackgroundColour = BLACK;
COLORREF SnakeColour = RED;
COLORREF FoodColour = BLUE;

class coordinate {
private:
public:
	int x;
	int y;
	coordinate(const coordinate& p) {
		x = p.x;
		y = p.y;
	}
	coordinate(const int X, const int Y) {
		x = X;
		y = Y;
	}
	void set(double theta) {
		x += static_cast<int>(cos(theta * PI / 180) * (width + 1));
		y += static_cast<int>(sin(theta * PI / 180) * (width + 1));
	}
	bool operator==(const coordinate& p) {
		if (this->x == p.x && this->y == p.y)
			return true;
		else
			return false;
	}
	~coordinate() {

	}
};

//enum direction { Up, Down, Right, Left };

class Snake {
private:
	int dir;
	queue<coordinate> pos;
	int initialLength;
	int Width;
	COLORREF colour;
	bool isEaten = false;
public:
	Snake(int Length, int wid, COLORREF col, const int& theta) {
		this->Width = wid;
		this->initialLength = Length;
		this->colour = col;
		dir = theta;
	}
	~Snake() {
	}
	void init();
	void Draw();
	void Background();
	void KeyboardControl();
	void HandleLogic();
	void EatSelf(queue<coordinate> &q, coordinate n);
};

coordinate head(10, 31);
coordinate tail(10, 10);
coordinate food(320, 240);
Snake snake(width * 30, width, RED, 0);

void ColRectangle(int bottom, int top, int left, int right, COLORREF col);
void CenterRectangle(int CenterX, int CenterY, int line, COLORREF col);

bool IsKeyDown(int key) {
	return (GetAsyncKeyState(key) & 0x8000 ? 1 : 0);
}

void Snake::init() {
	initgraph(640, 480);													// 初始化EasyX环境
	setbkcolor(BackgroundColour);											// 设置背景色为白色
	cleardevice();															// 用背景色清空屏幕
	rectangle(5, 5, 635, 475);
	for (coordinate i(tail); i.y <= initialLength; i.y += (width + 1)) {
		pos.push(i);
	}
	CenterRectangle(food.x, food.y, width, FoodColour);
}

void Snake::Draw() {
	head.set(dir);
	tail = pos.front();
	CenterRectangle(tail.x, tail.y, width, BackgroundColour);
	pos.pop();
	pos.push(head);
	CenterRectangle(head.x, head.y, width, SnakeColour);
}

void Snake::Background() {
	lastState = isEaten;
	HandleLogic();			// 处理游戏逻辑
	if (isEaten != lastState) {
		srand((unsigned)time(NULL));
		food.x = (rand() % (row_max - row_min + 1)) + row_min;
		food.y = (rand() % (colunm_max - colunm_min + 1)) + colunm_min;
		CenterRectangle(food.x, food.y, width, FoodColour);
	}
}

void  Snake::HandleLogic() {
	if (abs(head.x - food.x) <= abs(cos(dir * PI / 180)*width / 2) + width / 2 + 1
	 && abs(head.y - food.y) <= abs(sin(dir * PI / 180)*width / 2) + width / 2 + 1) {
		isEaten = true;
		head.set(dir);
		pos.push(head);
		initialLength++;
		//ColRectangle(); //food.x + width/2  food.y + width/2
	}
	else {
		isEaten = false;
	}
	queue<coordinate> tmp = pos;
	EatSelf(tmp, head);
}

void Snake::KeyboardControl() {
	if (IsKeyDown(VK_ESCAPE)) {			// esc键按下则退出循环结束游戏
		isGameRunning = false;
		return;
	}
	if (IsKeyDown(KEY_W)) {
		if(dir != 90)
			dir = -90;
	}
	if (IsKeyDown(KEY_S)) {
		if (dir != -90)
			dir = 90;
	}
	if (IsKeyDown(KEY_D)) {
		if (dir != 180)
			dir = 0;
	}
	if (IsKeyDown(KEY_A)) {
		if (dir != 0)
			dir = 180;
	}
}

void Snake::EatSelf(queue<coordinate> &q, coordinate n) {
	int size = q.size();
	for (int i = 0; i < size - 2; ++i) {
		coordinate a = q.front();
		if (a == n) {
			for (int j = 0; j < i; ++j) {
				CenterRectangle(pos.front().x, pos.front().y, width, BackgroundColour);
				pos.pop();
			}
		}
		q.pop();
	}
}

void ColRectangle(int bottom, int top, int left, int right, COLORREF col) {
	for (int i = left; i <= right; i++) {
		for (int j = bottom; j <= top; j++) {
			putpixel(i, j, col);
		}
	}
}

void CenterRectangle(int CenterX, int CenterY, int line, COLORREF col) {
	for (int i = CenterX - line / 2; i <= CenterX + line / 2; i++) {
		for (int j = CenterY - line / 2; j <= CenterY + line / 2; j++) {
			putpixel(i, j, col);
		}
	}
}

int main() {
	snake.init();
	while (isGameRunning) {
		snake.KeyboardControl();		// 处理键盘操作
		snake.Draw();			// 处理游戏逻辑
		snake.Background();
		Sleep(100);						// 等待30毫秒，控制帧率
	}
	closegraph();						// 清理EasyX环境
}




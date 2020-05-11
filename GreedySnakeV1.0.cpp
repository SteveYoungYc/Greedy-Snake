#include "stdafx.h"
#include <Windows.h>
#include <graphics.h>
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
bool isSuccessful = true;
bool lastState = true;							//record the laststate of whether the food is eaten
int row_min = 5, row_max = 635;					//boundary of the map
int colunm_min = 5, colunm_max = 475;			//boundary of the map
int width = 6;									//width of the snake
COLORREF BackgroundColour = CYAN;
COLORREF SnakeColour = LIGHTGREEN;
COLORREF HeadColour = RED;						//colour of its head
COLORREF FoodColour = YELLOW;
COLORREF PoisonColour = MAGENTA;

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
	void copy(const coordinate& p) {			
		x = p.x;
		y = p.y;
	}
	void move(double theta) {					//snake can move by the angle of the snake's direction
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

class Snake {
private:
	int dir;									//snake's way forward
	int Width;									//width of the snake
	int foodEaten;								//the amount of the eaten food
	bool isEaten = false;						//if the snake is about to eat isEaten will be true
	queue<coordinate> pos;						//a queue that save the position of the hole snake
public:
	Snake(int wid, const int& theta) {
		this->Width = wid;
		dir = theta;
		foodEaten = 0;
	}
	~Snake() {
	}
	void Init();										//initiate the snake
	void DrawSnake();									//draw the whole snake	
	void Background();									//mainly draw the food on the map
	void KeyboardControl();								//detect key press
	void HandleLogic();									//mainly change isEaten and isSuccessful
	void EatSelf(queue<coordinate> &q, coordinate n);   //if its head touch the body it will cut itself
};

coordinate head(10, 10  + (width + 1) * 10);
coordinate tail(10, 10);
coordinate food(320, 240);
Snake snake(width, 0);

void ColRectangle(int bottom, int top, int left, int right, COLORREF col);	//not used
void CenterRectangle(int CenterX, int CenterY, int line, COLORREF col);		//draw a square with center coordinates and side length

bool IsKeyDown(int key) {
	return (GetAsyncKeyState(key) & 0x8000 ? 1 : 0);						//detect key press
}

void Snake::Init() {
	initgraph(640, 480);													//initiate the map
	setbkcolor(BackgroundColour);											//set the background colour
	cleardevice();															//clear the screen
	rectangle(row_min, colunm_min, row_max, colunm_max);					//draw the boundary
	for (coordinate i(tail); i.y <= (width + 1) * 12; i.y += (width + 1)) { 
		pos.push(i);														//draw the initial snake
	}
	CenterRectangle(food.x, food.y, width, FoodColour);						//draw the first food in the center of the map
}

void Snake::DrawSnake() {
	if (isSuccessful) {														//if its head is within the boundary
		head.move(dir);														//the head is moving
		tail = pos.front();
		CenterRectangle(tail.x, tail.y, width, BackgroundColour);			//the tail is moving
		pos.pop();
		pos.push(head);
		queue<coordinate> tmp = pos;
		int size = tmp.size();
		for (int i = 0; i < size - 1; ++i) {								//traverse the snake to draw its body with SnakeColour
			CenterRectangle(tmp.front().x, tmp.front().y, width, SnakeColour);
			tmp.pop();
		}
		CenterRectangle(tmp.front().x, tmp.front().y, width, HeadColour);	//draw its head with HeadColour
	}
}

LPCTSTR failString = _T("GAME OVER");

void Snake::Background() {
	if (isSuccessful) {														//if its head is within the boundary
		lastState = isEaten;												//record the laststate of whether the food is eaten
		HandleLogic();														//assignment isEaten and isSuccessful
		if (isEaten && !lastState) {										//the snake is eating
			srand((unsigned)time(NULL));									
			food.x = (rand() % (row_max - row_min + 1 - 10)) + row_min + 5;
			food.y = (rand() % (colunm_max - colunm_min + 1 - 10)) + colunm_min + 5;
			CenterRectangle(food.x, food.y, width, FoodColour);				//draw the next food randomly
		}
	}
	else {
		outtextxy(280, 220, failString);									//if its head is within the boundary the game will be over
	}
}

void  Snake::HandleLogic() {
	if (abs(head.x - food.x) <= abs(cos(dir * PI / 180)*width / 2) + width / 2 + 1 //the head hits the food and the lateral offset is relatively small
	 && abs(head.y - food.y) <= abs(sin(dir * PI / 180)*width / 2) + width / 2 + 1) {
		isEaten = true;
		foodEaten++;														//record the amount of the eaten food
		head.move(dir);
		pos.push(head);														//lengthen its body
		CenterRectangle(food.x, food.y, width, BackgroundColour);			//clear the food
	}
	else {
		isEaten = false;
	}
	queue<coordinate> tmp = pos;
	EatSelf(tmp, head);
	if (pos.back().x < row_min + width/2 || pos.back().x > row_max - width/2 ||		//if its head is within the boundary
		pos.back().y < colunm_min + width/2 || pos.back().y > colunm_max - width/2) {
		isSuccessful = false;
	}
	Sleep(100 - foodEaten * 5);												//the snake moves faster with the amount of the eaten food is growing
}

void Snake::KeyboardControl() {
	if (IsKeyDown(VK_ESCAPE)) {			//the game will exit if esc is pressed
		isGameRunning = false;
		return;
	}
	if (IsKeyDown(KEY_W)) {				//go up
		if (dir != 90)
			dir = -90;
	}
	if (IsKeyDown(KEY_S)) {				//go down
		if (dir != -90)
			dir = 90;
	}
	if (IsKeyDown(KEY_D)) {				//go right
		if (dir != 180)
			dir = 0;
	}
	if (IsKeyDown(KEY_A)) {				//go left
		if (dir != 0)
			dir = 180;
	}
	if (!isSuccessful) {
		if (IsKeyDown(VK_SPACE)) {		//TODO: restart the game
			isSuccessful = true;
			Init();
		}
	}
}

void Snake::EatSelf(queue<coordinate> &q, coordinate n) {
	int size = q.size();
	for (int i = 0; i < size - 2; ++i) {
		coordinate a = q.front();
		if (a == n) {								//if its head touch its body
			for (int j = 0; j < i; ++j) {			//cut itself
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
	snake.Init();
	while (isGameRunning) {
		snake.KeyboardControl();		
		snake.DrawSnake();			
		snake.Background();
	}
	closegraph();						
}




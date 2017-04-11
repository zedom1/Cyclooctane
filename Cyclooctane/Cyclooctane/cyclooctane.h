#include<iostream>
#include<algorithm>
#include<string.h>
#include<cmath>
#include<string>
#include<stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
using namespace std;

struct Charactor;  // 角色
struct Monster;   //小怪
struct Boss;   //boss
struct Prop;   //道具
struct Obstacle;  // 障碍
struct Room;  // 房间

void gotoxy(int x,int y);
void startup();  //初始化 界面、游戏数据初始化
void show();  
void updateWithoutInput();  //与输入无关数据的更新
void updateWithInput();  // 与输入相关数据更新

struct Charactor //角色
{
	string name;
	int pos_x,pos_y;
	int direction;
	bool judge_round;
public:
	void print_now();
};

struct Monster //小怪
{
protected:
	string name;
	int pos_x,pos_y;
public:
	void print_now();
};

struct Boss:public Monster // Boss
{
	
public:
	void skill();  //技能
	void print_now();
};

struct Prop
{
	string name;
	string describe;
	int pos_x,pos_y;
	bool eat;
	bool exist;
public:
	void print_now();
};

struct Obstacle // 障碍
{
	int pos_x,pos_y;

public:
	void print_now();
};

struct Room  // 房间
{
	Obstacle *obstacle;

public:
	void print_now();
};
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
struct Bullet;  // 子弹

void gotoxy(int x,int y);
void hidden();  //隐藏光标

struct Bullet
{
	int pos_x,pos_y;
	Bullet *next;
public:

};

struct Charactor //角色
{
	string name;
	int pos_x,pos_y;
	bool judge_round;   // 判断能否旋转地图  
	Bullet *bullet;
	int speed;
	POINT print_chara[12];
public:
	Charactor();
	~Charactor();
	void print_cha_new(int x,int y,POINT print_chara[]);
	void print_cha_old(int x,int y,POINT print_chara[]);
	void new_point(int x,int y, POINT print_chara[]);
	void print_round_new(int x,int y,POINT print_chara[]);
	void print_part_cha_new(int x,int y, POINT print_chara[]);
	void judge_input();
};

struct Monster //小怪
{
protected:
	string name;
	int pos_x,pos_y;
	int speed;
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
	int speed;
	bool judge_show;
public:
	void print_now();
};

struct Room  // 房间
{
	Obstacle *obstacle;
	POINT pos_room[];
public:
	Room();
	~Room();
	void print_now();
	void new_point();
};
struct Level   // 层
{
	static int num_level;
public:
	Room *room;
	Level();
	~Level();
};

struct Game
{
	Charactor ben;
	Level *level;
	Prop *prop;
public:
	void startup();
	void updateWithInput();
	void updateWithoutInput();
	void show();
};
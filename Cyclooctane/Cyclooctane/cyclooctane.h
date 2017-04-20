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

struct Vector;
struct Charactor;  // 角色
struct Monster;   //小怪
struct Boss;   //boss
struct Prop;   //道具
struct Obstacle;  // 障碍
struct Room;  // 房间
struct Bullet;  // 子弹
struct Square;

void gotoxy(int x,int y);
void hidden();  //隐藏光标

struct Vector
{
	double x,y;
public:
	Vector(double x1, double y1);
	Vector(const Vector& a);
	Vector() ;

	Vector operator = (Vector a);
	Vector operator - (Vector a);

	Vector vertical() ; //把向量变成其垂直向量
	double get_lenth();
	Vector new_normalize();
	double dotmulti(Vector a);
};

struct Bullet
{
	double pos_x,pos_y;
	double xita;
	bool exist;
	int life;
	double speed;
	struct Bullet *nex;
public:
	static int num_time_count;
	Bullet(double x,double y,double xi);
	void print_bul_new(double pos_x, double pos_y);
	void print_bul_old(double pos_x, double pos_y);
};


struct Charactor //角色
{
	string name;
	double pos_x,pos_y;
	bool judge_round;   // 判断能否旋转地图  
	Bullet *head,*last;
	int num_bul;
	double speed;
	POINT print_chara[12];
public:
	Charactor();
	~Charactor();
	void print_cha_new(double x,double y,POINT print_chara[]);
	void print_cha_old(double x,double y,POINT print_chara[]);
	void new_point(double x,double y, POINT print_chara[]);
	void print_round_new(double x,double y,POINT print_chara[]);
	void print_part_cha_new(double x,double y, POINT print_chara[]);
	void judge_input();
};

struct Monster //小怪
{
protected:
	string name;
	double pos_x,pos_y;
	double speed;
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
	double pos_x,pos_y;
	bool eat;
	bool exist;
public:
	void print_now();
};

struct Obstacle // 障碍
{
	double pos_x,pos_y;
	double speed;
	bool judge_show;
public:
	void print_now();
};

struct Room  // 房间
{
public:
	//Room();
	//~Room();
	virtual void new_room_point (double pos_x, double pos_y, double angle , POINT pos[])=0;
	virtual void paint_room_new(double pos_x, double pos_y, POINT pos[], double angle)=0;
	virtual void paint_room_old(double pos_x, double pos_y, POINT pos[],double angle)=0;
};

struct Level   // 层
{
	static int num_level;
public:
	Room *room;
	Level();
	~Level();
};

struct Square
{
public:
	double pos_x,pos_y;
	double angle,init;
	int num_com,num_go; //door
	Obstacle *obstacle;
	POINT pos[10];
	POINT edge1[5],edge2[5],edge3[5],edge4[5];
	Square();
	~Square();
	virtual void new_room_point (double squ_x, double squ_y, double angle , POINT pos[]);
	virtual void paint_room_new(double pos_x, double pos_y, POINT pos[], double angle);
	virtual void paint_room_old(double pos_x, double pos_y, POINT pos[],double angle);
	virtual void judge_input(double speed,bool judge_round);
};

struct Game
{
	Charactor ben;
	Level *level;
	Prop *prop;
//	Room *room;
	Square square;
public:
	void startup();
	void updateWithInput();
	void updateWithoutInput();
	void show();
	void judge_bullet(int start, int end, POINT pos[], double &x, double &y, double &xita);
	void update_bullet();
	bool judge_coll_single(POINT first[], int num_first, POINT second[], int num_second, Vector &shadow, double& num_move);  // 动态墙壁与人的碰撞检测
	bool judge_coll();
	void print_new();
};
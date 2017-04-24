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
	Vector operator + (Vector a);
	Vector operator * (double a);
	Vector operator / (double a);

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
	POINT print_chara[14];
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
public:
	string name;
	double pos_x,pos_y;
	double speed;
	int num_edge;
	bool exist;
	Monster(int num);
	Monster();
	POINT pos[10];
	static int num_total;
	void print_now(int x, int y, int num, POINT pos[]);
	void new_point(int x, int y, int num, POINT pos[]);
	void print_old(int x, int y, int num, POINT pos[]);
	void create_new_monster();
};

struct Boss//:public Monster // Boss
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
	double init,dis;
	int count;
	POINT stab[7];
public:
	Obstacle();
	Obstacle(double x, double y);
	static const double r;
	void print_now(double angle);
	void print_old();
	void new_point();
	void new_center(double angle);
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
	double pos_x,pos_y;   // 中心坐标
	double angle,init;   //init为初始角度 ,angle为变化角度
	int num_com,num_go; //door
	//Obstacle *obstacle;
	POINT pos[10];
	POINT edge1[5],edge2[5],edge3[5],edge4[5];
	POINT teleport[4][2];
	POINT corner[4];
	Square();
	~Square();
	virtual void new_room_point (double pos_x, double pos_y, double angle , POINT pos[]); //更新坐标数组
	virtual void paint_room_new(double pos_x, double pos_y, POINT pos[], double angle); // 画新房间
	virtual void paint_room_old(double pos_x, double pos_y, POINT pos[],double angle); //抹去旧房间
	virtual void judge_input(double speed,bool judge_round);   // 根据输入更新角度

//	void tester();
};

struct Game
{
	Charactor ben;
	Level *level;
	Prop *prop;
	Obstacle *obstacle;
//	Room *room;
	Square square;
	Monster monster[500];
public:
	void startup();
	void updateWithInput();
	void updateWithoutInput();
	void show();
	void judge_bullet(int start, int end, POINT pos[], double &x, double &y, double &xita);
	void update_bullet();
	bool judge_coll_single(POINT first[], int num_first, POINT second[], int num_second, Vector &shadow, double& num_move);  // 动态墙壁与人的碰撞检测
	bool judge_coll_chara_to_wall();
	void print_new();
	bool judge_circle_coll(Vector circle_up, Vector circle_down,POINT second[],int num_second);
	void judge_coll_mon_to_wall();
	void judge_coll_cha_to_mon();
	void judge_coll_mon_to_mon();
	void judge_coll_corner(double &pos_x, double& pos_y, POINT second[], int num_second, double center_x, double center_y);
	void judge_coll_cha_to_corner();
	void judge_coll_mon_to_corner(int i);
	void judge_coll_mon_to_obstacle();
};
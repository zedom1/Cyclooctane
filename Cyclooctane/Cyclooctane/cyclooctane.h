#ifndef MY_GAME
#define MY_GAME

#include<iostream>
#include<cmath>
#include<string>
#include<string.h>
#include<stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <assert.h>
#include <tchar.h>
using namespace std;

// 基础类
struct Node;
struct Vector;

// 游戏组成类
struct Charactor; 
struct Monster; 
struct Obstacle; 
struct Room; 
struct Bullet;  
struct Square;
struct Game;

// 游戏数据类
struct Data_Base;

// 游戏状态类
struct State;   
struct MENU_START;
struct MENU_CHA;
struct ON_GAME;
struct MENU_PAUSE;
struct MENU_DEAD;
struct EXIT;
struct MENU_CHANGE;


struct Node
{
public:
	POINT pos;
	int fa;
	double fx,gx,hx;
	Node operator = (Node a);
	bool operator ==(Node a);
	bool operator != (Node a);
	Node();
	Node(double x,double y, int a, double fx1, double gx1, double hx1 );
};

struct Vector
{
	double x,y;
public:
	Vector(double x1, double y1);
	Vector(POINT a, POINT b);
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
	double speed;
	int life;
	Bullet *nex;
public:
	static int num_time_count;
	Bullet(double x,double y,double xi);
	Bullet();
	void print_bul_new(double pos_x, double pos_y);
	void print_bul_old(double pos_x, double pos_y);
	void operator =(Bullet a);
};

struct Charactor //角色
{
	string name;
	double pos_x,pos_y;
	int judge_cha_state;   //  后为大招是否使用
	int judge_dir; // 判断此时的常态方向
	Bullet *head,*last;
	Bullet line,last_line;
	POINT line_array[100];
	int num_bul,num_line_array;
	double speed;
	int range;
	int life,life_now;
	POINT print_chara[14];
	int mod;
	int num_count[3];
public:
	Charactor();
	void print_cha_new(double x,double y,POINT print_chara[]);
	void print_cha_old(double x,double y,POINT print_chara[]);
	void new_point(double x,double y, POINT print_chara[]);
	void print_round_new(double x,double y,POINT print_chara[]);
	void print_part_cha_new(double x,double y, POINT print_chara[]);
	void judge_input();
	void print_cha_line(double x, double y);
	void print_cha_ball(double x, double y,bool judge_old);
	void set_new_data();
	// void set_old_data();
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
	Node path;
	static int num_total;
	void print_now(int x, int y, int num, POINT pos[]);
	void new_point(int x, int y, int num, POINT pos[]);
	void print_old(int x, int y, int num, POINT pos[]);
	void create_new_monster();
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
	Room();
	~Room();
	Obstacle *obstacle;
	Square square;
	Monster monster[500];
	int time_count;
	int old_door;
	void new_room();
	void new_door();
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
	virtual void new_room_point (double pos_x, double pos_y, double angle , POINT pos[]); //更新坐标数组
	virtual void paint_room_new(double pos_x, double pos_y, POINT pos[], double angle); // 画新房间
	virtual void paint_room_old(double pos_x, double pos_y, POINT pos[],double angle); //抹去旧房间
	virtual void judge_input(double speed,int judge_cha_state,int mod);   // 根据输入更新角度

//	void tester();
};

struct Game
{
	Charactor ben;
	Room room;
	Node map[45][45];
	int death_count;
	int room_count;
	friend struct Data_Base;
public:
	Game();
	void startup();
	void updateWithInput();
	void updateWithoutInput();
	void show();
	static void clear();
	void judge_bullet(int start, int end, POINT pos[], double x, double y, double &xita);
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
	void get_path(double x ,double  y, Node &path);
};

struct Data_Base
{
	Charactor co_ben;
	Obstacle *co_obstacle;
	Square co_square;
	Monster co_monster[500];
	int co_death_count;
	int co_Bullet_num_time_count;
	int co_Monster_num_total;
	int co_num_monster_fresh;
	int co_room_count;
	~Data_Base();
	Data_Base();
	Data_Base(const Data_Base& a);
	void store_data(const Data_Base& a);  // 复制数据
	void store_data(const Game& b);  // 保存当前数据（备份）
	void fresh_data();  // 重置数据
	void set_data(Game& a);   // 数据上传至游戏
	void write_data();    // 存档（从文件）
	void read_data();    // 读档（从文件）
};

struct State  
{  
    virtual State* transition(int) = 0;   // 状态转移
	virtual void eventt()=0;  // 该状态的事件
};

struct MENU_START:public State  
{  
	void eventt();
    State* transition(int); 
};  
  
struct MENU_CHA:public State  
{  
    State* transition(int);  
	void eventt();
};  
  
struct ON_GAME:public State  
{  
    State* transition(int);  
	void eventt();
};  
  
struct MENU_PAUSE:public State  
{  
    State* transition(int);  
	void eventt();
};  
  
struct MENU_DEAD:public State  
{  
    State* transition(int);
	void eventt();
};  

struct EXIT:public State  
{
    State* transition(int);  
	void eventt();
};  

struct CHANGE:public State  
{
    State* transition(int);  
	void eventt();
};  

struct FSM
{  	
public:  
	static void reset();  
	static State *current;
};

void gotoxy(int x,int y);
int normalize_x(double x);  // 找到坐标所在方格的中心点x坐标
int normalize_y(double y);   // 找到坐标所在方格的中心点y坐标
int get_i(double x);   // 该中心对应map的i值
int get_j(double y);  // 该中心对应map的j值
void quicksort(int first, int last , Node* a);
bool judge_coll_line(POINT a , POINT b, POINT c, POINT d, POINT &cut);  // 线段相交判定并求交点（若有）
void initi();  // 窗体初始化
double point_to_line(POINT a, POINT head, POINT last); // 点到线段距离

#endif
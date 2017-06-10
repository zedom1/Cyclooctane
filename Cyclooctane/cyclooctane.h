#ifndef MY_GAME
#define MY_GAME

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <assert.h>
#include <tchar.h>
using namespace std;

// ������
struct Node;
struct Vector;

// ��Ϸ�����
struct Charactor; 
struct Monster; 
struct Obstacle; 
struct Stab;
struct Stone;
struct Bullet;  
struct Square;
struct Room; 
struct Game;

// ��Ϸ������
struct Data_Base;

// ��Ϸ״̬��
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
	POINT fa;
	int fx,gx,hx;
	int ground;

	Node& operator = ( Node &a);
	bool operator ==(Node a);
	bool operator != (Node a);
	Node();
	Node(int a); // 0 ͨ· 1 �ϰ� 2��� 3�յ�
	~Node();
	int cal_hx(int x, int y);
	int cal_fx();
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

	Vector vertical() ; //����������䴹ֱ����
	double get_lenth();
	Vector new_normalize();  // ��λ��
	double dotmulti(Vector a);  // ���ڻ�
};

struct Bullet
{
	double pos_x,pos_y;
	double xita;
	bool exist;
	double speed;
	bool special;
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

struct Charactor //��ɫ
{
	char name[15];
	double pos_x,pos_y;
	int judge_cha_state;   //  ״̬
	int judge_dir; // �жϴ�ʱ�ĳ�̬����
	int judge_hurt; // ���˺��޵�һС��ʱ��
	Bullet *head,*last;
	Bullet line,last_line;
	Bullet special;
	POINT line_array[100];
	int num_bul,num_line_array;
	double speed;
	int range;
	int life,life_now;
	POINT print_chara[14];
	int mod;
	int num_count[4];
public:
	Charactor();
	void print_cha_new(double x,double y,POINT print_chara[]);
	void print_cha_old(double x,double y,POINT print_chara[]);
	void new_point(double x,double y, POINT print_chara[]);
	void print_round_new(double x,double y,POINT print_chara[]);
	void print_part_cha_new(double x,double y, POINT print_chara[]);
	void print_cha_line(double x, double y);
	void print_cha_ball(double x, double y,bool judge_old);
	void set_new_data();
	void update();
	// void set_old_data();
};

struct Monster //С��
{
public:
	double pos_x,pos_y;
	double speed;
	int num_edge;
	bool exist;
	int special;
	Monster(int num);
	Monster();
	POINT pos[10];
	POINT path;
	static int num_total;
	static int num_count;
	void print_now(int x, int y, int num, POINT pos[]);
	void new_point(int x, int y, int num, POINT pos[]);
	void print_old(int x, int y, int num, POINT pos[]);
	void create_new_monster(int x, int y, Square square);
};

struct Obstacle // �ϰ�
{
public:
	double pos_x,pos_y;//���ϰ����λ��
	double init,dis;//�뷿�����ĵĳ�ʼ�ǶȺ;��� (������)
	POINT pos[5];
	Obstacle();
	static const double r;
	void print_old();
	virtual void new_point()=0;
	void new_center(double angle);
};
struct Stab:public Obstacle
{
	int count;// �ش����ϰ���ļ�ʱ��
	POINT stab[7];// �ش����ϰ���Ĵ���������
	bool judge_show;// �ش����ϰ��ж��Ƿ������˺�
	int count_max;
	virtual void new_point();
	void print_now(double angle);
	void reset();
	void fresh_point();
	Stab();
	Stab(double x, double y);
};
struct Stone:public Obstacle
{
	virtual void new_point();
	Stone();
	Stone(double x, double y);
	void reset();
	void print_now(double angle);
	void fresh_point();
};

struct Square
{
public:
	double pos_x,pos_y;   // ��������
	double angle,init;   //initΪ��ʼ�Ƕ� ,angleΪ�仯�Ƕ�
	POINT pos[10];
	POINT edge1[5],edge2[5],edge3[5],edge4[5];
	POINT corner[4];
	Square();
	virtual void new_room_point (double pos_x, double pos_y, double angle , POINT pos[]); //������������
	virtual void paint_room_new(double pos_x, double pos_y, POINT pos[], double angle); // ���·���
	virtual void paint_room_old(double pos_x, double pos_y, POINT pos[],double angle); //Ĩȥ�ɷ���
	virtual void judge_input(double speed,int judge_cha_state,int mod);   // ����������½Ƕ�
//	void tester();
};

struct Room  // ����
{
public:
	Room();
	~Room();
	Stab *stab;
	Stone *stone;
	Monster monster[500];
	POINT door[5];
	int num_stab,num_stone;
	int time_count;  // ��ʱ ʱ��һ�����ţ�����ʧȥ��������
	int rand_c;   //������λ����ص������
	int time_max;//��ʱ�����ޣ�ÿ������������ɣ��������洳�ؽ��ж�����
	void new_room(int a);// �·���
	void new_door(POINT door[], double angle);// ���ɲ�����
	void update_monster(int x, int y, Square square);
	void get_path(int x ,int  y, int aim_x, int aim_y, POINT &path, int special);
	//Room operator = (const Room & a );
};

struct Game
{
	Charactor ben;
	Room room;
	Square square;
	int death_count;
	int room_count;
	bool judge_update;
	friend struct Data_Base;
public:
	Game();
	void startup();
	void updateWithInput(); // ��������صĸ���
	void updateWithoutInput(); // �������޹صĸ���
	void show();
	static void clear();  //������
	void judge_bullet(int start, int end, POINT pos[], double x, double y, double &xita); //�ӵ�����
	void update_bullet(); // �ӵ����¡�ɱ��
	bool judge_coll_chara_to_wall();
	void print_new();
	void judge_coll_mon_to_wall();
	void judge_coll_cha_to_mon();
	void judge_coll_mon_to_mon();
	void judge_coll_corner(double &pos_x, double& pos_y, POINT second[], int num_second, double center_x, double center_y);
	void judge_coll_cha_to_obstacle();
	void judge_coll_mon_to_corner(int i);
	void judge_coll_mon_to_obstacle();
	void fresh_map();  // ʵʱ���µ�ͼ
	void fresh_room(); // �����л��ж�
	void bomb_hurt(); //������̬�չ���ը�ж�
};

struct Data_Base
{
	Charactor co_ben;
	Square co_square;
	Room co_room;
	bool co_judge_update;
	int co_death_count;
	int co_Bullet_num_time_count;
	int co_Monster_num_total;
	int co_num_monster_fresh;
	int co_room_count;
	int co_Monster_num_count;
	int current_state;
	Bullet store_bul[100]; int num_store_bul;
	~Data_Base();
	Data_Base();
	void store_data(const Game& b);  // ���浱ǰ���ݣ����ݣ�
	void fresh_data();  // ��������
	void set_data(Game& a);   // �����ϴ�����Ϸ
	void write_data();    // �浵�����ļ���
	bool read_data();    // ���������ļ���
};

struct State  
{  
    virtual State* transition(int) = 0;   // ״̬ת��
	virtual void eventt()=0;  // ��״̬���¼�
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
	static void change(int n);
	static State *current;
};

int normalize_x(double x);  // �ҵ��������ڷ�������ĵ�x����
int normalize_y(double y);   // �ҵ��������ڷ�������ĵ�y����
int get_i(double x);   // �����Ķ�Ӧmapp��iֵ
int get_j(double y);  // �����Ķ�Ӧmapp��jֵ
int get_x_from_i(int i);  //����i��÷������ĵ�x����
int get_y_from_j(int j); //����i��÷������ĵ�x����
void quicksort(int first, int last , Node* a);
bool judge_coll_line(POINT a , POINT b, POINT c, POINT d, POINT &cut);  // �߶��ཻ�ж����󽻵㣨���У�
void initi();  // �����ʼ��
double point_to_line(POINT a, POINT head, POINT last); // �㵽�߶ξ���
bool judge_coll_single(POINT first[], int num_first, POINT second[], int num_second, Vector &shadow, double& num_move);  // ��ײ���
bool judge_circle_coll(Vector circle_up, Vector circle_down,POINT second[],int num_second);
bool judge_p_left_right(POINT a, POINT line1, POINT line2);
#endif
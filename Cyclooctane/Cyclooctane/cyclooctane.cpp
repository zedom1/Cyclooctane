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
#include "cyclooctane.h"
using namespace std;
#pragma warning(disable:4244)
HDC hdc;
HWND hwnd;
HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
Vector temp_vector;
const double pi2=2*3.1415926535;
const double pi=3.1415926535;
const double MAX_DOUBLE=1.79769e+308;
const double MIN_DOUBLE=-MAX_DOUBLE;
const int MAX_INT=0x7FFFFFFF;
const int MIN_INT=-MAX_INT-1;
int Bullet::num_time_count=0;
int Monster::num_total=0;
int num_monster_fresh=0;
int num_fresh_count=0;

void gotoxy(int x,int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle,pos);
}
void hidden()
{
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hOut,&cci);
	cci.bVisible=0;//赋1为显示，赋0为隐藏
	SetConsoleCursorInfo(hOut,&cci);
}


void Game::startup()
{
	hidden();
	srand(time(0));
	hwnd=GetConsoleWindow();
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hdc=GetDC(hwnd);
	GetConsoleScreenBufferInfo(hOut, &bInfo ); 
	COORD size={150,43};
	SetConsoleCursorPosition(hOut,size);
	SetConsoleScreenBufferSize(hOut,size);
	SMALL_RECT rc = {0,0, 150-1, 43-1};
	SetConsoleWindowInfo(hOut,true ,&rc);
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));


	ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
	
	
//	monster.print_now(600,600,4,monster.pos);
}
void Game::show()
{
	ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
//	square.tester();
//	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
}
void Game::print_new()
{
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Rectangle(hdc,0,0,1500,990);
	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
	ben.print_cha_new(ben.pos_x,ben.pos_y, ben.print_chara);
}
void Game::updateWithoutInput()
{
	num_monster_fresh++;
	if(num_monster_fresh>10)
	{	
		num_monster_fresh=0;
		monster[Monster::num_total++].create_new_monster();
		if(Monster::num_total>400)
			Monster::num_total=0;
	}
	for(int i=0 ; i<400; i++)
		if(monster[i].exist==true)
		{
			monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
		}
	
	update_bullet();
	judge_coll_cha_to_mon();
	judge_coll_mon_to_mon();
	judge_coll_chara_to_wall();
	judge_coll_mon_to_wall();
	//print_new();
}
void Game::update_bullet()
{
	if(ben.last!=ben.head)
	{
		Bullet *bul=ben.head->nex,*pre_bul=ben.head;
		while(bul!=NULL)
		{
			bul->life--;
			if(bul->life==0)
				bul->exist=false;
			if(bul->exist==true)
			{
				if( ( (bul->pos_x-square.pos_x)*(bul->pos_x-square.pos_x)  )+( (bul->pos_y-square.pos_y)*(bul->pos_y-square.pos_y) )>=350*350*2  )
					bul->exist=false;
				bul->print_bul_old(bul->pos_x,bul->pos_y);
				bul->pos_x+=bul->speed*cosf(bul->xita);
				bul->pos_y-=bul->speed*sinf(bul->xita);
				bul->print_bul_new(bul->pos_x,bul->pos_y);
				judge_bullet(5,8,square.pos,bul->pos_x, bul->pos_y, bul->xita);
				Vector circle_up(bul->pos_x-5,bul->pos_y-5),circle_down(bul->pos_x+5,bul->pos_y+5);
				for(int i=0; i<400; i++)
					if( judge_circle_coll(circle_up,circle_down,monster[i].pos,monster[i].num_edge)==true  )
					{	
						bul->exist=false;
						monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
						monster[i].num_edge--;
						if(monster[i].num_edge<3)
						{	
							monster[i].exist=false;
						}
						if(monster[i].exist!=false)
							monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
					}
			}
			else
			{
				bul->print_bul_old(bul->pos_x,bul->pos_y);
				if(bul->nex!=NULL)
				{	pre_bul->nex=bul->nex;
				delete bul;
				bul=pre_bul->nex;
				continue;
				}
			}
			pre_bul=pre_bul->nex;
			bul=bul->nex;
		}
	}
	if(ben.head->nex==NULL)
	{	ben.head->nex=ben.last;
	ben.last->nex=NULL;
	}
	return;
}
void Game::updateWithInput()
{
	ben.judge_input();
	if((GetAsyncKeyState('W')<0)||(GetAsyncKeyState('S')<0)|| (GetAsyncKeyState('A')<0) ||(GetAsyncKeyState('D')<0))
	{
		double old_x=ben.pos_x,old_y=ben.pos_y;
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		if(GetAsyncKeyState('W')<0) 
			ben.pos_y=old_y-ben.speed;
		if(GetAsyncKeyState('S')<0) 
			ben.pos_y=old_y+ben.speed; 
		if(GetAsyncKeyState('A')<0) 
			ben.pos_x=old_x-ben.speed;
		if(GetAsyncKeyState('D')<0) 
			ben.pos_x=old_x+ben.speed; 
		
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);	
	}
	ben.print_round_new(ben.pos_x, ben.pos_y,ben.print_chara);
	square.judge_input(ben.speed*3.0/100.0,ben.judge_round);
	if(GetAsyncKeyState('Y')<0) 
	{	
		if(ben.judge_round==false)
		{	
			ben.judge_round=true;
			ben.print_part_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		}
		else
		{	ben.judge_round=false;
			ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		}
	}
}
void Game::judge_bullet(int start, int end, POINT pos[], double &x, double &y, double &xita)
{
	for(int i=start; i<=end; i++)
	{
		int j=i+1;
		double k=(pos[i].y*1.0-pos[j].y*1.0)/(pos[i].x*1.0-pos[j].x*1.0);
		double b=pos[i].y*1.0-k*pos[i].x;
		double c=k*x*1.0+b-y*1.0;
		double d=abs(c*1.0)/sqrt(k*k+1);
		if(pos[i].x==pos[j].x)
			d=abs(x-pos[i].x);
		if(d<10)
		{
			if(abs(k)<1e-6|| abs(pos[i].x*1.0-pos[j].x*1.0)<2|| abs(pos[i].y*1.0-pos[j].y*1.0)<2) {xita=pi+xita; return;}
			if( !( (x<=max(pos[i].x,pos[j].x))&&(x>=min(pos[i].x,pos[j].x))&&(y<=max(pos[i].y,pos[j].y))&&(y>=min(pos[i].y,pos[j].y))))
				return; 
			double linex=pos[i].x*1.0-pos[j].x*1.0,liney=pos[i].y*1.0-pos[j].y*1.0;
			double ix=cosf(xita),iy=sinf(xita);
			if(abs(linex)<1e-6) {liney=1.0; linex=0.0;}
			else {liney/=linex; linex=-1.0;}
			
			double tem=sqrt(ix*ix+iy*iy),angle=0;
			if(abs(tem)<1e-6)  angle=0;
			else
			{
				angle=asinf(iy/tem);
				if(ix>1e-6)
				{
					if(iy>1e-6) angle=angle;
					else angle=angle+pi2;
				}
				else
					angle=pi-angle;
			}
			 tem=sqrt(linex*linex+liney*liney);
			 double angle1=0;
			if(abs(tem)<1e-6)  angle1=0;
			else
			{
				angle1=asinf(liney/tem);
				if(linex>1e-6)
				{
					if(liney>1e-6) angle1=angle1;
					else angle1=angle1+pi2;
				}
				else
					angle1=pi-angle1;
			}
			
			xita=2*angle1-angle;
			return;
		}
	}
}
bool Game::judge_coll_single(POINT first[], int num_first, POINT second[], int num_second, Vector& shadow, double& num_move)
{
	double min_move=MAX_DOUBLE;
	int flag=0;
	for(int i=0; i<num_first; i++)   // 求第一个图形的各个投影轴
	{
		double maxn_first=MIN_DOUBLE,minx_first=MAX_DOUBLE;
		double maxn_second=MIN_DOUBLE,minx_second=MAX_DOUBLE;
		int j=i+1;
		Vector *v1=new Vector( first[i].x,first[i].y);
		Vector *v2=new Vector( first[j].x,first[j].y);
		Vector edge(*v1-*v2);  // 得到人的一条边向量
		edge.vertical();  // 求得边的垂直向量作为投影轴
		edge.new_normalize();   // 计算投影轴的单位向量

		for(int k=0; k<num_first; k++)    // 在该投影轴下主角的最大及最小投影值
		{
			Vector tem(first[k].x,first[k].y);
			double tot=tem.dotmulti(edge);
			maxn_first=max(tot,maxn_first);
			minx_first=min(tot,minx_first);		
		}
		for(int k=0; k<num_second; k++)
		{
			Vector tem(second[k].x,second[k].y);
			double tot=tem.dotmulti(edge);
			maxn_second=max(tot,maxn_second);
			minx_second=min(tot,minx_second);
		}
		if( (minx_second>maxn_first) || (minx_first>maxn_second)   )  // 代表中间有空隙
		{	flag=1; break; }
		if(minx_second<=maxn_first)
		{
			if(abs(min_move)>=abs(minx_second-maxn_first))
			{	
				shadow=edge;
				min_move=abs(minx_second-maxn_first);
			}
		}
		else if(minx_first<=maxn_second)
		{
			if(abs(min_move)>=abs(minx_first-maxn_second))
			{	
				shadow=edge;
				min_move=abs(minx_first-maxn_second);
			}
		}
	}
	if(flag==1)
		return false;  // false代表没有发生碰撞
	for(int i=0; i<num_second; i++)   // 求第一个图形的各个投影轴
	{
		double maxn_first=MIN_DOUBLE,minx_first=MAX_DOUBLE;
		double maxn_second=MIN_DOUBLE,minx_second=MAX_DOUBLE;
		int j=i+1;
		Vector *v1=new Vector( second[i].x,second[i].y);
		Vector *v2=new Vector( second[j].x,second[j].y);
		Vector edge(*v1-*v2);  // 得到人的一条边向量
		edge.vertical();  // 求得边的垂直向量作为投影轴
		edge.new_normalize();   // 计算投影轴的单位向量

		for(int k=0; k<num_first; k++)    // 在该投影轴下主角的最大及最小投影值
		{
			Vector tem(first[k].x,first[k].y);
			double tot=tem.dotmulti(edge);
			maxn_first=max(tot,maxn_first);
			minx_first=min(tot,minx_first);
		}
		for(int k=0; k<num_second; k++)
		{
			Vector tem(second[k].x,second[k].y);
			double tot=tem.dotmulti(edge);
			maxn_second=max(tot,maxn_second);
			minx_second=min(tot,minx_second);
		}
		if( (minx_second>maxn_first) || (minx_first>maxn_second)   )  // 代表中间有空隙
		{	flag=1; break; }
		if(minx_second<=maxn_first)
		{
			if(abs(min_move)>=abs(minx_second-maxn_first))
			{	
				shadow=edge;
				min_move=abs(minx_second-maxn_first);
			}
		}
		else if(minx_first<=maxn_second)
		{
			if(abs(min_move)>=abs(minx_first-maxn_second))
			{	
				shadow=edge;
				min_move=abs(minx_first-maxn_second);
			}
		}
	}
	if(flag==1)
		return false;  // false代表没有发生碰撞
	num_move=min_move;
	return true;  //true代表发生了碰撞
}
bool Game::judge_coll_chara_to_wall()
{
	Vector shadow;
	double num_move=0;
	if(judge_coll_single(ben.print_chara,7,square.edge1,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		return true;
	}
	//	printf("Collision!  %d\n", ++count);
	if(judge_coll_single(ben.print_chara,7,square.edge2,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		return true;
	}
	//	printf("Collision!  %d\n", ++count);
	if(judge_coll_single(ben.print_chara,7,square.edge3,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		return true;
	}
	//	printf("Collision!  %d\n", ++count);
	if(judge_coll_single(ben.print_chara,7,square.edge4,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		return true;
	}
	return false;
}
bool Game::judge_circle_coll(Vector circle_up, Vector circle_down,POINT second[],int num_second)
{
	int flag=0;
	Vector edge;   
	double minx=MAX_DOUBLE;
	double mid_circle_x=(circle_up.x+circle_down.x)*1.0/2.0,mid_circle_y=(circle_up.y+circle_down.y)*1.0/2.0;
	for(int i=0; i<num_second ; i++) // 找出多边形与圆心距离最小的点，并将连线作为投影轴
	{
		double tot= (mid_circle_x-second[i].x)*(mid_circle_x-second[i].x)+(mid_circle_y-second[i].y)*(mid_circle_y-second[i].y) ;
		if(tot<minx)
		{
			edge.x=mid_circle_x-second[i].x;
			edge.y=mid_circle_y-second[i].y;
		}
	}
	edge.new_normalize();   // 计算投影轴的单位向量

	double maxn_first=MIN_DOUBLE,minx_first=MAX_DOUBLE;
	double maxn_second=MIN_DOUBLE,minx_second=MAX_DOUBLE;

	{  // 找出子弹在投影轴的投影值
		double tot=circle_up.dotmulti(edge);
		maxn_first=max(tot,maxn_first);
		minx_first=min(tot,minx_first);	
		tot=circle_down.dotmulti(edge);
		maxn_first=max(tot,maxn_first);
		minx_first=min(tot,minx_first);
	}
	for(int k=0; k<num_second; k++) //找出多边形在投影轴的投影值
	{
		Vector tem(second[k].x,second[k].y);
		double tot=tem.dotmulti(edge);
		maxn_second=max(tot,maxn_second);
		minx_second=min(tot,minx_second);
	}
	if( (minx_second>maxn_first) || (minx_first>maxn_second)   )  // 代表中间有空隙
		return false;  // false代表没有发生碰撞
	
	return true;  //true代表发生了碰撞
}
void Game::judge_coll_mon_to_wall()
{
	Vector shadow;
	for(int i=0; i<400; i++)
		if(monster[i].exist)
		{
			double num_move=0;
			if(judge_coll_single(monster[i].pos,monster[i].num_edge,square.edge1,5,shadow,num_move)==true)
			{	
				monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				monster[i].pos_x-=shadow.x*num_move;
				monster[i].pos_y-=shadow.y*num_move;
				monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			}
			if(judge_coll_single(monster[i].pos,monster[i].num_edge,square.edge2,5,shadow,num_move)==true)
			{	
				monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				monster[i].pos_x-=shadow.x*num_move;
				monster[i].pos_y-=shadow.y*num_move;
				monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			}
			if(judge_coll_single(monster[i].pos,monster[i].num_edge,square.edge3,5,shadow,num_move)==true)
			{	
				monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				monster[i].pos_x-=shadow.x*num_move;
				monster[i].pos_y-=shadow.y*num_move;
				monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			}
			if(judge_coll_single(monster[i].pos,monster[i].num_edge,square.edge4,5,shadow,num_move)==true)
			{
				monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				monster[i].pos_x-=shadow.x*num_move;
				monster[i].pos_y-=shadow.y*num_move;
				monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			}
			judge_coll_mon_to_corner(i);
			if( ( (monster[i].pos_x-square.pos_x)*(monster[i].pos_x-square.pos_x)  )+( (monster[i].pos_y-square.pos_y)*(monster[i].pos_y-square.pos_y) )>=350*350*2  )
			{	
				monster[i].exist=false;
				monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			}
		}
	return ;
}
void Game::judge_coll_cha_to_mon()
{
	Vector shadow;
	for(int i=0; i<400; i++)
		if(monster[i].exist)
		{
			double num_move=0;
			if(judge_coll_single(monster[i].pos,monster[i].num_edge,ben.print_chara,7,shadow,num_move)==true)
			{	
				monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				monster[i].pos_x-=shadow.x*num_move;
				monster[i].pos_y-=shadow.y*num_move;
				monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			}
		}
	return ;
}
void Game::judge_coll_mon_to_mon()
{
	Vector shadow;
	for(int i=0; i<400; i++)
		if(monster[i].exist)
			for(int j=i+1; j<400; j++)
				if(monster[j].exist)
				{
					double num_move=0;
					if(judge_coll_single(monster[i].pos,monster[i].num_edge,monster[j].pos,monster[j].num_edge,shadow,num_move)==true)
					{	
						monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
						//monster[j].print_now(monster[j].pos_x,monster[j].pos_y,monster[j].num_edge,monster[j].pos);
						monster[i].pos_x-=shadow.x*num_move;
						monster[i].pos_y-=shadow.y*num_move;
						monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
					}
				}
	return ;
}
void Game::judge_coll_corner(double& pos_x,double& pos_y, POINT second[], int num_second ,double center_x,double center_y)
{
	Vector hori(1,0);
	for(int i=0; i<num_second; i++)
	{
		double tem=sqrt( (pos_x-second[i].x)*(pos_x-second[i].x) + (pos_y-second[i].y)*(pos_y-second[i].y) );
		if( tem<40  )
		{
			Vector a(center_x-second[i].x,center_y-second[i].y);
			double xita=acosf( (a.x-hori.x) / a.get_lenth());
			pos_x+=tem*(cosf(xita));
			pos_y-=tem*(sinf(xita));
		}
	}
	return;
}
void Game::judge_coll_cha_to_corner()
{
	ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
	judge_coll_corner(ben.pos_x, ben.pos_y, square.corner,4,square.pos_x,square.pos_y);
	ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
	return;
}
void Game::judge_coll_mon_to_corner(int i)
{
	monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
	judge_coll_corner(monster[i].pos_x,monster[i].pos_y, square.corner, 4, square.pos_x, square.pos_y);
	monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
	return;
}

Charactor::Charactor()
{
	pos_x=1200; 
	pos_y=812;
	speed=10;
	name="Cyclooo~";
	num_bul=0;
	new_point(pos_x,pos_y,print_chara);
	judge_round=false;
	head=new Bullet(pos_x,pos_y,0);
	last=head;
	head->nex=NULL;
}
Charactor::~Charactor()
{
	delete head;
	delete last;
}
void Charactor::print_cha_new(double x,double y,POINT print_chara[])
{
	new_point(x,y,print_chara);
	::SetDCPenColor(hdc, RGB(123,123,123));  //灰色
	::SetDCBrushColor(hdc,RGB(123,123,123)); //灰色
	Polygon(hdc,print_chara ,7);
	if(judge_round==false)
	{	
		::SetDCPenColor(hdc, RGB(217,31,37)); //红色
		::SetDCBrushColor(hdc,RGB(217,31,37));  //红色
		if(GetAsyncKeyState(VK_UP)<0) 
		{	Ellipse(hdc,x-10,y-25,x+10,y-5); return ; }
		else if(GetAsyncKeyState(VK_DOWN)<0) 
		{	Ellipse(hdc,x-10,y+5,x+10,y+25); return ; }
		else if(GetAsyncKeyState(VK_LEFT)<0)
		{	Ellipse(hdc,x-25,y-10,x-5,y+10); return ; }
		else if(GetAsyncKeyState(VK_RIGHT)<0) 
		{	Ellipse(hdc,x+5,y-10,x+25,y+10); return ; }
			Ellipse(hdc,x+10,y-10,x-10,y+10);
	}
}
void Charactor::print_cha_old(double x,double y,POINT print_chara[])
{
	new_point(x,y,print_chara);
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Polygon(hdc,print_chara ,7);
}
void Charactor::new_point(double x,double y, POINT print_chara[])
{
	POINT apt1[]={x,y-40,x-30,y-14,x-30,y+14,x,y+40,x+30,y+14,x+30,y-14,x,y-40};
	for(int i=0; i<7 ; i++)
	{
		print_chara[i].x=apt1[i].x;
		print_chara[i].y=apt1[i].y;
	}
	return;
}
void Charactor::print_round_new(double x,double y,POINT print_chara[])
{
	if(judge_round==true) { print_cha_new(pos_x,pos_y,print_chara);	 return; }
	Bullet::num_time_count++;
	if(Bullet::num_time_count<3) return;
	Bullet::num_time_count=0;
	if((GetAsyncKeyState(VK_UP)<0)||(GetAsyncKeyState(VK_DOWN)<0)||(GetAsyncKeyState(VK_LEFT)<0)||(GetAsyncKeyState(VK_RIGHT)<0))
	{	
		print_part_cha_new(x,y,print_chara);
		::SetDCPenColor(hdc, RGB(217,31,37));
		::SetDCBrushColor(hdc,RGB(217,31,37));
		if(GetAsyncKeyState(VK_UP)<0) 
		{	
			Ellipse(hdc,x-10,y-25,x+10,y-5);
			last->nex=new Bullet(pos_x,pos_y-25,pi/2);
			last=last->nex;
			return;
		}
		if(GetAsyncKeyState(VK_DOWN)<0) 
		{	
			Ellipse(hdc,x-10,y+5,x+10,y+25);
			last->nex=new Bullet(pos_x,pos_y+25,pi*3/2);
			last=last->nex;
			return;
		}
		if(GetAsyncKeyState(VK_LEFT)<0) 
		{	
			Ellipse(hdc,x-25,y-10,x-5,y+10); 
			last->nex=new Bullet(pos_x-25,pos_y,pi+0.1);
			last=last->nex;
			return;
		}
		if(GetAsyncKeyState(VK_RIGHT)<0) 
		{	
			Ellipse(hdc,x+5,y-10,x+25,y+10);
			last->nex=new Bullet(pos_x+25,pos_y,0);
			last=last->nex;
			return;
		}
	}
	else
		print_cha_new(pos_x,pos_y,print_chara);	
	return ;
}
void Charactor::print_part_cha_new(double x,double y, POINT print_chara[])
{
	new_point(x,y,print_chara);
	::SetDCPenColor(hdc, RGB(123,123,123));
	::SetDCBrushColor(hdc,RGB(123,123,123));
	Polygon(hdc,print_chara ,7);
	return;
}
void Charactor::judge_input()
{
	
}


/*void Square::tester()
{
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2); double tot=0.0822469;
	POINT  tedge1[]=
	{
		pos_x+r2*cos(init*3.0+angle),     pos_y-r2*sin(3.0*init+angle-tot),
		pos_x+r2*cos(init*5.0+angle),     pos_y-r2*sin(init*5.0+angle+tot),
		pos_x+r1*cos(init*5.0+angle),     pos_y-r1*sin(init*5.0+angle+tot),
		pos_x+r1*cos(init*3.0+angle),     pos_y-r1*sin(3.0*init+angle-tot),
		pos_x+r2*cos(init*3.0+angle),     pos_y-r2*sin(3.0*init+angle-tot)
	};
	::SetDCPenColor(hdc, RGB(0,0,255));
	::SetDCBrushColor(hdc,RGB(0,0,255));
	Polygon(hdc,tedge1 ,5);
}  */
void Square::new_room_point(double squ_x, double squ_y, double angle , POINT pos[])
{
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	double tot=0.0822469;
	POINT squ1[]=
	{
		squ_x+r1*cos(init*3.0+angle),     squ_y-r1*sin(3.0*init+angle),		//左上远
		squ_x+r1*cos(init*5.0+angle),     squ_y-r1*sin(init*5.0+angle),		//左下远
		squ_x+r1*cos(-init+angle),     squ_y-r1*sin(-init+angle),		//右下远
		squ_x+r1*cos(init+angle),     squ_y-r1*sin(init+angle),		//右上远
		squ_x+r1*cos(init*3.0+angle),     squ_y-r1*sin(3.0*init+angle),  	//左上远

		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),		//左上近
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle),		//左下近
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle),		//右下近
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle),		//右上近
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),  	//左上近
	};
	POINT tem_corner[]=
	{
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),	
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle),	
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle),	
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle)
	};
	POINT  tedge1[]=
	{
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle),
		squ_x+r1*cos(init*5.0+angle),     squ_y-r1*sin(init*5.0+angle),
		squ_x+r1*cos(init*3.0+angle),     squ_y-r1*sin(3.0*init+angle),
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle)
	};
	POINT tedge2[]=
	{
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle),
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle),
		squ_x+r1*cos(init+angle),     squ_y-r1*sin(init+angle),
		squ_x+r1*cos(-init+angle),     squ_y-r1*sin(-init+angle),
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle)
	};
	POINT tedge3[]=
	{
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle),		//左下近
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle),
		squ_x+r1*cos(-init+angle),     squ_y-r1*sin(-init+angle),
		squ_x+r1*cos(init*5.0+angle),     squ_y-r1*sin(init*5.0+angle),		//左下远
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle)
	};
	POINT tedge4[]=
	{
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle),		//右上近
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),
		squ_x+r1*cos(init*3.0+angle),     squ_y-r1*sin(3.0*init+angle),
		squ_x+r1*cos(init+angle),     squ_y-r1*sin(init+angle),		//右上远
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle)
	};
	for(int i=0; i<10 ; i++)
	{pos[i].x=squ1[i].x;pos[i].y=squ1[i].y;}
	for(int i=0; i<5; i++)
	{edge1[i].x=tedge1[i].x;edge1[i].y=tedge1[i].y;}
	for(int i=0; i<5; i++)
	{edge2[i].x=tedge2[i].x;edge2[i].y=tedge2[i].y;}
	for(int i=0; i<5; i++)
	{edge3[i].x=tedge3[i].x;edge3[i].y=tedge3[i].y;}
	for(int i=0; i<5; i++)
	{edge4[i].x=tedge4[i].x;edge4[i].y=tedge4[i].y;}
	for(int i=0; i<4; i++)
	{corner[i].x=tem_corner[i].x; corner[i].y=tem_corner[i].y;}
	return;
}
void Square::paint_room_new(double squ_x, double squ_y, POINT squ[], double angle)
{
	::SetDCPenColor(hdc, RGB(255,255,255));
	::SetDCBrushColor(hdc,RGB(255,255,255));
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	new_room_point(squ_x,squ_y,angle,squ);
	Polygon(hdc,squ ,10);
}
void Square::paint_room_old(double squ_x, double squ_y, POINT squ[],double angle)
{
	::SetDCPenColor(hdc, RGB(0,0,0));
	::SetDCBrushColor(hdc,RGB(0,0,0));
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	new_room_point(squ_x,squ_y,angle,squ);
	Polygon(hdc,squ ,10);
}
Square::Square()
{
	pos_x=900; 
	pos_y=495;
	angle=0.0;
	init=pi/4.0;
	new_room_point(pos_x,pos_y,angle,pos);
}
Square::~Square(){}
void Square::judge_input(double speed,bool judge_round)
{
	if(judge_round==true)
	if((GetAsyncKeyState(VK_LEFT)<0)||(GetAsyncKeyState(VK_RIGHT)<0))
	{
		paint_room_old(pos_x,pos_y,pos,angle);
		if(GetAsyncKeyState(VK_LEFT)<0) angle+=speed/3.0;
		if(GetAsyncKeyState(VK_RIGHT)<0) angle-=speed/3.0;
		paint_room_new(pos_x,pos_y,pos,angle);
	}
	return;
}



Bullet::Bullet(double x,double y,double xi)
{
	pos_x=x; pos_y=y;
	xita=xi;
	nex=NULL;
	speed=20;
	exist=true;
	life=20;
}
void Bullet::print_bul_new(double pos_x, double pos_y)
{
	::SetDCPenColor(hdc, RGB(100,210,140));
	::SetDCBrushColor(hdc,RGB(100,210,140));
	Ellipse( hdc, pos_x-5, pos_y-5, pos_x+5, pos_y+5);
}
void Bullet::print_bul_old(double pos_x, double pos_y)
{
	::SetDCPenColor(hdc, RGB(0,0,0));
	::SetDCBrushColor(hdc,RGB(0,0,0));
	Ellipse( hdc, pos_x-5, pos_y-5, pos_x+5, pos_y+5);
}


Vector::Vector(double x1, double y1)
{
	x=x1;  y=y1;
}
Vector::Vector(const Vector& a)
{
	x=a.x;   y=a.y;
}
Vector::Vector() 
{
	x=0;  y=0;
}
Vector Vector::vertical()  //把向量变成其垂直向量
{
	double tem=x; x=y; y=-tem;
	return *this;
} 
double Vector::get_lenth()
{
	return sqrt(x*x+y*y);
}
Vector Vector::new_normalize()
{
	double a=get_lenth();
	x/=a; y/=a;
	return *this;
}
double Vector::dotmulti(Vector a)
{
	return a.x*x+a.y*y;
}
Vector Vector::operator = (Vector a)
{
	x=a.x; y=a.y;
	return *this;
}
Vector Vector::operator - (Vector a)
{
	temp_vector.x=x-a.x; temp_vector.y=y-a.y;
	return temp_vector;
}
Vector Vector::operator + (Vector a)
{
	temp_vector.x=x+a.x; temp_vector.y=y+a.y;
	return temp_vector;
}
Vector Vector::operator * (double a)
{
	temp_vector.x=x*a; temp_vector.y=y*a;
	return temp_vector;
}
Vector Vector::operator / (double a)
{
	temp_vector.x=x/a; temp_vector.y=y/a;
	return temp_vector;
}


Monster::Monster(int num)
{
//	create_new_monster();
	exist=false;
}
Monster::Monster()
{
/*	pos_x=1000;
	pos_y=600;
	speed=10;
	srand(time(0));
	num_edge=rand()%4+3;
	exist=true;
	new_point(pos_x,pos_y,num_edge,pos);*/
	exist=false;
}
void Monster::new_point(int x, int y, int num_edge, POINT pos[])
{
	if(num_edge==3)
	{
		POINT temp[]={x+20, y-20, x-20 , y-20, x, y+20 ,x+20, y-20 };
		for(int i=0; i<num_edge+1; i++)
		{
			pos[i].x=temp[i].x;
			pos[i].y=temp[i].y;
		}
	}
	if(num_edge==4)
	{
		POINT temp[]={x-20,y-20, x+20,y-20, x+20,y+20, x-20,y+20, x-20,y-20};
		//POINT temp[]={x-100,y-100,x+100,y-100,x-100,y+100,x+100,y+100,x-100,y-100};
		for(int i=0; i<num_edge+1; i++)
		{
			pos[i].x=temp[i].x;
			pos[i].y=temp[i].y;
		}
	}
	if(num_edge==5)
	{
		POINT temp[]={x,y-25,  x-25,y-3 ,x-15,y+25,x+15,y+25 ,  x+25,y-3  ,x,y-25};
		//POINT temp[]={x-100,y-100,x+100,y-100,x-100,y+100,x+100,y+100,x-100,y-100};
		for(int i=0; i<num_edge+1; i++)
		{
			pos[i].x=temp[i].x;
			pos[i].y=temp[i].y;
		}
	}
	if(num_edge==6)
	{
		POINT temp[]={x,y-30,x-25,y-15,x-25,y+15,x,y+30,x+25,y+15,x+25,y-15,x,y-30};
		//POINT temp[]={x-100,y-100,x+100,y-100,x-100,y+100,x+100,y+100,x-100,y-100};
		for(int i=0; i<num_edge+1; i++)
		{
			pos[i].x=temp[i].x;
			pos[i].y=temp[i].y;
		}
	}
}
void Monster::print_now(int x, int y, int num, POINT pos[])
{
	new_point(x,y,num,pos);
	if(num_edge==3)
	{
		::SetDCPenColor(hdc, RGB(255,99,71));  
		::SetDCBrushColor(hdc,RGB(64,224,205)); 
	}
	if(num_edge==4)
	{
		::SetDCPenColor(hdc, RGB(255,0,202));  
		::SetDCBrushColor(hdc,RGB(255,192,202)); 
	}
	if(num_edge==5)
	{
		::SetDCPenColor(hdc, RGB(64,224,205));  
		::SetDCBrushColor(hdc,RGB(245,245,245)); 
	}
	if(num_edge==6)
	{
		::SetDCPenColor(hdc, RGB(0,255,0));  
		::SetDCBrushColor(hdc,RGB(160,32,240)); 
	}
	Polygon(hdc,pos ,num+1);
}
void Monster::print_old(int x, int y, int num, POINT pos[])
{
	new_point(x,y,num,pos);
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Polygon(hdc,pos ,num+1);
}
void Monster::create_new_monster()
{
	exist=true;
	
	int rand1=rand()%2==0?1:-1,rand2=rand()%2==0?1:-1;
	
	pos_x=rand1*rand()%300+900;   // 900 495
	pos_y=rand2*rand()%300+495;
	speed=10;
	num_edge=rand()%4+3;
	new_point(pos_x,pos_y,num_edge,pos);
}
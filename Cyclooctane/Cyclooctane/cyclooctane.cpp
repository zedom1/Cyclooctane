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
HDC hdc;
HWND hwnd;
HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
const double pi2=2*3.1415926535;
const double pi=3.1415926535;
int Bullet::num_time_count=0;
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
}
void Game::show()
{
//	ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
//	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
}
void Game::updateWithoutInput()
{
	update_bullet();
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
				bul->print_bul_old(bul->pos_x,bul->pos_y);
				bul->pos_x+=bul->speed*cosf(bul->xita);
				bul->pos_y-=bul->speed*sinf(bul->xita);
				bul->print_bul_new(bul->pos_x,bul->pos_y);
				judge_bullet(5,8,square.pos,bul->pos_x, bul->pos_y, bul->xita);
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
		int old_x=ben.pos_x,old_y=ben.pos_y;
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		if(GetAsyncKeyState('W')<0) 
			ben.pos_y=old_y-ben.speed;
		if(GetAsyncKeyState('S')<0) 
			ben.pos_y=old_y+ben.speed; 
		if(GetAsyncKeyState('A')<0) 
			ben.pos_x=old_x-ben.speed;
		if(GetAsyncKeyState('D')<0) 
			ben.pos_x=old_x+ben.speed; 
		if(judge_edge(5,8,square.pos)==false) //静止矩形边界判定
		{ben.pos_x=old_x; ben.pos_y=old_y;}
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
bool Game::judge_edge(int start, int end, POINT pos[])
{
	for(int i=start; i<=end; i++)
	{
		int j=i+1;
		double k=(pos[i].y*1.0-pos[j].y*1.0)/(pos[i].x*1.0-pos[j].x*1.0);
		double b=pos[i].y*1.0-k*pos[i].x;
		double c=k*ben.pos_x*1.0+b-ben.pos_y*1.0;
		double d=abs(c)/sqrt(k*k+1);
		if(pos[i].x==pos[j].x)
			d=abs(ben.pos_x-pos[i].x);
		if(d<43)
			return false;
	}
	return true;
}
void Game::judge_bullet(int start, int end, POINT pos[], int &x, int &y, double &xita)
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
			if(k==0|| abs(pos[i].x*1.0-pos[j].x*1.0)<2|| abs(pos[i].y*1.0-pos[j].y*1.0)<2) {xita=pi+xita; return;}
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

Charactor::Charactor()
{
	pos_x=1200; 
	pos_y=790;
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
void Charactor::print_cha_new(int x,int y,POINT print_chara[])
{
	new_point(x,y,print_chara);
	::SetDCPenColor(hdc, RGB(123,123,123));  //灰色
	::SetDCBrushColor(hdc,RGB(123,123,123)); //灰色
	Polygon(hdc,print_chara ,6);
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
void Charactor::print_cha_old(int x,int y,POINT print_chara[])
{
	new_point(x,y,print_chara);
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Polygon(hdc,print_chara ,6);
}
void Charactor::new_point(int x,int y, POINT print_chara[])
{
	POINT apt1[]={x,y-40,x-30,y-14,x-30,y+14,x,y+40,x+30,y+14,x+30,y-14};
	for(int i=0; i<6 ; i++)
	{
		print_chara[i].x=apt1[i].x;
		print_chara[i].y=apt1[i].y;
	}
	return;
}
void Charactor::print_round_new(int x,int y,POINT print_chara[])
{
	if(judge_round==true) { print_cha_new(pos_x,pos_y,print_chara);	 return; }
	//Bullet::num_time_count++;
	//if(Bullet::num_time_count<3) return;
	//Bullet::num_time_count=0;
	/*if((GetAsyncKeyState(VK_UP)<0)||(GetAsyncKeyState(VK_DOWN)<0)||(GetAsyncKeyState(VK_LEFT)<0)||(GetAsyncKeyState(VK_RIGHT)<0))
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
	}*/
	if(kbhit())
	{
		char order=getch();
		print_part_cha_new(x,y,print_chara);
		::SetDCPenColor(hdc, RGB(217,31,37));
		::SetDCBrushColor(hdc,RGB(217,31,37));
		if(order=='i')
		{
				Ellipse(hdc,x-10,y-25,x+10,y-5);
				last->nex=new Bullet(pos_x,pos_y-25,pi/2);
				last=last->nex;
				return;
		}
		if(order=='k')
		{	
			Ellipse(hdc,x-10,y+5,x+10,y+25);
			last->nex=new Bullet(pos_x,pos_y+25,pi*3/2);
			last=last->nex;
			return;
		}
		if(order=='j')
		{	
			Ellipse(hdc,x-25,y-10,x-5,y+10); 
			last->nex=new Bullet(pos_x-25,pos_y,pi+0.1);
			last=last->nex;
			return;
		}
		if(order=='l')
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

void Charactor::print_part_cha_new(int x,int y, POINT print_chara[])
{
	new_point(x,y,print_chara);
	::SetDCPenColor(hdc, RGB(123,123,123));
	::SetDCBrushColor(hdc,RGB(123,123,123));
	Polygon(hdc,print_chara ,6);
	return;
}
void Charactor::judge_input()
{
	
}

void Square::new_room_point(int squ_x, int squ_y, double angle , POINT pos[])
{
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	POINT squ1[]=
	{
		squ_x+r1*cos(init*3.0+angle),     squ_y-r1*sin(3.0*init+angle),		//左上远
		squ_x+r1*cos(init*5.0+angle),     squ_y-r1*sin(init*5.0+angle),		//左下远
		squ_x+r1*cos(-init+angle),     squ_y-r1*sin(-init+angle),		//右下远
		squ_x+r1*cos(init+angle),     squ_y-r1*sin(init+angle),		//右上远
		squ_x+r1*cos(init*3.0+angle),     squ_y-r1*sin(3.0*init+angle),  	//左上远

		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),		//左上
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle),		//左下
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle),		//右下
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle),		//右上
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),  	//左上
	};
	for(int i=0; i<10 ; i++)
	{
		pos[i].x=squ1[i].x;
		pos[i].y=squ1[i].y;
	}
	return;
}
void Square::paint_room_new(int squ_x, int squ_y, POINT squ[], double angle)
{
	::SetDCPenColor(hdc, RGB(255,255,255));
	::SetDCBrushColor(hdc,RGB(255,255,255));
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	new_room_point(squ_x,squ_y,angle,squ);
	Polygon(hdc,squ ,10);
}
void Square::paint_room_old(int squ_x, int squ_y, POINT squ[],double angle)
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
		if(GetAsyncKeyState(VK_LEFT)<0) angle+=speed;
		if(GetAsyncKeyState(VK_RIGHT)<0) angle-=speed;
		paint_room_new(pos_x,pos_y,pos,angle);
	}
	return;
}

Bullet::Bullet(int x,int y,double xi)
{
	pos_x=x; pos_y=y;
	xita=xi;
	nex=NULL;
	speed=20;
	exist=true;
	life=20;
}
void Bullet::print_bul_new(int pos_x, int pos_y)
{
	::SetDCPenColor(hdc, RGB(100,210,140));
	::SetDCBrushColor(hdc,RGB(100,210,140));
	Ellipse( hdc, pos_x-5, pos_y-5, pos_x+5, pos_y+5);
}
void Bullet::print_bul_old(int pos_x, int pos_y)
{
	::SetDCPenColor(hdc, RGB(0,0,0));
	::SetDCBrushColor(hdc,RGB(0,0,0));
	Ellipse( hdc, pos_x-5, pos_y-5, pos_x+5, pos_y+5);
}
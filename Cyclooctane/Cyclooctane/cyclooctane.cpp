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
}
void Game::updateWithoutInput()
{

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
		if(judge_edge()==false)
		{ben.pos_x=old_x; ben.pos_y=old_y;}
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);	
		ben.print_round_new(ben.pos_x, ben.pos_y,ben.print_chara);
	}
	square.judge_input(ben.speed*3.0/100.0,ben.judge_round);
	if(GetAsyncKeyState('Y')<0)
	{	ben.judge_round=true;
		ben.print_part_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
	}
}
bool Game::judge_edge()
{
	for(int i=5; i<=8; i++)
	{
		int j=i+1;
		double k=(square.pos[i].y*1.0-square.pos[j].y*1.0)/(square.pos[i].x*1.0-square.pos[j].x*1.0);
		double b=square.pos[i].y*1.0-k*square.pos[i].x;
		double c=k*ben.pos_x*1.0+b-ben.pos_y*1.0;
		double d=abs(c)/sqrt(k*k+1);
		if(square.pos[i].x==square.pos[j].x)
			d=abs(ben.pos_x-square.pos[i].x);
		if(d<43)
		{
			printf("%d %d %d %d  ",square.pos[i].x,square.pos[i].y,square.pos[j].x,square.pos[j].y);
			printf("%lf %lf %d %d\n",k,b,c,abs(c)/sqrt(k*k+1));
			return false;
		}
	}
	return true;
}

Charactor::Charactor()
{
	pos_x=900; 
	pos_y=495;
	speed=10;
	name="Cyclooo~";
	new_point(pos_x,pos_y,print_chara);
	judge_round=false;
}
Charactor::~Charactor()
{
	delete []bullet;
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
		Ellipse(hdc,x-15,y-15,x+15,y+15);
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
	POINT apt1[]={x,y-50,x-40,y-24,x-40,y+24,x,y+50,x+40,y+24,x+40,y-24};
	for(int i=0; i<6 ; i++)
	{
		print_chara[i].x=apt1[i].x;
		print_chara[i].y=apt1[i].y;
	}
	return;
}
void Charactor::print_round_new(int x,int y,POINT print_chara[])
{
	if(judge_round==false)
	if((GetAsyncKeyState(VK_UP)<0)||(GetAsyncKeyState(VK_DOWN)<0)||(GetAsyncKeyState(VK_LEFT)<0)||(GetAsyncKeyState(VK_RIGHT)<0))
	{	
		print_part_cha_new(x,y,print_chara);
		::SetDCPenColor(hdc, RGB(217,31,37));
		::SetDCBrushColor(hdc,RGB(217,31,37));
		if(GetAsyncKeyState(VK_UP)<0) Ellipse(hdc,x-15,y-35,x+15,y-5);
		if(GetAsyncKeyState(VK_DOWN)<0) Ellipse(hdc,x-15,y+5,x+15,y+35);
		if(GetAsyncKeyState(VK_LEFT)<0) Ellipse(hdc,x-35,y-15,x-5,y+15); 
		if(GetAsyncKeyState(VK_RIGHT)<0) Ellipse(hdc,x+5,y-15,x+35,y+15); 
	}
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

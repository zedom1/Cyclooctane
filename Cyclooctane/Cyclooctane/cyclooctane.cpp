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
}
void Game::show()
{
	MoveToEx(hdc,100,0,NULL);
	LineTo(hdc,100,990);
}
void Game::updateWithoutInput()
{

}
void Game::updateWithInput()
{
	ben.judge_input();
}

Charactor::Charactor()
{
	pos_x=pos_y=100;
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
	if(kbhit())
	{
		int old_x=pos_x,old_y=pos_y;
		print_cha_old(pos_x,pos_y,print_chara);
		if(GetAsyncKeyState('W')<0) pos_y=old_y-speed;
		if(GetAsyncKeyState('S')<0) pos_y=old_y+speed; 
		if(GetAsyncKeyState('A')<0) pos_x=old_x-speed;
		if(GetAsyncKeyState('D')<0) pos_x=old_x+speed; 
		print_cha_new(pos_x,pos_y,print_chara);	
		print_round_new( pos_x, pos_y,print_chara);
		Sleep(50);
	}
}
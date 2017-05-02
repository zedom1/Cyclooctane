#include<iostream>
#include<stdio.h>
#include<algorithm>
#include<cmath>
#include<string>
#include<string.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <tchar.h>
#include "cyclooctane.h"
using namespace std;
#pragma warning(disable:4244)
HDC hdc;
HWND hwnd;
HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
Vector temp_vector;
HPEN hPen,pen_black; 
HFONT hFont,hFont_title;
const double pi2=2*3.1415926535;
const double pi=3.1415926535;
const int MAX_INT=0x7FFFFFFF;
const int MIN_INT=-MAX_INT-1;
const double MAX_DOUBLE=1.79769e+308;
const double MIN_DOUBLE=-MAX_DOUBLE;
int Bullet::num_time_count=0;
int Monster::num_total=0;
int num_monster_fresh=0;
int num_fresh_count=0;
const double Obstacle::r=20.0;


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
	cci.bVisible=0;
	SetConsoleCursorInfo(hOut,&cci);
}
int normalize_x(double x)  //找到坐标所在方格的中心点x坐标
{
	double pos_x=900-360;  //pos_y=495-360;
	int ans1=(abs(x-pos_x)/(2*Obstacle::r) );
	double ans=(ans1)*2*Obstacle::r+pos_x+Obstacle::r;
	return ans;
} 
int normalize_y(double y)   //找到坐标所在方格的中心点y坐标
{
	double pos_y=495-360;
	int ans1=((y-pos_y)/(2*Obstacle::r) );
	double ans=(ans1)*2*Obstacle::r+pos_y+Obstacle::r;
	return ans;
}
int get_i(double x)   //该中心对应map的i值
{
	double pos_x=900-360;
	return abs(x-pos_x-Obstacle::r)/(2*Obstacle::r);
}
int get_j(double y)  // 该中心对应map的j值
{
	double pos_y=495-360;
	return (y-pos_y-Obstacle::r)/(2*Obstacle::r);
}
void quicksort(int first, int last , Node* a)
{
	int i,j;
	Node tem=a[first],t;
	i=first; j=last;
	if(i>j) return;
	while(i!=j)
	{
		while(i<j && a[j].fx>=tem.fx)
			j--;
		while(i<j && a[i].fx<=tem.fx)
			i++;
		if(i<j)
		{
			t=a[j]; a[j]=a[i]; a[i]=t;
		}
	}
	a[first]=a[j]; a[i]=tem;
	quicksort(first, i-1,a);
	quicksort(i+1,last,a);
	return;
}
bool judge_coll_line(POINT a , POINT b, POINT c, POINT d, POINT &cut)
{
	double det=(b.x-a.x)*(c.y-d.y)-(c.x-d.x)*(b.y-a.y);
	if(abs(det)<1e-6) return false;
	double n1=((b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y) )/det;
	if(n1>1 || n1<0) return false;
	n1=((c.x-a.x)*(c.y-d.y)-(c.x-d.x)*(c.y-a.y) )/det;
	if(n1>1 || n1<0) return false;
	cut.x=a.x+n1*(b.x-a.x);
	cut.y=a.y+n1*(b.y-a.y);
	return true;
	/*
	double sa=(a.x-c.x)*(b.y-c.y)-(a.y-c.y)*(b.x-c.x);
	double sb=(a.x-d.x)*(b.y-d.y)-(a.y-d.y)*(b.x-d.x);
	if(sa*sb>=0) return false;
	double sc=(c.x - a.x) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);
	double sd=sa+sc-sb;
	if(sc*sd>=0) return false;
	double t=sc/(sb-sa);
	cut.x=a.x+t*(b.x-a.x);
	cut.y=a.y+t*(b.y-a.y);
	return true;*/
}


Node Node::operator=(Node a)
{
	this->fx=a.fx;
	this->gx=a.gx;
	this->hx=a.hx;
	this->pos=a.pos;
	this->fa=a.fa;
	return *this;
}
bool Node::operator == (Node a)
{
	return (this->fx==a.fx&&this->gx==a.gx&&this->hx==a.hx&&this->pos.x==a.pos.x&&this->pos.y==a.pos.y&&this->fa==a.fa);
}
bool Node::operator != (Node a)
{
	return  !(*this==a);
}
Node::Node()
{
	fx=gx=hx=pos.x=pos.y=0;
	fa=0;
}
Node::Node(double x,double y, int a, double fx1, double gx1, double hx1 )
	:fa(a),fx(fx1),gx(gx1),hx(hx1)
{
	pos.x=x; pos.y=y;
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

Game::Game()
{
	death_count=0;
}
void Game::startup()
{
	hidden();
	srand(time(0));
	hwnd=GetConsoleWindow();
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hdc=GetDC(hwnd);
	hPen= CreatePen( PS_SOLID ,5 , RGB( 255 , 0 , 0 ));
	pen_black= CreatePen( PS_SOLID , 5 , RGB( 0 , 0 , 0 ));
	GetConsoleScreenBufferInfo(hOut, &bInfo ); 
	COORD size={150,43};
	SetConsoleCursorPosition(hOut,size);
	SetConsoleScreenBufferSize(hOut,size);
	SMALL_RECT rc = {0,0, 150-1, 43-1};
	SetConsoleWindowInfo(hOut,true ,&rc);
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	obstacle=new Obstacle[5];
	hFont=CreateFont(80,40,0,0,FW_NORMAL,false,false,false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_DECORATIVE, _T("方正姚体"));
	hFont_title=CreateFont(160,60,0,0,FW_NORMAL,false,false,false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_DECORATIVE, _T("微软雅黑"));
	ben.mod=1;

	for(int i=0; i<=42; i++)
	{
		for(int j=0; j<=42; j++)
		{	
			map[i][j].pos.x=square.pos_x-360+Obstacle::r+i*2*Obstacle::r;
			map[i][j].pos.y=square.pos_y-360+Obstacle::r+j*2*Obstacle::r;
		}
	}
	monster[0].create_new_monster();
	for(int i=0; i<5; i++)
	{
		int tem_x=get_i(normalize_x(obstacle[i].pos_x)),tem_y=get_j(normalize_y(obstacle[i].pos_y));
		for(int xx=tem_x-1; xx<tem_x+1; xx++)
			for(int yy=tem_y-1; yy<tem_y+1; yy++)
				map[xx][yy].gx=MAX_INT/1000;
	}
//	while(1)
//	ben.print_cha_new(ben.pos_x, ben.pos_y, ben.print_chara);
//	menu_cha();
	menu_start();
}
void Game::clear()
{
	SelectObject(hdc,pen_black);
	::SetDCPenColor(hdc, RGB(0,0,0));   
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Rectangle(hdc,0,0,1500,990);
	return;
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
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Rectangle(hdc,0,0,1500,990);
	square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
	ben.print_cha_new(ben.pos_x,ben.pos_y, ben.print_chara);
}
void Game::updateWithoutInput()
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Bullet::num_time_count++;
	if(ben.judge_round==true)  // CD
	{
		ben.num_count[ben.mod]++;
		if(ben.num_count[ben.mod]>200)
		{
			ben.num_count[ben.mod]=0;
			ben.judge_round=false;
		}
	}
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
			//get_path(monster[0].pos_x,monster[0].pos_y,monster[i].path);
			monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
			Vector tem(monster[i].path.pos.x-monster[i].pos_x,monster[i].path.pos.y-monster[i].pos_y);
			tem.new_normalize();
			if(monster[i].path.pos.y-monster[i].pos_y==0)
			{	tem.y=0;  tem.x=1;}
			if(monster[i].path.pos.x-monster[i].pos_x==0)
			{	tem.x=0;  tem.y=1;}
			if((monster[i].path.pos.x-monster[i].pos_x==0)&&(monster[i].path.pos.y-monster[i].pos_y==0))
				tem.x=tem.y=0;  
			monster[i].pos_x+= tem.x*monster[i].speed;
			monster[i].pos_y+= tem.y*monster[i].speed;
			monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
		}
	for(int i=0; i<5; i++)
	{	
		obstacle[i].count++;
		obstacle[i].print_now(square.angle);
		if(obstacle[i].count>10)
		{	obstacle[i].judge_show=!obstacle[i].judge_show;
			obstacle[i].count=0;
		}
	}
	update_bullet();
	judge_coll_cha_to_mon();
	judge_coll_mon_to_mon();
	judge_coll_chara_to_wall();
	judge_coll_mon_to_wall();
	judge_coll_mon_to_obstacle();
	//print_new();
	
}
void Game::update_bullet()
{
	if(ben.mod==1)
	{
		if(ben.last!=ben.head)
		{
			Bullet *bul=ben.head->nex,*pre_bul=ben.head;
			while(bul!=NULL)
			{
				bul->life++;
				if(bul->life==ben.range)
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
						if(monster[i].exist==true)
							if( judge_circle_coll(circle_up,circle_down,monster[i].pos,monster[i].num_edge)==true  )
							{	
								bul->exist=false;
								monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
								monster[i].num_edge--;
								if(monster[i].num_edge<3)
								{	
									monster[i].exist=false;
									death_count++;
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
		{	
			ben.head->nex=ben.last;
			ben.last->nex=NULL;
		}
	}
	if(ben.mod==2)
	{
		if(ben.line.exist==false&&Bullet::num_time_count!=3) return;
		ben.line=ben.last_line;
		POINT line1_head,line1_last,line2_head,line2_last;
		POINT tem,cut; 
		line1_head.x=ben.line.pos_x;
		line1_head.y=ben.line.pos_y;
		while(ben.line.life>0)
		{
			tem.x=line1_head.x+ben.line.life*cosf(ben.line.xita);
			tem.y=line1_head.y-ben.line.life*sinf(ben.line.xita);
			line1_last=tem;
			int i=0;
			for(; i<4; i++)
			{
				if(i==0)
				{	
					line2_head=square.edge1[0]; 
					line2_last=square.edge1[1]; 
				}
				if(i==1)
				{	line2_head=square.edge2[0]; 
					line2_last=square.edge2[1]; }
				if(i==2)
				{	line2_head=square.edge3[0]; 
					line2_last=square.edge3[1]; }
				if(i==3)
				{	line2_head=square.edge4[0]; 
					line2_last=square.edge4[1]; }
				if(judge_coll_line(line1_head,line1_last,line2_head,line2_last,cut)==true)
				{
					if( !(abs(cut.x-line1_head.x)<2&&abs(cut.y-line1_head.y)<2)&& !(abs(cut.x-line1_last.x)<2&&abs(cut.y-line1_last.y)<2))
					{
						SelectObject(hdc,hPen);
						if(Bullet::num_time_count==3)
							SelectObject(hdc,pen_black);
						MoveToEx(hdc,line1_head.x,line1_head.y,NULL);
						LineTo(hdc,cut.x,cut.y);
						::SelectObject(hdc,GetStockObject(DC_PEN));
						::SelectObject(hdc,GetStockObject(DC_BRUSH));
						ben.line.life-=sqrt( (ben.line.pos_x-cut.x)*(ben.line.pos_x-cut.x)  + (ben.line.pos_y-cut.y)*(ben.line.pos_y-cut.y) );
						judge_bullet(5,8,square.pos,cut.x, cut.y, ben.line.xita);
						line1_head.x=cut.x; line1_head.y=cut.y;
						break;
					}
				}
			}
			if(i<4) continue;
			SelectObject(hdc,hPen);
			if(Bullet::num_time_count==3)
				SelectObject(hdc,pen_black);
			MoveToEx(hdc,line1_head.x,line1_head.y,NULL);
			LineTo(hdc,line1_last.x,line1_last.y);
			::SelectObject(hdc,GetStockObject(DC_PEN));
			::SelectObject(hdc,GetStockObject(DC_BRUSH));
			break;
		}
		ben.line.life=0;
	}
	return;
}
void Game::updateWithInput()
{
	if( GetAsyncKeyState(VK_ESCAPE)<0 )
		menu_exit();
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
	if(_kbhit()) 
	{	
		char order=_getch();
		if(order=='q')
		{
			if(ben.judge_round==false)
			{	
				ben.judge_round=true;
				for(int i=0; i<Monster::num_total; i++)
				{	
					if(monster[i].exist=false) continue; 
					monster[i].exist=false;
					death_count++;
					monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				}
				ben.print_part_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
			}
		}
	}
}
void Game::judge_bullet(int start, int end, POINT pos[], double x, double y, double &xita)
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
		Vector edge(*v1-*v2);  // 得到第一个图形的一条边向量
		edge.vertical();  // 求得边的垂直向量作为投影轴
		edge.new_normalize();   // 计算投影轴的单位向量

		for(int k=0; k<num_first; k++)    // 在该投影轴下第一个图图形的最大及最小投影值
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
	for(int i=0; i<num_second; i++)   // 求第二个图形的各个投影轴
	{
		double maxn_first=MIN_DOUBLE,minx_first=MAX_DOUBLE;
		double maxn_second=MIN_DOUBLE,minx_second=MAX_DOUBLE;
		int j=i+1;
		Vector *v1=new Vector( second[i].x,second[i].y);
		Vector *v2=new Vector( second[j].x,second[j].y);
		Vector edge(*v1-*v2);  // 得到第二个图形的一条边向量
		edge.vertical();  // 求得边的垂直向量作为投影轴
		edge.new_normalize();   // 计算投影轴的单位向量

		for(int k=0; k<num_first; k++)    // 在该投影轴下第一个图形的最大及最小投影值
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
	int flag=0;
	if(judge_coll_single(ben.print_chara,7,square.edge1,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		flag=1;
		//return true;
	}
	//	printf("Collision!  %d\n", ++count);
	if(judge_coll_single(ben.print_chara,7,square.edge2,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		flag=1;
	//	return true;
	}
	//	printf("Collision!  %d\n", ++count);
	if(judge_coll_single(ben.print_chara,7,square.edge3,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		flag=1;
	//	return true;
	}
	//	printf("Collision!  %d\n", ++count);
	if(judge_coll_single(ben.print_chara,7,square.edge4,5,shadow,num_move)==true)
	{	
		ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
		ben.pos_x-=shadow.x*(num_move+5);
		ben.pos_y-=shadow.y*(num_move+5);
		ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		flag=1;
	//	return true;
	}
	return flag==1;
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
				death_count++;
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
		if( tem<50  )
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
void Game::judge_coll_mon_to_obstacle()
{
	Vector shadow; double num_move;
	for(int i=0; i<400; i++)
		if(monster[i].exist==true)
			for(int j=0; j<5; j++)
			{
				if(obstacle[j].judge_show==false) continue;
				POINT tem[]={
					obstacle[j].pos_x-Obstacle::r,obstacle[j].pos_y-Obstacle::r,
					obstacle[j].pos_x-Obstacle::r,obstacle[j].pos_y+Obstacle::r,
					obstacle[j].pos_x+Obstacle::r,obstacle[j].pos_y+Obstacle::r,
					obstacle[j].pos_x+Obstacle::r,obstacle[j].pos_y-Obstacle::r
				};
				if(judge_coll_single(monster[i].pos,monster[i].num_edge, tem ,4,shadow,num_move)==true)
				{	
					monster[i].print_old(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
					monster[i].num_edge--;
					if(monster[i].num_edge<3)
					{	
						monster[i].exist=false;
						death_count++;
					}
					if(monster[i].exist!=false)
						monster[i].print_now(monster[i].pos_x,monster[i].pos_y,monster[i].num_edge,monster[i].pos);
				}
			}
}
void Game::menu_start()
{
	SelectObject(hdc,hFont);
	SelectObject(hdc,hPen);
	LPCTSTR str_start=L"Start";
	LPCTSTR str_exit=L"Exit";
	LPCTSTR str_load=L"Load";
	LPCTSTR str_title=L"Cyclooctane";
	int gamestatus=1;
	::SetDCPenColor(hdc, RGB(123,123,123));  //灰色
	::SetDCBrushColor(hdc,RGB(123,123,123)); //灰色
	SelectObject(hdc,hPen);
	POINT sqr_now[]={ 655,500, 810,500,  810,583,  655,583 ,  655,500 };
	while(1)	
	{
		SetBkColor(hdc, RGB(0,0,0));
		SetTextColor(hdc,RGB(255,255,255));
		SelectObject(hdc,hFont_title);
		TextOut(hdc,380,200,str_title,11);
		TextOut(hdc,380,200,str_title,11);
		SelectObject(hdc,hFont);
		TextOut(hdc,665,500,str_start,5);
		TextOut(hdc,665,600,str_load,4);
		TextOut(hdc,680,700,str_exit,4);
		TextOut(hdc,680,700,str_exit,4);
		Polyline(hdc,sqr_now, 5);
		char aaa=_getch();
		if(aaa=='\r') break;
		if(aaa=='w'||aaa=='s')
		{
			if(aaa=='w')
				gamestatus=gamestatus>1?gamestatus-1:3;
			if(aaa=='s')
				gamestatus=gamestatus<3?gamestatus+1:1;
			clear();
		}
		SelectObject(hdc,hPen);
		if(gamestatus==1)
		{POINT sqr_a[]={ 655,500, 810,500,  810,583,  655,583 ,  655,500 }; for(int i=0; i<5; i++) {sqr_now[i].x=sqr_a[i].x;sqr_now[i].y=sqr_a[i].y;}}
		else if(gamestatus==2)
		{POINT sqr_a[]={ 655,600, 815,600, 815,683,  655,683 , 655,600 };for(int i=0; i<5; i++) {sqr_now[i].x=sqr_a[i].x;sqr_now[i].y=sqr_a[i].y;}}
		else {POINT sqr_a[]={ 672,700, 790,700, 790,783, 672,783 , 672,700 }; for(int i=0; i<5; i++) {sqr_now[i].x=sqr_a[i].x;sqr_now[i].y=sqr_a[i].y;}}
		Polyline(hdc,sqr_now, 5);
	}
	clear();
	if(gamestatus==3) 
		exit(1);
	if(gamestatus==2) // load
	{
	}
	if(gamestatus==1)
	{	
		menu_cha();
	}
	return;
}
void Game::menu_exit()
{
	clear();
	int gamestatus=1;
	LPCTSTR str_continue=L"Continue";
	LPCTSTR str_save=L"Save";
	LPCTSTR str_exit=L"Exit";
	LPCTSTR str_pause=L"Pause";
	SetBkColor(hdc, RGB(0,0,0));
	SetTextColor(hdc,RGB(255,255,255));
	::SetDCPenColor(hdc, RGB(255,0,0));  
	::SetDCBrushColor(hdc,RGB(255,0,0)); 
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Ellipse(hdc,575,535,585,545);
	while(1)	
	{
		SelectObject(hdc,hFont_title);
		TextOut(hdc,550,200,str_pause,5);
		SelectObject(hdc,hFont);
		TextOut(hdc,600,500,str_continue,8);
		TextOut(hdc,665,600,str_save,4);
		TextOut(hdc,680,700,str_exit,4);
		TextOut(hdc,680,700,str_exit,4);
		if(_kbhit())
		{
			char aaa=_getch();
			if(aaa=='\r') break;
			if(aaa=='s'||aaa=='w')
			{
				if(aaa=='w')
					gamestatus=gamestatus>1?gamestatus-1:3;
				if(aaa=='s')
					gamestatus=gamestatus<3?gamestatus+1:1;
				::SetDCPenColor(hdc, RGB(0,0,0));  
				::SetDCBrushColor(hdc,RGB(0,0,0)); 
				Ellipse(hdc,575,535,585,545);
				Ellipse(hdc,640,635,650,645);
				Ellipse(hdc,655,735,665,745);
			}
		}
		::SetDCPenColor(hdc, RGB(255,0,0));  
		::SetDCBrushColor(hdc,RGB(255,0,0)); 
		if(gamestatus==1)
		{Ellipse(hdc,575,535,585,545);}
		else if(gamestatus==2)
		{Ellipse(hdc,640,635,650,645);}
		else {Ellipse(hdc,655,735,665,745);}
		
	}
	if(gamestatus==3) 
	{
		clear();
		menu_start();
	}
	if(gamestatus==2) // save
	{

	}
	clear();
	return;
}
void Game::menu_cha()
{
	SelectObject(hdc,hFont);
	LPCTSTR str_ben=L"Benzene";
	LPCTSTR str_cyc=L"Cyclohexadiene";
	LPCTSTR str_pyran=L"Pyran";
	LPCTSTR str_title=L"Charactor";
	int gamestatus=1;
	::SetDCPenColor(hdc, RGB(255,0,0));  
	::SetDCBrushColor(hdc,RGB(255,0,0)); 
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Ellipse(hdc,310,790,325,800);
	ben.set_new_data();
	while(1)	
	{
		SetBkColor(hdc, RGB(0,0,0));
		SetTextColor(hdc,RGB(255,255,255));
		SelectObject(hdc,hFont_title);
		TextOut(hdc,420,150,str_title,9);
		TextOut(hdc,420,150,str_title,9);
		SelectObject(hdc,hFont);
		TextOut(hdc,200,700,str_ben,7);
		TextOut(hdc,550,700,str_cyc,14);
		TextOut(hdc,1120,700,str_pyran,5);
		TextOut(hdc,1120,700,str_pyran,5);
		ben.mod=1;
		ben.print_cha_new(320,620,ben.print_chara);
		ben.mod=2;
		ben.print_cha_new(770,620,ben.print_chara);
		ben.mod=3;
		ben.print_cha_new(1200,620,ben.print_chara);
		::SetDCPenColor(hdc, RGB(255,0,0));  
		::SetDCBrushColor(hdc,RGB(255,0,0)); 
		if(GetAsyncKeyState(VK_ESCAPE)<0)
		{	
			clear();
			menu_start();
		}
		if(_kbhit())
		{
			char aaa=_getch();
			if(aaa=='\r') break;
			if(aaa=='a'||aaa=='d')
			{
				if(aaa=='a')
					gamestatus=gamestatus>1?gamestatus-1:3;
				if(aaa=='d')
					gamestatus=gamestatus<3?gamestatus+1:1;
				
				::SetDCPenColor(hdc, RGB(0,0,0));  
				::SetDCBrushColor(hdc,RGB(0,0,0)); 
				Ellipse(hdc,310,790,325,800);
				Ellipse(hdc,760,790,775,800);
				Ellipse(hdc,1200,790,1215,800);
			}
			if(aaa=='q')
				ben.judge_round=!ben.judge_round;
		}
		if(gamestatus==1)
		{Ellipse(hdc,310,790,325,800);}
		else if(gamestatus==2)
		{Ellipse(hdc,760,790,775,800);}
		else {Ellipse(hdc,1200,790,1215,800);}
	}
	ben.mod=gamestatus;
	ben.set_new_data();
	clear();
}
void Game::get_path(double x ,double  y, Node &path)
{
/*//  特判 若怪物到人路径上无障碍物， 则直线走
	{
		double k=(ben.pos_y-y)/(ben.pos_x-x);
		double b=y-k*x;
		int i=0;
		for(; i<5; i++)
		{
			if(abs(ben.pos_x-x)<5)
			{
				if(obstacle[i].pos_x<= x+Obstacle::r/2 &&obstacle[i].pos_x>= x-Obstacle::r/2 && obstacle[i].pos_y<=max(y,ben.pos_y)  &&obstacle[i].pos_y>=min(y,ben.pos_y) )
					break;//障碍物位于两者之间
			}
			if(abs(ben.pos_y-y)<5)
			{
				if(obstacle[i].pos_y<= y+Obstacle::r/2 &&obstacle[i].pos_y>= y-Obstacle::r/2 && obstacle[i].pos_x<=max(x,ben.pos_x)  &&obstacle[i].pos_x>=min(x,ben.pos_x) )
					break;//障碍物位于两者之间
			}
			double dis=abs(k*obstacle[i].pos_x-obstacle[i].pos_y+b)/(k*k+1);
			double x1=( 1/k*obstacle[i].pos_x+obstacle[i].pos_y-b )/(k+1/k);
			double y1=k*x1+b;
			if(  (dis<=(1.4*Obstacle::r+0)) &&  (   ((  (x1>=min(ben.pos_x,x)))  ) && ( x1<=max(ben.pos_x,x))) ) 
			{
				break;  //障碍物位于两者之间
			}
		}
		if(i==5) 
		{
			path.pos.x=ben.pos_x; 
			path.pos.y=ben.pos_y; 
			return; 
		}
	}
	return;*/
	double aim_x=normalize_x(ben.pos_x),aim_y=normalize_y(ben.pos_y);
	Node OPEN[1000],CLOSE[1000];
	memset(OPEN,0,sizeof(OPEN));
	memset(CLOSE,0,sizeof(CLOSE));
	int first=0,last=1,last_close=0;
	int now_x=get_i(normalize_x(x)),now_y=get_j(normalize_y(y));
	map[now_x][now_y].fa=now_x*42+now_y;
	OPEN[0]=map[now_x][now_y];
	OPEN[0].hx=sqrt(  (get_i(normalize_x(x))-get_i(aim_x))*(get_i(normalize_x(x))-get_i(aim_x)) + (get_j(normalize_y(y))-get_j(aim_y))*(get_j(normalize_y(y))-get_j(aim_y))  );
	OPEN[0].gx=0;
	Node empt;
	while(first<last)
	{
		now_x=max(get_i(normalize_x(OPEN[first].pos.x)),0),now_y=max(get_j(normalize_y(OPEN[first].pos.y)),0);
		if( OPEN[first].pos.x==aim_x && OPEN[first].pos.y==aim_y )
		{
			Node tem=OPEN[first];
			while(map[tem.fa/42][tem.fa%42].fa!=tem.fa/42+tem.fa%42)
				tem=map[tem.fa/42][tem.fa%42];
			path=tem;
			return;
		}
		for(int i=now_x-1; i<=now_x+1; i++)
		{
			for(int j=now_y-1; j<=now_y+1; j++)
			{
				if(i==now_x && j==now_y ) continue;
				//////////////////////////////////////////////
				map[i][j].fa=now_x*42+now_y;
				int flag=0;
				for(int k=first ; k<last ; k++)
					if(OPEN[k]==map[i][j])  
					{flag=1; break;}
				// 计算该点的fx值，tem保存用以与可能存在的旧fx值比较
				double tem_value=map[now_x][now_y].gx+((i==now_x || j==now_y))?10:14+sqrt( (get_i(aim_x)-i)*(get_i(aim_x)-i) +(get_j(aim_y)-j)*(get_j(aim_y)-j) );
				if(flag==1) // 在OPEN表里找到
				{
					if(tem_value<map[i][j].fx)
					{	
						map[i][j].fx=tem_value;
				//////////////////////////////////////////////
						map[i][j].fa=now_x*42+now_y;
						map[i][j].gx=map[now_x][now_y].gx;
						if(i==now_x || j==now_y)
							map[i][j].gx+=10;
						else
							map[i][j].gx+=14;
					}
				}
				else
				{
					int k=0;
					for( ;k<last_close; k++)
						if(CLOSE[k]==map[i][j])
						{ break;}
					if(k<last_close) // 在CLOSE表里找到
					{
						if(tem_value<map[i][j].fx)
						{	
							map[i][j].fx=tem_value;
					//////////////////////////////////////////////
							map[i][j].fa=now_x*42+now_y;
							map[i][j].gx=map[now_x][now_y].gx;
							if(i==now_x || j==now_y)
								map[i][j].gx+=10;
							else
								map[i][j].gx+=14;
							CLOSE[k]=empt;
							OPEN[last++]=map[i][j];
						}
					}
					else
					{
						map[i][j].fx=tem_value;
			//////////////////////////////////////////////
						map[i][j].fa=now_x*42+now_y;
						map[i][j].gx=map[now_x][now_y].gx;
						if(i==now_x || j==now_y)
							map[i][j].gx+=10;
						else
							map[i][j].gx+=14;
						OPEN[last++]=map[i][j];
					}
				}
				first++;
				CLOSE[last_close++]=map[now_x][now_y];
				quicksort(first, last-1, OPEN);
			}
		}
	}
	return;
}


Charactor::Charactor() // 默认1号
{
	pos_x=700; 
	pos_y=495;
	speed=10;
	judge_round=false;
	judge_dir=1;
	new_point(pos_x,pos_y,print_chara);
	memset(num_count,0,sizeof(num_count));
	mod=1;
	name="Benzene";
	num_bul=0;
	head=new Bullet(pos_x,pos_y,0);
	last=head;
	head->nex=NULL;
	range=10;
}
Charactor::~Charactor()
{
	delete head;
	delete last;
}
void Charactor::print_cha_new(double x,double y,POINT print_chara[])
{
	if(mod==1)
	{
		new_point(x,y,print_chara);
		::SetDCPenColor(hdc, RGB(123,123,123));  //灰色
		::SetDCBrushColor(hdc,RGB(123,123,123)); //灰色
		::SelectObject(hdc,GetStockObject(DC_PEN));
		::SelectObject(hdc,GetStockObject(DC_BRUSH));
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
		else
		{
			SelectObject(hdc,hPen);
			Ellipse(hdc,x-15,y-15,x+15,y+15);
		}
	}
	else if(mod==2|| mod==3)
	{
		::SelectObject(hdc,GetStockObject(DC_PEN));
		::SelectObject(hdc,GetStockObject(DC_BRUSH));
		print_part_cha_new(x,y,print_chara);
		if(mod==3) 
			print_cha_ball(x,y,0);
		print_cha_line(x,y);
			if(GetAsyncKeyState(VK_UP)<0) 
				{
					print_cha_old(x,y,print_chara);
					judge_dir=1;print_part_cha_new(x,y,print_chara);
					if(mod==2)
					{
						if(judge_round==false)
							SelectObject(hdc,hPen);
						else
							SelectObject(hdc,pen_black);
						MoveToEx(hdc,x-18,y-10,NULL); LineTo(hdc,x-2,y-25);
						MoveToEx(hdc,x+18,y-10,NULL);LineTo(hdc,x+2,y-25);
					}
					if(mod==3) {print_cha_line(x,y); print_cha_ball(x,y,0);}
				}
			else if(GetAsyncKeyState(VK_DOWN)<0) 
				{	
					print_cha_old(x,y,print_chara);judge_dir=3;
					print_part_cha_new(x,y,print_chara);
					if(mod==2){
						if(judge_round==false)
							SelectObject(hdc,hPen);
						else
							SelectObject(hdc,pen_black);
						MoveToEx(hdc,x-18,y+10,NULL);LineTo(hdc,x-2,y+25);
						MoveToEx(hdc,x+18,y+10,NULL);LineTo(hdc,x+2,y+25);
					}
					if(mod==3) {print_cha_line(x,y); print_cha_ball(x,y,0);}
			}
			else if(GetAsyncKeyState(VK_LEFT)<0)
				{	
					SelectObject(hdc,hPen);
					print_cha_old(x,y,print_chara);judge_dir=2;
					print_part_cha_new(x,y,print_chara);
					if(mod==2)
					{
						if(judge_round==false)
							SelectObject(hdc,hPen);
						else
							SelectObject(hdc,pen_black);
						MoveToEx(hdc,x-20,y-5,NULL);LineTo(hdc,x-8,y-20);
						MoveToEx(hdc,x-20,y+5,NULL);LineTo(hdc,x-8,y+20);
					}
					if(mod==3) {print_cha_line(x,y); print_cha_ball(x,y,0);}
			}
			else if(GetAsyncKeyState(VK_RIGHT)<0) 
			{	
					SelectObject(hdc,hPen);print_cha_old(x,y,print_chara);
					judge_dir=4;print_part_cha_new(x,y,print_chara);
					if(mod==2)
					{
						if(judge_round==false)
							SelectObject(hdc,hPen);
						else
							SelectObject(hdc,pen_black);
						MoveToEx(hdc,x+20,y-5,NULL);LineTo(hdc,x+8,y-20);
						MoveToEx(hdc,x+20,y+5,NULL);LineTo(hdc,x+8,y+20);
					}
					if(mod==3) {print_cha_line(x,y); print_cha_ball(x,y,0);}
			}
	}
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
}
void Charactor::print_cha_old(double x,double y,POINT print_chara[])
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0));
	new_point(x,y,print_chara);	
	Polygon(hdc,print_chara ,7);
	if(mod==3|| mod==2)
	{
		int tem=judge_dir;
		judge_dir=1;
		new_point(x,y,print_chara);	 Polygon(hdc,print_chara ,7);
		judge_dir=2;
		new_point(x,y,print_chara);	 Polygon(hdc,print_chara ,7);
		judge_dir=tem;
	}
	print_cha_ball(x,y,1);
}
void Charactor::new_point(double x,double y, POINT print_chara[])
{
	if( (mod==2|| mod==3)&& (judge_dir==2||judge_dir==4))
	{
		POINT apt1[]={x-40,y,x-14,y-30,x+14,y-30,x+40,y,x+14,y+30,x-14,y+30,x-40,y};
		for(int i=0; i<7 ; i++)
		{
			print_chara[i].x=apt1[i].x;
			print_chara[i].y=apt1[i].y;
		}
		return;
	}
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
	if(Bullet::num_time_count<3) return;
	Bullet::num_time_count=0;
	if((GetAsyncKeyState(VK_UP)<0)||(GetAsyncKeyState(VK_DOWN)<0)||(GetAsyncKeyState(VK_LEFT)<0)||(GetAsyncKeyState(VK_RIGHT)<0))
	{	
		if(mod==1)
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
		if(mod==2)
		{
			num_bul=1;
			last_line.life=line.life=range;
			line.exist=true;
			last_line.pos_x=line.pos_x=pos_x;
			last_line.pos_y=line.pos_y=pos_y;
			if(GetAsyncKeyState(VK_UP)<0) 
			{	
				last_line.xita=line.xita=pi/2.0;
				return;
			}
			if(GetAsyncKeyState(VK_DOWN)<0) 
			{	
				last_line.xita=line.xita=pi*3.0/2.0;
				return;
			}
			if(GetAsyncKeyState(VK_LEFT)<0) 
			{	
				last_line.xita=line.xita=pi;
				return;
			}
			if(GetAsyncKeyState(VK_RIGHT)<0) 
			{	
				last_line.xita=line.xita=0.0;
				return;
			}
		}
	}
	else
		print_cha_new(pos_x,pos_y,print_chara);	
	return ;
}
void Charactor::print_part_cha_new(double x,double y, POINT print_chara[])
{
	new_point(x,y,print_chara);
	if(mod==1)
	{
		::SetDCPenColor(hdc, RGB(123,123,123));
		::SetDCBrushColor(hdc,RGB(123,123,123));
	}
	if(mod==2)
	{
		::SetDCPenColor(hdc, RGB(199,97,20));
		::SetDCBrushColor(hdc,RGB(3,168,158)); 
	}
	if(mod==3)
	{
		::SetDCPenColor(hdc, RGB(255,99,71)); 
		::SetDCBrushColor(hdc,RGB(218,112,214));
	}
	Polygon(hdc,print_chara ,7);
	if(mod==3)
	{
		if(judge_round==false)
			SelectObject(hdc,hPen);
		else
			SelectObject(hdc,pen_black);
		if(judge_dir==1 ||judge_dir==3)
		{
			MoveToEx(hdc,x-20,y-12,NULL);LineTo(hdc,x-20,y+12);
			MoveToEx(hdc,x+20,y-12,NULL);LineTo(hdc,x+20,y+12);}
		else
		{MoveToEx(hdc,x-12,y-20,NULL);LineTo(hdc,x+12,y-20);
		MoveToEx(hdc,x-12,y+20,NULL);LineTo(hdc,x+12,y+20);}
	}
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	return;
}
void Charactor::judge_input()
{
}
void Charactor::print_cha_line(double x, double y)
{
	if(judge_round==false)
		SelectObject(hdc,hPen);
	else
		SelectObject(hdc,pen_black);
	if(judge_dir==1 || judge_dir==3){   
		MoveToEx(hdc,x-20,y-12,NULL);LineTo(hdc,x-20,y+12);
		MoveToEx(hdc,x+20,y-12,NULL);LineTo(hdc,x+20,y+12);}
	else{   
		MoveToEx(hdc,x-12,y-20,NULL);LineTo(hdc,x+12,y-20);
		MoveToEx(hdc,x-12,y+20,NULL);LineTo(hdc,x+12,y+20);}
}
void Charactor::print_cha_ball(double x, double y,bool judge_old)
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	if(judge_old==0)
		::SetDCBrushColor(hdc,RGB(8,46,84));
	else
	{
		::SetDCPenColor(hdc, RGB(0,0,0));  
		::SetDCBrushColor(hdc,RGB(0,0,0));
		Ellipse(hdc,x-10,y-45,x+10,y-25);
		Ellipse(hdc,x-45,y-10,x-25,y+10);
		Ellipse(hdc,x-10,y+45,x+10,y+25);
		Ellipse(hdc,x+45,y-10,x+25,y+10);
	}
	if(judge_dir==1)
		Ellipse(hdc,x-10,y-45,x+10,y-25);
	if(judge_dir==2)
		Ellipse(hdc,x-45,y-10,x-25,y+10);
	if(judge_dir==3)
		Ellipse(hdc,x-10,y+45,x+10,y+25);
	if(judge_dir==4)
		Ellipse(hdc,x+45,y-10,x+25,y+10);
}
void Charactor::set_new_data()
{
	pos_x=1100; 
	pos_y=680;
	speed=10;
	judge_round=false;
	judge_dir=1;
	new_point(pos_x,pos_y,print_chara);
	memset(num_count,0,sizeof(num_count));
	num_bul=0;
	if(mod==1)
	{
		name="Benzene";
		head=new Bullet(pos_x,pos_y,0);
		last=head;
		
		head->nex=NULL;
		range=20;
	}
	if(mod==2)
	{
		name="Cyclohexadiene";
		range=550;
	}
	if(mod==3)
	{
		name="Pyran";
		range=20;
	}
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
		squ_x+r2*cos(init*3.0+angle),     squ_y-r2*sin(3.0*init+angle),	  //左上近
		squ_x+r2*cos(init*5.0+angle),     squ_y-r2*sin(init*5.0+angle),	  //左下近
		squ_x+r2*cos(-init+angle),     squ_y-r2*sin(-init+angle),	  //右下近
		squ_x+r2*cos(init+angle),     squ_y-r2*sin(init+angle)    //右上近
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
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(255,255,255));
	::SetDCBrushColor(hdc,RGB(255,255,255));
	double init=3.1415926/4.0,r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	new_room_point(squ_x,squ_y,angle,squ);
	Polygon(hdc,squ ,10);
}
void Square::paint_room_old(double squ_x, double squ_y, POINT squ[],double angle)
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
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
	life=0;
}
Bullet::Bullet()
{
	pos_x=pos_y=0;
	exist=false;
	xita=0;
	life=0;
}
void Bullet::print_bul_new(double pos_x, double pos_y)
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(100,210,140));
	::SetDCBrushColor(hdc,RGB(100,210,140));
	Ellipse( hdc, pos_x-5, pos_y-5, pos_x+5, pos_y+5);
}
void Bullet::print_bul_old(double pos_x, double pos_y)
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(0,0,0));
	::SetDCBrushColor(hdc,RGB(0,0,0));
	Ellipse( hdc, pos_x-5, pos_y-5, pos_x+5, pos_y+5);
}
void Bullet::operator =(Bullet a)
{
	this->pos_x=a.pos_x,
	this->pos_y=a.pos_y;
	this->xita=a.xita;
	this->exist=a.exist;
	this->speed=a.speed;
	this->life=a.life;
	return;
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
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
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
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
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
	speed=5;
	num_edge=rand()%4+3;
	path.pos.x=pos_x;path.pos.y=pos_y;
	new_point(pos_x,pos_y,num_edge,pos);
}


void Obstacle::print_old()
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Rectangle(hdc,pos_x-r,pos_y-r,pos_x+r,pos_y+r );
}
void Obstacle::print_now(double angle)
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	print_old();
	new_center(angle);
	::SetDCPenColor(hdc, RGB(255,0,0));  
	::SetDCBrushColor(hdc,RGB(255,0,0)); 
	Rectangle(hdc,pos_x-r,pos_y-r,pos_x+r,pos_y+r );
	new_point();
}
void Obstacle::new_point()
{
	if(judge_show==false) return;
	int tot=0;
	for(int i=0 ;i<3; i++)
	{
		POINT tem_stab[]=
		{pos_x-r+5,pos_y-r+tot+10,pos_x-r+10,pos_y-r+tot+5,pos_x-r+15,pos_y-r+tot+10,pos_x-r+20,pos_y-r+tot+5,pos_x-r+25,pos_y-r+tot+10,pos_x-r+30,pos_y-r+tot+5,pos_x-r+35,pos_y-r+tot+10};
		for(int i=0; i<7; i++)
		{stab[i].x=tem_stab[i].x;stab[i].y=tem_stab[i].y;}
		tot+=13;
		SelectObject(hdc,pen_black);
		Polyline(hdc,stab,7);
		::SelectObject(hdc,GetStockObject(DC_PEN));
		::SelectObject(hdc,GetStockObject(DC_BRUSH));
	}
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
}
void Obstacle::new_center(double angle)
{
	pos_x=900+dis*(cosf(angle+init));
	pos_y=495-dis*(sinf(angle+init));
}
Obstacle::Obstacle(double x, double y)
	:pos_x(x),pos_y(y)
{
	count=rand()%5;
	new_point();
	Vector a(900-pos_x,495-pos_y);
	init=acosf( (a.x-1) / a.get_lenth());
}
Obstacle::Obstacle()
{
	judge_show=true;
	count=rand()%5;
	int rand1=rand()%2==0?1:-1,rand2=rand()%2==0?1:-1;
	pos_x=rand1*rand()%300+900;   // 900 495
	pos_y=rand2*rand()%300+495;
	new_point();
	Vector a(pos_x-900,495-pos_y);
	double tem=sqrt((pos_x-900)*((pos_x-900))+(pos_y-495)*(pos_y-495));
	if(abs(tem)<1e-6)  init=0;
	else
	{
		init=asinf((495-pos_y)/tem);
		if((pos_x-900)>1e-6)
		{
			if((495-pos_y)>1e-6) init=init;
			else init=init+pi2;
		}
		else
			init=pi-init;
	}
	dis=sqrt( (900-pos_x)*(900-pos_x) + (495-pos_y)*(495-pos_y) );
}
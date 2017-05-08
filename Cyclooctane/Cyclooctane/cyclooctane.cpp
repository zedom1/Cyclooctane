#include "cyclooctane.h"
using namespace std;
#pragma warning(disable:4244)

/////////// 常量 ////////////////
const double pi=3.1415926535;
const double pi2=2*pi;
const int MAX_INT=0x7FFFFFFF;
const int MIN_INT=-MAX_INT-1;
const double MAX_DOUBLE=1.79769e+308;
const double MIN_DOUBLE=-MAX_DOUBLE;
const double Obstacle::r=20.0;

/////////// 变量 ////////////////
int Bullet::num_time_count=0;
int Monster::num_count=0;
int Monster::num_total=0;
int num_monster_fresh=0;

/////////// 全局对象 ////////////////
HDC hdc;
HWND hwnd;
HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO bInfo;
Vector temp_vector;
HPEN hPen,pen_black,big_pen,big_black_pen; 
HFONT hFont,hFont_title,hFont_sub_title;
Game cyclooctane,empt;
MENU_START s1;  
MENU_CHA s2;  
ON_GAME s3;  
MENU_PAUSE s4;  
MENU_DEAD s5;  
EXIT s6;
CHANGE s7;
State* FSM::current=NULL; 
Data_Base new_data;
Node mapp[45][45];

/////////// 全局函数 ////////////////
void gotoxy(int x,int y)
{
	COORD pos111;
	pos111.X = x;
	pos111.Y = y;
	SetConsoleCursorPosition( GetStdHandle(STD_OUTPUT_HANDLE),pos111);
	SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE),0x01|0x05);
}
int normalize_x(double x)  //找到坐标所在方格的中心点x坐标
{
	double pos_x=900-360;  //pos_y=495-360;
	int ans1=((x-pos_x)/(2*Obstacle::r) );
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
int get_i(double x)   //该中心对应mapp的i值
{
	double pos_x=900-360;
	return (x-pos_x-Obstacle::r)/(2*Obstacle::r);
}
int get_j(double y)  // 该中心对应mapp的j值
{
	double pos_y=495-360;
	return (y-pos_y-Obstacle::r)/(2*Obstacle::r);
}
int get_x_from_i(int i)
{
	double pos_x=900-360;
	return (i*(2*Obstacle::r))+pos_x+Obstacle::r;
}
int get_y_from_j(int j)
{
	double pos_y=495-360;
	return (j*(2*Obstacle::r))+pos_y+Obstacle::r;
}
void quicksort(int first, int last , Node* a)
{
	int i,j;
	Node tem,t;
	tem=a[first];
	i=first; j=last;
	if(i>j) return;
	while(i!=j)
	{
		while(i<j && a[j].cal_fx()>=tem.cal_fx())
			j--;
		while(i<j && a[i].cal_fx()<=tem.cal_fx())
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
}
double point_to_line(POINT a, POINT head, POINT last)
{
	Vector head_a(head,a),last_a(last,a),head_last(head,last);
	double r=head_a.dotmulti(head_last)/(head_last.get_lenth()*head_last.get_lenth());
	if(r<=0)
		return head_a.get_lenth();
	if(r>=1)
		return last_a.get_lenth();
	double xita=head_a.dotmulti(head_last)/(head_last.get_lenth()*head_a.get_lenth());
	return head_a.get_lenth()*sqrt(1-xita*xita);
}
bool judge_coll_single(POINT first[], int num_first, POINT second[], int num_second, Vector& shadow, double& num_move)
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
void initi()
{
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
	hFont=CreateFont(80,40,0,0,FW_NORMAL,false,false,false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_DECORATIVE, _T("方正姚体"));
	hFont_title=CreateFont(160,60,0,0,FW_NORMAL,false,false,false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_DECORATIVE, _T("微软雅黑"));
	hFont_sub_title=CreateFont(40,15,0,0,FW_NORMAL,false,false,false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_DECORATIVE, _T("微软雅黑"));
	FSM::reset();
}

/////////// Node ////////////////
Node &Node::operator=( Node &a)
{
	(*this).fx=a.fx;
	(*this).gx=a.gx;
	(*this).hx=a.hx;
	(*this).pos.x=a.pos.x;
	(*this).pos.y=a.pos.y;
	(*this).fa.x=a.fa.x;
	(*this).fa.y=a.fa.y;
	(*this).ground=a.ground;
	return *this;
}
bool Node::operator == (Node a)
{
	return (this->fx==a.fx&&this->gx==a.gx&&this->hx==a.hx&&this->pos.x==a.pos.x&&this->pos.y==a.pos.y&&this->fa.x==a.fa.x&&this->fa.y==a.fa.y);
}
bool Node::operator != (Node a)
{
	return  !(*this==a);
}
Node::Node()
{
	fx=gx=hx=0;
	ground=0;
}
Node::~Node(){}
Node::Node(int a)
{
	ground=a;
	fx=gx=hx=0;
}
int Node::cal_hx(int x, int y)
{
	hx=abs(x-pos.x)*10+abs(y-pos.y)*10;
	return hx;
}
int Node::cal_fx()
{
	fx=gx+hx;
	return fx;
}

/////////// Vector ////////////////
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
Vector::Vector(POINT a , POINT b)
{
	x=b.x-a.x; y=b.y-a.y;
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

/////////// Game ////////////////
Game::Game()
{
	startup();
}
void Game::startup()
{
	memset(mapp,0,sizeof(mapp));
	for(int i=0; i<=42; i++)
	{
		for(int j=0; j<=42; j++)
		{	
			mapp[i][j].pos.x=i;
			mapp[i][j].pos.y=j;
		}
	}
	death_count=0;
	judge_update=0;
	room.new_room(room_count);
	room_count=0;
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
	if(room.time_count>=room.time_max) return;
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
	room.time_count++;
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Bullet::num_time_count++;
	if(ben.mod==1)
	{
		if(ben.judge_cha_state)  // CD
		{
			ben.num_count[ben.mod]++;
			if(ben.num_count[ben.mod]>50)
			{
				ben.num_count[ben.mod]=0;
				ben.judge_cha_state=0;
			}
		}
	}
	num_monster_fresh++;
	if(num_monster_fresh>10)
	{	
		num_monster_fresh=0;
		room.monster[Monster::num_count++].create_new_monster();
		Monster::num_total++;
		if(Monster::num_count>400)
			Monster::num_count=0;
	}
	//////////////////////////
	memset(mapp,0,sizeof(mapp));
	for(int i=0; i<42; i++)   // 实时更新mapp
		for(int j=0; j<42; j++)
		{
			POINT p={ i,j };
			mapp[i][j].pos=p;
			mapp[i][j].cal_hx(get_i(normalize_x(ben.pos_x)), get_j(normalize_y(ben.pos_y)));
			mapp[i][j].ground=0;
		}
	for(int i=0; i<room.num_stab; i++)
	{
		int tem_x=get_i(normalize_x(room.stab[i].pos_x)),tem_y=get_j(normalize_y(room.stab[i].pos_y));
	//	mapp[tem_x][tem_y].ground=1;
		for(int j=tem_x-1; j<=tem_x+1; j++)
			for(int k=tem_y-1; k<=tem_y+1; k++)
			{
		//		if(j==tem_x&& k==tem_y) continue;
				mapp[j][k].ground+=20;
			}
	}
	for(int i=0; i<room.num_stone; i++)
	{
		int tem_x=get_i(normalize_x(room.stone[i].pos_x)),tem_y=get_j(normalize_y(room.stone[i].pos_y));
		mapp[tem_x][tem_y].ground+=20;
		for(int j=tem_x-1; j<=tem_x+1; j++)
			for(int k=tem_y-1; k<=tem_y+1; k++)
			{
				if(j==tem_x&& k==tem_y) continue;
				mapp[j][k].ground+=10;
			}
	}
//	for(int i=0; i<Monster::num_count; i++)
	//	mapp[get_i(normalize_x(room.monster[i].pos_x))][get_j(normalize_y(room.monster[i].pos_y))].ground+=20;
	mapp[get_i(normalize_x(ben.pos_x))][get_j(normalize_y(ben.pos_y))].ground=3;
	/////////////////////////
	for(int i=0 ; i<400; i++)
		if(room.monster[i].exist==true)
		{
			get_path(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].path);
			room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			Vector tem(room.monster[i].path.x-room.monster[i].pos_x,room.monster[i].path.y-room.monster[i].pos_y);
			tem.new_normalize();
		/*	if((room.monster[i].path.y-room.monster[i].pos_y<2))
			{	tem.y=0;  tem.x=1;}
			if((room.monster[i].path.x-room.monster[i].pos_x)<2)
			{	tem.x=0;  tem.y=1;}
			if(( (room.monster[i].path.x-room.monster[i].pos_x)<2)&&( (room.monster[i].path.y-room.monster[i].pos_y<2)))
				tem.x=tem.y=0;  */
			room.monster[i].pos_x+= tem.x*room.monster[i].speed;
			room.monster[i].pos_y+= tem.y*room.monster[i].speed;
			room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			if(room.time_count>=room.time_max) 
				room.monster[i].speed+=0.1;
		}
	update_bullet();
	judge_coll_cha_to_mon();
	judge_coll_mon_to_mon();
	judge_coll_chara_to_wall();
	judge_coll_cha_to_corner();
	judge_coll_mon_to_wall();
	judge_coll_mon_to_obstacle();
	
	for(int i=0; i<room.num_stab; i++)
	{	
		room.stab[i].count++;
		room.stab[i].print_now(square.angle);
		if(room.stab[i].count>room.stab[i].count_max)
		{	room.stab[i].judge_show=!room.stab[i].judge_show;
			room.stab[i].count=0;
		}
	}
	for(int i=0; i<room.num_stone; i++)
		room.stone[i].print_now(square.angle);
	if(room.time_count>=room.time_max) 
	{	
		Vector tem; double tem1;
		square.paint_room_new(square.pos_x,square.pos_y,square.pos,square.angle);
		room.new_door(room.door,square.angle);
		if(judge_coll_single(ben.print_chara,7,room.door,5,tem,tem1)==true   )
		{
			memset(room.monster,0,sizeof(room.monster));
			Monster::num_total=0;
			Monster::num_count=0;
			Game::clear();
			room.new_room(room_count);
			room_count++;
			judge_update=0;
			ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
			ben.pos_x=2*square.pos_x-ben.pos_x;
			ben.pos_y=2*square.pos_y-ben.pos_y;
			ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
			room.time_count=0;
		}
	}
	
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
						if(room.monster[i].exist==true)
							if( judge_circle_coll(circle_up,circle_down,room.monster[i].pos,room.monster[i].num_edge)==true  )
							{	
								bul->exist=false;
								room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
								room.monster[i].num_edge--;
								if(room.monster[i].num_edge<3)
								{	
									room.monster[i].exist=false;
									death_count++;
									Monster::num_total--;
								}
								if(room.monster[i].exist!=false)
									room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
							}
				}
				else
				{
					bul->print_bul_old(bul->pos_x,bul->pos_y);
					if(bul->nex!=NULL)
					{	
						pre_bul->nex=bul->nex;
						if(bul!=NULL)
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
	if(ben.mod==2&& ( ben.judge_cha_state==0||ben.judge_cha_state==1))
	{
		if(ben.line.exist==false&&Bullet::num_time_count!=3) return;
		ben.line=ben.last_line;
		POINT line1_head,line1_last,line2_head,line2_last;
		POINT tem,cut,tttem; 
		line1_head.x=ben.line.pos_x;
		line1_head.y=ben.line.pos_y;
		if(ben.judge_cha_state==1)
			ben.line_array[ben.num_line_array++]=line1_head;
		while(ben.line.life>0)
		{
			tem.x=line1_head.x+ben.line.life*cosf(ben.line.xita);
			tem.y=line1_head.y-ben.line.life*sinf(ben.line.xita);
			line1_last=tem;
			if(ben.judge_cha_state==0)
			{
				for(int j=0; j<400; j++)
				if(room.monster[j].exist==true)
				{
					for(int k=0; k<room.monster[j].num_edge; k++)
					{
						if(judge_coll_line(line1_head,line1_last,room.monster[j].pos[k],room.monster[j].pos[k+1],tttem)==true)
						{
							room.monster[j].print_old(room.monster[j].pos_x,room.monster[j].pos_y,room.monster[j].num_edge,room.monster[j].pos);
							room.monster[j].num_edge--;
							if(room.monster[j].num_edge<3)
							{	
								room.monster[j].exist=false;
								death_count++;
								Monster::num_total--;
							}
							if(room.monster[j].exist!=false)
								room.monster[j].print_now(room.monster[j].pos_x,room.monster[j].pos_y,room.monster[j].num_edge,room.monster[j].pos);
						}
					}
				}
			}
			else
			{
				double minx=MAX_DOUBLE;
				for(int j=0; j<400; j++) // 找激光可射中的最近的怪物的距离平方最小值
				{
					if(room.monster[j].exist==false) continue;
					for(int k=0; k<room.monster[j].num_edge; k++)
						if(judge_coll_line(line1_head,line1_last,room.monster[j].pos[k],room.monster[j].pos[k+1],cut)==true)
							if( !(abs(cut.x-line1_head.x)<2&&abs(cut.y-line1_head.y)<2)&& !(abs(cut.x-line1_last.x)<2&&abs(cut.y-line1_last.y)<2))
							{	
								minx=min(minx, (line1_head.x-room.monster[j].pos_x)*(line1_head.x-room.monster[j].pos_x)+(line1_head.y-room.monster[j].pos_y)*(line1_head.y-room.monster[j].pos_y)  );
								break;
							}
				}
				int j=0;
				for( ;j<400; j++) //找激光可射中的最近的怪物
					if(room.monster[j].exist==true)
					{
						if(minx!=(line1_head.x-room.monster[j].pos_x)*(line1_head.x-room.monster[j].pos_x)+(line1_head.y-room.monster[j].pos_y)*(line1_head.y-room.monster[j].pos_y))
							continue;
						double minx1=MAX_DOUBLE;
						for(int k=0; k<room.monster[j].num_edge; k++) // 找该怪物距离出射点最近的一条边并记录最小值
						{
							minx1=min(minx1,point_to_line(line1_head,room.monster[j].pos[k],room.monster[j].pos[k+1]));
						}
						int k=0;
						for(; k<room.monster[j].num_edge; k++)
						{
							if(minx1!=point_to_line(line1_head,room.monster[j].pos[k],room.monster[j].pos[k+1])) continue;
							judge_coll_line(line1_head,line1_last,room.monster[j].pos[k],room.monster[j].pos[k+1],cut);
							room.monster[j].print_old(room.monster[j].pos_x,room.monster[j].pos_y,room.monster[j].num_edge,room.monster[j].pos);
							room.monster[j].exist=false;
							Monster::num_total--;
							//printf("1111\n");
							death_count++;
							SelectObject(hdc,hPen);
							if(Bullet::num_time_count==3)
								SelectObject(hdc,pen_black);
							MoveToEx(hdc,line1_head.x,line1_head.y,NULL);
							LineTo(hdc,cut.x,cut.y);
							::SelectObject(hdc,GetStockObject(DC_PEN));
							::SelectObject(hdc,GetStockObject(DC_BRUSH));
							ben.line.life-=sqrt( (ben.line.pos_x-cut.x)*(ben.line.pos_x-cut.x)  + (ben.line.pos_y-cut.y)*(ben.line.pos_y-cut.y) );
							judge_bullet(k,k+1,room.monster[j].pos,cut.x, cut.y, ben.line.xita);
							line1_head.x=cut.x; line1_head.y=cut.y;
							ben.line_array[ben.num_line_array++]=cut;
							break;
						}
					if(k<room.monster[j].num_edge) break;
				}
				if(j<400) continue;
			}
			int i=0;
			for(; i<4; i++)
			{
				if(i==0)
				{	line2_head=square.edge1[0]; line2_last=square.edge1[1]; }
				if(i==1)
				{	line2_head=square.edge2[0];line2_last=square.edge2[1]; }
				if(i==2)
				{	line2_head=square.edge3[0]; line2_last=square.edge3[1]; }
				if(i==3)
				{	line2_head=square.edge4[0]; line2_last=square.edge4[1]; }
				if(judge_coll_line(line1_head,line1_last,line2_head,line2_last,cut)==true)
				{
					if( !(abs(cut.x-line1_head.x)<2&&abs(cut.y-line1_head.y)<2)&& !(abs(cut.x-line1_last.x)<2&&abs(cut.y-line1_last.y)<2))
					{
						if(ben.judge_cha_state==1)
							ben.line_array[ben.num_line_array++]=cut;
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
			if(i<4) continue; // 与墙壁也没发生反射 激光直射至射程最大值
			SelectObject(hdc,hPen);
			MoveToEx(hdc,line1_head.x,line1_head.y,NULL);
			LineTo(hdc,line1_last.x,line1_last.y);
			if(ben.judge_cha_state==1)
			{	
				ben.line_array[ben.num_line_array++]=line1_last;
				if(Bullet::num_time_count==3)
				{	
					SelectObject(hdc,pen_black);
					for(int i=0; i<ben.num_line_array-1; i++)
					{
						MoveToEx(hdc,ben.line_array[i].x,ben.line_array[i].y,NULL);
						LineTo(hdc,ben.line_array[i+1].x,ben.line_array[i+1].y);
					}
					ben.num_line_array=0;
				}
			}
			if(ben.judge_cha_state==0)
			{
				if(Bullet::num_time_count==3)
					SelectObject(hdc,pen_black);
				MoveToEx(hdc,line1_head.x,line1_head.y,NULL);
				LineTo(hdc,line1_last.x,line1_last.y);
			}
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
	if(room.time_count>=room.time_max) return;
	ben.print_round_new(ben.pos_x, ben.pos_y,ben.print_chara);
	square.judge_input(ben.speed*3.0/100.0,ben.judge_cha_state,ben.mod);
	if(_kbhit()) 
	{	
		char order=_getch();
		if(order=='q')
		{
			if(ben.mod==1)
			{
				if(ben.judge_cha_state==0)
				{	
					ben.judge_cha_state=1;
					for(int i=0; i<Monster::num_count; i++)
					{	
						if(room.monster[i].exist=false) continue; 
						room.monster[i].exist=false;
						death_count++;
						Monster::num_total--;
						room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
					}
					ben.print_part_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
				}
			}
			if(ben.mod==2)
			{
				ben.judge_cha_state++;
				if(ben.judge_cha_state>2)
					ben.judge_cha_state=0;
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
		if( abs(pos[i].y-pos[j].y)<2 )
			d=abs(y-pos[i].y);
		if( abs(pos[i].x-pos[j].x)<2 )
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
		if(room.monster[i].exist)
		{
			double num_move=0;
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge,square.edge1,5,shadow,num_move)==true)
			{	
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].pos_x-=shadow.x*num_move;
				room.monster[i].pos_y-=shadow.y*num_move;
				room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge,square.edge2,5,shadow,num_move)==true)
			{	
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].pos_x-=shadow.x*num_move;
				room.monster[i].pos_y-=shadow.y*num_move;
				room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge,square.edge3,5,shadow,num_move)==true)
			{	
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].pos_x-=shadow.x*num_move;
				room.monster[i].pos_y-=shadow.y*num_move;
				room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge,square.edge4,5,shadow,num_move)==true)
			{
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].pos_x-=shadow.x*num_move;
				room.monster[i].pos_y-=shadow.y*num_move;
				room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
			judge_coll_mon_to_corner(i);
			if( ( (room.monster[i].pos_x-square.pos_x)*(room.monster[i].pos_x-square.pos_x)  )+( (room.monster[i].pos_y-square.pos_y)*(room.monster[i].pos_y-square.pos_y) )>=350*350*2  )
			{	
				room.monster[i].exist=false;
				death_count++;
				Monster::num_total--;
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
		}
	return ;
}
void Game::judge_coll_cha_to_mon()
{
	Vector shadow;
	for(int i=0; i<400; i++)
		if(room.monster[i].exist)
		{
			double num_move=0;
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge,ben.print_chara,7,shadow,num_move)==true)
			{	
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].pos_x-=shadow.x*num_move;
				room.monster[i].pos_y-=shadow.y*num_move;
				room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
		}
	return ;
}
void Game::judge_coll_mon_to_mon()
{
	Vector shadow;
	for(int i=0; i<400; i++)
		if(room.monster[i].exist)
			for(int j=i+1; j<400; j++)
				if(room.monster[j].exist)
				{
					double num_move=0;
					if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge,room.monster[j].pos,room.monster[j].num_edge,shadow,num_move)==true)
					{	
						room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
						//room.monster[j].print_now(room.monster[j].pos_x,room.monster[j].pos_y,room.monster[j].num_edge,room.monster[j].pos);
						room.monster[i].pos_x-=shadow.x*num_move;
						room.monster[i].pos_y-=shadow.y*num_move;
						room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
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
	Vector shadow;
	double num_move=0;
	for(int i=0; i<room.num_stone; i++)
		if(judge_coll_single(ben.print_chara,7,room.stone[i].pos,5,shadow,num_move)==true)
		{	
			ben.print_cha_old(ben.pos_x,ben.pos_y,ben.print_chara);
			ben.pos_x-=shadow.x*(num_move);
			ben.pos_y-=shadow.y*(num_move);
			ben.print_cha_new(ben.pos_x,ben.pos_y,ben.print_chara);
		}
	return;
}
void Game::judge_coll_mon_to_corner(int i)
{
	room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
	judge_coll_corner(room.monster[i].pos_x,room.monster[i].pos_y, square.corner, 4, square.pos_x, square.pos_y);
	room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
	return;
}
void Game::judge_coll_mon_to_obstacle()
{
	Vector shadow; double num_move;
	for(int i=0; i<400; i++)
	{	
		if(room.monster[i].exist==false) continue;
		for(int j=0; j<room.num_stab; j++)
		{
			if(room.stab[j].judge_show==false) continue;
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge, room.stab[j].pos ,5,shadow,num_move)==true)
			{	
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].num_edge--;
				if(room.monster[i].num_edge<3)
				{	
					room.monster[i].exist=false;
					death_count++;
					Monster::num_total--;
				}
				if(room.monster[i].exist!=false)
					room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
		}
		for(int j=0; j<room.num_stone; j++)
		{
			if(judge_coll_single(room.monster[i].pos,room.monster[i].num_edge, room.stone[j].pos ,5,shadow,num_move)==true)
			{	
				room.monster[i].print_old(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
				room.monster[i].pos_x-=shadow.x*(num_move+5);
				room.monster[i].pos_y-=shadow.y*(num_move+5);
				room.monster[i].print_now(room.monster[i].pos_x,room.monster[i].pos_y,room.monster[i].num_edge,room.monster[i].pos);
			}
		}
	}
}
void Game::get_path(double x ,double  y, POINT &path)
{ // x,y为目前位置，目标位置为 (ben.pos_x,ben.pos_y)
	//  特判 若怪物到人路径上无障碍物， 则直线走
	{
		double k=(ben.pos_y-y)/(ben.pos_x-x);
		double b=y-k*x;
		int i=0;
		for(; i<room.num_stab; i++)
		{
			if(abs(ben.pos_x-x)<5)
			{
				if(room.stab[i].pos_x<= x+Obstacle::r/2 &&room.stab[i].pos_x>= x-Obstacle::r/2 && room.stab[i].pos_y<=max(y,ben.pos_y)  &&room.stab[i].pos_y>=min(y,ben.pos_y) )
				break;//障碍物位于两者之间
			}
			if(abs(ben.pos_y-y)<5)
			{if(room.stab[i].pos_y<= y+Obstacle::r/2 &&room.stab[i].pos_y>= y-Obstacle::r/2 && room.stab[i].pos_x<=max(x,ben.pos_x)  &&room.stab[i].pos_x>=min(x,ben.pos_x) )
				break;//障碍物位于两者之间
			}
			double dis=abs(k*room.stab[i].pos_x-room.stab[i].pos_y+b)/(k*k+1);
			double x1=( 1/k*room.stab[i].pos_x+room.stab[i].pos_y-b )/(k+1/k);
			double y1=k*x1+b;
			if(  (dis<=(1.4*Obstacle::r+0)) &&  (   ((  (x1>=min(ben.pos_x,x)))  ) && ( x1<=max(ben.pos_x,x))) ) 
				break;  //障碍物位于两者之间
		}
		int j=0;
		for(; j<room.num_stone; j++)
		{
			if(abs(ben.pos_x-x)<5)
			{
				if(room.stone[j].pos_x<= x+Obstacle::r/2 &&room.stone[j].pos_x>= x-Obstacle::r/2 && room.stone[j].pos_y<=max(y,ben.pos_y)  &&room.stone[j].pos_y>=min(y,ben.pos_y) )
				break;//障碍物位于两者之间
			}
			if(abs(ben.pos_y-y)<5)
			{
				if(room.stone[j].pos_y<= y+Obstacle::r/2 &&room.stone[j].pos_y>= y-Obstacle::r/2 && room.stone[j].pos_x<=max(x,ben.pos_x)  &&room.stone[j].pos_x>=min(x,ben.pos_x) )
				break;//障碍物位于两者之间
			}
			double dis=abs(k*room.stone[j].pos_x-room.stone[j].pos_y+b)/(k*k+1);
			double x1=( 1/k*room.stone[j].pos_x+room.stone[j].pos_y-b )/(k+1/k);
			double y1=k*x1+b;
			if(  (dis<=(1.4*Obstacle::r+0)) &&  (   ((  (x1>=min(ben.pos_x,x)))  ) && ( x1<=max(ben.pos_x,x))) ) 
				break;  //障碍物位于两者之间
		}
		if(i==room.num_stab&&j==room.num_stone) 
		{
			path.x=ben.pos_x; 
			path.y=ben.pos_y; 
			return; 
		}
	}
	//////////////////////////////////////////
	int aim_x=get_i(normalize_x(ben.pos_x)),aim_y=get_j(normalize_y(ben.pos_y));
	int now_x= get_i(normalize_x(x)), now_y=get_j(normalize_y(y));
	int mon_x=get_i(normalize_x(x)),mon_y=get_j(normalize_y(y));
	int count=0;
	for(int i=0; i<42; i++)  // gx清空
		for(int j=0; j<42; j++)
			mapp[i][j].gx=0;
	//////////////////////////////////////
	
	POINT aim={ aim_x ,aim_y };
	POINT start_point={now_x , now_y};
	Node OPEN[10000],CLOSE[10000];
	POINT PATH[10000];
	Node empt;
	memset(OPEN,0,sizeof(OPEN));
	memset(CLOSE,0,sizeof(CLOSE));
	memset(PATH,0,sizeof(PATH));
	int first=0,last=0,last_close=0, end_path=0;
	OPEN[last++]=mapp[now_x][now_y];
	CLOSE[last_close]=mapp[now_x][now_y];
	///////
	bool findd=false;
while(findd==false)
{
	for(int i=first; i<last; i++)
		if( (OPEN[i].ground%10)==3)
		{
			OPEN[i].fa=CLOSE[last_close-1].pos;
			findd=true; break;
		}
		// 从OPEN表里找fx最小的并加入CLOSE表
	quicksort(first, last-1, OPEN);
	if(first<last)
		CLOSE[last_close++]=OPEN[first++];

		// 从新加入CLOSE表的点开始扩展
	now_x=CLOSE[last_close-1].pos.x;
	now_y=CLOSE[last_close-1].pos.y;
	for(int i=now_x-1; i<=now_x+1 ; i++ ) // 枚举 now的周围八个方格
	{for(int j=now_y-1; j<=now_y+1; j++ )
	{
		if( i<0 || i>42 || j<0 || j>42 ) continue;
		if(i==now_x && j==now_y) continue;
		int flag=0;
		for(int cc=first; cc<last; cc++) // 在OPEN列表里找
			if(OPEN[cc].pos.x==i && OPEN[cc].pos.y==j)
			{	flag=1; break; } // flag==1 表明找到了
		if(flag==1 && last_close>0)  // 在OPEN列表里找到
		{
			int tem_find_place=0;
			for(int cc=first; cc<last ; cc++)
				if( OPEN[cc].pos.x==i && OPEN[cc].pos.y==j  )
					tem_find_place=cc; // 在OPEN表里定位
			int tem_gx=0;
			if( i==now_x || j==now_y ) // 判断是否是斜对角
				tem_gx=CLOSE[last_close-1].gx+10;
			else
				tem_gx=CLOSE[last_close-1].gx+14;
			if(tem_gx<OPEN[tem_find_place].gx)
			{
				OPEN[tem_find_place].fa.x=i;
				OPEN[tem_find_place].fa.y=j;
				mapp[OPEN[tem_find_place].pos.x][OPEN[tem_find_place].pos.y].fa.x =i;
				mapp[OPEN[tem_find_place].pos.x][OPEN[tem_find_place].pos.y].fa.y =j;
			}
			continue;
		}
		for(int cc=0; cc<last_close ; cc++)
			if(CLOSE[cc].pos.x==i && CLOSE[cc].pos.y==j)
			{flag=1; break;}
		if(flag==1) continue;
		int stamp=0;
		for(int qwe=i-1; qwe<=i+1; qwe++)
			for(int asd=j-1; asd<=j+1; asd++)
				if((mapp[qwe][asd].ground%10)==3)
				{stamp=1; break;}
		if( stamp==0  && ( mapp[i][j].ground>10)  ) continue;
		mapp[i][j].fa=mapp[now_x][now_y].pos;
		if( i==now_x || j==now_y ) // 判断是否是斜对角
			mapp[i][j].gx+=10;
		else
			mapp[i][j].gx+=14;
		OPEN[last++]=mapp[i][j];
	}
	}
	count++;    // 异常状态弹出
	if(count>9600)
	{
		path.x=ben.pos_x;
		path.y=ben.pos_y;
		return;
	}
}
	count=0;
	///////////////////////////
	Node temp=mapp[aim_x][aim_y];
	while( temp.pos.x!=mapp[mon_x][mon_y].pos.x || temp.pos.y!=mapp[mon_x][mon_y].pos.y )
	{
		PATH[end_path++]=temp.pos;   // 记录路径
		temp=mapp[temp.fa.x][temp.fa.y];   // 回溯至父节点
		count++;
		if(count>9600)
		{path.x=ben.pos_x;path.y=ben.pos_y;return;}
	}
	POINT ans={ get_x_from_i(PATH[end_path-1].x) , get_y_from_j(PATH[end_path-1].y) };
	path=ans;
	return;
}

//////////// Room ////////////////
Room::Room()
{
	new_room(0);
}
Room::~Room(){}
void Room::new_door(POINT door[], double angle)
{
	int squ_x=900,squ_y=495;
	double r1=sqrt(375*375*2),r2=sqrt(350*350*2);
	double x1=(sin(pi/4.0)/sin(5.0/8.0*pi))*r1-10,x2=(sin(pi/4.0)/sin(5.0/8.0*pi))*r2-10;
	double rand_angle=pi/2*rand_c;
	POINT door11[]=
	{
		squ_x+x1*cos(3.0/8.0*pi+rand_angle+pi/110.0+angle), squ_y-x1*sin(3.0/8.0*pi+rand_angle+angle),
		squ_x+x1*cos(3.0/8.0*pi-pi/110.0+rand_angle+pi/4.0+angle), squ_y-x1*sin(3.0/8.0*pi+rand_angle+pi/4.0+angle)-1,
		squ_x+x2*cos(3.0/8.0*pi+rand_angle+pi/4.0+angle), squ_y-x2*sin(3.0/8.0*pi+rand_angle+pi/4.0+angle),
		squ_x+x2*cos(3.0/8.0*pi+rand_angle+angle), squ_y-x2*sin(3.0/8.0*pi+rand_angle+angle),
		squ_x+x1*cos(3.0/8.0*pi+rand_angle+pi/110.0+angle), squ_y-x1*sin(3.0/8.0*pi+rand_angle+angle)
	};
	for(int i=0; i<5; i++)
	{
		door[i].x=door11[i].x;
		door[i].y=door11[i].y;
	}
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(255,0,0));
	::SetDCBrushColor(hdc,RGB(255,0,0));
	Polygon(hdc,door ,5);
}
void Room::new_room(int a)
{
	num_stab=rand()%3+2;
	stab=new Stab[num_stab];
	num_stone=rand()%4+2;
	stone=new Stone[num_stone];
	time_count=0;
	rand_c=rand()%4;
	time_max=200+a*20+(rand()%2-1)*40;
	for(int i=0; i<num_stab; i++)
	{
		for(int j=0; j<num_stab; j++)
		{
			if(i==j) continue;
			if( ((stab[i].pos_x-stab[j].pos_x)*(stab[i].pos_x-stab[j].pos_x)+(stab[i].pos_y-stab[j].pos_y)*(stab[i].pos_y-stab[j].pos_y))  <=3200  )
				stab[i].reset();
		}
		for(int j=0; j<num_stone; j++)
			if( ((stab[i].pos_x-stone[j].pos_x)*(stab[i].pos_x-stone[j].pos_x)+(stab[i].pos_y-stone[j].pos_y)*(stab[i].pos_y-stone[j].pos_y))  <=3200  )
				stone[j].reset();
	}
	for(int i=0 ; i<num_stone; i++)
	{
		for(int j=0; j<num_stone; j++)
		{
			if(i==j) continue;
			if( ((stone[i].pos_x-stone[j].pos_x)*(stone[i].pos_x-stone[j].pos_x)+(stone[i].pos_y-stone[j].pos_y)*(stone[i].pos_y-stone[j].pos_y))  <=3200  )
				stone[i].reset();
		}
	}
	Bullet::num_time_count=0;
	Monster::num_count=0;
	Monster::num_total=0;
	num_monster_fresh=0;
	
}

/////////// Charactor ////////////////
Charactor::Charactor() // 默认1号
{
	pos_x=700; 
	pos_y=495;
	speed=10;
	judge_cha_state=0;
	judge_dir=1;
	new_point(pos_x,pos_y,print_chara);
	memset(num_count,0,sizeof(num_count));
	memset(line_array,0,sizeof(line_array));
	num_line_array=0;
	mod=1;
	char temp[]="Benzene";
	for(int i=0; i<7; i++) name[i]=temp[i];name[8]='\0';
	num_bul=0;
	head=new Bullet(pos_x,pos_y,0);
	last=head;
	head->nex=NULL;
	range=10;
	life_now=life=10;
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
		if(judge_cha_state==0)
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
		if( !(mod==2&&judge_cha_state==2) )
			print_cha_line(x,y);
		if(GetAsyncKeyState(VK_UP)<0) 
		{
			print_cha_old(x,y,print_chara);
			judge_dir=1;print_part_cha_new(x,y,print_chara);
			if(mod==2)
			{
				if(judge_cha_state==0 || judge_cha_state==1)
				{
					if(judge_cha_state==0)
						SelectObject(hdc,hPen);
					else
						SelectObject(hdc,pen_black);
					MoveToEx(hdc,x-18,y-10,NULL); LineTo(hdc,x-2,y-25);
					MoveToEx(hdc,x+18,y-10,NULL);LineTo(hdc,x+2,y-25);
				}
				else
				{
					SelectObject(hdc,hPen);
					Ellipse(hdc,x-15,y-15,x+15,y+15);
				}
			}
			if(mod==3) {print_cha_line(x,y); print_cha_ball(x,y,0);}
		}
		else if(GetAsyncKeyState(VK_DOWN)<0) 
		{	
			print_cha_old(x,y,print_chara);judge_dir=3;
			print_part_cha_new(x,y,print_chara);
			if(mod==2)
			{
				if(judge_cha_state==0 || judge_cha_state==1)
				{
					if(judge_cha_state==0)
						SelectObject(hdc,hPen);
					else
						SelectObject(hdc,pen_black);
					MoveToEx(hdc,x-18,y+10,NULL);LineTo(hdc,x-2,y+25);
					MoveToEx(hdc,x+18,y+10,NULL);LineTo(hdc,x+2,y+25);
				}
				else
				{
					SelectObject(hdc,hPen);
					Ellipse(hdc,x-15,y-15,x+15,y+15);
				}
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
				if(judge_cha_state==0 || judge_cha_state==1)
				{
					if(judge_cha_state==0)
						SelectObject(hdc,hPen);
					else
						SelectObject(hdc,pen_black);
					MoveToEx(hdc,x-20,y-5,NULL);LineTo(hdc,x-8,y-20);
					MoveToEx(hdc,x-20,y+5,NULL);LineTo(hdc,x-8,y+20);
				}
				else
				{
					SelectObject(hdc,hPen);
					Ellipse(hdc,x-15,y-15,x+15,y+15);
				}
			}
			if(mod==3) {print_cha_line(x,y); print_cha_ball(x,y,0);}
		}
		else if(GetAsyncKeyState(VK_RIGHT)<0) 
		{	
			SelectObject(hdc,hPen);print_cha_old(x,y,print_chara);
			judge_dir=4;print_part_cha_new(x,y,print_chara);
			if(mod==2)
			{
				if(judge_cha_state==0 || judge_cha_state==1)
				{
					if(judge_cha_state==0)
						SelectObject(hdc,hPen);
					else
						SelectObject(hdc,pen_black);
					MoveToEx(hdc,x+20,y-5,NULL);LineTo(hdc,x+8,y-20);
					MoveToEx(hdc,x+20,y+5,NULL);LineTo(hdc,x+8,y+20);
				}
				else
				{
					SelectObject(hdc,hPen);
					Ellipse(hdc,x-15,y-15,x+15,y+15);
				}
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
	if( (judge_cha_state!=0&& (mod==1||mod==3) ) || (judge_cha_state==2&&mod==2)  ) { print_cha_new(pos_x,pos_y,print_chara);	 return; }
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
		if(judge_cha_state==false)
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
	if(judge_cha_state==false)
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
	life_now=life=10;
	judge_cha_state=false;
	judge_dir=1;
	new_point(pos_x,pos_y,print_chara);
	memset(num_count,0,sizeof(num_count));
	num_bul=0;
	if(mod==1)
	{
		char temp[]="Benzene";
		for(int i=0; i<7; i++) name[i]=temp[i];name[7]='\0';
		head=new Bullet(pos_x,pos_y,0);
		last=head;
		
		head->nex=NULL;
		range=20;
	}
	if(mod==2)
	{
		char temp[]="Cyclohexadiene";
		for(int i=0; i<14; i++) name[i]=temp[i];name[14]='\0';
		range=550;
	}
	if(mod==3)
	{
		char temp[]="Pyran";
		for(int i=0; i<5; i++) name[i]=temp[i];name[5]='\0';
		range=40;
	}
}

/////////// Square ////////////////
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
void Square::judge_input(double speed,int judge_cha_state,int mod)
{
	if(  (judge_cha_state!=0&&mod==1) || (judge_cha_state==2&&mod==2)  )
	if((GetAsyncKeyState(VK_LEFT)<0)||(GetAsyncKeyState(VK_RIGHT)<0))
	{
		paint_room_old(pos_x,pos_y,pos,angle);
		if(GetAsyncKeyState(VK_LEFT)<0) angle+=speed/3.0;
		if(GetAsyncKeyState(VK_RIGHT)<0) angle-=speed/3.0;
		paint_room_new(pos_x,pos_y,pos,angle);
	}
	return;
}

/////////// Bullet ////////////////
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

/////////// Monster ////////////////
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
	path.x=pos_x;
	path.y=pos_y;
	new_point(pos_x,pos_y,num_edge,pos);
}

/////////// Obstacle ////////////////
void Obstacle::print_old()
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	::SetDCPenColor(hdc, RGB(0,0,0));  
	::SetDCBrushColor(hdc,RGB(0,0,0)); 
	Rectangle(hdc,pos_x-r,pos_y-r,pos_x+r,pos_y+r );
}
void Obstacle::new_center(double angle)
{
	pos_x=900+dis*(cosf(angle+init));
	pos_y=495-dis*(sinf(angle+init));
	new_point();
}
Obstacle::Obstacle()
{
	
}

Stab::Stab()
{
	reset();
}
void Stab::print_now(double angle)
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
Stab::Stab(double x, double y)
{
	pos_x=x; pos_y=y;
	count=rand()%5;
	new_point();
	Vector a(900-pos_x,495-pos_y);
	init=acosf( (a.x-1) / a.get_lenth());
}
void Stab::new_point()
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
	POINT edge1[]=
	{
		pos_x-r,pos_y-r,pos_x-r,pos_y+r,pos_x+r,pos_y+r,pos_x+r,pos_y-r,pos_x-r,pos_y-r
	};
	for(int i=0; i<5; i++)
	{
		pos[i].x=edge1[i].x;pos[i].y=edge1[i].y;
	}
	
}
void Stab::reset()
{
	judge_show=true;
	count=rand()%5;
	count_max=rand()%7+3;
	int c=rand()%3;
	if(c==2) count_max=MAX_INT;
	int rand1=rand()%2-1,rand2=rand()%2-1;
	pos_x=rand()%12+3;    // 900 495
	pos_y=rand()%12+3;
	pos_x=get_x_from_i(pos_x);
	pos_y=get_y_from_j(pos_y);
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

Stone::Stone()
{
	reset();
}
Stone::Stone(double x, double y)
{
	pos_x=x; pos_y=y;
	new_point();
	Vector a(900-pos_x,495-pos_y);
	init=acosf( (a.x-1) / a.get_lenth());
}
void Stone::new_point()
{
	POINT edge1[]=
	{
		pos_x-r,pos_y-r,pos_x-r,pos_y+r,pos_x+r,pos_y+r,pos_x+r,pos_y-r,pos_x-r,pos_y-r
	};
	for(int i=0; i<5; i++)
	{
		pos[i].x=edge1[i].x;pos[i].y=edge1[i].y;
	}
	return;
}
void Stone::print_now(double angle)
{
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	print_old();
	new_center(angle);
	::SetDCPenColor(hdc, RGB(199,97,20));  
	::SetDCBrushColor(hdc,RGB(199,97,20)); 
	Rectangle(hdc,pos_x-r,pos_y-r,pos_x+r,pos_y+r );
	new_point();
}
void Stone::reset()
{
	int rand1=rand()%2-1,rand2=rand()%2-1;
	pos_x=rand()%12+3;    // 900 495
	pos_y=rand()%12+3;
	pos_x=get_x_from_i(pos_x);
	pos_y=get_y_from_j(pos_y);
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

/////////// Status ////////////////
State* MENU_START::transition(int x)  
{  
    switch(x)  
    {  
        case 2:  
            return &s2;  
        case 6:  
            return &s6;  
        default:  
            return &s1;  
    }  
}  
void MENU_START::eventt()
{
	SelectObject(hdc,hFont);
	SelectObject(hdc,hPen);
	LPCTSTR str_start=L"Start";
	LPCTSTR str_exit=L"Exit";
	LPCTSTR str_load=L"Load";
	LPCTSTR str_title=L"Cyclooctane";
	LPCTSTR str_sub_title=L"----Who's the Hunter Now?";
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
		SelectObject(hdc,hFont_sub_title);
		TextOut(hdc,850,380,str_sub_title,25);
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
			Game::clear();
		}
		SelectObject(hdc,hPen);
		if(gamestatus==1)
		{POINT sqr_a[]={ 655,500, 810,500,  810,583,  655,583 ,  655,500 }; for(int i=0; i<5; i++) {sqr_now[i].x=sqr_a[i].x;sqr_now[i].y=sqr_a[i].y;}}
		else if(gamestatus==2)
		{POINT sqr_a[]={ 655,600, 815,600, 815,683,  655,683 , 655,600 };for(int i=0; i<5; i++) {sqr_now[i].x=sqr_a[i].x;sqr_now[i].y=sqr_a[i].y;}}
		else {POINT sqr_a[]={ 672,700, 790,700, 790,783, 672,783 , 672,700 }; for(int i=0; i<5; i++) {sqr_now[i].x=sqr_a[i].x;sqr_now[i].y=sqr_a[i].y;}}
		Polyline(hdc,sqr_now, 5);
	}
	Game::clear();
	if(gamestatus==1 || gamestatus==2)
		gamestatus=2;
	else
		gamestatus=6;
	new_data.fresh_data();
	FSM::current=transition(gamestatus);
	return;
}

State* MENU_CHA::transition(int x)  
{  
    switch(x)  
    {  
        case 1:  
            return &s1;  
        case 3:  
            return &s3;  
        default:  
            return &s2;  
    }  
}  
void MENU_CHA::eventt()
{
	SelectObject(hdc,hFont);
	LPCTSTR str_ben=L"Benzene";
	LPCTSTR str_cyc=L"Cyclohexadiene";
	LPCTSTR str_pyran=L"Pyran";
	LPCTSTR str_title=L"Charactor";
	int gamestatus=1;
	int tem_mod=1;
	::SetDCPenColor(hdc, RGB(255,0,0));  
	::SetDCBrushColor(hdc,RGB(255,0,0)); 
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Ellipse(hdc,310,790,325,800);
	new_data.co_ben.set_new_data();
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
		new_data.co_ben.mod=1;
		new_data.co_ben.print_cha_new(320,620,new_data.co_ben.print_chara);
		new_data.co_ben.mod=2;
		new_data.co_ben.print_cha_new(770,620,new_data.co_ben.print_chara);
		new_data.co_ben.mod=3;
		new_data.co_ben.print_cha_new(1200,620,new_data.co_ben.print_chara);
		::SetDCPenColor(hdc, RGB(255,0,0));  
		::SetDCBrushColor(hdc,RGB(255,0,0)); 
		if(GetAsyncKeyState(VK_ESCAPE)<0)
		{	
			gamestatus=1;
			break;
		}
		if(_kbhit())
		{
			char aaa=_getch();
			if(aaa=='\r') {	gamestatus=3;break;}
			if(aaa=='a'||aaa=='d')
			{
				if(aaa=='a')
					tem_mod=tem_mod>1?tem_mod-1:3;
				if(aaa=='d')
					tem_mod=tem_mod<3?tem_mod+1:1;
				::SetDCPenColor(hdc, RGB(0,0,0));  
				::SetDCBrushColor(hdc,RGB(0,0,0)); 
				Ellipse(hdc,310,790,325,800);
				Ellipse(hdc,760,790,775,800);
				Ellipse(hdc,1200,790,1215,800);
			}
			if(aaa=='q')
				new_data.co_ben.judge_cha_state=1-new_data.co_ben.judge_cha_state;
		}
		if(tem_mod==1)
		{Ellipse(hdc,310,790,325,800);}
		else if(tem_mod==2)
		{Ellipse(hdc,760,790,775,800);}
		else {Ellipse(hdc,1200,790,1215,800);}
	}
	new_data.co_ben.mod=tem_mod;
	new_data.co_ben.set_new_data();
	//cyclooctane.ben.mod=tem_mod;
	//cyclooctane.ben.set_new_data();
	Game::clear();
	FSM::current=transition(gamestatus);
}

State* ON_GAME::transition(int x)  
{  
    switch(x)  
    {  
        case 4:  
            return &s4;  
        case 5:  
            return &s5;  
		case 7:
			return &s7;
        default:  
            return &s3; 
    }  
}  
void ON_GAME::eventt()
{
	int gamestatus=3;
	new_data.set_data(cyclooctane);
	//new_data.fresh_data();
	Game::clear();
	while(1)  //  游戏循环执行
	{
		cyclooctane.show();   // 显示画面
		cyclooctane.updateWithoutInput();  // 与输入无关的更新
		cyclooctane.updateWithInput();    // 与输入有关的更新
		cyclooctane.show();
		Sleep(50);
		if( GetAsyncKeyState(VK_ESCAPE)<0 )
		{
			gamestatus=4; break;
		}
		if(cyclooctane.ben.life==0)
		{
			gamestatus=5; break;
		}
		if(cyclooctane.room_count%2==0&&cyclooctane.room_count&&cyclooctane.judge_update==0)
		{
			cyclooctane.judge_update=1;
			gamestatus=7; break;
		}
	}
	Game::clear();
	FSM::current=transition(gamestatus);
	return;
} 

State* MENU_PAUSE::transition(int x)  
{  
    switch(x)  
    {  
        case 1:  
            return &s1;  
        case 3:  
            return &s3;  
        default:  
            return &s4;  
    }  
}  
void MENU_PAUSE::eventt()
{
	new_data.store_data(cyclooctane);
	int gamestatus=4;
	int tem_status=1;
	Game::clear();
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
					tem_status=tem_status>1?tem_status-1:3;
				if(aaa=='s')
					tem_status=tem_status<3?tem_status+1:1;
				::SetDCPenColor(hdc, RGB(0,0,0));  
				::SetDCBrushColor(hdc,RGB(0,0,0)); 
				Ellipse(hdc,575,535,585,545);
				Ellipse(hdc,640,635,650,645);
				Ellipse(hdc,655,735,665,745);
			}
		}
		::SetDCPenColor(hdc, RGB(255,0,0));  
		::SetDCBrushColor(hdc,RGB(255,0,0)); 
		if(tem_status==1)
		{Ellipse(hdc,575,535,585,545);}
		else if(tem_status==2)
		{Ellipse(hdc,640,635,650,645);}
		else {Ellipse(hdc,655,735,665,745);}
	}
	if( tem_status==1 || tem_status==2 ) 
		gamestatus=3;
	else if( tem_status==3 ) 
		gamestatus=1;
	Game::clear();
	FSM::current=transition(gamestatus);
} 

State* MENU_DEAD::transition(int x)  
{  
    switch(x)  
    {  
        case 1:  
            return &s1;  
        case 6:  
            return &s6;  
        default:  
            return &s5;  
    }  
}  
void MENU_DEAD::eventt()
{
	int gamestatus=4;
	int tem_status=1;
	Game::clear();
	LPCTSTR str_restart=L"Restart";
	LPCTSTR str_exit=L"Exit";
	LPCTSTR str_title=L"You Died";
	SetBkColor(hdc, RGB(0,0,0));
	SetTextColor(hdc,RGB(255,255,255));
	::SetDCPenColor(hdc, RGB(255,0,0));  
	::SetDCBrushColor(hdc,RGB(255,0,0)); 
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Ellipse(hdc,610,565,620,575);
	while(1)	
	{
		SelectObject(hdc,hFont_title);
		TextOut(hdc,500,200,str_title,8);
		SelectObject(hdc,hFont);
		TextOut(hdc,640,530,str_restart,7);
		TextOut(hdc,695,630,str_exit,4);
		TextOut(hdc,695,630,str_exit,4);
		if(_kbhit())
		{
			char aaa=_getch();
			if(aaa=='\r') break;
			if(aaa=='s'||aaa=='w')
			{
				if(aaa=='w')
					tem_status=tem_status>1?tem_status-1:2;
				if(aaa=='s')
					tem_status=tem_status<2?tem_status+1:1;
				::SetDCPenColor(hdc, RGB(0,0,0));  
				::SetDCBrushColor(hdc,RGB(0,0,0)); 
				Ellipse(hdc,610,565,620,575);
				Ellipse(hdc,665,665,675,675);
			}
		}
		::SetDCPenColor(hdc, RGB(255,0,0));  
		::SetDCBrushColor(hdc,RGB(255,0,0)); 
		if(tem_status==1)
		{Ellipse(hdc,610,565,620,575);}
		else if(tem_status==2)
		{Ellipse(hdc,665,665,675,675);}
	}
	switch(tem_status)
	{
	case 1: gamestatus=1; break;
	case 2: gamestatus=6; break;
	}
	Game::clear();
	FSM::current=transition(gamestatus);
} 

State* EXIT::transition(int)  
{  
    return NULL;  
}  
void EXIT::eventt()
{
	new_data.fresh_data();
	exit(0);
} 

State* CHANGE::transition(int x)  
{  
    switch(x)  
    {  
        case 3:  
            return &s3;    
        default:  
            return &s7;  
    }  
}  
void CHANGE::eventt()
{
	new_data.store_data(cyclooctane);
	SelectObject(hdc,hFont);
	LPCTSTR str_ben=L"Benzene";
	LPCTSTR str_cyc=L"Cyclohexadiene";
	LPCTSTR str_pyran=L"Pyran";
	LPCTSTR str_title=L"Update";
	int gamestatus=7;
	int tem_mod=1;
	::SetDCPenColor(hdc, RGB(255,0,0));  
	::SetDCBrushColor(hdc,RGB(255,0,0)); 
	::SelectObject(hdc,GetStockObject(DC_PEN));
	::SelectObject(hdc,GetStockObject(DC_BRUSH));
	Ellipse(hdc,310,790,325,800);
	while(1)	
	{
		SetBkColor(hdc, RGB(0,0,0));
		SetTextColor(hdc,RGB(255,255,255));
		SelectObject(hdc,hFont_title);
		TextOut(hdc,530,150,str_title,6); TextOut(hdc,530,150,str_title,6);
		SelectObject(hdc,hFont);
		TextOut(hdc,200,700,str_ben,7);TextOut(hdc,550,700,str_cyc,14);
		TextOut(hdc,1120,700,str_pyran,5);TextOut(hdc,1120,700,str_pyran,5);
		cyclooctane.ben.mod=1;
		cyclooctane.ben.print_cha_new(320,620,cyclooctane.ben.print_chara);
		cyclooctane.ben.mod=2;
		cyclooctane.ben.print_cha_new(770,620,cyclooctane.ben.print_chara);
		cyclooctane.ben.mod=3;
		cyclooctane.ben.print_cha_new(1200,620,cyclooctane.ben.print_chara);
		::SetDCPenColor(hdc, RGB(255,0,0));   ::SetDCBrushColor(hdc,RGB(255,0,0)); 
		if(_kbhit())
		{
			char aaa=_getch();
			if(aaa==27) break;
			if(aaa=='\r') {gamestatus=3;break;}
			if(aaa=='a'||aaa=='d')
			{
				if(aaa=='a')
					tem_mod=tem_mod>1?tem_mod-1:3;
				if(aaa=='d')
					tem_mod=tem_mod<3?tem_mod+1:1;
				::SetDCPenColor(hdc, RGB(0,0,0));  ::SetDCBrushColor(hdc,RGB(0,0,0)); 
				Ellipse(hdc,310,790,325,800);Ellipse(hdc,760,790,775,800); Ellipse(hdc,1200,790,1215,800);
			}
		}
		if(tem_mod==1)
		{Ellipse(hdc,310,790,325,800);}
		else if(tem_mod==2)
		{Ellipse(hdc,760,790,775,800);}
		else {Ellipse(hdc,1200,790,1215,800);}
	}
	Game::clear();
	if(tem_mod!=new_data.co_ben.mod)
	{	
		new_data.co_ben.mod=tem_mod;
		new_data.co_ben.set_new_data();
	}
	else
	{
		LPCTSTR str_recovery=L"恢复生命至满";
		LPCTSTR str_life=L"增加生命上限";
		LPCTSTR str_speed=L"增加移速";
		LPCTSTR str_range=L"增加射程";
		SetBkColor(hdc, RGB(0,0,0));
		SetTextColor(hdc,RGB(255,255,255));
		SelectObject(hdc,hFont_title);
		TextOut(hdc,530,100,str_title,6);
		TextOut(hdc,530,100,str_title,6);
		SelectObject(hdc,hFont);
		TextOut(hdc,600,500,str_recovery,6);
		TextOut(hdc,600,600,str_life,6);
		TextOut(hdc,600,700,str_speed,4);
		TextOut(hdc,600,800,str_range,4);
		TextOut(hdc,600,800,str_range,4);
		while(1)
		{
			if(_kbhit())
			{
				char aaa=_getch();
				if(aaa==27) {gamestatus=7;break;}
				if(aaa=='\r') {gamestatus=3;break;}
				if(aaa=='w'||aaa=='s')
				{
					if(aaa=='w')
						tem_mod=tem_mod>1?tem_mod-1:4;
					if(aaa=='s')
						tem_mod=tem_mod<4?tem_mod+1:1;
					::SetDCPenColor(hdc, RGB(0,0,0));  
					::SetDCBrushColor(hdc,RGB(0,0,0)); 
					Ellipse(hdc,565,525,585,545);
					Ellipse(hdc,565,625,585,645);
					Ellipse(hdc,565,725,585,745);
					Ellipse(hdc,565,825,585,845);
				}
			}
			::SetDCPenColor(hdc, RGB(255,0,0));  
			::SetDCBrushColor(hdc,RGB(255,0,0)); 
			if(tem_mod==1)
				Ellipse(hdc,565,525,585,545);
			else if(tem_mod==2)
				Ellipse(hdc,565,625,585,645);
			else if(tem_mod==3)
				Ellipse(hdc,565,725,585,745);
			else
				Ellipse(hdc,565,825,585,845);
		}
		switch(tem_mod)
		{
		case 1: new_data.co_ben.life_now=new_data.co_ben.life; break;
		case 2: new_data.co_ben.life+=3; new_data.co_ben.life_now+=3; break;
		case 3: new_data.co_ben.speed+=5; break;
		case 4: if(new_data.co_ben.mod==1) 
					new_data.co_ben.range+=15;
				else
					new_data.co_ben.range+=50; break;
		}
	}
	Game::clear();
	new_data.set_data(cyclooctane);
	new_data.fresh_data();
	new_data.store_data(cyclooctane);
	FSM::current=transition(gamestatus);
	return;
}

/////////// Data Base ////////////////
Data_Base::Data_Base()
{
	fresh_data();
}
Data_Base::Data_Base(const Data_Base& a)
{
	store_data(a);
}
Data_Base::~Data_Base()
{
}
void Data_Base::fresh_data()
{
	empt.startup();
	store_data(empt);
	co_Bullet_num_time_count=0;
	co_Monster_num_count=0;
	co_Monster_num_total=0;
	co_num_monster_fresh=0;
	co_judge_update=0;
	return;
}
void Data_Base::store_data(const Data_Base& a)
{
	co_death_count=a.co_death_count;
	co_room_count=a.co_room_count;
	co_Bullet_num_time_count=a.co_Bullet_num_time_count;
	co_Monster_num_total=a.co_Monster_num_total;
	co_Monster_num_count=a.co_Monster_num_count;
	co_num_monster_fresh=a.co_num_monster_fresh;
	co_square=a.co_square;
	co_room=a.co_room;
	co_ben=a.co_ben;
	co_judge_update=a.co_judge_update;
	return;
}
void Data_Base::store_data(const Game& b)
{
	co_ben=b.ben;
	co_room=b.room;
	co_square=b.square;
	co_death_count=b.death_count;
	co_room_count=b.room_count;
	co_Bullet_num_time_count=Bullet::num_time_count;
	co_Monster_num_total=Monster::num_total;
	co_Monster_num_count=Monster::num_count;
	co_num_monster_fresh=num_monster_fresh;
	co_judge_update=b.judge_update;
	return;
}
void Data_Base::set_data(Game& a)
{
	a.ben=co_ben;
	a.room=co_room;
	a.square=co_square;
	a.room_count=co_room_count;
	a.death_count=co_death_count;
	Bullet::num_time_count=co_Bullet_num_time_count;
	Monster::num_total=co_Monster_num_total;
	Monster::num_count=co_Monster_num_count;
	num_monster_fresh=co_num_monster_fresh;
	a.judge_update=co_judge_update;
	return;
}
void Data_Base::read_data()
{

}
void Data_Base::write_data()
{

}

/////////// FSM ////////////////
void FSM::reset()  
{  
    current = &s1;  
}
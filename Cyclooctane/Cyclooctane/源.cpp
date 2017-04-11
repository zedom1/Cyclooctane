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


int main()
{
	startup();    // 数据初始化
	srand(time(0));
	while(1)  //  游戏循环执行
	{
		show();   // 显示画面
		updateWithoutInput();  // 与用户输入无关的更新
		updateWithInput();    // 与用户输入有关的更新
		Sleep(100);
	}
	return 0;
}
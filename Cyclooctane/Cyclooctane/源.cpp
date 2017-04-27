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
	Game cycloocatane;
	cycloocatane.startup();    // 数据初始化
	while(1)  //  游戏循环执行
	{
		cycloocatane.show();   // 显示画面
		cycloocatane.updateWithoutInput();  // 与输入无关的更新
		cycloocatane.updateWithInput();    // 与输入有关的更新
		cycloocatane.show();
		Sleep(50);
	}
	return 0;
}


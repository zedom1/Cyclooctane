#include "cyclooctane.h"
int main()
{
	initi();
	FSM::reset();
	while(1)
		FSM::current->eventt();
	return 0;
}
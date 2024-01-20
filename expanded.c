#include "DebugHdr.h"
int main()
{
    int a, b;
    b = 0;
	add_TT('b');
    a = 1;
	add_TT('a');
    while (b < 10)
    {
a = a + b;
	update_TT('a', a);
	display_TT('a');
b = b + 1;
	update_TT('b', b);
    }
	display_TT('b');
    return 0;
}
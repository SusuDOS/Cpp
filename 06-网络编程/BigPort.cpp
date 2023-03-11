// 大端小端测试,小端显示结果:x0:34
#include <stdio.h>
using namespace std;

int main()
{
    short int x;
    char x0, x1;
    x = 0x1122;
    x0 = ((char *)&x)[0]; // 低地址单元
    x1 = ((char *)&x)[1]; // 高地址单元
    printf("x0:%d\n", x0);
    return 0;
}

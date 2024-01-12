#include <stdio.h>

int main(int argc, char* argv[])
{
    printf("Hello world!\n");
    printf("begin vector\n");

    __asm__  __volatile__(
        "vsetivli t0, 8, e64, m1, ta, ma\n"
        "vmv.v.i v1, 1\n"
        "vmv.v.i v2, 2\n"
        "vadd.vv v3, v1, v2"
    );

    printf("end vector\n");


    return 0;
}

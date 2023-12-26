#include <stdio.h>

int main(int argc, char* argv[])
{
    printf("Hello world!\n");
    printf("begin floating\n");

    __asm__  __volatile__(
        "fmv.s f0, f5"
    );

    printf("end floating\n");


    return 0;
}

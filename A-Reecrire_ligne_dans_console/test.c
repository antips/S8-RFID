#include <stdio.h>
#include <unistd.h>
#include <conio.h>

int main()
{
    int seconds = 0;
    while (1)
    {
        printf("%d secondes ‚coul‚es.\nTest\n", seconds++);
        printf("\r");
        for (int i = 0; i < 2; i++)
            printf("\033[A");

        sleep(1);
    }
}
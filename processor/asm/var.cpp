#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main ()
{
    /*printf ("1");

    const char *a[20] = {};
    a[0] = "push 84";
    a[1] = "pop ";
    a[2] = "gfldg";
    char text[33] = "push 85\npussh\n\n\n 4\nout\n";
    int arr [20] = {};
    int number = 0;

    char arrr[20] = {};
    int val = 0;
    int temp = 0;

    sscanf (text, "%s %n", arr, &number );

    printf ("n = %d, a text[0] = %s\n", number, arr);



    //text[number - 1] = ' ';

    sscanf (text + number, "%d %n", &val, &temp);

    printf ("%d\n", temp);
    printf ("[%d]", val);
    printf ("5");

    number += temp;

    printf ("n = %d, a text = %s\n", number, arr);
    printf ("5");

    temp = 0;

    sscanf (text + number, "%s %n", arr, &temp);
    number += temp;

    printf ("%d\n", temp);
    printf ("n = %d, a text = %s", number, arr);
    printf ("5");*/
    /*char p[100] = "[fdfd";
    if (isalpha (*p))
    {
        printf ("232");
    }*/

    const char *jmp = "next";
    char arg[10] = {};
    scanf ("%s", arg);
    if (strcmp (jmp, arg) == 0)
    {
        printf ("win");
    }

    return 0;
}

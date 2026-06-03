1. Create folder ex01 and implement basic HelloWorld program.
2. Program feature: output string `Hello World!` on console.

```c
#include <stdio.h>
int main()
{
    printf("Hello World!\n");
    return 0;
}

1. Modify source code to implement keyboard input with scanf function.
2. Read the input string from user and echo the input content on the terminal.
#include <stdio.h>
int main()
{
    char str[100];
    printf("??????");
    scanf("%s", str);
    printf("???????%s\n", str);
    return 0;
}

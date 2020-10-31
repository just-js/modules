const { tcc } = just.library('./tcc.so', 'tcc')

const source = `#include <tcclib.h>
extern int add(int a, int b);
extern const char hello[];

int fib(int n)
{
    if (n <= 2)
        return 1;
    else
        return fib(n-1) + fib(n-2);
}

int foo(int n)
{
    printf("%s\\n", hello);
    printf("fib(%d) = %d\\n", n, fib(n));
    printf("add(%d, %d) = %d\\n", n, 2 * n, add(n, 2 * n));
    return 0;
}
`
tcc.compile(source)

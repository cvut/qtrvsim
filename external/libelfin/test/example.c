int fib(int x) 
{
        if (x <= 1)
                return x;
        return fib(x - 1) + fib(x - 2);
}

int main(int argc, char **argv) 
{
        return fib(10);
}

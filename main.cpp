#include <cstdio>
#include "include/stacktrace.h"

#include <iostream>
#include <iomanip>
#include "stacktrace.h"

void recursion(int i);
void func_0(int i);
void func_1(int i);
void method_0();
void method_1();
void method_2();
void method_3();
void some_handler();

void recursion(int i)

{
    if (i == 5)
        func_0(0);
    else
        recursion(i + 1);
}

void func_0(int i)
{
    if (i == 2)
        some_handler();
    else
        func_1(i + 1);
}

void func_1(int i)
{
    func_0(i);
}

void method_0()
{
    method_1();
}

void method_1()
{
    method_2();
}

void method_2()
{
    method_3();
}

void method_3()
{
    recursion(0);
}

void some_handler()
{
    throw_dbg(stacktrace::stack_aware_exception, "test_");
}

int main()
{
    try
    {
        method_0();
    }
    catch(stacktrace::stack_aware_exception &e)
    {
        std::cout << stacktrace::stacktrace << e;
    }
}
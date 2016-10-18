using System;

class StringArg
{
    static void Print(string strVal)
    {
        Console.WriteLine(strVal);
    }

    static void Main(string[] args)
    {
        string val = "Test 123";
        Print(val);
    }
}
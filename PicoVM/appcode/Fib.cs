/**
 * Very, very, very simple testing program.
 */

using System;

class Fib
{
	static long fib(long n)
	{
		return n > 1 ? fib(n - 1) + fib(n - 2) : n;
	}

	static void Main(string[] args)
	{
		for (long n = 0; n <= 35; ++n) {
		    Console.WriteLine(fib(n));
		}
	}
}

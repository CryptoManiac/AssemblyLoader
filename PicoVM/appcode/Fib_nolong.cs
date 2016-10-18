/**
 * Very, very, very simple testing program.
 */

using System;

class Fib
{
	static uint fib(uint n)
	{
		return n > 1 ? fib(n - 1) + fib(n - 2) : n;
	}

	static void Main(string[] args)
	{
		for (uint n = 0; n <= 45; ++n) {
		    Console.WriteLine(fib(n));
		}
	}
}

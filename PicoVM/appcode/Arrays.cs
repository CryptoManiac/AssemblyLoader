using System;

class Arrays
{
    static void Main(string[] args)
    {
        long[] tL = new long[65536];

        for (int n = 0; n < tL.Length; ++n) {
            tL[n] = n;
        }

        for (int n = 0; n < tL.Length; ++n) {
            Console.WriteLine(tL[n]);
        }
    }
}

using System;
using System.Runtime.CompilerServices;

namespace CSScript
{
    public class Class1
    {
        private void PrintMessage()
        {
            Console.WriteLine("Hello, Mono!!!");
        }

        // C++の関数（内部呼び出し）
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static int Multiply(int a, int b);

        // C++の関数を呼び出す版
        private void PrintMessage2()
        {
            // C++の関数を内部呼び出し
            Console.WriteLine("2 * 3 = " + Multiply(2, 3));
        }
    }
}

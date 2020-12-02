// Never try to compile this. Just need all of the odd numbers to put syntax
// highlighting through its paces.
//
//
#include <stdio>

int main()
{

    int a = 212;
    unsigned int b = 215u;
    long c = 0xFeeL;

    int d = 85;
    int e = 0213;
    int f = 0x04b;
    int g = 30;
    unsigned int h = 30u;
    unsigned long i = 30ul;
    unsigned long j = 30lu;
    unsigned long k = 30LU;
    unsigned long l = 30UL;
    long m = 30l;
    unsigned long n = 0xFeeUL;
    int o = 0x04B;

    int p = 0765;
    unsigned int q = 0654U;
    unsigned long r = 0765432176ul;

    unsigned int s = 0b010101U;
    unsigned long t = 0b111101110ul;
    int u = 0b101;
    unsigned long v = 0b1101101ul;
    unsigned long w = 0b1101101UL;

    unsigned long x1 = 0B1101101ul;

    unsigned long y3 = 123+456+789UL;
    unsigned long y4 = 123*456*789UL;

    double aa = 3.14159;
    double bb = 314159E-5L;
    double cc = 3.14159e-5l;
    double dd = 3.14159e-5;
    double ee = 3.14159E+5;

    float ff = 3.14159E+2F;

    return EXIT_SUCCESS;
}
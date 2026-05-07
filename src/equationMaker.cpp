#include <Arduino.h>

int ANS;
String eq;

int getNum(int x) {
  return random(x);
}

void setAns(int a, int b, String sign)
{
    if (sign == "+") { ANS = a + b; }
    else if (sign == "-") { ANS = a - b; }
    else if (sign == "*") { ANS = a * b; }
    else { ANS = a/b; }
}

void equation()
{
    String sign;
    int a;
    int b;
    switch(getNum(4))
    {
        case 0:
            sign = "+";
            break;
        case 1:
            sign = "-";
            break;
        case 2:
            sign = "*";
            break;
        case 3:
            sign = "/";
            break;
    }
    if (sign == "/")
    {
        b = getNum(10) + 1;
        int result = getNum(100) + 1;
        a = result * b;
    }
    else if (sign == "*")
    {
        a = getNum(20) + 1;
        b = getNum(20) + 1;
    }
    else
    {
        a = getNum(80) + 20;
        b = getNum(80) + 20;

        if (b > a)
        {
            int temp = b;
            b = a;
            a = temp;
        }
    }

    setAns(a, b, sign);

    eq = String(a) + " " + sign + " " + String(b);

}

#include <Arduino.h>

char add(int a, int b)
{
    return a + b;
}

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    char a = '1';
    char b = '2';
    int c = add(a, b);
    Serial.println(c);
    int number1, number2, result;
    Serial.print("enters two numbers you wish to add:");
    Serial.print("%d %d , &number1, &number2");
    result = add(number1, number2); // function call
    Serial.print(String("addition is %d , result", result));
}
// Test file for C++ Lexical Scanner
// This file contains various C++ constructs to test the scanner

#include <iostream>
#include <string>
using namespace std;

/* Multi-line comment
   Testing the scanner with
   various tokens */

class Calculator {
private:
    int result;

public:
    Calculator() : result(0) {}

    int add(int a, int b) {
        return a + b;
    }

    float divide(float x, float y) {
        if (y != 0.0f) {
            return x / y;
        }
        return 0.0f;
    }

    void display() {
        cout << "Result: " << result << endl;
    }
};

int main() {
    // Variable declarations
    int num1 = 42;
    float num2 = 3.14159f;
    double num3 = 2.718281828;
    char ch = 'A';
    bool flag = true;
    string message = "Hello, World!";

    // Array declaration
    int arr[10] = {1, 2, 3, 4, 5};

    // Pointer operations
    int* ptr = &num1;

    // Arithmetic operations
    int sum = num1 + 10;
    int diff = num1 - 5;
    int product = num1 * 2;
    int quotient = num1 / 2;
    int remainder = num1 % 3;

    // Compound assignment operators
    sum += 5;
    diff -= 2;
    product *= 3;
    quotient /= 2;

    // Increment/decrement
    ++num1;
    num2--;

    // Comparison operations
    if (num1 == 43) {
        cout << "num1 is 43" << endl;
    } else if (num1 != 43) {
        cout << "num1 is not 43" << endl;
    }

    if (num1 > 40 && num1 < 50) {
        cout << "num1 is between 40 and 50" << endl;
    }

    // Logical operations
    bool result1 = (num1 > 0) && (num2 > 0.0f);
    bool result2 = (num1 < 0) || (num2 < 0.0f);
    bool result3 = !flag;

    // Bitwise operations
    int bitwise1 = num1 & 0xFF;
    int bitwise2 = num1 | 0x10;
    int bitwise3 = num1 ^ 0xAA;
    int bitwise4 = ~num1;
    int bitwise5 = num1 << 2;
    int bitwise6 = num1 >> 1;

    // Loops
    for (int i = 0; i < 10; i++) {
        cout << "i = " << i << endl;
    }

    int j = 0;
    while (j < 5) {
        cout << "j = " << j << endl;
        j++;
    }

    // Switch statement
    switch (ch) {
        case 'A':
            cout << "Character is A" << endl;
            break;
        case 'B':
            cout << "Character is B" << endl;
            break;
        default:
            cout << "Unknown character" << endl;
            break;
    }

    // Object creation and method calls
    Calculator calc;
    int addResult = calc.add(10, 20);
    float divResult = calc.divide(10.0f, 3.0f);
    calc.display();

    // Namespace and scope resolution
    std::cout << "Using std namespace" << std::endl;

    // Ternary operator
    int max = (num1 > sum) ? num1 : sum;

    // Function pointer
    int (*funcPtr)(int, int) = &Calculator::add;

    // Escape sequences in strings
    string escapeTest = "Line 1\nLine 2\tTabbed\n\"Quoted\"\\Backslash";
    cout << escapeTest << endl;

    // Hexadecimal and octal numbers (if supported in future)
    // int hex = 0xFF;
    // int octal = 077;

    cout << "Program completed successfully!" << endl;
    return 0;
}

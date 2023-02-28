#include <bits/stdc++.h>
using namespace std;

int A = 0, B = 0, PC = 0, SP = 0;

// Change 500000 to desired value if more than 2MB of memory is to be store.
#define MEMORY_CAPACITY 500000
// Change 2000 to desired value if more than 2000 instructions have to be executed.
#define EXECUTION_LIMIT 200000
vector<int> memory(MEMORY_CAPACITY, 0); // 2MB reserved for emulation
typedef long long int ll;
bool isHex(string str);
string tohex(ll n);
int hexCharToInt(char c);
int hexStringToInt(string hexString);
void padding(string &s, int n, int cnt);
void execute(pair<string, string> code);
string int_to_hex(int cnt);

string filename;
string history_log = "";

string tohex(int n)
{
    string s = int_to_hex(n);
    padding(s, 8, n);
    return s;
}
bool isHex(string str)
{
    if (str.length() < 3)
    { // The string must have at least 3 characters (e.g. "0x0")
        return false;
    }
    if (str.substr(0, 2) != "0x")
    { // The string must start with "0x"
        return false;
    }
    return std::all_of(str.begin() + 2, str.end(), [](char c)
                       { 
        // The remaining characters must be hexadecimal digits
        return std::isxdigit(c); });
}

int hexCharToInt(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1; // Invalid hexadecimal character
    }
}
int hexStringToInt(string hexString)
{
    int intValue = 0;
    int base = 1;

    for (int i = hexString.length() - 1; i >= 0; i--)
    {
        int hexDigit = hexCharToInt(hexString[i]);

        if (hexDigit == -1)
        {
            // Invalid hexadecimal character
            return -1;
        }

        intValue += hexDigit * base;
        base *= 16;
    }

    return intValue;
}
void padding(string &s, int n, int cnt)
{
    string str = "";
    string t = "";
    if (cnt >= 0)
        t = "0";
    else
        t = "F";

    for (int i = 0; i < n; i++)
    {
        str = t + str;
    }
    s = str + s;
    s = s.substr(s.length() - n);
}

string int_to_hex(int cnt)
{
    stringstream stream;
    stream << std::hex << cnt;
    return stream.str();
}

void execute(string code)
{
    string operand = code.substr(0, 6);
    string opcode = code.substr(7, 2);

    if (opcode == "00") // ldc
    {
        B = A;
        A = hexStringToInt(operand);
    }
    else if (opcode == "01") // adc
    {
        A = A + hexStringToInt(operand);
    }
    else if (opcode == "02") // ldl
    {
        B = A;
        int loc = SP + hexStringToInt(operand);
        illegalmemacc(loc);
        A = memory[loc];
    }
    else if (opcode == "03") // stl
    {
        int loc = SP + hexStringToInt(operand);
        illegalmemacc(loc);
        memory[loc] = A;
        A = B;
    }
    else if (opcode == "04") // ldnl
    {
        int loc = A + hexStringToInt(operand);
        illegalmemacc(loc);
        A = memory[loc];
    }
    else if (opcode == "05") // stnl
    {
        int loc = A + hexStringToInt(operand);
        illegalmemacc(loc);
        memory[A] = B;
    }
    else if (opcode == "06") // add
    {
        A = A + B;
    }
    else if (opcode == "07") // sub
    {
        A = B - A;
    }
    else if (opcode == "08") // shl
    {
        A = (B << A);
    }
    else if (opcode == "09") // shr
    {
        A = (B >> A);
    }
    else if (opcode == "0a") // adj
    {
        SP = SP + hexStringToInt(operand);
    }
    else if (opcode == "0b") // a2sp
    {
        SP = A;
        A = B;
    }
    else if (opcode == "0c") // sp2a
    {
        B = A;
        A = SP;
    }
    else if (opcode == "0d") // call
    {
        B = A;
        A = PC;
        PC = PC + hexStringToInt(operand);
    }
    else if (opcode == "0e") // return
    {
        PC = A;
        A = B;
    }
    else if (opcode == "0f") // brz
    {
        if (A == 0)
            PC = PC + hexStringToInt(operand);
    }
    else if (opcode == "10") // brlz
    {
        if (A < 0)
            PC = PC + hexStringToInt(operand);
    }
    else if (opcode == "11") // br
    {
        PC = PC + hexStringToInt(operand);
    }
    else if (opcode == "12") // Halt
    {
        cout << "HALT ENCOUNTERED: EMULATION STOPPED" << endl;
        writeToFile();
    }
    else // data
    {
        int loc = SP;
        illegalmemacc(loc);
        memory[loc] = hexStringToInt(operand);
    }
}

void illegalmemacc(int loc)
{
    if ((loc > MEMORY_CAPACITY) or (loc < 0))
    {
        cout << "ILLEGAL MEMORY ACCESS!" << endl;
        exit(0);
    }
}
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
void writeToFile();

string filename;
string history_log = "";
int main(int argc, char **argv)
{
    // test();
    bool trace = false;
    if (argc == 2 || argc == 3)
    {
        filename = argv[1];
        if (argc == 3)
        {
            string arg = argv[2];
            if (arg == "-trace")
                trace = true;
            else
            {
                cout << "EMULATION FAILED: Only -trace is valid 2nd argument. \"" << arg << "\" Passed" << endl;
                exit(1);
            }
        }
    }
    else
    {
        cout << "EMULATION FAILED: Invalid number of arguments passed (" << argc - 1 << ") Only the file name is to be entered with or without -trace" << endl;
        exit(1);
    }
    ifstream in(filename);
    if (!in.is_open())
    {
        cout << "EMULATOR FAILED: No file named \"" << filename << "\" exists." << endl;
        exit(1);
    }
    cout << "1/4 FILE OPENED SUCCESSFULLY" << endl;
    vector<vector<string>> parsedLines;
    vector<string> lines;
    string line;
    while (getline(in, line))
    {
        lines.push_back(line);
    }
    for(auto itx:lines)
    {
        string str = itx;
        vector<string> temp;
        for (int i = 0; i < str.length(); i++)
        {
            if (str[i] == ' ')
            {
                temp.push_back(str.substr(0, i));
                str = str.substr(i + 1);
                i = 0;
            }
        }
        temp.push_back(str);
        parsedLines.push_back(temp);
    }

    cout << "2/4 FILE PARSED SUCCESSFULLY" << endl;

    vector<pair<string, string>> instructions;
    for (auto v : parsedLines)
    {
        if (v.size() > 1)
        {
            string binary = v[1];
            string opcode = binary.substr(6, 2);
            string operand = binary.substr(0, 6);
            pair<string, string> temp;
            temp.first = operand;
            temp.second = opcode;
            instructions.push_back(temp);
        }
    }

    cout << "3/4 OPCODE AND OPERAND SPLITTED" << endl;
    int pointer = 0;

    int limit = EXECUTION_LIMIT;
    int prevPC = -1;
    while (limit--)
    {
        if (PC > instructions.size() - 1)
        {
            cout << PC << " " << instructions.size() - 1 << endl;
            cout << "FLAG: PC exceeds the instruction set. Termination Stopped / Completed" << endl;
            break;
        }
        if (trace)
            cout << instructions[PC].first << instructions[PC].second << endl
                 << " ";
        execute(instructions[PC]);
        string temp = "A: " + tohex(A) + "\t" + "B: " + tohex(B) + "\t"
                                                                   "PC: " +
                      tohex(PC) + "\t" + "SP: " + tohex(SP) + "\n";
        if (trace)
            cout << temp;
        history_log += temp;
        if (prevPC == PC)
        {
            cout << "FLAG: INFINITE LOOP DETECTED\n";
            history_log += "INFINITE LOOP DETECTED\n";
            // cout<<prevPC<<" "<<PC;
            break;
        }
        prevPC = PC;
        PC++;
    }

    if (limit <= 0)
    {
        cout << "\nFLAG: EXECUTION LIMIT CROSSED. (>" << EXECUTION_LIMIT << " Executions, to execute more instructions, change \"EXECUTION_LIMIT\" in source code)\n";
        history_log += "\nEXECUTION LIMIT CROSSED. (>" +
                       to_string(EXECUTION_LIMIT) + " Executions, to execute more instructions, change \"EXECUTION_LIMIT\" in source code)\n";
    }

    string logFileName = "";
    for (int i = 0; i < filename.length(); i++)
    {

        if (filename[i] == '.')
            break;
        else
            logFileName += filename[i];
    }
    logFileName += ".trace";
    ofstream outBin(logFileName);
    outBin << history_log;
    outBin.close();
    cout << "4/4 test FILE GENERATED" << endl;
    cout << "EMULATION COMPLETED SUCCESSFULLY" << endl;
    in.close();
    return 0;
}





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

    int op = hexStringToInt(opcode);
    int opd = hexStringToInt(operand);
    switch (op)
    {
    case 0:
        B = A;
        A = opd;
        break;
    case 1:
        A = A + opd;
        break;
    case 2:
        B = A;
        int loc = SP + opd;
        illegalmemacc(loc);
        A = memory[loc];
        break;
    case 3:
        int loc = SP + opd;
        illegalmemacc(loc);
        memory[loc] = A;
        A = B;
        break;
    case 4:
        int loc = A + opd;
        illegalmemacc(loc);
        A = memory[loc];
        break;
    case 5:
        int loc = A + opd;
        illegalmemacc(loc);
        memory[loc] = B;
        break;
    case 6:
        A = A + B;
        break;
    case 7:
        A = B - A;
        break;
    case 8:
        A = (B << A);
        break;
    case 9:
        A = (B >> A);
        break;
    case 10:
        SP = SP + opd;
        break;
    case 11:
        SP = A;
        A = B;
        break;
    case 12:
        B = A;
        A = SP;
        break;
    case 13:
        B = A;
        A = PC;
        PC = PC + opd;
        break;
    case 14:
        PC = A;
        A = B;
        break;
    case 15:
        if (A == 0)
            PC = PC + opd;
        break;
    case 16:
        if (A < 0)
            PC = PC + opd;
        break;
    case 17:
        PC = PC + opd;
        break;
    case 18:
        cout << "HALT ENCOUNTERED: EMULATION STOPPED" << endl;
        writeToFile();
        break;
    default:
        int loc = SP;
        illegalmemacc(loc);
        memory[loc] = opd;
        break;
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

void writeToFile()
{
    string logFileName = "";
    for (int i = 0; i < filename.length(); i++)
    {
        if (filename[i] == '.')
            break;
        else
            logFileName += filename[i];
    }
    logFileName += ".trace";
    ofstream outBin(logFileName);
    outBin << history_log;
    outBin.close();
    cout << "4/4 trace FILE GENERATED" << endl;
    exit(0);
}

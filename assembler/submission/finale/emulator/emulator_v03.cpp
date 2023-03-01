#include <bits/stdc++.h>
using namespace std;

int A = 0, B = 0, PC = 0, SP = 0;

// Change 500000 to desired value if more than 2MB of memory is to be store.
#define MEMORY_CAPACITY 500000
// Change 2000 to desired value if more than 2000 instr have to be executed.
#define EXECUTION_LIMIT 200000
vector<int> memory(MEMORY_CAPACITY, 0); // 2MB reserved for emulation
typedef long long int ll;
bool isHex(string str);
string tohex(ll n);
int hexCharToInt(char c);
int hexStringToInt(string hexString);
void padding(string &s, int n, int cnt);
void execute(string code);
string int_to_hex(int cnt);
void writeToFile();
void section2(vector<vector<string>> &part_line, vector<pair<string, string>> &instr);
void section3(int &limit, bool trace, vector<pair<string, string>> &instr, int &prevPC);
void illegalmemacc(int loc);
void signedextension(string &hex);

string filename;
string history_log = "";
int main(int argc, char **argv)
{
    bool trace = false;
    if (argc == 2 or argc == 3)
    {
        filename = argv[1];
        if (argc == 3)
        {
            string arg = argv[2];
            if (arg == "-trace")
                trace = true;
            else
            {
                cout << "EMULATION FAILED ---> Only -trace is valid 2nd argument.But \"" << arg << "\" Passed" << endl;
                exit(1);
            }
        }
    }
    else
    {
        cout << "EMULATION FAILED ---> Invalid number of arguments passed (" << argc - 1 << ") Only the file name is to be entered with or without -trace" << endl;
        exit(1);
    }
    // filename = "test1.l";
    ifstream in(filename);
    if (!in.is_open())
    {
        cout << "EMULATOR FAILED ---> No file named -- \"" << filename << "\" exists here." << endl;
        exit(1);
    }
    cout << "|#   | FILE OPENED SUCCESSFULLY" << endl;
    vector<vector<string>> part_line;
    vector<string> lines;
    string line;
    while (getline(in, line))
    {
        lines.push_back(line);
    }
    for (auto itx : lines)
    {
        string str = itx;
        vector<string> temp;
        istringstream ss(str);
        string word;
        while (ss >> word)
        {
            temp.push_back(word);
            cout << word << endl;
        }
        part_line.push_back(temp);
    }

    cout << "|##  | FILE PARSING DONE SUCCESSFULLY" << endl;

    vector<pair<string, string>> instr;
    section2(part_line, instr);

    cout << "|### | OPCODE AND OPERAND SPLITTED" << endl;
    int pointer = 0;

    int limit = EXECUTION_LIMIT;
    int prevPC = -1;
    section3(limit, trace, instr, prevPC);

    string logFileName = "";
    cout << filename << endl;
    logFileName = filename.substr(4);
    logFileName = logFileName.substr(0, logFileName.length() - 2);
    logFileName += ".trace";
    logFileName = "../trace" + logFileName;
    cout << logFileName << endl;
    ofstream outBin(logFileName);
    outBin << history_log;
    outBin.close();
    cout << "|####| test FILE GENERATED" << endl;
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

int hexStringToInt(string n)
{
    std::stringstream ss;
    ss << std::hex << n;
    long long intValue;
    ss >> intValue;
    if (intValue > (ll)0x7fffffff)
    {
        intValue -= 0x100000000;
    }
    return intValue;
}

// only required for the operand so assuming to be 6 byte long already
void signedextension(string &hex)
{
    char c = hex[0];
    if (c == 'f' or
        c == 'e' or
        c == 'd' or
        c == 'c' or
        c == 'b' or
        c == 'a' or
        c == '9' or
        c == '8')
    {
        hex = "ff" + hex;
    }
    else
    {
        hex = "00" + hex;
    }
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
    string opcode = code.substr(6);
    int loc;
    signedextension(operand);
    signedextension(opcode);
    int op = hexStringToInt(opcode);
    int opd = hexStringToInt(operand);
    cout << "opd " << operand << " -- " << opd << endl;
    cout << "op " << opcode << " -- " << op << endl;

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
        loc = SP + opd;
        illegalmemacc(loc);
        A = memory[loc];
        break;
    case 3:
        loc = SP + opd;
        illegalmemacc(loc);
        memory[loc] = A;
        A = B;
        break;
    case 4:
        loc = A + opd;
        illegalmemacc(loc);
        A = memory[loc];
        break;
    case 5:
        loc = A + opd;
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
        cout << "HALT ENCOUNTERED ---> EMULATION STOPPED" << endl;
        writeToFile();
        break;
    default:
        loc = SP;
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
    logFileName = filename.substr(0, filename.length() - 2);
    logFileName = "../trace" + logFileName;
    // logFileName += ".trace";
    ofstream outBin(logFileName);
    outBin << history_log;
    outBin.close();
    cout << "|####| trace FILE GENERATED" << endl;
    exit(0);
}

void section2(vector<vector<string>> &part_line, vector<pair<string, string>> &instr)
{
    for (auto v : part_line)
    {
        if (v.size() > 1)
        {
            string binary = v[1];
            string opcode = binary.substr(6, 2);
            string operand = binary.substr(0, 6);
            pair<string, string> temp;
            temp.first = operand;
            temp.second = opcode;
            instr.push_back(temp);
        }
    }
}

void section3(int &limit, bool trace, vector<pair<string, string>> &instr, int &prevPC)
{
    while (limit--)
    {
        if (PC > instr.size() - 1)
        {
            cout << PC << " " << instr.size() - 1 << endl;
            cout << "FLAG ---> PC exceeds the instruction set. Termination Stopped / Completed" << endl;
            break;
        }
        if (trace)
            cout << instr[PC].first << instr[PC].second << endl
                 << " ";
        execute(instr[PC].first + instr[PC].second);
        string temp = "A: " + tohex(A) + "\t" + "B: " + tohex(B) + "\t"
                                                                   "PC: " +
                      tohex(PC) + "\t" + "SP: " + tohex(SP) + "\n";
        if (trace)
            cout << temp;
        history_log += temp;
        if (prevPC == PC)
        {
            cout << " !! FLAG ---> INFINITE LOOP DETECTED\n";
            history_log += "INFINITE LOOP DETECTED\n";
            // cout<<prevPC<<" "<<PC;
            break;
        }
        prevPC = PC;
        PC++;
    }

    if (limit <= 0)
    {
        cout << "\nFLAG ---> EXECUTION LIMIT CROSSED. (>" << EXECUTION_LIMIT << " Executions, to execute more instr, change \"EXECUTION_LIMIT\" in source code)\n";
        history_log += "\nEXECUTION LIMIT CROSSED. (>" +
                       to_string(EXECUTION_LIMIT) + " Executions, to execute more instr, change \"EXECUTION_LIMIT\" in source code)\n";
    }
}

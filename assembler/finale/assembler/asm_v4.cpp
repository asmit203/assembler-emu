#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
const string WHITESPACE = " \n\r\t\f\v";
int counter = 0;
string pc = "00000000";

vector<string> error;
// whole file
vector<string> lines;
vector<vector<string>> line_part;

map<string, int> opCodes;

multimap<int, string> labelAddr;

vector<string> instructions;
vector<string> warning;
string warn;
bool errFlag;

void raiseError(int code, string label)
{
    /*
        0- more than 1 arg passed
        1- 1 argument but unknown mnemonic
        2- 0 argument but unknown mnemonic
        3- bogus label name
        4- Invalid number
        5- No such label
        6- Duplicate label
        7- Infinite Loop
        8- inappropiate arguments
        9- invalid mnemonic
    */
    string temp;
    if (code == 0)
    {
        temp = "ERROR: Unknown mnemonic \"" + label + "\" found with more than one argument\n";
        error.push_back(temp);
    }
    else if (code == 1)
    {
        temp = "ERROR: Unknown mnemonic \"" + label + "\" found. With one argument\n";
        error.push_back(temp);
    }
    else if (code == 2)
    {
        temp = "ERROR: Unknown mnemonic \"" + label + "\"  found. With no argument\n";
        error.push_back(temp);
    }
    else if (code == 3)
    {
        temp = "ERROR: Bogus Label Name \"" + label + "\"\n";
        error.push_back(temp);
    }
    else if (code == 4)
    {
        temp = "ERROR: Invalid Number Entered \"" + label + "\"\n";
        error.push_back(temp);
    }
    else if (code == 5)
    {
        temp = "ERROR: No such label \"" + label + "\" exists\n";
        error.push_back(temp);
    }
    else if (code == 6)
    {
        temp = "ERROR: Duplicate label \"" + label + "\" found\n";
        error.push_back(temp);
    }
    else if (code == 7)
    {
        temp = "WARNING: Infinite loop detected\n";
        error.push_back(temp);
    }

    else if (code == 8)
    {
        temp = "ERROR: Inappropiate arguments given: " + label + " \n";
        error.push_back(temp);
    }
    else if (code == 9)
    {
        temp = "ERROR: Unknown mnemonic \"" + label + "\" found\n";
        error.push_back(temp);
    }
}
void padding(string &s, int n, int cnt);
void pcincrement(int cnt);
string int_to_hex(int cnt);
void commentRemover();
void emptyCleaner();
bool ishex(string str);
bool isnum(string s);
string labelspace(string s);
bool islabel(string str);
bool isHex(string str);

void opCodeInit()
{
    opCodes["ldc"] = 0;
    opCodes["adc"] = 1;
    opCodes["ldl"] = 2;
    opCodes["stl"] = 3;
    opCodes["ldnl"] = 4;
    opCodes["stnl"] = 5;
    opCodes["add"] = 6;
    opCodes["sub"] = 7;
    opCodes["shl"] = 8;
    opCodes["shr"] = 9;
    opCodes["adj"] = 10;
    opCodes["a2sp"] = 11;
    opCodes["sp2a"] = 12;
    opCodes["call"] = 13;
    opCodes["return"] = 14;
    opCodes["brz"] = 15;
    opCodes["brlz"] = 16;
    opCodes["br"] = 17;
    opCodes["HALT"] = 18;
    opCodes["data"] = 19;
    opCodes["SET"] = 20;
}
string ltrim(const string &str);
string rtrim(const string &str);
string trim(const string &s);
void debug(vector<vector<string>> f);
int labelfind(string s);
int numargs(int opcode);
void prntmultimap(multimap<int, string> m);

void addressdecider(string str, bool &pol);
void firstPassAddress(ifstream &inputFile, ofstream &outputLFile);
void secondpass(ofstream &outputLFile);
void decide(ofstream &outputLFile, string str);
void branchmech(ofstream &outputLFile, string str, int opcode);
void opcode1_handler(ofstream &outputLFile, string str, int opcode, string iflabel);
void modify(string str, string labelname);
void valuewriter(ofstream &outputLFile, int opcode, int str);
int string_to_int(string str);
int hexCharToInt(char c);
int hexStringToInt(string hexString);
bool acceptlabel(string str);

void run(string filename)
{
    ifstream inputFile(filename + ".asm");

    ofstream outputLFile(filename + ".l");
    ofstream outputOFile(filename + ".o");

    ofstream outputLogFile(filename + ".log");

    opCodeInit();
    string line;
    while (getline(inputFile, line))
    {
        lines.push_back(line);
    }

    for (int i = 0; i < lines.size(); i++)
    {
        lines[i] = labelspace(lines[i]);
    }

    // comment removal
    commentRemover();
    emptyCleaner();

    // debug(line_part);
    firstPassAddress(inputFile, outputLFile);
    secondpass(outputLFile);
    inputFile.close();
    prntmultimap(labelAddr);

    inputFile.open(filename + ".asm");

    // secondPassConvert(inputFile, outputLFile);

    for (auto val : error)
    {
        outputLogFile<< val << endl;
    }

    outputLFile.close();
    outputOFile.close();
    outputLogFile.close();
}

int main()
{
    string filename = "test";
    vector<string> filenames = {"1", "2", "3", "4", "5", "7", "8"};
    // vector<string> filenames = {"8"};
    for (string it : filenames)
    {
        counter = 0;
        pc = "00000000";
        error.clear();
        warning.clear();
        lines.clear();
        line_part.clear();
        labelAddr.clear();
        opCodes.clear();
        run(filename + it);
    }
    return 0;
}

string ltrim(const string &str)
{
    int start = str.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : str.substr(start);
}

string rtrim(const string &str)
{
    int end = str.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : str.substr(0, end + 1);
}

string trim(const string &s)
{
    return rtrim(ltrim(s));
}

void firstPassAddress(ifstream &inputFile, ofstream &outputLFile)
{
    // get back the whole processed data
    string fl = "";
    vector<string> lines;
    counter = 0;
    for (auto it : line_part)
    {
        string temp = "";
        for (auto it1 : it)
        {
            fl += it1 + " ";
            temp += it1 + " ";
        }
        lines.push_back(temp);
        fl += "\n";
    }
    bool previousonlylabel = false;
    for (int i = 0; i < lines.size(); i++)
    {
        addressdecider(lines[i], previousonlylabel);
    }
}

void pcincrement(int cnt, string &pc)
{
    counter = cnt;
    pc = int_to_hex(counter);
    padding(pc, 8, counter);
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

void commentRemover()
{
    for (int i = 0; i < lines.size(); i++)
    {
        int pos = lines[i].find(';');
        if (pos != -1)
        {
            lines[i].erase(lines[i].begin() + pos, lines[i].end());
        }
    }
    // parting it all
    for (auto val : lines)
    {
        vector<string> temp;
        string temp1;
        for (int i = 0; i < val.length(); i++)
        {
            if (val[i] == ' ' || val[i] == '\t')
            {
                if (temp1 != "")
                {
                    temp.push_back(temp1);
                    temp1 = "";
                }
            }
            else
            {
                temp1 += val[i];
            }
        }
        if (temp1 != "")
        {
            temp.push_back(temp1);
        }
        line_part.push_back(temp);
    }
    for (int i = 0; i < line_part.size(); i++)
    {
        for (int j = 0; j < line_part[i].size(); j++)
        {
            int pos = line_part[i][j].find(';');
            if (pos != -1)
            {
                line_part[i].erase(line_part[i].begin() + pos, line_part[i].end());
            }
        }
    }
}

void debug(vector<vector<string>> f)
{
    for (int i = 0; i < f.size(); i++)
    {
        for (int j = 0; j < f[i].size(); j++)
        {
            cout << f[i][j] << " ";
        }
        cout << endl;
    }
}

void emptyCleaner()
{

    for (int i = 0; i < line_part.size(); i++)
    {
        for (int j = 0; j < line_part[i].size(); j++)
        {
            if (line_part[i][j] == "")
            {
                line_part[i].erase(line_part[i].begin() + j);
                j--;
            }
        }
    }
    for (int i = 0; i < line_part.size(); i++)
    {
        if (line_part[i].size() == 0)
        {
            line_part.erase(line_part.begin() + i);
            i--;
        }
    }
}

int labelfind(string s)
{
    for (auto itx : labelAddr)
    {
        if (itx.second == s)
        {
            return itx.first;
        }
    }
    return -1;
}

bool isonlylabel(vector<string> s)
{
    bool flagger = false;
    if (s.size() == 1)
    {
        if (s[0].find(":") != -1)
            return true;
    }
    else
        return false;
    // for (auto itx : s)
    // {
    //     if (itx.find(":") != -1)
    //     {
    //         flagger = flagger or true;
    //     }
    // }
    // return flagger;
}

void secondpass(ofstream &outputLFile)
{
    // get back the whole processed data
    string fl = "";
    vector<string> lines;
    for (auto it : line_part)
    {
        string temp = "";
        for (auto it1 : it)
        {
            fl += it1 + " ";
            temp += it1 + " ";
        }
        lines.push_back(temp);
        fl += "\n";
    }
    // cout<<fl<<endl;
    counter = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        lines[i] = labelspace(lines[i]);
        decide(outputLFile, lines[i]);
    }

    // cout << fl << endl;
}

int numargs(int opcode)
{
    if (opcode == 0 or opcode == 1 or opcode == 2 or opcode == 3 or opcode == 4 or opcode == 5 or opcode == 10 or opcode == 13 or opcode == 15 or opcode == 16 or opcode == 17 or opcode == 19 or opcode == 20)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void prntmultimap(multimap<int, string> m)
{
    for (auto itx : m)
    {
        cout << itx.first << " " << itx.second << endl;
    }
}

bool isnum(string s)
{
    bool flag = true;
    // hexnumber
    if (isHex(s))
    {
        return flag;
    }
    if (s[0] == '-' or s[0] == '+')
    {
        s.erase(s.begin());
    }
    for (auto itx : s)
    {
        if (!isdigit(itx))
        {
            flag = false;
            break;
        }
    }
    if (!flag)
        raiseError(4, s);

    return flag;
}

void decide(ofstream &outputLFile, string str)
{
    stringstream ss(str);
    string word;
    vector<string> words;
    while (ss >> word)
        words.push_back(word);

    // label
    if (islabel(words[0]) and words.size() == 1)
    {
        // no pc increment passon
        outputLFile << pc << endl;
    }
    else
    {
        outputLFile << pc << " ";
        counter++;
        pcincrement(counter, pc);
        // max label + op + arg
        if (words.size() <= 3)
        {
            if (words.size() == 1)
            {
                // either label or a opcode of 0 arg
                // label is done
                int opcode = opCodes[words[0]];
                if (opcode == -1)
                {
                    // error
                    raiseError(9, words[0]);
                    outputLFile << endl;
                }
                else
                {
                    if (numargs(opcode) == 0)
                    {
                        // outputLFile << opcode << endl;
                        valuewriter(outputLFile, opcode, 0);
                    }
                    else
                    {
                        raiseError(8, words[0] + " counter-->  " + to_string(counter));
                        outputLFile << endl;
                    }
                }
            }
            else if (words.size() == 2)
            {
                // label + op0 or op1
                if (islabel(words[0]))
                {
                    int opcode = opCodes[words[1]];
                    if (opcode == -1)
                    {
                        // error
                        raiseError(9, words[1]);
                        outputLFile << endl;
                    }
                    else
                    {
                        if (numargs(opcode) == 0)
                        {
                            // outputLFile << opcode << endl;
                            valuewriter(outputLFile, opcode, 0);
                        }
                        else
                        {
                            raiseError(8, words[1] + " counter-->  " + to_string(counter));
                            outputLFile << endl;
                        }
                    }
                }
                else
                {
                    int opcode = opCodes[words[0]];
                    if (opcode == -1)
                    {
                        // error
                        raiseError(9, words[0]);
                        outputLFile << endl;
                    }
                    else
                    {
                        if (numargs(opcode) == 1)
                        {
                            // branching possible
                            if (opcode == 15 or opcode == 16 or opcode == 17)
                            {
                                branchmech(outputLFile, words[1], opcode);
                            }
                            else
                            {
                                // outputLFile << opcode << " " << words[1] << endl;
                                opcode1_handler(outputLFile, words[1], opcode, words[1]);
                            }
                        }
                        else
                        {
                            raiseError(8, words[0] + " counter-->  " + to_string(counter));
                            outputLFile << endl;
                        }
                    }
                }
            }
            else
            {
                // label + op1
                if (islabel(words[0]))
                {
                    int opcode = opCodes[words[1]];
                    if (opcode == -1)
                    {
                        // error
                        raiseError(9, words[1]);
                        outputLFile << endl;
                    }
                    else
                    {
                        if (numargs(opcode) == 1)
                        {
                            // branching present
                            if (opcode == 15 or opcode == 16 or opcode == 17)
                                branchmech(outputLFile, words[2], opcode);
                            else
                            {
                                opcode1_handler(outputLFile, words[2], opcode, words[0]);
                                string labelname = words[0];
                            }
                        }
                        else
                        {
                            raiseError(8, words[1] + " counter-->  " + to_string(counter));
                            outputLFile << endl;
                        }
                    }
                }
                else
                {
                    raiseError(3, words[0] + " counter-->  " + to_string(counter));
                    outputLFile << endl;
                }
            }
        }
        else
        {
            raiseError(8, " counter-->  " + to_string(counter));
            outputLFile << endl;
        }
    }
}

string labelspace(string s)
{
    int pos = s.find(':');
    if (pos != -1)
    {
        if (s[pos + 1] != ' ')
        {
            s = s.substr(0, pos + 1) + ' ' + s.substr(pos + 1);
        }
        if (s[pos - 1] == ' ')
        {
            s = s.substr(0, pos - 1) + ": " + s.substr(pos + 1);
        }
    }
    return s;
}

bool islabel(string str)
{
    bool flag = false;
    if (str.find(':') != -1)
        flag = true;
    if (flag)
    {
        str.pop_back();
        if (acceptlabel(str))
            return flag;
        else
        {
            raiseError(3, str);
            return false;
        }
    }
    else
        return flag;
}

void branchmech(ofstream &outputLFile, string str, int opcode)
{
    int labeladdr = labelfind(str);
    if (labeladdr != -1)
    {
        int offset = labeladdr - counter;
        // outputLFile << opcode << " " << offset << endl;
        if (offset == -1)
            raiseError(7, "INF LOOP");
        valuewriter(outputLFile, opcode, offset);
    }
    else
    {
        // outputLFile << opcode << " " << 0 << endl;
        raiseError(5, str);
        valuewriter(outputLFile, opcode, 0);
    }
}

void opcode1_handler(ofstream &outputLFile, string str, int opcode, string iflabel)
{
    if (opcode == 20 or opcode == 0 or opcode == 1 or opcode == 10 or opcode == 19)
    {
        // only number value
        if (isnum(str))
        {
            // outputLFile << opcode << " " << str << endl;
            if (opcode == 20)
                modify(str, iflabel);

            valuewriter(outputLFile, opcode, string_to_int(str));
        }
        else if ((opcode == 0 or opcode == 1) and (isnum(str) or labelfind(iflabel) != -1))
        {
            // outputLFile << opcode << " " << labelfind(iflabel) << endl;
            valuewriter(outputLFile, opcode, labelfind(iflabel));
        }
        else
        {
            raiseError(8, str);
            outputLFile << endl;
        }
    }
    else
    {
        //  label
        int labeladdr = labelfind(str);
        // offset ones
        if (opcode == 2 or opcode == 3 or opcode == 4 or opcode == 5 or opcode == 13)
        {

            if (labeladdr != -1)
            {
                // outputLFile << opcode << " " << labeladdr << endl;
                valuewriter(outputLFile, opcode, labeladdr);
            }
            else
            {
                if (isnum(str))
                {
                    // outputLFile << opcode << " " << str << endl;
                    valuewriter(outputLFile, opcode, string_to_int(str));
                }
                else
                {
                    raiseError(8, str);
                    outputLFile << endl;
                }
            }
        }
    }
}

void addressdecider(string str, bool &pol)
{
    stringstream ss(str);
    string word;
    vector<string> words;
    while (ss >> word)
        words.push_back(word);

    if (pol)
    {
        // now decide the future
        if (words.size() == 1)
        {
            if (islabel(words[0]))
            {
                labelAddr.insert(pair<int, string>(counter, words[0].substr(0, words[0].size() - 1)));
                if (labelfind(words[0].substr(0, words[0].size() - 1)) != -1)
                    raiseError(6, words[0].substr(0, words[0].size() - 1));
                pol = true;
            }
            else
            {
                pol = false;
            }
        }
        else
        {
            if (islabel(words[0]))
            {
                labelAddr.insert(pair<int, string>(counter, words[0].substr(0, words[0].size() - 1)));
                if (labelfind(words[0].substr(0, words[0].size() - 1)) != -1)
                    raiseError(6, words[0].substr(0, words[0].size() - 1));
                pol = false;
            }
            else
            {
                pol = false;
            }
        }
    }
    else
    {
        // decide future
        if (words.size() == 1)
        {
            if (islabel(words[0]))
            {
                labelAddr.insert(pair<int, string>(counter, words[0].substr(0, words[0].size() - 1)));
                if (labelfind(words[0].substr(0, words[0].size() - 1)) != -1)
                    raiseError(6, words[0].substr(0, words[0].size() - 1));
                pol = true;
            }
            else
            {
                pol = false;
            }
        }
        else
        {
            if (islabel(words[0]))
            {
                labelAddr.insert(pair<int, string>(counter, words[0].substr(0, words[0].size() - 1)));
                if (labelfind(words[0].substr(0, words[0].size() - 1)) != -1)
                    raiseError(6, words[0].substr(0, words[0].size() - 1));
                pol = false;
            }
            else
            {
                pol = false;
            }
        }
        counter++;
    }
}

//! set operation
void modify(string str, string labelname)
{
    int address = stoi(str);
    auto itr = labelAddr.find(labelfind(labelname.substr(0, labelname.size() - 1)));
    if (itr != labelAddr.end())
        labelAddr.erase(itr);
    labelAddr.insert(pair<int, string>(address, labelname.substr(0, labelname.size() - 1)));
}

void valuewriter(ofstream &outputLFile, int opcode, int str)
{
    string op = int_to_hex(opcode);
    padding(op, 2, opcode);
    string val = int_to_hex(str);
    padding(val, 6, str);
    outputLFile << val << op << endl;
}

int string_to_int(string str)
{
    int num = 0;
    int t = 1;
    if (str[0] == '-')
    {
        t = -1;
        str = str.substr(1);
    }
    if (str[0] == '+')
        str = str.substr(1);
    // hex support
    if (isHex(str))
    {
        const char *c = str.c_str();
        num = (int)strtol(c, NULL, 0);
        // cout << "--------" << num << endl;
        return num;
    }
    for (int i = 0; i < str.size(); i++)
    {
        num = num * 10 + (str[i] - '0');
    }
    return num * t;
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

bool acceptlabel(string str)
{
    if (isdigit(str[0]))
        return false;
    else
    {
        for (int i = 0; i < str.size(); i++)
        {
            if (!isalnum(str[i]))
                return false;
        }
        return true;
    }
}

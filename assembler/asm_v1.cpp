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
        5- No such label label
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
        temp = "inappropiante arguments given\n";
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

void firstPassAddress(ifstream &inputFile, ofstream &outputLFile);
void secondpass(ofstream &outputLFile);
void decide(ofstream &outputLFile, string str);
void branchmech(ofstream &outputLFile, string str, int opcode);
void opcode1_handler(ofstream &outputLFile, string str, int opcode);

int main()
{
    string filename = "test1";

    ifstream inputFile(filename + ".asm");

    ofstream outputLFile(filename + ".l");

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
        outputLogFile << "ERROR: " << val << endl;
    }

    for (auto val : warning)
    {
        outputLogFile << "WARNING: " << val << endl;
    }

    outputLFile.close();
    outputLogFile.close();
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
    if (!inputFile.is_open())
    {
        cout << "Error opening the file" << endl;
    }
    string instruction;
    int counter = 0;
    for (int i = 0; i < line_part.size(); i++)
    {
        if (line_part[i].size() == 1)
        {
            if (opCodes.find(line_part[i][0]) != opCodes.end())
            {
                if (opCodes[line_part[i][0]] == 19)
                {
                    raiseError(4, line_part[i][0]);
                }
                else if (opCodes[line_part[i][0]] == 20)
                {
                    raiseError(4, line_part[i][0]);
                }
                else
                {
                    counter++;
                }
            }
            else if (line_part[i][0].find(":") != -1)
            {
                string temp = line_part[i][0].substr(0, line_part[i][0].find(":"));
                // temp.pop_back();
                if (labelfind(temp) != -1)
                {
                    raiseError(6, temp);
                }
                else if (isdigit(temp[0]))
                {
                    raiseError(3, temp);
                }
                else
                {
                    labelAddr.insert(pair<int, string>(counter, temp));
                }
            }
            else
            {
                raiseError(2, line_part[i][0]);
            }
        }
        else if (line_part[i].size() == 2)
        {
            if (opCodes.find(line_part[i][0]) != opCodes.end())
            {
                if (opCodes[line_part[i][0]] == 19)
                {
                    raiseError(4, line_part[i][0]);
                }
                else if (opCodes[line_part[i][0]] == 20)
                {
                    raiseError(4, line_part[i][0]);
                }
                else
                {
                    counter++;
                }
            }
            else
            {
                raiseError(1, line_part[i][0]);
            }
        }
        // this is label + op + arg
        else if (line_part[i].size() == 3)
        {
            if (line_part[i][0].find(":") != -1)
            {
                string temp = line_part[i][0];
                temp.pop_back();
                if (labelfind(temp) != -1)
                {
                    raiseError(6, temp);
                }
                else
                {
                    if (line_part[i][1] == "SET")
                    {
                        int ct = 0;
                        stringstream ss(line_part[i][2]);
                        ss >> ct;
                        labelAddr.insert(pair<int, string>(ct, temp));
                        counter++;
                    }
                    else
                    {
                        labelAddr.insert(pair<int, string>(counter, temp));
                    }
                }
            }
            else
            {
                raiseError(3, line_part[i][0]);
            }
        }
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
    if (opcode == 0 or opcode == 1 or opcode == 2 or opcode == 3 or opcode == 4 or opcode == 5 or opcode == 10 or opcode == 13 or opcode == 15 or opcode == 16 or opcode == 17 or opcode == 19)
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

bool ishex(string str)
{
    bool flag = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (!isxdigit(str[i]))
        {
            flag = 1;
            break;
        }
    }
    return flag;
}

bool isnum(string s)
{
    bool flag = true;
    for (auto itx : s)
    {
        if (!isdigit(itx))
        {
            flag = false;
            break;
        }
    }
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
                    outputLFile << endl;
                }
                else
                {
                    if (numargs(opcode) == 0)
                        outputLFile << opcode << endl;
                    else
                        outputLFile << endl;
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
                        outputLFile << endl;
                    }
                    else
                    {
                        if (numargs(opcode) == 0)
                            outputLFile << opcode << endl;
                        else
                            outputLFile << endl;
                    }
                }
                else
                {
                    int opcode = opCodes[words[0]];
                    if (opcode == -1)
                    {
                        // error
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
                                opcode1_handler(outputLFile, words[1], opcode);
                            }
                        }
                        else
                            outputLFile << endl;
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
                                opcode1_handler(outputLFile, words[2], opcode);
                        }
                        else
                            outputLFile << endl;
                    }
                }
                else
                {
                    outputLFile << endl;
                }
            }
        }
        else
        {
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
    if (str.find(':') != -1)
        return true;
    return false;
}

void branchmech(ofstream &outputLFile, string str, int opcode)
{
    int labeladdr = labelfind(str);
    if (labeladdr != -1)
    {
        int offset = labeladdr - counter;
        outputLFile << opcode << " " << offset << endl;
    }
    else
    {
        outputLFile << opcode << " " << 0 << endl;
    }
}

void opcode1_handler(ofstream &outputLFile, string str, int opcode)
{
    if (opcode == 20 or opcode == 0 or opcode == 1 or opcode == 10 or opcode == 19)
    {
        // only number value
        if (isnum(str))
        {
            outputLFile << opcode << " " << str << endl;
        }
        else
        {
            outputLFile << endl;
        }
    }
    else
    {
        // label
        int labeladdr = labelfind(str);
        if (labeladdr != -1)
        {
            outputLFile << opcode << " " << labeladdr << endl;
        }
        else
        {
            outputLFile << endl;
        }
    }
}
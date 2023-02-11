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
}
void padding(string &s, int n);
void pcincrement(int cnt);
string int_to_hex(int cnt);
void commentRemover();
void emptyCleaner();

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

void firstPassAddress(ifstream &inputFile, ofstream &outputLFile);

int main()
{
    string filename = "test2";

    ifstream inputFile(filename + ".asm");

    ofstream outputLFile(filename + ".l");

    ofstream outputLogFile(filename + ".log");

    opCodeInit();
    string line;
    while (getline(inputFile, line))
    {
        lines.push_back(line);
    }

    // comment removal
    commentRemover();
    emptyCleaner();
    debug(line_part);
    firstPassAddress(inputFile, outputLFile);

    inputFile.close();

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
            else if (line_part[i][0].find(":"))
            {
                string temp = line_part[i][0];
                temp.pop_back();
                if (labelfind(temp) != -1)
                {
                    raiseError(6, temp);
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
        else if (line_part[i].size() == 3)
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
                raiseError(0, line_part[i][0]);
            }
        }
        else if (line_part[i].size() == 0)
        {
            continue;
        }
        else
        {
            raiseError(0, line_part[i][0]);
        }
    }
}

void pcincrement(int cnt, string &pc)
{
    counter = cnt;
    pc = int_to_hex(counter);
    padding(pc, 8);
}

void padding(string &s, int n)
{
    string str = "";
    for (int i = 0; i < n; i++)
    {
        str = "0" + str;
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
    for (auto itx : s)
    {
        if (itx.find(":") != -1)
        {
            flagger = flagger or true;
        }
    }
    return flagger;
}

void secondpass(ofstream &outputLFile)
{
    counter = 0;
    for (int lineit = 0; lineit < line_part.size(); lineit++)
    {
        // if only label there then passon
        if (isonlylabel(line_part[lineit]))
        {
            outputLFile << pc << endl;
            continue;
        }
        // now that is not only label so pc will move on
        outputLFile << pc;
        counter++;
        pcincrement(counter, pc);
        // now we will check for the opcodes
        if (line_part[lineit].size() == 1)
        {
            // it must be an opcode of 0 argument
            if (opCodes.find(line_part[lineit][0]) == opCodes.end())
            {
                raiseError(2, line_part[lineit][0]);
            }
            else
            {
                // now check if the opcode is of 0 argument
                int opcode = opCodes[line_part[lineit][0]];
                if (numargs(opcode) == 0)
                {
                    outputLFile << " " << opcode << endl;
                    continue;
                }
                else
                {
                    raiseError(2, line_part[lineit][0] + "it should have argumnets\n");
                }
            }
        }
        else
        {
            // it must be an opcode of 1 argument
            if (opCodes.find(line_part[lineit][0]) == opCodes.end())
            {
                raiseError(2, line_part[lineit][0]);
            }
            else
            {
                // now check if the opcode is of 1 argument
                int opcode = opCodes[line_part[lineit][0]];
                if (numargs(opcode) == 1)
                {
                    outputLFile << " " << opcode;
                    // now check if the argument is a label or a number
                    if (line_part[lineit][1].find(":") != -1)
                    {
                        // it is a label
                        int labeladdr = labelfind(line_part[lineit][1]);
                        if (labeladdr == -1)
                        {
                            raiseError(3, line_part[lineit][1]);
                        }
                        else
                        {
                            outputLFile << " " << labeladdr << endl;
                            continue;
                        }
                    }
                    else
                    {
                        // it is a number
                        int num = stoi(line_part[lineit][1]);
                        if (num < 0 or num > 255)
                        {
                            raiseError(1, line_part[lineit][1]);
                        }
                        else
                        {
                            outputLFile << " " << num << endl;
                            continue;
                        }
                    }
                }
                else
                {
                    raiseError(2, line_part[lineit][0] + "it should not have argumnets\n");
                }
            }
        }
    }
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

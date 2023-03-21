/*****************************************************************************
TITLE: Assembler																																
AUTHOR: Atharva Deshmukh
ROLL NUMBER: 2101CS14
Declaration of Authorship
This cpp file, asm.cpp, is part of the assignment of CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/


#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;

int problem_counter = 0, i = 0;

// All data structure used
vector<vector<string>> ParsedLines;
vector<pair<string,int>> Errors;
map<int,string> CheckLabel;
unordered_set<string> labelSet;
unordered_map<string, int> labelTable;
map<string, pair<int, int_fast32_t>> opcodes{// Table for mnemonics in the form {name, {opcode, num of operands}}
                                     {"ldc", {0, 1}},
                                     {"adc", {1, 1}},
                                     {"ldl", {2, 1}},
                                     {"stl", {3, 1}},
                                     {"ldnl", {4, 1}},
                                     {"stnl", {5, 1}},
                                     {"add", {6, 2}},
                                     {"sub", {7, 2}},
                                     {"shl", {8, 2}},
                                     {"shr", {9, 2}},
                                     {"adj", {10, 1}},
                                     {"a2sp", {11, 2}},
                                     {"sp2a", {12, 2}},
                                     {"call", {13, 1}},
                                     {"return", {14, 2}},
                                     {"brz", {15, 1}},
                                     {"brlz", {16, 1}},
                                     {"br", {17, 1}},
                                     {"HALT", {18, 2}},
                                     {"data", {0, 1}},
                                     {"SET", {-1, 1}}};
string outputCode = "";
string binaryCode = "";

// All functions used
void raiseError(int code, string label);
vector<string> SplittingWords(string s);
bool validLabel(string label);
int Converttoint(string n);
string readNumbers(string n);
void ReadingLines(vector<string> line, int index);
string tohex(int n);
bool checkValidNumber(string n);
string readNumberOrLabel(string value, int problem_counter);
string CheckInstruction(vector<string> line, int problem_counter);

void raiseError(int code, string label)
{
    /*
        0- more than 1 arg passed
        1- Unknown mnemonic
        2- bogus label name
        3- Invalid number
        4- Invalid label
        5- Duplicate label
        6 - Infinite Loop
        7 - Operand is Missing
    */
    string temp;
    static int id = 0;
    switch (code)
    {

    case 0:
        temp = "ERROR: Unknown mnemonic \"" + label + "\" found with more than one argument\n";
        break;

    case 1:
        temp = "ERROR: Unknown mnemonic \"" + label + "\" found.\n";
        break;

    case 2:
        temp = "ERROR: Bogus Label Name \"" + label + "\"\n";
        break;

    case 3:
        temp = "ERROR: Invalid Number Entered \"" + label + "\"\n";
        break;

    case 4:
        temp = "ERROR: No such label \"" + label + "\" exists\n";
        CheckLabel[id] = label;
        break;

    case 5:
        temp = "ERROR: Duplicate label \"" + label + "\" found\n";
        break;

    case 6:
        temp = "WARNING: Infinite loop detected\n";
        break;
    
    case 7:
        temp = "Number of arguments in  \"" + label + "\" is wrong. \n";
        break;
    }
    Errors.push_back(make_pair(temp,id));
    id++;
}

vector<string> SplittingWords(string s)
{
    cout<<s<<endl;
    vector<string> words;
    s += '\n';
    string word = "";
    for (char x : s)
    {
        if (x == ';')
        {
            if (word != "")
                words.push_back(word);
            break;
        }

        if (x == '\t' || x == ' ' || x == ':' || x == '\n' || x == ',')
        {
            if (word != "") words.push_back(word);
            word = "";
        }

        if( x == ':' ){
            word += x;
            words.push_back(word);
            word ="";
        }

        if (x != ' ' && x != '\t' && x != ',' && x != ':')
            word += x;
    }
    for( int i = 0 ; i < words.size() ; i++ ){
        cout<<words[i]<<" ";
    }
    cout<<endl;
    return words;
}

bool validLabel(string label)
{
    if (isalpha(label[0]))
    {
        for (int i = 1; i < label.length(); i++)
        {
            if (!iswalnum(label[i]))
                return false;
        }
        return true;
    }
    return false;
}

int Converttoint(string n)
{
    int value;
    value = strtol(n.c_str(), NULL, 0);
    return value;
}

string readNumbers(string n)
{
    ll value;
    value = strtol(n.c_str(), NULL, 0);
    if (value >= 0)
    {
        return tohex(value);
    }
    else
    {
        value = 16777216 + value;
        return tohex(value);
    }
    return "000000";
}

void ReadingLines(vector<string> line, int index)
{
    if (line.size() > 1)
    {
        if (*(line.begin() + 1) == ":")
        {
            vector<string> instructions;
            if (validLabel(line[0]))
            {
                if (labelTable.find(line[0]) == labelTable.end())
                {
                    if (line.size() > 2)
                    {
                        if (line[2] == "SET")
                            labelTable[*(line.begin())] = Converttoint(line[3]);
                        else
                            labelTable[*(line.begin())] = index;
                    }
                    else{                    
                        labelTable[*(line.begin())] = index;
                    }

                    labelSet.insert(line[0]);
                }
                else
                    raiseError(5, line[0]);
            }
            else
            {
                raiseError(2, line[0]);
            }
            for (auto it = line.begin() + 2; it != line.end(); it++)
            {
                instructions.push_back(*it);
            }
            line = instructions;
        }
    }
    
    
    string num = readNumbers(to_string(problem_counter));

    string temp = CheckInstruction(line, problem_counter);
    
    outputCode += "00" + num + " ";

    outputCode += temp + " ";

    binaryCode += "00" + num + " " + temp;

    for (auto s : ParsedLines[i])
    {
        outputCode += s + " ";
    }

    outputCode += "\n";

    binaryCode += "\n";
    
    if (line.size())
        problem_counter++;

    i++;
}

string tohex(int n)
{
    int i = 0;
    string arr = "00000000";
    while (n != 0)
    {
        int temp = 0;
        temp = n % 16;
        if (temp < 10)
        {
            arr[5 - i] = temp + 48;
            i++;
        }
        else
        {
            arr[5 - i] = temp + 55;
            i++;
        }
        n = n / 16;
    }
    return arr;
}

bool checkValidNumber(string n)
{
    transform(n.begin(), n.end(), n.begin(), ::tolower);
    int len = n.length();
    if (len == 0)
        return false;
    if (n[0] == '+' || n[0] == '-')
    {
        if (len < 2)
            return false;
        n = n.substr(1, len - 1);
    }

    if (n[0] == '0')
    {
        if (n[1] == 'x')
        {
            for (int i = 2; i < n.length(); i++)
            {
                if (!((n[i] >= '0' && n[i] <= '9') || (n[i] >= 'a' && n[i] <= 'f')))
                {
                    raiseError(3, n);
                    return false;
                }
            }
            return true;
        }
        else
        {
            for (int i = 0; i < n.length(); i++)
            {
                if (!(n[i] >= '0' && n[i] <= '7'))
                {
                    raiseError(3, n);
                    return false;
                }
            }
            return true;
        }
    }
    for (int i = 0; i < n.length(); i++)
    {
        if (!(n[i] >= '0' && n[i] <= '9'))
        {
            return false;
        }
    }
    return true;
}



string readNumberOrLabel(string value, int problem_counter)
{
    if (checkValidNumber(value))
        return readNumbers(value);
    else
    {
        if (labelTable.find(value) != labelTable.end())
            return readNumbers(to_string(labelTable[value] - problem_counter - 1));
        else
        {
            if (validLabel(value))
                raiseError(4, value);
        }
    }

    return "000000";
}

string CheckInstruction(vector<string> line, int problem_counter)
{
    
    if(line.size() < 1 )
    {
        return "";
    }

    if (line.size() > 2)
    {
        raiseError(0, line[0]);
    }

    string mnemonic = line[0], value, opcode, operand;
    
    if (opcodes.find(mnemonic) != opcodes.end())
    {
        if (opcodes[mnemonic].second == 1)
        {
            if (mnemonic == "SET")
            {
                opcode = "00";

                operand = readNumberOrLabel(to_string(Converttoint(value)), problem_counter);

                return opcode + operand;
            }

            else
            {
                if(line.size() == 1){
                    raiseError(7,mnemonic);
                    return "";
                }
                value = line[1];

                opcode = opcodes[mnemonic].first;
                operand = readNumberOrLabel(value, problem_counter);

                if (mnemonic == "br" && operand == "000000")
                {
                    raiseError(6, value);
                }
                return operand + opcode;
            }
        }

        else if (opcodes[mnemonic].second == 2)
        {
            cout<<"#3"<<endl;
            if(line.size() == 2){
                cout<<"#4"<<endl;
                raiseError(7,mnemonic);
                return "";
            }
            cout<<"#3"<<endl;
            operand = "000000";
            opcode = opcodes[mnemonic].first;
            return operand + opcode;
        }
    }

    
    raiseError(1, line[0]);
    return "";
    
}

// int main()
int main(int argc, char **argv)
{
    // Step 1: Opening the file that we want to read
    // First we check the command that we ran in terminal.

    if (argc != 2)
    {
        cout << "Invalid Command!. Only the file name is to be entered." << endl;
        return 0;
    }
    // Step 1: Opening the file that we want to read
    // First we check the command that we ran in terminal.
    string file_name_command = argv[1]; // String(argv[1]) and argv[1] should give same result. Remove this before submitting.
    // string s = "test4.asm"; //For trial purposees
    // string file_name_command = s;

    ifstream file;
    file.open(file_name_command);

    if (!file.is_open())
    {
        cout << "Error: Could not open file. " << file_name_command << " Please check if file exists, or does it have permission" << endl;
        return 0;
    }

    string file_name;

    for (int i = 0; i < file_name_command.length(); i++) // Extracting FileName for using it to convert output files
    {
        if (file_name_command[i] == '.')
            break;
        file_name += file_name_command[i];
    }
    // Step 2: Reading the file line by line
    // 1st Pass

    string line;
    while (bool(getline(file, line)))
    {
        vector<string> words = SplittingWords(line);

        if (words.size())
        {
            ParsedLines.push_back(words);
        }
    }
    file.close();

    int index = 0;
    
    for (auto line : ParsedLines)
    {
        ReadingLines(line,index);

        if (line.size())
            index++;
    }

    string errorString = "; NO ERROR FOUND\n";
    if(Errors.size() != 0)
    {
        errorString = "";
        int tp = 0;
        for(auto s: Errors){

            if( CheckLabel.find(s.second) != CheckLabel.end() ){
                if(labelSet.find(CheckLabel[s.second]) != labelSet.end()){
                    continue;
                }
            }

            errorString += s.first;
        }
    }

    string Output_File_Name = "", Error_File_Name, Binary_File_Name;

    Error_File_Name = file_name + ".log";
    Binary_File_Name = file_name + ".o";
    file_name += ".l";
    
    ofstream out(file_name);
    out<<outputCode;
    out.close();

    ofstream outError(Error_File_Name);
    outError<<errorString;
    outError.close();

    ofstream outBin(Binary_File_Name);
    outBin<<binaryCode;
    outBin.close();

    cout<<"File was successfully executed"<<endl;
}
/*
********************************************************************************************
TITLE: emu																																
AUTHOR: Archit Sharma
ROLL NO.: 2101AI05
STUDENT ID: archit_2101ai05@iitp.ac.in

#DECLARATION OF AUTHORSHIP
All contents of this file have been solely written by me for CS210 assignment at the Dept.
of Computer Science and Engineering IIT Patna.

#NOTE:
This file is compatible with g++ (V 11.2.0) compiler, file name has to be passed through
console which will be read using argc and argv.
********************************************************************************************
*/
#include <bits/stdc++.h>
using namespace std;
int A = 0, B = 0, PC = 0, SP = 0;

//Change 500000 to desired value if more than 2MB of memory is to be store.
#define MEMORY_CAPACITY 500000
//Change 2000 to desired value if more than 2000 instructions have to be executed.
#define EXECUTION_LIMIT 200000
int memory[MEMORY_CAPACITY] = {0}; //2MB reserved for emulation
typedef long long int ll;
string filename;
string history_log = "";

void writeToFile()
{
    string logFileName = "";
    for(int i = 0; i < filename.length(); i++)
    {
        
        if(filename[i] == '.')
            break;
        else
        
            logFileName += filename[i];
    }
    logFileName += ".trace";
    ofstream outBin(logFileName);
    outBin<<history_log;
    outBin.close();
    cout<<"4/4 trace FILE GENERATED"<<endl;
    exit(0);
}

int convertToNumber(string n)
{
    int msb = 0;
    int num = -8388608;
    if((n[0] >= '8' && n[0] <= '9') || (n[0] >= 'A' && n[0] <= 'F'))
    {
        msb = 1;
    }
    int rawNumber = (int)strtol(n.c_str(), NULL, 16);
    if(msb) rawNumber += num;
    
    return (num * msb + rawNumber);
}

string tohex(ll n)
{
    int i = 0;
    string arr = "00000000";
    if(n < 0)
    {
        long long int val;
        val = 4294967296 + n;
        return tohex(val);
    }
    while(n != 0)
    {
        int temp = 0;
        temp = n % 16;
        if(temp < 10)
        {
            arr[7-i] = temp + 48;
            i++;
        }
        else
        {
            arr[7-i] = temp + 55;
            i++;
        }
        n = n / 16;
   }
   return arr;
}

vector<string> getWords(string s)
{
    vector<string> words;
    s += '\n';
    string word = "";
    for(char x: s)
    {
        if(x == ' '|| x == '\n')
        {
            if(word != "") words.push_back(word);
            word = "";
        }

        if(x != ' ')
            word += x;
    }
    return words;
}

void execute(pair<string, string> code)
{
    string operand = code.first;
    string opcode = code.second;

    if(opcode == "00") //ldc
    {
        B = A;
        A = convertToNumber(operand);
    }
    else if(opcode == "01") //adc
    {
        A = A + convertToNumber(operand);
    }
    else if(opcode == "02") //ldl
    {
        B = A;
        int loc = SP + convertToNumber(operand);
        if((loc > MEMORY_CAPACITY) || (loc < 0))
        {
            cout<<"ILLEGAL MEMORY ACCESS!"<<endl;
            exit(0);
        }
        A = memory[loc];
    }
    else if(opcode == "03") //stl
    {
        int loc = SP + convertToNumber(operand);
        if((loc > MEMORY_CAPACITY) || (loc < 0))
        {
            cout<<"ILLEGAL MEMORY ACCESS!"<<endl;
            exit(0);
        }
        memory[loc] = A;
        A = B;
    }
    else if(opcode == "04") //ldnl
    {
        int loc = A + convertToNumber(operand);
        if((loc > MEMORY_CAPACITY) || (loc < 0))
        {
            cout<<"ILLEGAL MEMORY ACCESS!"<<endl;
            exit(0);
        }
        A = memory[loc];
    }
    else if(opcode == "05") //stnl
    {
        int loc = A + convertToNumber(operand);
        if((loc > MEMORY_CAPACITY) || (loc < 0))
        {
            cout<<"ILLEGAL MEMORY ACCESS!"<<endl;
            exit(0);
        }
        memory[A] = B;
    }
    else if(opcode == "06") //add
    {
        A = A + B;
    }
    else if(opcode == "07") //sub
    {
        A = B - A;
    }
    else if(opcode == "08") //shl
    {
        A = (B<<A);
    }
    else if(opcode == "09") //shr
    {
        A = (B>>A);
    }
    else if(opcode == "0a") //adj
    {
        SP = SP + convertToNumber(operand);
    }
    else if(opcode == "0b") //a2sp
    {
        SP = A;
        A = B;
    }
    else if(opcode == "0c") //sp2a
    {
        B = A;
        A = SP;
    }
    else if(opcode == "0d") //call
    {
        B = A;
        A = PC;
        PC = PC + convertToNumber(operand);
    }
    else if(opcode == "0e") //return
    {
        PC = A;
        A = B;
    }
    else if(opcode == "0f") //brz
    {
        if(A == 0)
            PC = PC + convertToNumber(operand);
    }
    else if(opcode == "10") //brlz
    {
        if(A < 0)
            PC = PC + convertToNumber(operand);
    }
    else if(opcode == "11") //br
    {
        PC = PC + convertToNumber(operand);
    }
    else if(opcode == "12") //Halt
    {
        cout<<"HALT ENCOUNTERED: EMULATION STOPPED"<<endl;
        writeToFile();
    }
    else //data
    {
        int loc = SP;
        if((loc > MEMORY_CAPACITY) || (loc < 0))
        {
            cout<<"ILLEGAL MEMORY ACCESS!"<<endl;
            exit(0);
        }
        memory[loc] = convertToNumber(operand);
    }
}

// void test()
// {
//     cout<<tohex(-5);
//     exit(0);
// }
int main(int argc, char** argv)
{
    //test();
    bool trace = false;
    if(argc == 2 || argc == 3)
    {
        filename =  argv[1];
        if(argc == 3)
        {
            string arg = argv[2];
            if(arg == "-trace")
                trace = true;
            else
            {
                cout<<"EMULATION FAILED: Only -trace is valid 2nd argument. \""<<arg <<"\" Passed (check claims.txt / makefile.txt)"<<endl;
                exit(1);
            }
        }
    }
    else
    {
        cout<<"EMULATION FAILED: Invalid number of arguments passed ("<<argc - 1<<") Only the file name is to be entered with or without -trace (check claims.txt)"<<endl;
        exit(1);
    }
    ifstream in(filename);
    if(!in.is_open())
    {
        cout<<"EMULATOR FAILED: No file named \""<<filename<<"\" exists."<<endl;
        exit(1);
    }
    cout<<"1/4 FILE OPENED SUCCESSFULLY"<<endl;
    vector<vector <string>> parsedLines;
    while(in.eof() == 0)
    {
        string code, comment;
        getline(in, code);
        vector<string> words = getWords(code);
        if(words.size()) parsedLines.push_back(words);
    }
    cout<<"2/4 FILE PARSED SUCCESSFULLY"<<endl;

    vector<pair<string, string>> instructions;
    for(auto v: parsedLines)
    {
        if(v.size() > 1)
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

    cout<<"3/4 OPCODE AND OPERAND SPLITTED"<<endl;
    int pointer = 0;
    

    int limit = EXECUTION_LIMIT;
    int prevPC = -1;
    while(limit--)
    {
        if(PC > instructions.size() - 1)
        {
            cout<<PC<<" "<<instructions.size()-1<<endl;
            cout<<"FLAG: PC exceeds the instruction set. Termination Stopped / Completed"<<endl;
            break;
        }
        if(trace)cout<<instructions[PC].first<<instructions[PC].second<<endl<<" ";
        execute(instructions[PC]);
        string temp = "A: " + tohex(A) + "\t" + "B: " + tohex(B) + "\t"
                        "PC: " + tohex(PC) + "\t" + "SP: " + tohex(SP) + "\n";
        if(trace) cout<<temp;
        history_log += temp;
        if(prevPC == PC)
        {
            cout<<"FLAG: INFINITE LOOP DETECTED\n";
            history_log += "INFINITE LOOP DETECTED\n";
            //cout<<prevPC<<" "<<PC;
            break;
        }
        prevPC = PC;
        PC++;
    }
    
    if(limit <= 0)
    {
        cout<<"\nFLAG: EXECUTION LIMIT CROSSED. (>"<<EXECUTION_LIMIT<<" Executions, to execute more instructions, change \"EXECUTION_LIMIT\" in source code)\n";
        history_log += "\nEXECUTION LIMIT CROSSED. (>" +
        to_string(EXECUTION_LIMIT) + " Executions, to execute more instructions, change \"EXECUTION_LIMIT\" in source code)\n";
    }

    string logFileName = "";
    for(int i = 0; i < filename.length(); i++)
    {
        
        if(filename[i] == '.')
            break;
        else
            logFileName += filename[i];
    }
    logFileName += ".trace";
    ofstream outBin(logFileName);
    outBin<<history_log;
    outBin.close();
    cout<<"4/4 test FILE GENERATED"<<endl;
    return 0;
}
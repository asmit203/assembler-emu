#include <bits/stdc++.h>

using namespace std;

map<string, int> opCodes;
map<string, int> labelAddr;
vector<string> instructions;
vector<string> warning;
vector<string> error;
string err;
string warn;
bool errFlag;

const string WHITESPACE = " \n\r\t\f\v";







template< typename T >
string int_to_hex( T i, int size )
{
  stringstream stream;
  stream << setfill ('0') << setw(size) 
         << hex << i;
    // cut off the msb 2 bits
  string ans = stream.str();
  return ans.substr(ans.length() - size, size);
}

bool isNumber(string value) {
    for(auto i : value) {
        if(!(isdigit(i) || i == '+' || i == '-' || i == 'x')) return false;
    }
    return true;
}




 
string ltrim(const string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}
 
string rtrim(const string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}
 
string trim(const string &s) {
    return rtrim(ltrim(s));
}


void tokenize(string const &str, vector<string> &out) {
    istringstream ss(str);
    string word;
    while(getline(ss, word , ' ')) {
        out.push_back(word);
    }
}

void tokenizeLabel(string const &str, vector<string> &out) {
    
    int index = str.find(':');
    out.push_back(str.substr(0, index));
    istringstream ss(ltrim(str.substr(index+1)));
    string word;
    while(getline(ss, word, ' ')) {
        out.push_back(word);
    }
}

string toString(vector<string> list, int start, int end) {
    string value;
    for(int i = start; i < end; i++) {
        value += list[i] + " ";
    }
    return value;
}

int searchComment(vector<string> list, int start) {
    
    for(int i = start; i < list.size(); i++) {
        int j = list[i].find(";");
        if(j != string::npos) {
            return i;
        }
    }
    return list.size();
}

// add end also as there might be comments in line so that increases the size

bool validateInstruction(vector<string> token, int start, int counter) {
    string opCode = token[start];
    string instr;
    string value;
    int end = searchComment(token, start);
    // vector<string>::iterator commentLocation = find(token.begin() + start, token.end(), ";");
    // int end = commentLocation - token.begin();
    
    // int end = commentIndex - start;
    switch(opCodes[opCode]) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 10:
        case 13:
        case 15:
        case 16:
        case 17:
        case 19:
        case 20:
            if(end - start < 2) {
                error.push_back("missing operands provided at line " + to_string(counter));
                return false;
            }
            else if(end - start > 2) {
                error.push_back("extra on end of line " + to_string(counter));
                return false;
            }
            else {
                value = token[start+1];
                if(!isNumber(value)) {
                    if(labelAddr.count(value)) {
                        int offset;
                        if(opCodes[opCode] == 0 || opCodes[opCode] == 1 || opCodes[opCode] == 10 || opCodes[opCode] == 19 || opCodes[opCode] == 20)
                            offset = labelAddr[value];
                        else
                            offset = labelAddr[value] - counter - 1;
                        instr += int_to_hex(offset, 6);
                    }
                    else {
                        error.push_back("Incorrect argument " + value + " provided at line " + to_string(counter));
                        return false;
                    }
                }
                else {
                    if(value.find("0x") == 0) {
                        instr += value.substr(2, 6);
                    }
                    else if(value.find("0") == 0) {
                        // char* end;
                        if(value.substr(1).empty()) {
                            instr += int_to_hex(0, 6);
                        }
                        else {
                            // instr += int_to_hex(strtoul(value.substr(1).c_str(), &end, 8), 6);
                            instr += int_to_hex(stoi(value.substr(1), 0, 8), 6);
                        }
                    }
                    else {
                        instr += int_to_hex(stoi(value), 6);
                    }
                }
            }
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 14:
        case 18:
            instr += int_to_hex(0, 6);
            if(end - start > 0) {
                warning.push_back("unexpected operand at line " + to_string(counter));
            }
            break;

        default:
            err = "invalid opCode found";
            return false;
    }
    instr += int_to_hex(opCodes[opCode], 2);
    instructions.push_back(instr);
    return true;
}


void firstPassAddress(ifstream &inputFile, ofstream &outputLFile) {
    if(!inputFile.is_open()) {
        cout << "Error opening the file" << endl;
    }
    string instruction;
    int counter = 0;
    while (getline(inputFile, instruction)) {
        instruction = trim(instruction);
        regex r("\\s+");
        instruction = regex_replace(instruction, r, " ");
        string instr = instruction.substr(0, instruction.find(" "));
        // cout << counter << " " << instruction << endl;
        // outputLFile << int_to_hex(counter, sizeof(int) * 2) << " " << instruction << endl;
        if(instr == ";") {
            continue;
        }
        if(opCodes.count(instr) == 0) {
            string label = instruction.substr(0, instruction.find(":"));
            if(labelAddr.count(label) == 0)
                labelAddr[label] = counter;
            else
                warning.push_back("Duplicate label found at line " + to_string(counter));
            vector<string> labelToken;
            tokenize(instruction, labelToken);
            if(labelToken.size() > 1 && labelToken[1] != ";") {
                counter++;
                // instructions.push_back(toString(labelToken, 1, labelToken.size()));
            }
            
            continue;
        }
        else {
            // instructions.push_back(instruction);
        }
        counter++;
    }
}

void secondPassConvert(ifstream &inputFile, ofstream &outputLFile) {
    if(!inputFile.is_open()) {
        cout << "Error opening the file" << endl;
    }
    string instruction;
    int counter = 0;
    while(getline(inputFile, instruction)) {
        instruction = trim(instruction);
        regex r("\\s+");
        instruction = regex_replace(instruction, r, " ");
        
        vector<string> token;
        tokenize(instruction, token);
        
        if(token[0] == ";") {
            continue;
        }
        outputLFile << int_to_hex(counter, sizeof(int) * 2) << " ";
        bool flag = false;
        if(opCodes.count(token[0])) {
            
            // todo : if instruction is normal opcode one
            flag = validateInstruction(token, 0, counter);
            if(!flag) {
                
                outputLFile << setw(9) << " ";
                goto instructionlabel;
            }
            outputLFile << instructions[instructions.size() - 1] << " ";
        }
        else {
            vector<string> labelToken;
            tokenizeLabel(instruction, labelToken);
            if(labelAddr.count(labelToken[0])) {
                // todo : add code here if label found and check if there is code also
                if(labelToken.size() > 1 && labelToken[1] != ";") {
                    flag = validateInstruction(labelToken, 1, counter);
                    if(!flag) {
                        
                        outputLFile << setw(9) << " ";
                        goto instructionlabel;
                    }
                    outputLFile << instructions[instructions.size() - 1] << " ";
                }
                else {
                    outputLFile << setw(9) << " ";
                    counter--;
                }
            }
            else {
                error.push_back("Invalid token found at line "+ counter);
                outputLFile << setw(9) << " ";
            }
        }
        instructionlabel: outputLFile << instruction << endl;
        counter++;
    }


}

void opCodeInit() {
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


int main() {

    string filename = "test3";

    ifstream inputFile(filename + ".asm");

    ofstream outputLFile(filename + ".l");

    ofstream outputLogFile(filename + ".log");

    opCodeInit();



    firstPassAddress(inputFile, outputLFile);

    inputFile.close();

    inputFile.open(filename + ".asm");

    secondPassConvert(inputFile, outputLFile);

    for(auto val : error) {
        outputLogFile << "ERROR: " << val << endl;
    }

    for(auto val : warning) {
        outputLogFile << "WARNING: " << val << endl;
    }

    return 0;
}
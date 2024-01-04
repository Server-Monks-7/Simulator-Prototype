#include <iostream>
#include <map>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <sstream>
#include <regex>
#include <string>
#include <string.h>

using namespace std;

class output
{
//we are going to handle the output using a separate class to just allow us to define a function which we can then use to call each time we want to write to the output file
private:
    ofstream outputfile;

public:
    output(const string& filename){
    outputfile.open(filename, std::ios::app);
    //we open the file in append mode

    if(!outputfile.is_open())
    {
        cout << "Error: Unable to open the file." << endl;
    }
    }
    void writetoOutputFile(const string &data){
    if (outputfile.is_open()){
        outputfile << data << "\n";
    }
    else {
        cout << "Error: The output file is not open" << endl;
    }
    }
    ~output(){
    if (outputfile.is_open()){
        outputfile.close();
    }
    }
};
class assembler
{
private:
/*
We add two maps; one for the opcodes and another for the registers
Basically we create a database for our assembler to obtain the binary representations of the opcodes and registers
in binary
instructionMapping map takes in an opcode on Assembly and return the opcode in machine code
*/
       map<string, string> instructionMapping = {
        {"add", "100000"},
        {"addu" "100001"},
        {"addi", "001000"},
        {"subu", "100011"},
        {"div", "011010"},
        {"sub", "100010"},
        {"lw", "100011"},
        {"sw", "101011"},
        {"beq", "000100"},
        {"j", "000010"},
        {"beq"	,"000100"},
        {"bne"	,"000101"},
        {"jr"	,"001000"},
        {"jal"	,"000011"},
        {"lw"	,"100011"},
        {"sw"	,"101011"},
        //These are the instruction that the IoT device will have to execute.
    };

    // This map is to store the registers and their binary mappings/codes
    std::map<std::string, std::string> registerMapping = {
        {"$zero", "00000"},   // Hardwired zero
        {"$at", "00001"},     // Assembler temporary
        {"$v0", "00010"},     // Return values
        {"$v1", "00011"},
        {"$a0", "00100"},     // Arguments
        {"$a1", "00101"},
        {"$a2", "00110"},
        {"$a3", "00111"},
        {"$t0", "01000"},     // Temporaries
        {"$t1", "01001"},
        {"$t2", "01010"},
        {"$t3", "01011"},
        {"$t4", "01100"},
        {"$t5", "01101"},
        {"$t6", "01110"},
        {"$t7", "01111"},
        {"$s0", "10000"},     // Saved values
        {"$s1", "10001"},
        {"$s2", "10010"},
        {"$s3", "10011"},
        {"$s4", "10100"},
        {"$s5", "10101"},
        {"$s6", "10110"},
        {"$s7", "10111"},
        {"$t8", "11000"},     // More temporaries
        {"$t9", "11001"},
        {"$k0", "11010"},     // Kernel registers
        {"$k1", "11011"},
        {"$gp", "11100"},     // Global pointer
        {"$sp", "11101"},     // Stack pointer
        {"$fp", "11110"},     // Frame pointer
        {"$ra", "11111"},     // Return address
        /*
        We included more registers than our initial plan has stated
        Yes, some may be redundant but the wastage of registers is to be compensated or made up for in future when the device is improved for functionality and
        may require an increase in registers
        */
    };
public:
    string stringtobinary(const string &linee)
    {
        string binaryResult = " ";
        for(char c : linee){
            bitset<8> binaryChar(c);
            binaryResult += binaryChar.to_string();
        }
        return binaryResult;
    }
    string decimaltobinary(const string &word) {
    int decimal = stoi(word);
    if (decimal == 0) {
        return "0";
    }

    bitset<32> binary(decimal);
    return binary.to_string();
    }
    void parseAssemblyLine(const string &line)
    {
        if(line.empty())
        {
            //cout << "This is an empty line" << endl;
            return;
        }
        if(line[0] == '#')
        {
            string binComment;
            binComment = stringtobinary(line);
            output::writetoOutputFile(binComment);
            //remember to write this to the output file
            return;
        }
        regex instructionRegex(R"(\b(add|sub|lw|sw|beq|j)\b)");//add more instructions
        regex stringRegex(R"(\.asciiz\s+\"(.*)\")");
        if (regex_search(line, instructionRegex))
        {
            //Handle different instructions by perhaps calling another function
            instructionHandle(line);
        }
        else if (regex_search(line, stringRegex))
        {
            string binstr;
            binstr = stringtobinary(line); //convert the string to binary
            output::writetoOutputFile(line);
            //remember to write this to output file
        }
    }
    void instructionHandle(const &line)
    {
        stringstream linestream(line);
        string opcode, string1, string2, string3, string4;
        //we define these strings so we can use to break the line apart and assign the different parts of the line to the different strings
        //some strings may be empty
        string op, rs, rt, rd, imm, address, binline;
        if (getline(linestream, opcode, ' ' ) && getline(linestream, string1, ',') && getline(linestream, string2, ',') && getline(linestream, string3) && getline(linestream, string4))
             {
                 int count = 0;
                 bool full = true;
                 string arry[5] = {opcode, string1, string2, string3, string4};
                 for (int i = 0; i<=4; i++)
                 {
                     if (arry[i] == " ")
                     {count = count + 1;}
                 }
                 if(count == 5)//R-Type Instruction
                 {
                     op = instructionMapping[opcode];
                     rs = registerMapping[string1];
                     rt = registerMapping[string2];
                     rd = registerMapping[string3];
                     binline = op + rs + rt + rd;
                     output::writetoOutputFile(binline);
                 }
                 else if (count == 4)//I-Type Instruction
                 {
                     op = instructionMapping[opcode];
                     rs = registerMapping[string1];
                     rt = registerMapping[string2];
                     imm = decimaltobinary(string3);
                     binline = op + rs + rt + imm;
                     output::writetoOutputFile(binline);
                 }
                 else if (count == 2)
                 {
                     op = instructionMapping[opcode];
                     address = stringtobinary(string1);
                 }
                 else {cout << "Error!" << endl;}
             }
    }
    void filetranslatetoMachine(const string &assemblyFileName)
    {
        ifstream input(assemblyFileName);
        output filetowriteto("machinecode.bin");

        if(!input.is_open())
        {
            cout << "Unable to open files" << endl;
            return;
        }
        string line;
        while (getline(input, line))
        {
           parseAssemblyLine(line);
        }
    }
};

int main()
{
    assembler task;

    return 0;
}

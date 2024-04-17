#include <bits/stdc++.h>
using namespace std;
ofstream intmed("intmedFile.txt");
int startflag = 0, addr = 0;
int newaddr = 0; // 
int startaddr;
int lenprgm;
string TempTextRec[10]; // A Temporary buffer text record
int temptextindex; // an interator index to iterate through the text record
int locctr;
int lenTR = 0;
string label;
string Progname;
static int numinstr = 0; //  number of instructions
static int numsym = 0; // Number of symbols in Symbol Table

string hextodecobj(int x)
{
    // This function takes the location counter in decimal form.....converts it back to hexadecimal and then
    // makes a string of it. Now it adds two zeroes in front of it.
    // For Eg. If 4096 is passed into the function, 001000 is the output.
    stringstream ss;
    ss << hex << x;
    string t;
    ss >> t;
    int len = t.length();
    string space = "";
    char ch[6 - len];
    for (int i = 0; i < 6 - len; i++)
    {
        ch[i] = '0';
        space = space + ch[i];
    }
    //  ss<<hex<<x;
    space = space+ t;
    return space;
}

class OpTable
{ // Class for Opcode Table
public:
    string instruction, opcode;
    friend string Opcode(string);
    friend int AssemblerDirectCheck(string);

} OPTAB[45];

class SymTable
{   // Class for 56Symbol Table
public:
    int addr = 0;
    int flag = 0;
    int value = -1;
    SymTable *next;
    string label;
    friend void Symcheck(string);
    friend void FinalTr(string, int, int);
    friend int getFlag(string);
    friend int Value_Address(string);
    SymTable()
    {   // Default Constructor
        this->next = NULL;
    }

} SYMTAB[50];

string Opcode(string s)
{   // Function that returns the respective opcode of the instruction passes in it.

    string opc = "00";
    for (int i = 0; i < numinstr; i++)
    {
        if (OPTAB[i].instruction == s)
        {
            opc = OPTAB[i].opcode;
            break;
        }
    }
    return opc;
}
void Symcheck(string s) // This function makes the entry of the symbols and also checks it for forward Referencing.
{
    int count = 0;
    if (numsym != 0)
    {
        for (int i = 0; i < numsym; i++)
        {

            if (SYMTAB[i].label == s)
            {
                if (SYMTAB[i].value == 0xffffffff) // loop-1
                {
                    SYMTAB[i].value = locctr; // current value of the LOcation ctr is saved in the symbol value that is further used to update the nodes of  the linked list
                    SYMTAB[i].flag = -1; // It has the problem of forward refencing ...HEnce the value of flag will always be -1
                    SymTable *temp;
                    temp = &SYMTAB[i];
                    while (temp->next != NULL)
                    {
                        temp = temp->next;
                        temp->value = locctr; // Every nodde of the linked list is updated with the Value of the Symbol(Eg. ENDfILL)
                        temp->label = s; // The label of the symbol is updated.
                    }
                    break;
                }
                else if (SYMTAB[i].value != 0xffffffff)
                {
                    break;
                }
            }
            else
            {
                count++;
            }
        }
        if (count == numsym) // We have reached the end of the table
        {
            // This if condition works when we reach the end of the symbol table and till now we have not find the 
            // respective symbol. Thus we need to make the entry of the symbol at the last.
            SYMTAB[numsym].label = s;
            SYMTAB[numsym].value = locctr;
            numsym++;
        }
    }
    else 
    {
        // This else condition works when numsym == 0, That is the symbol table is empty. Thus we make an 
        // entry of the symbol passed in the function.
        SYMTAB[numsym].label = s;
        SYMTAB[numsym].value = locctr;
        numsym++;
    }
}

int AssemblerDirectCheck(string s)
{   // It checks for the following assembly directives.

    if (s == "START" || s == "END")
        return 0;
    else if (s == "WORD")
        return 1;
    else if (s == "BYTE")
        return 2;
    else if (s == "RESB")
        return 3;
    else if (s == "RESW")
        return 4;
    else
        return -1;
}

int Value_Address(string s)
{   // This function gets us the value of the symbols
    int count = 0;
    int val = 0;
    for (int i = 0; i < numsym; i++)
    {
        if (SYMTAB[i].label == s)
        {
            if (SYMTAB[i].value == -1) 
            {   // "When Value is not resolved and then too, that label is called again, at that point, we have to update the linked list by traversing till the end"
                // A new node is added into the linked list and all the data members are defined

                SymTable *temp;
                temp = &SYMTAB[i]; 
                SymTable *temp1;
                temp1 = &SYMTAB[i];
                SYMTAB[i].flag = -1;
                while (temp->next != NULL)
                {
                    temp = temp->next;
                }
                temp->next = new SymTable;
                temp = temp->next;
                temp->addr = locctr + 0x1 * 1;
                temp->flag = -1;
                temp->label = s;
                temp->value = -1;
                temp->next = NULL;
            }
            else if (SYMTAB[i].value != -1)
            {   // The value is already defined.
                val = SYMTAB[i].value;
                break;
            }
        }
        else
        {
            count++;
        }
    }
    if (count == numsym)
    {   // This is the condition when the symbol is passed and is not present in the Symbol Table
        // In this case, a new entry is made, linked list is created, flag and value is set to -1  

        SYMTAB[numsym].label = s;
        SYMTAB[numsym].value = -1;
        SYMTAB[numsym].flag = -1;
        SYMTAB[numsym].addr = 0;
        SymTable *temp;
        temp = &SYMTAB[numsym];
        temp->next = new SymTable;
        temp = temp->next;
        SymTable *temp1;
        temp1 = &SYMTAB[numsym];
        temp->addr = locctr + 0x1 * 1;
        temp->flag = -1;
        temp->label = s;
        temp->value = -1;
        temp->next = NULL;
        numsym++;
    }
    return val;
}
void FinalTr(string , int , int ); // Declaration
void InsertTR(string s, int flg, int val)
{   // This is an intermediate function to check whether the object program can be accomodated in the same recoed
    // or we need to make a new text record.

    int newflag = flg;
    if (val == addr || s == "" || flg == -1 || flg == -3 || (lenTR + (s.length() / 2)) > 30 || lenTR == 0 )
    {   // In this every case, we will have to make a new text record.

        if (val == addr)
        {   // Space between two instructions, need to be defined seperately
            newflag = -2;
        }
        if (s == "")
        {   // In this case, the object program is not there i.e it is either an END or RESW, RESB
            if (flg == -3) 
            {   // This case is for END.
                newflag= -3;
            }
            else
            {   // This case is for RESW, RESB
                newflag = -4; 

                if (temptextindex != -1 && temptextindex != 0)
                {
                    intmed << hex << lenTR;
                }
                for (int i = 0; i < temptextindex; i++)
                {
                    intmed << "^" << TempTextRec[i];
                    TempTextRec[i] = "";
                }
                temptextindex = -1; // Index is reset
            }
        }
        if (newflag != -4)
        {   // If the variable is not forward refernced, then simply we add 
            if (temptextindex != -1)
            {
                intmed << hex << lenTR;
            }
            for (int i = 0; i < temptextindex; i++)
            {
                intmed << "^" << TempTextRec[i];
            }
        }
        FinalTr(s, newflag, val);
    }
    else
    {   // In rest case, the buffer record is created, the object programs are filled till 
        // the length becomes 30 or any other above condition is met.
        TempTextRec[temptextindex] = s;
        lenTR += (s.length() / 2);
        temptextindex++;
    }
}
void FinalTr(string objpgm, int flg, int val)
{   // This function generates the Text records and also the text records for the forward refernced labels

    if (flg == 0) 
    {   // Making of the new text record
        // Simply the object is genereated and sent to the file.
        // The string is further passed to the buffer text record.
        intmed << "\nT^" << hextodecobj(val) << "^";
        lenTR = 0;
        temptextindex = 0;
        TempTextRec[temptextindex] = objpgm;
        lenTR += objpgm.length() / 2;
        temptextindex++;
    }
    else if (flg == -1) 
    {   // This is the case where forward referncing is the issue
        for (int i = 0; i < numsym; i++)
        {   // The modification of the text records for the instructions that had
            // issue with the forward referncing are addressed.

            if (SYMTAB[i].value == val)
            {
                SymTable *temp;
                temp = &SYMTAB[i];
                temp = temp->next;
                while (temp != NULL)
                {
                    intmed << "\nT^" << hextodecobj(temp->addr) << "^02^" << val;
                    temp = temp->next;
                }
            }
        }
        
        FinalTr(objpgm, 0, val);
    }
    else if (flg == -2) 
    {   // Starting of execution when there is a gap between Memory definition and the program execution
        FinalTr(objpgm, 0, val);
    }
    else if (flg == -3) 
    {   // This is the case when END is encountered
        intmed << "\nE^" << hextodecobj(newaddr);
        intmed.close(); // object program file is closed
    }
    else if (flg == -4)  
    {   // This is the case where Forward referncing is done and the instruction is an RESW or RESB
        // In this case we will have to make changes for the all the variables in the linked list but
        // the object program for this particular line will not print.
        for (int i = 0; i < numsym; i++)
        {
            if (SYMTAB[i].value == val)
            {
                SymTable *temp;
                temp = &SYMTAB[i];
                temp = temp->next;
                while (temp != NULL)
                {
                    intmed << "\nT^" << hextodecobj(temp->addr) << "^02^" << val;
                    temp = temp->next;
                }
            }
        }
    }
}

int hextodec(string s) 
{   // This function takes a string that is in hexadecimal and converts it into the decimal equivalent of it. 
    // Eg. "1000" is passed and 4096 in int form is returned.
    int x;
    stringstream ss(s);
    ss >> hex >> x;
    return x;
}

int getFlag(string s)
{   // Returns the flag of the symbol
    int flg = 0;
    for (int i = 0; i < numsym; i++)
    {
        if (SYMTAB[i].label == s && SYMTAB[i].value != -1)
        {
            flg = SYMTAB[i].flag;
            break;
        }
    }
    return flg;
}
string sixfitfunc(string s)
{
    // This function makes sure that the name of the program is placed in the required field.... i.e. 
    // for Eg. COPY is of 4 letters but we have to fit it in space with 6 letters. Hence, we add two space after 
    // writing COPY.
    int len = s.length();
    char space[6];
    for (int i = 0; i < 6 - len; i++)
    {
        space[i] = ' ';
        s = s + space[i];
    }
    return s;
}

int main()
{
    ifstream prgm; // Reading Input file
    ifstream OpcTab; // Reading Opcode Table
    ofstream symtab; // Symbol Table
    ofstream finalObj; // Final Object Program
    ofstream sampleFile;  // to get the program with location counters assigned
    finalObj.open("mainobjfinal.txt");
    prgm.open("Input1.txt");
    OpcTab.open("optab.txt");
    symtab.open("symtab.txt");
    sampleFile.open("samplepgm.txt");

   

    // For Opcode Table
    string optabline;
    string opc, mle;
    char ch = ' ';
    while (OpcTab.eof() == 0)
    {
        // While end of the file is not reached, We take a line and then....the first string that is the instructions
        // stored in the opc using stringstream, a space is given using ch(' ') and then its respective machine
        // level equivalent is taken is mle. Both are then sent to the OPTAB array.
        getline(OpcTab, optabline);
        stringstream S(optabline);
        getline(S, opc, ch);
        getline(S, mle, ch);
        OPTAB[numinstr].instruction = opc;
        OPTAB[numinstr].opcode = mle;
        numinstr++;
    }
    OpcTab.close();

    string inputline;
    string st1, st2, st3; 
    while (label != "END")
    {
        getline(prgm, inputline);
        stringstream ss(inputline);
        char ch = ' ';
        getline(ss, st1, ch); // We read each line and store the variables in st1,st2,st3
        getline(ss, st2, ch);
        getline(ss, st3, ch);

        stringstream SS1;
        string h;
        if (st1 != ".") // while the line is not a comment line
        {
            if (st1 != "" && st2 != "" && st3 != "") 
            {// This tells us that label, instruction and the operand, all are there in the line

                label = st2;
                Symcheck(st1);

                if (st2 == "START")
                {
                    locctr = hextodec(st3); // ASsigning the starting address to the location counter in decimal form i.e. "1000 is stored as 4096 in locctr"
                    startaddr = locctr;
                    Progname = st1;
                    sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << endl;
                    intmed << "H^" << sixfitfunc(Progname) << "^" << hextodecobj(startaddr);
                }

                else if (AssemblerDirectCheck(st2) == 1)  // Word
                {
                    int d;
                    stringstream ss(st3);
                    ss >> d;
                    sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << " " << hextodecobj(d) << endl;
                    InsertTR(hextodecobj(d), getFlag(st1), locctr);
                    locctr = locctr + 0x3;
                }
                else if (AssemblerDirectCheck(st2) == 2)  // Byte
                {
                    
                    if (st3[0] == 'C' || st3[0] == 'c')
                    {   // For Eg. C'EOF', So except C and inverted commas(i.e. is 3 characters), We need to convert
                        // Rest all the characters into the ASCII CODE.

                        sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << " ";
                        int size = st3.length() - 3;
                        for (int i = 2; i <  st3.length() - 1; i++)
                        {
                            sampleFile << hex << (int)st3[i];
                            SS1 << hex << (int)st3[i];
                        }
                        SS1 >> h; // Integer value i.e. 454F46 is converted in String form i.e. "454F46"
                        InsertTR(h, getFlag(st1), locctr); // Object code is sent as h
                        locctr = locctr + 0x1 * size;
                        sampleFile << endl;
                    }
                    else if (st3[0] == 'X' || st3[0] == 'x')  // Input/Output device - BYTE
                    {
                        sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << " ";
                        for (int i = 2; i < st3.length() - 1; i++)
                        {
                            sampleFile << st3[i];
                            SS1 << st3[i];
                        }
                        SS1 >> h;
                        InsertTR(h, getFlag(st1), locctr);
                        locctr = locctr + 1;
                        sampleFile << endl;
                    }
                    
                }
                else if (AssemblerDirectCheck(st2) == 3) // RESB
                {
                    sampleFile << locctr << " " << st1 << " " << st2 << " " << st3 << " " << endl;
                    char temp[st3.length()];
                    for (int i = 0; i < st3.length(); i++)
                    {
                        temp[i] = st3[i];  // Conversion of String to character array.
                    }
                    InsertTR(h, getFlag(st1), locctr);
                    locctr = locctr + (0x1 * (atoi(temp)));  // For Eg. locctr is incremented to 4096....ALL calculation is done in decimal equivalent
                }
                else if (AssemblerDirectCheck(st2) == 4)  // RESW
                {
                    sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << " " << endl;
                    char temp[st3.length()];
                    for (int i = 0; i < st3.length(); i++)
                    {
                        temp[i] = st3[i]; // Converting string to character Array
                    }
                    InsertTR(h, getFlag(st1), locctr);
                    locctr = locctr + (0x3 * (atoi(temp)));
                }
                else if (AssemblerDirectCheck(st2) == -1) 
                {   // When the input line is an executable line that is not a Word,RESW,RESB,BYTE

                    if (startflag == 0) 
                    {   // This is for the first executable statement of the program.
                        // Runs only for the first time.
                        newaddr = locctr;
                        addr = locctr;
                    }
                    startflag++;
                    int temp2 = Value_Address(st3);
                    if (temp2 != 0)
                    {   // When the value of the operand is already defined, then we pass the coresponding strings with the object code.
                        sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << " " << Opcode(st2) << hex << temp2 << endl;
                        SS1 << Opcode(st2) << hex << temp2;
                        SS1 >> h;
                        InsertTR(h, getFlag(st1), locctr);
                    }
                    else if (temp2 == 0)
                    {   // In the case where the value is not yet defined i.e. problem of the not forward referenced labels, we generate the object code with opcode and 0000.
                        sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << st3 << " " << Opcode(st2) << "0000" << endl;
                        SS1 << Opcode(st2) << "0000";
                        SS1 >> h;
                        InsertTR(h, getFlag(st1), locctr);
                    }

                    locctr += 0x3;
                }
            }

            else if (st1 != "" && st2 != "" && st3 == "")
            {   // This tells us that only the instruction and the operand are there in the line

                label = st1;
                if (AssemblerDirectCheck(st1) == -1)
                {   // Executable instruction

                    if (startflag == 0)
                    {   // Only for the first executable instruction.
                        newaddr = locctr;
                        addr = locctr;
                    }
                    startflag++;
                    int t = st2.length();
                    if (st2[t - 2] == ',')
                    {   // To Handle Indexed Addressing, For Eg. BUFFER,X

                        string s;
                        char ch = ',';
                        stringstream ss(st2);
                        getline(ss, s, ch); // For Eg. We have seperated the 'X' from the BUFFER
                        int value = Value_Address(s);
                        if (value != 0)
                        {   // Value of the operand is already defined.

                            sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << Opcode(st1) << hex << value + 0x8000 * 1 << endl;
                            SS1 << Opcode(st1) << hex << value + 0x8000 * 1; 
                            SS1 >> h;
                            InsertTR(h, getFlag(st2), locctr);
                        }
                        else if (value == 0)
                        {   // Value is not yet defined.

                            sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << Opcode(st1) << "0000" << endl;
                            SS1 << Opcode(st1) << "0000";
                            SS1 >> h;
                            InsertTR(h, getFlag(st2), locctr);
                        }
                        locctr += 0x1 * 3;
                    }
                    else
                    {
                        int x = Value_Address(st2);
                        if (x != 0)
                        {
                            sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << Opcode(st1) << hex << x << endl;
                            SS1 << Opcode(st1) << hex << x;
                            SS1 >> h;
                            InsertTR(h, getFlag(st2), locctr);
                        }
                        else if (x == 0)
                        {
                            sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << Opcode(st1) << "0000" << endl;
                            SS1 << Opcode(st1) << "0000";
                            SS1 >> h;
                            InsertTR(h, getFlag(st2), locctr);
                        }
                        locctr += 0x1 * 3;
                    }
                }
                else if (AssemblerDirectCheck(st1) == 0)
                {   // This is the condition to check "END", flag = -3 is set for it
                    sampleFile << st1 << " " << st2 << " ";
                    InsertTR("", -3, locctr);
                }
            }
            else if (st1 != "" && st2 == "" && st3 == "")
            {   // This is the condition for the instructions like RSUB
                label = st1;
                sampleFile << hex << locctr << " " << st1 << " " << st2 << " " << Opcode(st1) << "0000" << endl;
                SS1 << Opcode(st1) << "0000";
                SS1 >> h;
                InsertTR(h, 0, locctr);
                locctr += 0x1 * 3;
            }
        }
        else
        {   // For comment lines
            sampleFile << st1 << endl;
            
        }

        // All strings are cleared so that no string is overwritten.
        st1.clear();
        st2.clear();
        st3.clear();
    }
    

    for (int i = 0; i < numsym; i++)
    {
       //   Symbol TAble is generated with Labels and Flags.
        symtab << SYMTAB[i].label << " " << hex << SYMTAB[i].value<< endl;
    }

    ifstream out;
    out.open("intmedFile.txt"); // We have opened output.txt file to make changes i.e to write the length of the program
    int temp3 = 0;
    lenprgm = locctr - startaddr;
    while (out.eof() == 0)
    {
        string t;
        getline(out, t);
        if (temp3 == 0)
        {   // The header record is edited and written in the final Object Program Code.
            finalObj << "H^" << sixfitfunc(Progname) << "^" << hextodecobj(startaddr) << "^" << hextodecobj(lenprgm) << endl;
        }
        else
        {   // Rest all lines are taken as it is.
            finalObj << t << endl;
        }
        temp3++;
    }
    // All files are closed
    OpcTab.close();
    out.close();
    finalObj.close();
    prgm.close();
    symtab.close();
    sampleFile.close();

    return 0;
}
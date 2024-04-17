       //////////////////////////////////////////////////////////WORKING OF ONE PASS ASSEMBLERS ///////////////////////////////////////////
->we made intermediate file InsertTr and Final_Tr which helps in intermediate processing.
-> the program which we will give  in the input1.txt file will be SIC type program.
-> comments can be identified and added using '.' 
->To input the program, the three columns should be filled with a single space separating them.
->In case the first column is absent, the instruction should be entered from the left margin without considering any gap between spaces.
 -> for optab we have made another file optab.txt having  instructions and  their respective opcodes.

->coversion function hextodec(string s)is made to take care of conversion btw int and hexadecimal.
->the output will be shown in file output.txt 

                        -------->in OPcode_ASSEM.cpp<-------
         starting of program-->>
->after entry of the input program inp1.txt is opened in OPcode_ASSEM.cpp .
-> in OPTAB the  array of objects each with instruction and opcode data members is  filled.

->optab.txt is created and opened in OPcode_ASSEM.cpp
->According to the no.of columns filled the conditions and  functions will  be executed.
->taking string input stops when we get END assembler directive.
->now line by line every line of input.txt is read as a string and three columns we get as each column is separated by a gap.
->location counter are updated accordingly while reading of every line of the symtab .
-> void Symcheck(string s) and int Value_Address(string s) functions generates the SYMBOL TABLE.
-> in first pass we do not know the length of program so after end assembler directive is encountered it is updated .
->Simultaneously the object code is generated and send to the object program and new text records are placed according to the conditions mentioned
->void Final_Tr(string obgpgm,  int flg, int val) and void InsertTr(string s, int flg, int val)are helpful in generating  the object program.

->the files generated after execution of the program are :
          1.intmedFile.txt  which contains object progam wihtout the updated length of program.
          2.samplepgm.txt which contains the input program with location counter and object code.
          3.symtab.txt  which contains labels and  values along with a flag given for forwardreferencing problem actually to handle that problem.
          4.mainobjfinal.txt  which contains the Final object program with length of program updated.
		  
		  
		  
		  
		  
		  
		  
		  
		  
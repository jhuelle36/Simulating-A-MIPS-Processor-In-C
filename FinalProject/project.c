#include "spimcore.h"

/* ALU
 Simulates an ALU(Arithmetic Logic Unit) in MIPS which includes the functions: Add, Subtract, Set Less Than, Set Less Than Unsigned, And, Or,
Shift Left by 16 bits, and NOT
*/
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero) {
    // Switch checks the ALUControl and does corresponding ALU operation on A and B while storing result in ALUresult.
    switch ((int)ALUControl) {
    case 0:
        *ALUresult = A + B;
        break;
    case 1:
        *ALUresult = A - B;
        break;
    //"if A < B, Z = 1; otherwise, Z = 0" - Ask about case 2 and 3 being very similar
    case 2:
        if (A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
        break;
    //"if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)" - Ask about case 2 and 3 being very similar(copied code for case 2 to case 3)
    case 3:
        if (A < B)
            *ALUresult = 1;
        else
            *ALUresult = 0;
        break;
    case 4:
        *ALUresult = (A & B);
        break;
    case 5:
        *ALUresult = (A | B);
        break;
    case 6:
        *ALUresult = B << 16;
        break;
    case 7: // Is this the best way?
        if (A == 1)
            *ALUresult = 0;
        else
            *ALUresult = 1;
        break;
    }

    // If result was 0 then set 0 to 1, otherwise set Zero to 0
    if (*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;
}
/* instruction fetch
    Checks if PC is word alligned and then sets instruction to the following location in memory, it is stored as a 32 bit unsigned.
    Holds the information for instruction within bits.
*/
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction) {
    // Use mod 4 because PC should be byte addressed. Memory Starts at 4000, therefore every index in memory should be divisible by 4
    if (PC % 4 == 0)
        *instruction = Mem[PC >> 2];
    else
        return 1;
    return 0;
}

/* instruction partition
    Breaks down the 32 bit unsigned instruction and assigns to corresponding global unsigned pointers.
    They will hold certain ranges of bits: op: 31-26, r1: 25-21, r2: 20-16, r3: 15-11, funct: 5-0, offset: 15-0, jsec: 25-0
*/
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) {
    // Combination of right shifts and bitwise operator '&' to assign only the correct bits to each corresponding pointer
    *op = instruction >> 26;
    *r1 = (instruction >> 21) & 0b00000011111;
    *r2 = (instruction >> 16) & 0b0000000000011111;
    *r3 = (instruction >> 11) & 0b000000000000000011111;
    *funct = instruction & 0b00000000000000000000000000111111;
    *offset = instruction & 0b00000000000000001111111111111111;
    *jsec = instruction & 0b00000011111111111111111111111111;
}

/* instruction decode
This function is for setting the control sigals based on the op code. This decode function only handels the Operation codes from MIPS Reference PowerPoint.
We are given a 6 bit operation code that corresponds to a different operation: R-Format, Jump, Branch Equal, Add Immediate, Set Less Than Immediate, Set Less Than
Immediate Unsigned, Load Upper Immediate, Load Word, and Store Word. Each of these have a nique operation code, if the operation code is all 0's, then it is an
r-type instruction which it will then look at bits 0-5(function code) to find the corect function: Add, Subtract, And, Or, Set Less Than, and Set Less Tham Unsigned.
But this will be handled by another function. This function will return 0 if no halt, otherwise 1.

*/
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls) {
    // Halt flag
    int flag = 1;

    // Switch assigns control signals based on corresponding operation code
    switch (op) {

    // R-Format instructions: 000000
    case 0:
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 7;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        flag = 0;
        break;

    // Jump instruction: 000010
    case 2:
        controls->RegDst = 0;
        controls->Jump = 1;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
        flag = 0;
        break;

    // Branch Equal instruction: 000100
    case 4:
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 1;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 0;
        flag = 0;
        break;

    // Add Immediate instruction: 001000
    case 8:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        flag = 0;
        break;

    // Set Less Than Immediate instruction: 001010
    case 10:
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 2;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        flag = 0;
        break;

    // Set Less Than Immediate Unsigned instruction: 001011
    case 11:
        controls->RegDst = 1;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 3;
        controls->MemWrite = 0;
        controls->ALUSrc = 0;
        controls->RegWrite = 1;
        flag = 0;
        break;

    // Load Upper Immediate instruction: 001111
    case 15:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->ALUOp = 6;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        flag = 0;
        break;

    // Load Word instruction: 100011
    case 35:
        controls->RegDst = 0;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->ALUOp = 0;
        controls->MemWrite = 0;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        flag = 0;
        break;

    // Store Word instruction: 101011
    case 43:
        controls->RegDst = 2;
        controls->Jump = 0;
        controls->Branch = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->ALUOp = 0;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        controls->RegWrite = 0;
        flag = 0;
        break;
    default:
        return flag;
    }

    return flag;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1, unsigned r2, unsigned *Reg, unsigned *data1, unsigned *data2) {

    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value) {
    int sign_bit = (offset & 0b1000000000000000) ? 1 : 0; // Check the sign bit (bit 15) (int)32768, (hex)0x8000

    int sign_extend_value = (sign_bit ? 0b11111111111111110000000000000000 : 0); // 0b11111111111111110000000000000000 is 0xFFFF0000 in binary  // 0b11111111111111110000000000000000 is 0xFFFF0000 in binary and 4294901760 in decimal

    if (sign_extend_value == 0)
        *extended_value = offset & 0x0000FFFF; // Clear upper 16 bits, leaving lower 16 bits intact
    else
        *extended_value = offset + sign_extend_value; // Apply sign extension using addition
}
/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero) {
    if (ALUSrc == 1) {
        data2 = extended_value;
    }
    // R-type instructions have opcode of 000000
    if (ALUOp == 7) {
        // instructions are determined by funct field.(6 bits)
        switch (funct) {
        case 32: // Addition
            ALUOp = 0;
            break;
        case 34: // subtraction
            ALUOp = 1;
            break;
        case 39: // nor
            ALUOp = 7;
            break;
        case 37: // or
            ALUOp = 5;
            break;
        case 43: // Slt (unsigned)
            ALUOp = 3;
            break;
        case 42: // Slt (signed)
            ALUOp = 2;
            break;
        case 36: // and
            ALUOp = 4;
            break;
        case 4: // shift right logical
            ALUOp = 6;
            break;
        default:
            return 1;
        }
        ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
    // for other types of instruction formats.
    else {
        ALU(data1, data2, ALUOp, ALUresult, Zero);
    }

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem) {
    // if MemRead or MemWrite is asserted but ALUresult doesn't represent a valid address, we should halt
    if ((MemRead || MemWrite) && ((ALUresult % 4) != 0)) {
        return 1;
    }
    // write the value of data2 to the memory location addressed by ALUresult
    if (MemWrite == 1) {
        Mem[ALUresult >> 2] = data2;
    }
    // read the content of memory location addressed by ALUresult to memdata
    if (MemRead == 1) {
        *memdata = Mem[ALUresult >> 2];
    }

    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg) {

    if (RegWrite == 1) {
        if (MemtoReg == 1) {
            // data is coming memory
            if (RegDst == 1) {
                Reg[r3] = memdata;
            } else {
                Reg[r2] = memdata;
            }
        } else {
            // data is coming from ALUresult
            if (RegDst == 1) {
                Reg[r3] = ALUresult;
            } else {
                Reg[r2] = ALUresult;
            }
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC) {
    // step 1, add 4 to PC
    *PC += 4;

    /*
    branch is taken and Zero signal is recieved from ALUresult
    PC = PC + 4 + (4 * offset)
    */
    if (Branch == 1 & Zero == 1) {
        *PC += (extended_value << 2);
    }
    // if Jump, left shift bits of jsec by 2 and use upper 4 bits of PC
    if (Jump == 1) {
        *PC = (*PC & 0xF0000000) | (jsec << 2);
    }
}
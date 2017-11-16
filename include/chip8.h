#include <iostream>

class chip8 {

    // Current opcode (2 bytes)
    unsigned short opcode;

    // 4K memory (0x000 to 0xFFF)
    // 0x000-0x1FF: Chip 8 interpreter (contains font set in emu)
    // 0x050-0x0A0: Used for the built-in 4x5 pixel font set (0-F)
    // 0x200-0xFFF: Program ROM and work RAM
    unsigned char memory[4096];

    // 16 8-bit registers, named V0,V1,...,VD,VE
    unsigned char V[16];

    // Index register
    unsigned short I;

    // Program counter
    unsigned short pc;

    // Graphics: 2048 pixels on 64x32 screen
    // Screen is B&W, so binary value is all that's needed
    unsigned char gfx[64 * 32];

    // Delay timer register - Count down if above zero at 60Hz
    unsigned char delay_timer;

    // Sound timer - Similar to delay timer; buzzes when above 0
    unsigned char sound_timer;

    // Stack: Not actually in CHIP-8? But needed for jump instructions
    // Used to remember the current location before a jump is performed
    // Store pc in stack before jumping
    unsigned short stack[16];

    // Stack pointer to remember which level of stack is used
    unsigned short sp;

    // Store current state of each key on a hex keypad (0x0 to 0xF)
    unsigned char key[16];

    // Draw flag determines whether or not to draw the screen each cycle
    bool drawFlag;
}

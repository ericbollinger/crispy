#include <chip8.h>

// Initialize registers and memory once
void chip8::initialize() {
    pc      = 0x200;    // Program counter starts at 0x200
    opcode  = 0;        // Reset current opcode
    I       = 0;        // Reset index register
    sp      = 0;        // Reset stack pointer

    // Clear display
    memset(gfx, 0, 2048);

    // Clear stack
    memset(stack, 0, 32);

    // Clear registers V0-VF
    memset(V, 0, 16);

    // Clear memory
    memset(memory, 0, 4096);

    // Load fontset
    for (int i = 0; i < 80; ++i) {
        //memory[i] = chip8_fontset[i];
    }

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;
}

// Load game's instructions into memory
void chip8::loadGame(std::string toOpen) {
    toOpen = "files/" + toOpen;
    FILE* gameFile;
    // Open in Binary Read mode
    gameFile = fopen(toOpen.c_str(), "rb");

    // Determine length of game file, in bytes
    fseek(gameFile, 0, SEEK_END);
    long fsize = ftell(gameFile);
    fseek(gameFile, 0, SEEK_SET);

    // Read game file into buffer array
    unsigned char buf[3583] = {0};
    int buf_size = fread(buf, 1, fsize, gameFile);

    // Copy contents of buffer into memory
    // Start at index 512, the beginning of the program memory
    if (gameFile) {
        int i;
        for (i = 0; i < buf_size; i++) {
            memory[512 + i] = buf[i];
        }
    }
}

void chip8::emulateCycle() {
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode Opcode
    switch (opcode & 0xF000) {
        case 0x0000:
            switch(opcode & 0x000F) {
                case 0x0000:    // 0x00E0: Clears the screen
                    memset(gfx, 0, 2048);
                    pc += 2;
                    break;
                case 0x000E:     // Returns from subroutine
                    sp--;
                    pc = stack[sp];
                    stack[sp] = 0;
                    break;

            }
            break;
        
        case 0x1000:    // 1NNN: Jump to address NNN
            pc = opcode & 0x0FFF;
            break;

        case 0x2000:    // 2NNN: Calls subroutine at NNN
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000:    // 3XNN: Skips next instruction if VX == NN
            if (V[opcode >> 2 & 0xF] == (opcode & 0xFF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x4000:    // 4XNN: Skips next instruction if VX != NN
            if (V[opcode >> 2 & 0xF] != (opcode & 0xFF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x5000:    // 5XY0: Skips next instruction if VX == VY
            if (V[opcode >> 2 & 0xF] == V[opcode >> 1 & 0xF]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x6000:    // 6XNN: Sets VX to NN
            V[opcode >> 2 & 0xF] = (opcode & 0xFF);
            pc += 2;
            break;

        case 0x7000:    // 7XNN: Adds NN to VX
            V[opcode >> 2 & 0xF] += (opcode & 0xFF);
            pc += 2;
            break;

        case 0x8000:    
            switch(opcode & 0x000F) {
                case 0x0000:    // 8XY0: Sets VX to value of VY
                    V[opcode >> 2 & 0xF] = V[opcode >> 1 & 0xF];
                    pc += 2;
                    break;

                case 0x0001:    // 8XY1: Sets VX to VX OR VY
                    break;

                case 0x0002:    // 8XY2: Sets VX to VX AND VY
                    break;

                case 0x0003:    // 8XY3: Sets VX to VX XOR VY
                    break;

                case 0x0004:    // 8XY4: VX = VX + VY. VF is set to 1 if there's a carry, 0 if not
                    break;

                case 0x0005:    // 8XY5: VX = VX - VY. VF is set to 1 if there's a borrow, 0 if not
                    break;

                case 0x0006:    // 8XY6: VX = VY >> 1. VF is set to VY's least significant bit before shift. 
                    break;

                case 0x0007:    // 8XY7: VX = VY - VX. VF is set to 0 if there's a borrow, 1 if not
                    break;

                case 0x000E:    // 8XYE: VX = VY << 1. VF is set to VY's most significant bit before shift.
                    break;
            }
            break;

        case 0x9000:    // 9XY0: Skips next instruction if VX != VY
            if (V[opcode >> 2 & 0xF] != V[opcode >> 1 & 0xF]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;


        case 0xA000:    // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;


        default:
            std::cout << "Unknown opcode: 0x" << std::setbase(16) << opcode;
            std::cout << "\n";
            break;
    }

    // Update timers
    if (delay_timer > 0)
        delay_timer--;

    if (sound_timer > 0) {
        if (sound_timer == 1) 
            std::cout << "BEEP!\n";
        sound_timer--;
    }
}

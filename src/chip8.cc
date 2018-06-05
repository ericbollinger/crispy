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
        memory[i] = chip8_fontset[i];
    }

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    unsigned char fontset_tmp[80] =
    { 
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80    // F
    };
    memcpy(chip8_fontset, fontset_tmp, 80);
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
    executeOpcode(opcode);

    // Update timers
    if (delay_timer > 0)
        delay_timer--;

    if (sound_timer > 0) {
        if (sound_timer == 1) 
            std::cout << "BEEP!\n";
        sound_timer--;
    }

    // Delay to keep processor running at 60Hz
    req.tv_sec = 0;
    req.tv_nsec = 16666666;
    nanosleep(&req, &rem);
}

void chip8::executeOpcode(unsigned short opcode) {
    int r; 

    switch (opcode & 0xF000) {
        case 0x0000:
            switch(opcode & 0x00FF) {
                case 0x00E0:    // 0x00E0: Clears the screen
                    memset(gfx, 0, 2048);
                    pc += 2;
                    break;

                case 0x00EE:     // Returns from subroutine
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
            if (V[opcode >> 8 & 0xF] == (opcode & 0xFF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x4000:    // 4XNN: Skips next instruction if VX != NN
            if (V[opcode >> 8 & 0xF] != (opcode & 0xFF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x5000:    // 5XY0: Skips next instruction if VX == VY
            if (V[opcode >> 8 & 0xF] == V[opcode >> 4 & 0xF]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0x6000:    // 6XNN: Sets VX to NN
            V[opcode >> 8 & 0xF] = (opcode & 0xFF);
            pc += 2;
            break;

        case 0x7000:    // 7XNN: Adds NN to VX
            V[opcode >> 8 & 0xF] += (opcode & 0xFF);
            pc += 2;
            break;

        case 0x8000:    
            switch(opcode & 0x000F) {
                case 0x0000:    // 8XY0: Sets VX to value of VY
                    V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF];
                    pc += 2;
                    break;

                case 0x0001:    // 8XY1: Sets VX to VX OR VY
                    V[opcode >> 8 & 0xF] |= V[opcode >> 4 & 0xF];
                    pc += 2;
                    break;

                case 0x0002:    // 8XY2: Sets VX to VX AND VY
                    V[opcode >> 8 & 0xF] &= V[opcode >> 4 & 0xF];
                    pc += 2;
                    break;

                case 0x0003:    // 8XY3: Sets VX to VX XOR VY
                    V[opcode >> 8 & 0xF] ^= V[opcode >> 4 & 0xF];
                    pc += 2;
                    break;

                case 0x0004:    // 8XY4: VX = VX + VY. VF is set to 1 if there's a carry, 0 if not
                    if (V[opcode >> 4 & 0xF] > (0xFF - V[opcode >> 8 & 0xF])) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[opcode >> 8 & 0xF] += V[opcode >> 4 & 0xF];
                    pc += 2;
                    break;

                case 0x0005:    // 8XY5: VX = VX - VY. VF is set to 1 if there's a borrow, 0 if not
                    if (V[opcode >> 4 & 0xF] > V[opcode >> 8 & 0xF]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[opcode >> 8 & 0xF] -= V[opcode >> 4 & 0xF];
                    pc += 2;
                    break;

                case 0x0006:    // 8XY6: VX = VY >> 1. VF is set to VY's least significant bit before shift. 
                    V[0xF] = V[opcode >> 4 & 0xF] & 0x1;
                    V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] >> 1 & 0xF;
                    pc += 2;
                    break;

                case 0x0007:    // 8XY7: VX = VY - VX. VF is set to 0 if there's a borrow, 1 if not
                    if (V[opcode >> 4 & 0xF] < V[opcode >> 8 & 0xF]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] - V[opcode >> 8 & 0xF];
                    pc += 2;
                    break;

                case 0x000E:    // 8XYE: VX = VY << 1. VF is set to VY's most significant bit before shift.
                    V[0xF] = V[opcode >> 4 & 0xF] >> 3 & 0x1;
                    V[opcode >> 8 & 0xF] = V[opcode >> 4 & 0xF] << 1 & 0xF;
                    pc += 2;
                    break;
            }
            break;

        case 0x9000:    // 9XY0: Skips next instruction if VX != VY
            if (V[opcode >> 8 & 0xF] != V[opcode >> 4 & 0xF]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;

        case 0xA000:    // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000:    // BNNN: Jumps to address NNN plus V0
            pc = (opcode & 0xFFF) + V[0];
            break;

        case 0xC000:    // CXNN: Sets VX to result of bitwise & operation on a random number and NN
            srand(time(NULL));
            r = rand() % 256;
            V[opcode >> 8 & 0xF] = (opcode & 0xFF) & r;
            pc += 2;
            break;

        // DXYN: Draw sprite at (VX,VY), width = 8, height = N. 
        // Each row of 8 pixels is read as bit-coded starting from memory location I.
        // I value doesn't change after execution.
        // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
        // and 0 if that does not happen.
        case 0xD000: 
            {
                unsigned short x = V[opcode >> 8 & 0xF];
                unsigned short y = V[opcode >> 4 & 0xF];
                unsigned short height = opcode & 0xF;
                unsigned short pixel;

                V[0xF] = 0;
                for (int yLine = 0; yLine < height; yLine++) {
                    pixel = memory[I + yLine];
                    for (int xLine = 0; xLine < 8; xLine++) {
                        if ((pixel & (0x80 >> xLine)) != 0) {
                            if (gfx[(x + xLine + ((y + yLine) * 64))] == 1) {
                                V[0xF] = 1;
                            }
                            gfx[x + xLine + ((y + yLine) * 64)] ^= 1;
                        }
                    }
                }

                drawFlag = true;
                pc += 2;
            }
            break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:    // EX9E: Skip next instruction if key stored in VX is pressed
                    if (key[V[opcode >> 8 & 0xF]] != 0) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;

                case 0x00A1:    // EXA1: Skip next instruction if key stored in VX is not pressed
                    if (key[V[opcode >> 8 & 0xF]] == 0) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:    // FX07: Sets VX to the value of the delay timer
                    V[opcode >> 8 & 0xF] = delay_timer;
                    pc += 2;
                    break;

                case 0x000A:    // FX0A: A key press is awaited, then stored in VX (blocking)
                    {
                        unsigned char pressed = -1;
                        unsigned char i;
                        for (i = 0; i <= 0xF; i++) {
                            if (key[i] != 0) {
                                pressed = i;
                                break;
                            }
                        }
                        if (pressed >= 0) {
                            V[opcode >> 8 & 0xF] = i;
                            // Only increment PC if a key is pressed
                            pc += 2;
                        }
                    }
                    break;

                case 0x0015:    // FX15: Sets the delay timer to VX
                    delay_timer = V[opcode >> 8 & 0xF];
                    pc += 2;
                    break;

                case 0x0018:    // FX18: Sets the sound timer to VX
                    sound_timer = V[opcode >> 8 & 0xF];
                    pc += 2;
                    break;

                case 0x001E:    // FX1E: Adds VX to I
                    I += V[opcode >> 8 & 0xF];
                    pc += 2;
                    break;

                case 0x0029:    // FX29: Sets I to location of sprite for character in VX
                    I = V[opcode >> 8 & 0xF] * 0x5;
                    pc += 2;
                    break;

                case 0x0033:    // FX33: Stores binary-coded decimal representation of VX
                    // Encoded in a special fashion at address I 
                    memory[I] = V[opcode >> 8 & 0xF] / 100;
                    memory[I+1] = (V[opcode >> 8 & 0xF] / 10) % 10;
                    memory[I+2] = (V[opcode >> 8 & 0xF] % 100) % 10;
                    pc += 2;
                    break;

                case 0x0055:    // FX55: Stores V0 to VX (inclusive) in memory starting at address I
                    {
                        int max = opcode >> 8 & 0xF;
                        for (int i = 0; i <= max; i++) {
                            memory[I] = V[i];
                            I++;
                        }
                        pc += 2;
                    }
                    break;
                case 0x0065:   
                    {
                        // FX65: Fills V0 through VX with values from memory starting at I
                        // (I is increased by 1 for each value written)
                        int fillTo = opcode >> 8 & 0xF;
                        for (int i = 0; i <= fillTo; i++) {
                            V[i] = memory[I];
                            I++;
                        }
                        pc += 2;
                    }
                    break;

            }
            break;

        default:
            std::cout << "Unknown opcode: 0x" << std::setbase(16) << opcode;
            std::cout << "\n";
            break;
    }
    std::cout << "Opcode: " << std::setbase(16) << opcode << "\n";
}

void chip8::testOpcodes() {

    V[4] = 0xFA;
    V[5] = 0xA0;
    V[0xF] = 0;

    unsigned short op = 0x8454;
    std::cout <<"Opcode: " << std::setbase(16) << op << "\n";
    std::cout <<"X = " << std::setbase(16) << (op >> 8 & 0xF) << "\n";

    executeOpcode(0x8454);
    std::cout << "V[4]: " << std::setbase(16) << +V[4] << "\nV[F]: " << std::setbase(16) << +V[0xF] << "\n";
}

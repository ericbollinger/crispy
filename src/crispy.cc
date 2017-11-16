#include <chip8.h>

chip8 chip;

int main(int argc, char **argv) {
    // Set up render system and register input callbacks
//    setupGraphics();
//    setupInput();

    // Initialize the CHIP-8 system and load the game into memory
    chip.initialize();
    chip.loadGame((std::string)"pong");

    // Emulation loop
    for (;;) {
        // Emulate a single cycle
        chip.emulateCycle();

        // If the draw flag is set, update the screen
        if (chip.drawFlag) {}
//            drawGraphics();

        // Store key press state (Press and Release)
//        chip.setKeys();
    }   

    return 0;
}


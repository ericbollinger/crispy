#include <chip8.h>

chip8 chip;

float xbase, ybase;

void drawPixel(int x, int y) {
    glBegin(GL_POLYGON);
        glVertex3f(xbase + (xbase * x), ybase + (ybase * y), 0);
        glVertex3f(0 + (xbase * x), ybase + (ybase * y), 0);
        glVertex3f(0 + (xbase * x), 0 + (ybase * y), 0);
        glVertex3f(xbase + (xbase * x), 0 + (ybase * y), 0);
    glEnd();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,1,1);

    int x,y;
    for (int i = 0; i < (64*32); i++) {
        if (chip.gfx[i] == 1) {
            x = i % 64;
            y = 31 - (i / 64);
            drawPixel(x,y);
        }
    }
    glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }

    switch(key) {
        case '1':   chip.key[0x1] = 1; break;
        case '2':   chip.key[0x2] = 1; break;
        case '3':   chip.key[0x3] = 1; break;
        case '4':   chip.key[0xC] = 1; break;

        case 'q':   chip.key[0x4] = 1; break;
        case 'w':   chip.key[0x5] = 1; break;
        case 'e':   chip.key[0x6] = 1; break;
        case 'r':   chip.key[0xD] = 1; break;

        case 'a':   chip.key[0x7] = 1; break;
        case 's':   chip.key[0x8] = 1; break;
        case 'd':   chip.key[0x9] = 1; break;
        case 'f':   chip.key[0xE] = 1; break;

        case 'z':   chip.key[0xA] = 1; break;
        case 'x':   chip.key[0x0] = 1; break;
        case 'c':   chip.key[0xB] = 1; break;
        case 'v':   chip.key[0xF] = 1; break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch(key) {
        case '1':   chip.key[0x1] = 0; break;
        case '2':   chip.key[0x2] = 0; break;
        case '3':   chip.key[0x3] = 0; break;
        case '4':   chip.key[0xC] = 0; break;

        case 'q':   chip.key[0x4] = 0; break;
        case 'w':   chip.key[0x5] = 0; break;
        case 'e':   chip.key[0x6] = 0; break;
        case 'r':   chip.key[0xD] = 0; break;

        case 'a':   chip.key[0x7] = 0; break;
        case 's':   chip.key[0x8] = 0; break;
        case 'd':   chip.key[0x9] = 0; break;
        case 'f':   chip.key[0xE] = 0; break;

        case 'z':   chip.key[0xA] = 0; break;
        case 'x':   chip.key[0x0] = 0; break;
        case 'c':   chip.key[0xB] = 0; break;
        case 'v':   chip.key[0xF] = 0; break;
    }
}

void initializeGraphics(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1280,640);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Crispy - CHIP-8 Emulator");

    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);

    glutDisplayFunc(display);
//    glutIdleFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
}

int main(int argc, char **argv) {
    // Set up render system and register input callbacks
    initializeGraphics(argc, argv);
    //setupInput();

    xbase = 1.0 / 64;
    ybase = 1.0 / 32;

    // Initialize the CHIP-8 system and load the game into memory
    chip.initialize();

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-test") == 0) {
            chip.testOpcodes();
            std::cout << "Done testing!\n";
            return 0;
        }
    }

    chip.loadGame((std::string) argv[1]);

    // Emulation loop
    int i = 0;
    for (;;) {
        std::cout << "i = " << std::setbase(10) << i % 60 << "\n";
        i++;
        // Emulate a single cycle
        chip.emulateCycle();

        // If the draw flag is set, update the screen
        if (chip.drawFlag) {
            glutMainLoopEvent();
            glutPostRedisplay();
        }

        // Store key press state (Press and Release)
//        chip.setKeys();
    }   

    return 0;
}



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

void initializeGraphics(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    //glutInitWindowSize(640,320);
    glutInitWindowSize(1280,640);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Crispy - CHIP-8 Emulator");

    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);

    glutDisplayFunc(display);
    glutIdleFunc(display);
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



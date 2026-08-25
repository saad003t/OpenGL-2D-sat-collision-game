#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ================= STRUCTS & GAME ENGINE =================

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    Vec2 pos;
    Vec2 targetPos;      // Target position for smooth interpolation (Triangle)
    float angle;          // Rotation angle in degrees
    float size;
    float speed;          // Movement speed in pixels/frame
    Vec2 localVerts[4];   // Local space vertices
    Vec2 worldVerts[4];   // Transformed world space vertices
    int numVerts;
} Shape;

// Global Game State
int windowWidth = 600;
int windowHeight = 600;
int isGameOver = 0;

Shape square;
Shape triangle;

// Convert degrees to radians
float degToRad(float deg) {
    return deg * ((float)M_PI / 180.0f);
}

// Transform local vertices to world coordinates
void updateWorldVertices(Shape* s) {
    float rad = degToRad(s->angle);
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    for (int i = 0; i < s->numVerts; i++) {
        float lx = s->localVerts[i].x;
        float ly = s->localVerts[i].y;
        s->worldVerts[i].x = s->pos.x + (lx * cosA - ly * sinA);
        s->worldVerts[i].y = s->pos.y + (lx * sinA + ly * cosA);
    }
}

// Initialize shape properties
void initGame() {
    isGameOver = 0;

    // --- SQUARE SETUP ---
    square.pos = (Vec2){ 100.0f, 100.0f };
    square.angle = 0.0f;
    square.size = 40.0f;
    square.speed = 10.0f; // Default Square Speed
    square.numVerts = 4;

    float hs = square.size / 2.0f;
    square.localVerts[0] = (Vec2){ -hs, -hs };
    square.localVerts[1] = (Vec2){  hs, -hs };
    square.localVerts[2] = (Vec2){  hs,  hs };
    square.localVerts[3] = (Vec2){ -hs,  hs };

    // --- TRIANGLE SETUP ---
    triangle.pos = (Vec2){ 400.0f, 400.0f };
    triangle.targetPos = triangle.pos;
    triangle.angle = 0.0f;
    triangle.size = 25.0f;
    triangle.speed = 5.0f; // Default Triangle Speed
    triangle.numVerts = 3;

    triangle.localVerts[0] = (Vec2){ 0.0f, triangle.size };
    triangle.localVerts[1] = (Vec2){ -triangle.size * 0.866f, -triangle.size * 0.5f };
    triangle.localVerts[2] = (Vec2){  triangle.size * 0.866f, -triangle.size * 0.5f };

    updateWorldVertices(&square);
    updateWorldVertices(&triangle);
}

// ================= SAT COLLISION ENGINE =================

int checkSATCollision(Shape* a, Shape* b) {
    Shape* shapes[2] = { a, b };

    for (int s = 0; s < 2; s++) {
        Shape* current = shapes[s];
        for (int i = 0; i < current->numVerts; i++) {
            Vec2 p1 = current->worldVerts[i];
            Vec2 p2 = current->worldVerts[(i + 1) % current->numVerts];

            // Calculate perpendicular normal vector to edge
            Vec2 normal = { -(p2.y - p1.y), p2.x - p1.x };

            // Project Shape A onto normal
            float minA = a->worldVerts[0].x * normal.x + a->worldVerts[0].y * normal.y;
            float maxA = minA;
            for (int j = 1; j < a->numVerts; j++) {
                float proj = a->worldVerts[j].x * normal.x + a->worldVerts[j].y * normal.y;
                if (proj < minA) minA = proj;
                if (proj > maxA) maxA = proj;
            }

            // Project Shape B onto normal
            float minB = b->worldVerts[0].x * normal.x + b->worldVerts[0].y * normal.y;
            float maxB = minB;
            for (int j = 1; j < b->numVerts; j++) {
                float proj = b->worldVerts[j].x * normal.x + b->worldVerts[j].y * normal.y;
                if (proj < minB) minB = proj;
                if (proj > maxB) maxB = proj;
            }

            // If there's a gap between projections, no collision exists
            if (maxA < minB || maxB < minA) return 0;
        }
    }
    return 1; // Overlapping on all axes -> Collision
}

// ================= RENDERING & TEXT =================

void drawText(float x, float y, const char *text) {
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1. Draw Square (Gray)
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < square.numVerts; i++) {
        glVertex2f(square.worldVerts[i].x, square.worldVerts[i].y);
    }
    glEnd();

    // 2. Draw Triangle (Orange)
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < triangle.numVerts; i++) {
        glVertex2f(triangle.worldVerts[i].x, triangle.worldVerts[i].y);
    }
    glEnd();

    // 3. UI Overlay (Speed Control Display)
    glColor3f(0.9f, 0.9f, 0.9f);
    char buffer[100];
    sprintf(buffer, "Square Speed [1/2]: %.1f", square.speed);
    drawText(10, windowHeight - 20, buffer);

    sprintf(buffer, "Triangle Speed [3/4]: %.1f", triangle.speed);
    drawText(10, windowHeight - 40, buffer);

    // 4. Game Over Screen
    if (isGameOver) {
        glColor3f(1.0f, 0.2f, 0.2f);
        drawText(windowWidth / 2.0f - 45.0f, windowHeight / 2.0f + 10.0f, "GAME OVER!");
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(windowWidth / 2.0f - 75.0f, windowHeight / 2.0f - 15.0f, "Press 'R' to Restart");
    }

    glutSwapBuffers();
}

// ================= GAME UPDATE LOOP =================

void update(int value) {
    if (!isGameOver) {
        // Smoothly move Triangle towards target mouse position at `triangle.speed`
        float dx = triangle.targetPos.x - triangle.pos.x;
        float dy = triangle.targetPos.y - triangle.pos.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 1.0f) {
            if (dist <= triangle.speed) {
                triangle.pos = triangle.targetPos;
            } else {
                triangle.pos.x += (dx / dist) * triangle.speed;
                triangle.pos.y += (dy / dist) * triangle.speed;
            }
        }

        // Recompute world coordinate boundaries
        updateWorldVertices(&square);
        updateWorldVertices(&triangle);

        // Check SAT Collision
        if (checkSATCollision(&square, &triangle)) {
            isGameOver = 1;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // Trigger ~60 FPS update loop
}

// ================= CONTROLS & INPUT =================

void keyboardFunc(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        initGame();
        return;
    }

    // SPEED CONTROLS (Always active)
    if (key == '1') square.speed = fmaxf(1.0f, square.speed - 1.0f);
    if (key == '2') square.speed += 1.0f;
    if (key == '3') triangle.speed = fmaxf(1.0f, triangle.speed - 1.0f);
    if (key == '4') triangle.speed += 1.0f;

    if (isGameOver) return;

    // SQUARE MOVEMENT CONTROLS
    switch (key) {
        case 'w': case 'W': square.pos.y += square.speed; break;
        case 's': case 'S': square.pos.y -= square.speed; break;
        case 'a': case 'A': square.pos.x -= square.speed; break;
        case 'd': case 'D': square.pos.x += square.speed; break;
        case 'f': case 'F': square.angle += 15.0f; break; // CCW Rotation
        case 'g': case 'G': square.angle -= 15.0f; break; // CW Rotation
    }
}

void mouseFunc(int button, int state, int x, int y) {
    if (isGameOver) return;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        triangle.targetPos = (Vec2){ (float)x, (float)(windowHeight - y) };
    }
}

void mouseMotionFunc(int x, int y) {
    if (isGameOver) return;
    triangle.targetPos = (Vec2){ (float)x, (float)(windowHeight - y) };
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// ================= MAIN FUNCTION =================

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Optimized 2D Tag Game");

    initGame();
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseMotionFunc);
    glutTimerFunc(16, update, 0); // Start 60FPS loop

    glutMainLoop();
    return 0;
}

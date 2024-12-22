#define STB_IMAGE_IMPLEMENTATION
#include <GL/glut.h>
#include "stb_image.h"
#include <cstdio>
#include <ctime>
#include <cmath>

GLuint universe_Texture;
GLuint spaceShip_Texture;
GLuint bullet_Texture;
GLuint chicken_Texture;
GLuint egg_Texture;

float shapeX = 0.0f;    // Shape's horizontal position
float shapeY = -0.87f;  // Shape's vertical position
int playerHealth = 100;
int chickenHealth = 100;
int score = 0;

#define MAX_BULLETS 500 // Maximum number of bullets
#define MAX_EGGS 50     // Maximum number of eggs
#define MAX_CHICKENs 10 // Maximum number of chickens

bool spacePressed = false; // Flag to check if the space key is pressed
typedef struct {
    float x, y;
    bool active;
} Bullet;

Bullet bullets[MAX_BULLETS];
int bulletCount = 0;

typedef struct {
    float x, y;
    bool active;
} Eggs;

Eggs eggs[MAX_EGGS];
int eggsCount = 0;

typedef struct {
    float x, y;
    bool active;
} Chickens;

Chickens chickens[MAX_CHICKENs];

GLuint loadTexture(const char *filename) {
    int width, height, channels;
    unsigned char *image = stbi_load(filename, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Error: Texture file not found: %s\n", filename);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    } else if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }

    stbi_image_free(image);
    return textureID;
}

void initTextures() {
    universe_Texture = loadTexture("C:/Users/Nermeen Kamal/CLionProjects/OpenGLProject/Universe.jpeg");
    spaceShip_Texture = loadTexture("C:/Users/Nermeen Kamal/CLionProjects/OpenGLProject/spaceship.jpeg");
    bullet_Texture = loadTexture("C:/Users/Nermeen Kamal/CLionProjects/OpenGLProject/fire.jpeg");
    chicken_Texture = loadTexture("C:/Users/Nermeen Kamal/CLionProjects/OpenGLProject/chicken.png");
    egg_Texture = loadTexture("C:/Users/Nermeen Kamal/CLionProjects/OpenGLProject/egg.jpeg");
}

void initChickens() {
    for (int i = 0; i < MAX_CHICKENs; i++) {
        chickens[i].x = float(rand() % 200 - 100) / 100.0f;
        chickens[i].y = 0.5f;
        chickens[i].active = true;
    }
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initTextures();
    srand(time(NULL));
    initChickens();
}

void drawTexturedUniverse() {
    glBindTexture(GL_TEXTURE_2D, universe_Texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void drawSpaceShip() {
    glBindTexture(GL_TEXTURE_2D, spaceShip_Texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(shapeX - 0.1f, shapeY - 0.1f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(shapeX + 0.1f, shapeY - 0.1f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(shapeX + 0.1f, shapeY + 0.1f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(shapeX - 0.1f, shapeY + 0.1f);
    glEnd();
}


void drawChickens() {
    for (int i = 0; i < MAX_CHICKENs; i++) {
        if (chickens[i].active) {
            glBindTexture(GL_TEXTURE_2D, chicken_Texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(chickens[i].x - 0.1f, chickens[i].y - 0.1f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(chickens[i].x + 0.1f, chickens[i].y - 0.1f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(chickens[i].x + 0.1f, chickens[i].y + 0.1f);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(chickens[i].x - 0.1f, chickens[i].y + 0.1f);
            glEnd();

            // تحديد اتجاه الحركة بناءً على موقع الفراخ
            if (i < MAX_CHICKENs / 2) { // نصف الفراخ الأولى تتحرك من اليسار إلى اليمين
                chickens[i].x += 0.005f; // حركة إلى اليمين
                if (chickens[i].x + 0.1f >= 1.0f) { // إذا وصلت إلى الحد الأيمن
                    chickens[i].x = -1.0f + 0.1f; // العودة إلى أقصى اليسار
                }
            } else { // نصف الفراخ الثانية تتحرك من اليمين إلى اليسار
                chickens[i].x -= 0.005f; // حركة إلى اليسار
                if (chickens[i].x - 0.1f <= -1.0f) { // إذا وصلت إلى الحد الأيسر
                    chickens[i].x = 1.0f - 0.1f; // العودة إلى أقصى اليمين
                }
            }

            // حركة عشوائية قليلة (اختياري) لإطلاق البيض
            if (rand() % 200 < 1) {
                eggs[eggsCount++] = {chickens[i].x, chickens[i].y - 0.1f, true};
            }
        }
    }
}


void drawEggs() {
    for (int i = 0; i < eggsCount; i++) {
        if (eggs[i].active) {
            glBindTexture(GL_TEXTURE_2D, egg_Texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(eggs[i].x - 0.02f, eggs[i].y - 0.02f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(eggs[i].x + 0.02f, eggs[i].y - 0.02f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(eggs[i].x + 0.02f, eggs[i].y + 0.02f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(eggs[i].x - 0.02f, eggs[i].y + 0.02f);
            glEnd();

            eggs[i].y -= 0.01f; // حركة البيض للأسفل

            // تحقق من الاصطدام مع المركبة الفضائية
            if (fabs(eggs[i].x - shapeX) < 0.1f && fabs(eggs[i].y - shapeY) < 0.1f) {
                eggs[i].active = false;
                playerHealth -= 10;

                // التأكد من أن الصحة لا تقل عن صفر
                if (playerHealth < 0) {
                    playerHealth = 0;
                }
            }

            if (eggs[i].y < -1.0f) {
                eggs[i].active = false; // تعطيل البيض الذي يخرج من الشاشة
            }
        }
    }
}

void drawBullets() {
    for (int i = 0; i < bulletCount; i++) {
        if (bullets[i].active) {
            glBindTexture(GL_TEXTURE_2D, bullet_Texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(bullets[i].x - 0.02f, bullets[i].y - 0.02f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(bullets[i].x + 0.02f, bullets[i].y - 0.02f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(bullets[i].x + 0.02f, bullets[i].y + 0.02f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(bullets[i].x - 0.02f, bullets[i].y + 0.02f);
            glEnd();

            bullets[i].y += 0.02f;
            if (bullets[i].y > 1.0f) bullets[i].active = false;

            // Check collision with chickens
            for (int j = 0; j < MAX_CHICKENs; j++) {
                if (chickens[j].active && fabs(bullets[i].x - chickens[j].x) < 0.1f && fabs(bullets[i].y - chickens[j].y) < 0.1f) {
                    bullets[i].active = false;
                    chickens[j].active = false;
                    score += 10;

                    // التأكد من أن النقاط لا تقل عن صفر
                    if (score < 0) {
                        score = 0;
                    }
                }
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // رسم الخلفية
    drawTexturedUniverse();

    // رسم السفينة
    drawSpaceShip();

    // رسم الرصاصات
    drawBullets();

    // رسم الفراخ
    drawChickens();

    // رسم البيض
    drawEggs();

    // تعطيل الـ texturing أثناء رسم النص
    glDisable(GL_TEXTURE_2D);

    // عرض النقاط والصحة بعد رسم كل شيء
    char info[64];
    sprintf(info, "Score: %d | Health: %d", score, playerHealth);
    glColor3f(1.0f, 1.0f, 1.0f);  // تحديد اللون الأبيض للنص
    glRasterPos2f(-0.99f, -0.7f);  // تحديد موضع النص
    for (char *c = info; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // رسم النص
    }

    // تحقق من فوز السفينة أو خسارتها
    bool allChickensDead = true;
    for (int i = 0; i < MAX_CHICKENs; i++) {
        if (chickens[i].active) {
            allChickensDead = false;
            break;
        }
    }

    if (allChickensDead && playerHealth > 0) {
        // إذا كانت الفراخ كلها ميتة والسفينة لا تزال على قيد الحياة
        char winMessage[] = "You Win!";
        glColor3f(0.0f, 0.8f, 0.3f); // اللون الأحمر لرسالة الفوز
        glRasterPos2f(-0.1f, 0.0f);  // تحديد موضع النص
        for (char *c = winMessage; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // رسم النص
        }
    } else if (playerHealth <= 0) {
        // إذا كانت صحة السفينة صفر أو أقل
        char loseMessage[] = "You Lose!";
        glColor3f(0.7f, 0.0f, 0.3f);
        glRasterPos2f(-0.1f, 0.0f);  // تحديد موضع النص
        for (char *c = loseMessage; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // رسم النص
        }
    }

    // إعادة تمكين الـ texturing بعد رسم النص
    glEnable(GL_TEXTURE_2D);

    glutSwapBuffers();  // التبديل بين ال buffer الأمامي والخلفي
}


void handleKeys(unsigned char key, int x, int y) {
    float speed = 0.05f;  // سرعة الحركة

    switch (key) {
        case 'a':
            if (shapeX - speed > -1.0f) {  // تأكد من أن السفينة لا تخرج عن الحد الأيسر
                shapeX -= speed;
            }
            break;
        case 'd':
            if (shapeX + speed < 1.0f) {  // تأكد من أن السفينة لا تخرج عن الحد الأيمن
                shapeX += speed;
            }
            break;
        case 32:  // مفتاح المسافة
            if (bulletCount < MAX_BULLETS) {
                bullets[bulletCount++] = {shapeX, shapeY + 0.1f, true};
            }
            break;
    }
    glutPostRedisplay();
}

void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // تحديث كل 16 مللي ثانية (حوالي 60 إطارًا في الثانية)
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Chicken Invaders");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}

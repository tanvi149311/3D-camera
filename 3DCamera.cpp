//
// Name: Tanvi R Pawale
// this program provides a view of 3d interactive camera
// It has two modes 1. First person mode 2. Focus mode
// First person mode : user can do camera rotation using mouse and move forward, back, left, right using W, S, A, D
// Focus mode: user can pan, zoom and rotate using mouse
//


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <string>
#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

glm::vec3 Cam_Eye, n, Cam_lookAt, Cam_Up, temp_pos, Cam_U, Cam_V;
GLfloat R_speed = 0.01f;
GLfloat move_speed = 4.0f;

int g_winWidth = 1024, g_winHeight = 512, prevX = 0, prevY = 0, mode = 1;
char mouse = 'L';
float radius = 17.32, yaw = -45.0, pitch = 30.0;
char mode2_name[] = "FOCUS MODE";
char mode1_name[] = "FP MODE";
char temp[12];
bool keyStatus[256];
bool flag = false;

// Resizes the canvas window
void ReSize(int w, int h)
{
    g_winWidth = w;
    g_winHeight = h;
    glViewport(0, 0, w, h);
}

void initialGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderBitmapString(float x, float y, void *font, const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

//initializes keyStatus of 256 keys, eye vector, lookAt position and up
void initialization()
{
    for(int i =0; i<256; i++)
        keyStatus[i] = false;
    
    Cam_Eye = glm::vec3(10.0f, 10.0f, -10.0f);
    Cam_lookAt[0] = Cam_Eye[0] - radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    Cam_lookAt[1] = Cam_Eye[1] - radius * sin(glm::radians(pitch));
    Cam_lookAt[2] = Cam_Eye[2] - radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    n = Cam_lookAt - Cam_Eye;
    Cam_Up = glm::vec3(0.0f, 1.0f, 0.0f);
}

//draws coordinate system and names x, y, z axis
void drawCS()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(10.0f, 10.0f, 10.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    
    //axis x
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.3f, 0.0f, 0.0f);
    
    //text x
    glVertex3f(0.4f, 0.03f, 0.0f);
    glVertex3f(0.43f, -0.03f, 0.0f);
    glVertex3f(0.4f, -0.03f, 0.0f);
    glVertex3f(0.43f, 0.03f, 0.0f);
    
    //axis y
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.3f, 0.0f);
    
    //text y
    glVertex3f(0.0f, 0.5f, 0.0f);
    glVertex3f(0.0f, 0.4f, 0.0f);
    glVertex3f(-0.05f, 0.55f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glVertex3f(0.05f, 0.55f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    
    //axis z
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.3f);
    
    //text z
    glVertex3f(-0.025f, 0.025f, 0.4f);
    glVertex3f(0.025f, 0.025f, 0.4f);
    glVertex3f(0.025f, 0.025f, 0.4f);
    glVertex3f(-0.025f, -0.025f, 0.4f);
    glVertex3f(-0.025f, -0.025f, 0.4f);
    glVertex3f(0.025f, -0.025f, 0.4f);
    
    glEnd();
    glLineWidth(1.0f);
    
    glPopMatrix();
}

//function to draw grid
void drawGrid()
{
    int size = 25;
    
    if (size % 2 != 0)
        ++size;
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glBegin(GL_LINES);
    
    for (int i = 0; i < size + 1; i++)
    {
        if ((float)i == size / 2.0f)
            glColor3f(0.0f, 0.0f, 0.0f);
        else
            glColor3f(0.8f, 0.8f, 0.8f);
        
        glVertex3f(-size / 2.0f, 0.0f, -size / 2.0f + i);
        glVertex3f(size / 2.0f, 0.0f, -size / 2.0f + i);
        glVertex3f(-size / 2.0f + i, 0.0f, -size / 2.0f);
        glVertex3f(-size / 2.0f + i, 0.0f, size / 2.0f);
    }
    
    glEnd();
    glPopMatrix();
}

// display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(Cam_Eye[0], Cam_Eye[1], Cam_Eye[2], Cam_lookAt[0], Cam_lookAt[1], Cam_lookAt[2], Cam_Up[0], Cam_Up[1], Cam_Up[2]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, g_winWidth / g_winHeight, 0.1, 100);
    
    drawGrid();
    drawCS();
    
    // if mode 1 displays "FP MODE" and mode 2 displays "FOCUS MODE"
    if(mode == 1)
    {
        for (int i =0; i <8; i++)
            temp[i] = mode1_name[i];
    }
    else if(mode == 2)
    {
        for (int i =0; i <10; i++)
            temp[i] = mode2_name[i];
    }
    
    //function to display text
    renderBitmapString(0, 0, GLUT_BITMAP_TIMES_ROMAN_24 , temp);
    
    glutSwapBuffers();
}

//fuction mouse click which takes action according to which button is clicked on mouse
void Mouse_click(int button, int state, int x, int y)
{
    prevX = x;
    prevY = y;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (mode == 2)
            mouse = 'L';
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        if (mode == 2)
            mouse = 'R';
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
    {
        if (mode == 2)
            mouse = 'S';
    }
    
    glutPostRedisplay();
}

// mouse motion detecting function
void motion(int x, int y)
{
    if (mode == 1)
    {
        yaw += (x - prevX) * R_speed;
        pitch += (y - prevY) * R_speed;
        // vertical rotation limited to 90 up and 90 down
        if (pitch > 89.9f)
            pitch = 89.9f;
        if (pitch < -89.9f)
            pitch = -89.9f;
        
        Cam_lookAt[0] = Cam_Eye[0] - radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        Cam_lookAt[1] = Cam_Eye[1] - radius * sin(glm::radians(pitch));
        Cam_lookAt[2] = Cam_Eye[2] - radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        prevX = x;
        prevY = y;
    }
    else if (mode == 2  && flag == true)
    {
        R_speed = 0.05f;
        yaw += (x - prevX) * R_speed;
        pitch += (y - prevY) * R_speed;
    
        if (mouse == 'L' && (keyStatus[90] ==true || keyStatus[122] ==true))
        {
            Cam_Eye[0] = Cam_lookAt[0] + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            Cam_Eye[1] = Cam_lookAt[1] + radius * sin(glm::radians(pitch));
            Cam_Eye[2] = Cam_lookAt[2] + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        }
        else if (mouse == 'R' && (keyStatus[90] ==true || keyStatus[122] ==true))
        {
            if (x > prevX)
            {
                Cam_Eye += 0.05f * glm::vec3(n);
                radius = glm::distance(Cam_lookAt, temp_pos);
            }
            else if (x < prevX)
            {
                Cam_Eye -= 0.05f * glm::vec3(n);
                radius = glm::distance(Cam_lookAt, temp_pos);
            }
        }
        else if (mouse == 'S' && (keyStatus[90] == true || keyStatus[122] == true))
        {
            GLfloat rotation_x = (x - prevX) * 0.05;
            GLfloat rotation_y = (y - prevY) * 0.05;
            
            Cam_U = glm::cross(Cam_Up, n);
            Cam_V = glm::cross(n, Cam_U);
            Cam_Eye += glm::normalize(Cam_U) * rotation_x;
            Cam_Eye += glm::normalize(Cam_V) * rotation_y;
            Cam_lookAt += glm::normalize(Cam_U) * rotation_x;
            Cam_lookAt += glm::normalize(Cam_V) * rotation_y;
        }
        prevX = x;
        prevY = y;
    }

    glutPostRedisplay();
}

// keyboard function takes action related to which key is pressed
void keyBoard(unsigned char key, int x, int y)
{
   
    keyStatus[key] = true;
    switch (key)
    {
        case 27:
            exit(0);
            break;
        
            
        case 99: //c key
            if (mode == 1)
                mode = 2;
            else if (mode == 2)
                mode = 1;
            break;
            
        case 97: //a key
            if (mode == 1)
                Cam_Eye -= glm::normalize(glm::cross(n, Cam_Up)) * move_speed;
            break;
            
        case 119: //w key
            if (mode == 1)
                Cam_Eye += move_speed * glm::normalize(n);
            break;
            
        case 100: //d key
            if (mode == 1)
                Cam_Eye += glm::normalize(glm::cross(n, Cam_Up)) * move_speed;
            break;
            
        case 115: //s key
            if (mode == 1)
                Cam_Eye -= move_speed * glm::normalize(n);
            break;
        
        case 105: // i key
            if(mode == 1)
            {
                if(pitch < -89.0f)
                    pitch = -89.0f;
                else
                    pitch -= (y - prevY) * R_speed;
            }
            break;
        
        case 106: // j key
            if (mode == 1)
            {
                if(yaw < -360.0f)
                    yaw = 0;
                else
                    yaw -= (x - prevX) * R_speed;
            }
            break;

        case 107: // k key
            if(mode == 1)
            {
                if(pitch > 89.0f)
                    pitch = 89.0f;
                else
                    pitch += (y - prevY) * R_speed;
                break;
            }

        case 108: // l key
            if(mode == 1)
            {
                if(yaw > 360.0f)
                    yaw = 0;
                else
                    yaw += (x - prevX) * R_speed;
            }
            break;

        case 122: //z key
            flag = true;
            break;
            
    }
    
    Cam_lookAt[0] = Cam_Eye[0] - radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    Cam_lookAt[1] = Cam_Eye[1] - radius * sin(glm::radians(pitch));
    Cam_lookAt[2] = Cam_Eye[2] - radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    glutPostRedisplay();
}

// to make key status null
void keyboardUp(unsigned char key, int x, int y)
{
    keyStatus[key] = false;
    flag = false;
}

//main function
int main(int argc, char *argv[])
{
    // Initializes the GLUT librarotation_y
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    
    // Sets the initial dimensions of the canvas window
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutInitWindowPosition(0, 0);
    
    // Creates the canvas window with the specified dimensions
    glutCreateWindow("3D Camera");
    initialGL();
    
    //Function call to Resize and display functions
    glutReshapeFunc(ReSize);
    glutDisplayFunc(display);
   
    // cout<<prevX<<" "<<prevY<<endl;
    glutMouseFunc(Mouse_click);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyBoard);
    glutKeyboardUpFunc(keyboardUp);
    initialization();
    
    glutMainLoop();
    return EXIT_SUCCESS;
}

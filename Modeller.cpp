#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <time.h>
#include <limits>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include "Object.h"
#include <vector>

float camPos[] = {100, 100, 100};
int selectedObj = -1;    // -1 means no object is selected, other numbers indicate the index of vector objects
int selectedLight = -1;  // -1 means no light is selected, 0: LIGHT0; 1: LIGHT0
int selectedAxis = -1;   // -1: no selected; 0: X axis; 1: Y axis; 2: Z axis
int nextType = 1;        // memory next shape to create
int currentMaterial = 1; // 1: copper; 2: silver; 3: emerald; 4: pearl; 5: obsidian

/* Scene's position and rotation */
float sceneX = 0;
float sceneY = 0;
float sceneZ = 0;
float angleY = 0.0;
float angleZ = 0.0;

const double SPEED_ROTATE = 2.0;
const double SPEED_TRANSLATE = 1.0;
const double SPEED_SCALE = 1;

const double SIZE_BOX = 6.0;

std::vector<Object> objects;

/* Lighting */
// LIGHT0
float light_pos0[] = {30, 60, 70, 1};
float amb0[] = {0.3, 0.5, 0.5, 1};
float diff0[] = {0.2, 0.2, 0.2, 1};
float spec0[] = {0.5, 0.5, 0.5, 1};
// LIGHT1
float light_pos1[] = {60, 20, 20, 1};
float amb1[] = {0.5, 0.5, 0.5, 1};
float diff1[] = {0.2, 0.2, 0.2, 1};
float spec1[] = {0.5, 0.5, 0.5, 1};

// struct
struct Point
{
    double x;
    double y;
    double z;
} m_start, m_end;

/* Reset all configurations */
void reset()
{
    // objects
    objects.clear();
    nextType = 1;
    currentMaterial = 1;
    // scene's rotation and position
    angleY = 0;
    angleZ = 0;
    sceneX = 0;
    sceneY = 0;
    sceneZ = 0;
    // camera's position
    camPos[0] = 100;
    camPos[1] = 100;
    camPos[2] = 100;
    // lights' position
    light_pos0[0] = 30;
    light_pos0[1] = 60;
    light_pos0[2] = 70;
    light_pos1[0] = 60;
    light_pos1[1] = 20;
    light_pos1[2] = 20;

    selectedAxis = -1;
    selectedObj = -1;
    selectedLight = -1;
}

/* Draw three axes, the selected axis will be red*/
void drawAxes()
{
    glBegin(GL_LINES);
    // axis x
    if (selectedAxis == 0)
        glColor3f(0, 1, 1);
    else
        glColor3f(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(SIZE_BOX * 3, 0, 0);
    // axos y
    if (selectedAxis == 1)
        glColor3f(0, 1, 1);
    else
        glColor3f(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, SIZE_BOX * 3, 0);
    // axis z
    if (selectedAxis == 2)
        glColor3f(0, 1, 1);
    else
        glColor3f(0, 0, 1);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, SIZE_BOX * 3);
    glEnd();

    // arrow in x
    glPushMatrix();
    glTranslatef(SIZE_BOX * 3, 0, 0);
    glRotatef(90, 0, 1, 0);
    if (selectedAxis == 0)
        glColor3f(0, 1, 1);
    else
        glColor3f(1, 0, 0);
    glutSolidCone(0.5, 1, 50, 50);
    glPopMatrix();
    // arrow in y
    glPushMatrix();
    glTranslatef(0, SIZE_BOX * 3, 0);
    glRotatef(-90, 1, 0, 0);
    if (selectedAxis == 1)
        glColor3f(0, 1, 1);
    else
        glColor3f(0, 1, 0);
    glutSolidCone(0.5, 1, 50, 50);
    glPopMatrix();
    // arrow in z
    glPushMatrix();
    glTranslatef(0, 0, SIZE_BOX * 3);
    if (selectedAxis == 2)
        glColor3f(0, 1, 1);
    else
        glColor3f(0, 0, 1);
    glutSolidCone(0.5, 1, 50, 50);
    glPopMatrix();
}

/* Write file with all data */
void save(std::string f)
{
    std::string filename = f + ".txt";
    const char *c_filename = filename.c_str();
    FILE *file = std::fopen(c_filename, "w");

    // scene
    fprintf(file, "%g %g %g %g %g\n", sceneX, sceneY, sceneZ, angleY, angleZ);
    // camera
    fprintf(file, "%g %g %g\n", camPos[0], camPos[1], camPos[2]);
    // lights
    fprintf(file, "%g %g %g %g %g %g\n", light_pos0[0], light_pos0[1], light_pos0[2], light_pos1[0], light_pos1[1], light_pos1[2]);

    // objects
    fprintf(file, "%d\n", objects.size());

    for (int i = 0; i < objects.size(); i++)
    {
        Object o = objects.at(i);

        fprintf(file, "%d %d %g %g %g %g %g %g %g %g %g\n", o.getType(), o.getMaterial(), o.getX(), o.getY(), o.getZ(), o.getRotateX(), o.getRotateY(), o.getRotateZ(), o.getScaleX(), o.getScaleY(), o.getScaleZ());
    }
    fclose(file);
    std::cout << "The file named " << filename << " is saved successfully.\n"
              << std::endl;
}

/* Read file and restore data */
void read(std::string f)
{
    reset();
    std::string filename = f + ".txt";
    const char *c_filename = filename.c_str();
    FILE *file = std::fopen(c_filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    // scene
    fscanf(file, "%f %f %f %f %f\n", &sceneX, &sceneY, &sceneZ, &angleY, &angleZ);
    // camera
    fscanf(file, "%f %f %f\n", &camPos[0], &camPos[1], &camPos[2]);
    // lights
    fscanf(file, "%f %f %f %f %f %f\n", &light_pos0[0], &light_pos0[1], &light_pos0[2], &light_pos1[0], &light_pos1[1], &light_pos1[2]);
    // objects
    int num;
    fscanf(file, "%d\n", &num);

    for (int i = 0; i < num; i++)
    {
        int type, material;
        float x, y, z, rx, ry, rz, sx, sy, sz;

        fscanf(file, "%d %d %f %f %f %f %f %f %f %f %f\n", &type, &material, &x, &y, &z, &rx, &ry, &rz, &sx, &sy, &sz);
        // create new object and set data then push back to the vector objects
        Object newObject(type, material);
        newObject.setX(x);
        newObject.setY(y);
        newObject.setZ(z);
        newObject.setRotateX(rx);
        newObject.setRotateY(ry);
        newObject.setRotateZ(rz);
        newObject.setScaleX(sx);
        newObject.setScaleY(sy);
        newObject.setScaleZ(sz);
        objects.push_back(newObject);
    }

    fclose(file);
    glutPostRedisplay();
}

/* To make codes simple and tidy, set all parameters of materials here */
void setMaterial(int i)
{
    float m_amb[4];
    float m_diff[4];
    float m_spec[4];
    float shiny;
    switch (i)
    {
    case 1: // copper
        m_amb[0] = 0.19125;
        m_amb[1] = 0.0735;
        m_amb[2] = 0.0225;
        m_diff[0] = 0.7038;
        m_diff[1] = 0.27048;
        m_diff[2] = 0.0828;
        m_spec[0] = 0.256777;
        m_spec[1] = 0.137622;
        m_spec[2] = 0.086014;
        shiny = 0.3;
        break;
    case 2: // silver
        m_amb[0] = 0.19225;
        m_amb[1] = 0.19225;
        m_amb[2] = 0.19225;
        m_diff[0] = 0.50754;
        m_diff[1] = 0.50754;
        m_diff[2] = 0.50754;
        m_spec[0] = 0.508273;
        m_spec[1] = 0.508273;
        m_spec[2] = 0.508273;
        shiny = 0.4;
        break;
    case 3: // emerald
        m_amb[0] = 0.0215;
        m_amb[1] = 0.1745;
        m_amb[2] = 0.0215;
        m_diff[0] = 0.07568;
        m_diff[1] = 0.61424;
        m_diff[2] = 0.07568;
        m_spec[0] = 0.633;
        m_spec[1] = 0.727811;
        m_spec[2] = 0.633;
        shiny = 0.6;
        break;
    case 4: // pearl
        m_amb[0] = 0.25;
        m_amb[1] = 0.20725;
        m_amb[2] = 0.20725;
        m_diff[0] = 1;
        m_diff[1] = 0.829;
        m_diff[2] = 0.829;
        m_spec[0] = 0.296648;
        m_spec[1] = 0.296648;
        m_spec[2] = 0.296648;
        shiny = 0.088;
        break;
    case 5: // obsidian
        m_amb[0] = 0.05375;
        m_amb[1] = 0.05;
        m_amb[2] = 0.06625;
        m_diff[0] = 0.18275;
        m_diff[1] = 0.17;
        m_diff[2] = 0.22525;
        m_spec[0] = 0.332741;
        m_spec[1] = 0.328634;
        m_spec[2] = 0.346435;
        shiny = 0.3;
        break;
    case 6: // turquoise (for lights)
        m_amb[0] = 0.1;
        m_amb[1] = 0.18725;
        m_amb[2] = 0.1745;
        m_diff[0] = 0.396;
        m_diff[1] = 0.74151;
        m_diff[2] = 0.69102;
        m_spec[0] = 0.297254;
        m_spec[1] = 0.30829;
        m_spec[2] = 0.306678;
        shiny = 0.1;
        break;
    case 7: // jade (for floor and walls)
        m_amb[0] = 0.135;
        m_amb[1] = 0.2225;
        m_amb[2] = 0.1575;
        m_diff[0] = 0.54;
        m_diff[1] = 0.89;
        m_diff[2] = 0.63;
        m_spec[0] = 0.316228;
        m_spec[1] = 0.316228;
        m_spec[2] = 0.316228;
        shiny = 0.3;
        break;
    }
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny * 128.0);
}

void drawLights(void)
{
    glPushMatrix();
    glTranslatef(light_pos0[0], light_pos0[1], light_pos0[2]);
    if (selectedLight == 0)
    {
        glDisable(GL_LIGHTING);
        drawAxes();
        glColor3f(0, 1, 1);
        glutWireCube(2);
        glEnable(GL_LIGHTING);
    }
    glutSolidSphere(1, 50, 50);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(light_pos1[0], light_pos1[1], light_pos1[2]);
    if (selectedLight == 1)
    {
        glDisable(GL_LIGHTING);
        drawAxes();
        glColor3f(0, 1, 1);
        glutWireCube(2);
        glEnable(GL_LIGHTING);
    }
    glutSolidSphere(1, 50, 50);
    glPopMatrix();
}

void drawPlane(void)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    // left
    glNormal3f(0, 1, 0);
    glVertex3d(0, 0, 0);
    glNormal3f(0, 1, 0);
    glVertex3d(0, 0, 100);
    glNormal3f(0, 1, 0);
    glVertex3d(100, 0, 100);
    glNormal3f(0, 1, 0);
    glVertex3d(100, 0, 0);
    // back
    glNormal3f(1, 0, 0);
    glVertex3d(0, 0, 0);
    glNormal3f(1, 0, 0);
    glVertex3d(0, 100, 0);
    glNormal3f(1, 0, 0);
    glVertex3d(0, 100, 100);
    glNormal3f(1, 0, 0);
    glVertex3d(0, 0, 100);
    // floor
    glNormal3f(0, 0, 1);
    glVertex3d(0, 0, 0);
    glNormal3f(0, 0, 1);
    glVertex3d(100, 0, 0);
    glNormal3f(0, 0, 1);
    glVertex3d(100, 100, 0);
    glNormal3f(0, 0, 1);
    glVertex3d(0, 100, 0);
    glEnd();
}

/* Add an object of the shape in order of cube, sphere, cone, torus and dodecahedron in random position */
void addObject(int type)
{
    // set default type and meterial according to nextType, different type have different default material
    Object newObject(type, type);
    /**
     * Set random place 
     * 
     * with srand, to avoid objects created at the same position, click maximum once per second
     */
    srand(time(NULL));
    int rx, ry, rz;
    rx = rand() % 50 + 25;
    ry = rand() % 50 + 25;
    rz = rand() % 50 + 25;
    newObject.setX(rx);
    newObject.setY(ry);
    newObject.setZ(rz);

    objects.push_back(newObject);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 0, 1);

    /* Lighting */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    // light 1
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
    // light 2
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);

    glShadeModel(GL_SMOOTH);
    glPushMatrix();

    glPushMatrix();
    if (selectedObj != -1)
    {
        glColor3f(1, 0, 0);
        glDisable(GL_LIGHTING);
        glRasterPos3i(40, 5, 70);
        std::string test = "Position: ";
        Object o = objects.at(selectedObj);
        test += std::to_string((int)o.getX()) + " " + std::to_string((int)o.getY()) + " " + std::to_string((int)o.getZ()) + "   ";
        test += "Rotation: " + std::to_string((int)o.getRotateX()) + " " + std::to_string((int)o.getRotateY()) + " " + std::to_string((int)o.getRotateZ()) + "   ";
        test += "Scale: " + std::to_string((int)o.getScaleX()) + " " + std::to_string((int)o.getScaleY()) + " " + std::to_string((int)o.getScaleZ());
        for (std::string::iterator i = test.begin(); i != test.end(); ++i)
        {
            char c = *i;
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        }
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();

    // to rotate the scene about its center, translate it so that the center is in the origin then rotate then translate back
    glTranslatef(50, 50, 50);
    glRotatef(angleY, 0, 1, 0);
    glRotatef(angleZ, 0, 0, 1);
    glTranslatef(-50, -50, -50);

    /* draw planes */
    setMaterial(7);
    drawPlane();
    /* draw lights */
    setMaterial(6);
    drawLights();

    /* enable backface culling */
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        Object o = objects[i];
        glPushMatrix();
        glTranslatef(o.getX(), o.getY(), o.getZ());

        glDisable(GL_LIGHTING); // disable lighting for axes and wire box

        glRotated(o.getRotateX(), 1, 0, 0);
        glRotated(o.getRotateY(), 0, 1, 0);
        glRotated(o.getRotateZ(), 0, 0, 1);
        if (i == selectedObj)
            drawAxes();
        glScaled(o.getScaleX(), o.getScaleY(), o.getScaleZ());
        if (i == selectedObj)
        {
            glColor3f(0, 1, 1);
            glutWireCube(SIZE_BOX); // for simplying ray-box testing, don't rotate this wire box
        }

        glEnable(GL_LIGHTING); //  enble lighting for objects and planes
        setMaterial(o.getMaterial());
        switch (o.getType())
        {
        case 1: // Cube
            glutSolidCube(5);
            break;
        case 2: // Sphere
            glutSolidSphere(3, 50, 50);
            break;
        case 3: // Cone
            glTranslatef(0, 0, -2);
            glutSolidCone(3, 5, 50, 50);
            break;
        case 4: // Torus
            glutSolidTorus(1, 2, 50, 50);
            break;
        case 5: // Dodecahedron
            glScalef(2, 2, 2);
            glutSolidDodecahedron();
            break;
        }
        glPopMatrix();
    }
    glPopMatrix();
    glutSwapBuffers();
}

// a lot of printing warning to make sure some keys work
void keyboard(unsigned char key, int xIn, int yIn)
{
    switch (key)
    {
    case 'q':
    case 27:
        exit(0);
        break;
    case ' ':
        addObject(nextType);
        nextType = nextType == 5 ? 1 : nextType + 1;
        selectedObj = objects.size() - 1;
        break;
    case 'r':
        reset();
        break;
    case 's':
    case 'S':
    {
        std::string filename;
        std::cout << "Please enter the filename: \n";
        getline(std::cin, filename);
        save(filename);
        break;
    }
    case 'l':
    case 'L':
    {
        std::string filename;
        std::cout << "Please enter the filename: \n";
        getline(std::cin, filename);
        read(filename);
        break;
    }
    // delete selected object if there is a selected object, else print warning
    case 'd':
        if (selectedObj != -1)
        {
            objects.erase(objects.begin() + selectedObj);
            selectedObj = -1;
        }
        else
            printf("[WARNING] You need to select an object to delete.\n");
        break;
    // select an axis of the selected object
    case 'x':
        if (selectedObj != -1 || selectedLight != -1)
            selectedAxis = selectedAxis == 0 ? -1 : 0; // if x axis is already selected, unselect it
        else
            printf("[WARNING] You need to select an object to select axis.\n");
        break;
    case 'y':
        if (selectedObj != -1 || selectedLight != -1)
            selectedAxis = selectedAxis == 1 ? -1 : 1; // if y axis is already selected, unselect it
        else
            printf("[WARNING] You need to select an object to select axis.\n");
        break;
    case 'z':
        if (selectedObj != -1 || selectedLight != -1)
            selectedAxis = selectedAxis == 2 ? -1 : 2; // if z axis is already selected, unselect it
        else
            printf("[WARNING] You need to select an object to select axis.\n");
        break;
    // rotate selected light or selected object
    case '+':
        if (selectedObj == -1)
            printf("[WARNING] You need to select an object to scale.\n");
        else
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to scale.\n");
            else if (selectedAxis == 0)
            {
                double s = objects.at(selectedObj).getScaleX();
                if (s < 5.0)
                    objects.at(selectedObj).setScaleX(s + SPEED_SCALE);
                else
                    printf("[WARNING] The selected object cannot be bigger any more in the direction x.\n");
            }
            else if (selectedAxis == 1)
            {
                double s = objects.at(selectedObj).getScaleY();
                if (s < 5.0)
                    objects.at(selectedObj).setScaleY(s + SPEED_SCALE);
                else
                    printf("[WARNING] The selected object cannot be bigger any more in the direction y.\n");
            }
            else if (selectedAxis == 2)
            {
                double s = objects.at(selectedObj).getScaleZ();
                if (s < 5.0)
                    objects.at(selectedObj).setScaleZ(s + SPEED_SCALE);
                else
                    printf("[WARNING] The selected object cannot be bigger any more in the direction z.\n");
            }
        }
        break;
    case '-':
        if (selectedObj == -1)
            printf("[WARNING] You need to select an object to scale.\n");
        else
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to scale.\n");
            else if (selectedAxis == 0)
            {
                double s = objects.at(selectedObj).getScaleX();
                if (s > 1)
                {
                    objects.at(selectedObj).setScaleX(s - SPEED_SCALE);
                }
                else
                    printf("[WARNING] The selected object cannot be smaller any more in the direction x.\n");
            }
            else if (selectedAxis == 1)
            {
                double s = objects.at(selectedObj).getScaleY();
                if (s > 1)
                {
                    objects.at(selectedObj).setScaleY(s - SPEED_SCALE);
                }
                else
                    printf("[WARNING] The selected object cannot be smaller any more in the direction y.\n");
            }
            else if (selectedAxis == 2)
            {
                double s = objects.at(selectedObj).getScaleZ();
                if (s > 1)
                {
                    objects.at(selectedObj).setScaleZ(s - SPEED_SCALE);
                }
                else
                    printf("[WARNING] The selected object cannot be smaller any more in the direction z.\n");
            }
        }
        break;
    // set material
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        currentMaterial = (int)(key - '0');
        break;
    case 'm':
        if (selectedObj != -1)
        {
            objects.at(selectedObj).setMaterial(currentMaterial);
        }
        else
        {
            printf("[WARNING] You need to select an object to set material.\n");
        }
        break;
    // rotate scene in y
    case 'u':
        if (angleY < 10)
            angleY += SPEED_ROTATE;
        break;
    case 'i':
        if (angleY > -10)
            angleY -= SPEED_ROTATE;
        break;
    // rotate scene in z
    case 'o':
        if (angleZ < 10)
            angleZ += SPEED_ROTATE;
        break;
    case 'p':
        if (angleZ > -10)
            angleZ -= SPEED_ROTATE;
        break;
    // translate camera's position in x
    case 'g':
        if (camPos[0] < 150)
            camPos[0] += SPEED_TRANSLATE;
        break;
    case 'h':
        if (camPos[0] > 0)
            camPos[0] -= SPEED_TRANSLATE;
        break;
    // translate camera's position in y
    case 'j':
        if (camPos[1] < 150)
            camPos[1] += SPEED_TRANSLATE;
        break;
    case 'k':
        if (camPos[1] > 0)
            camPos[1] -= SPEED_TRANSLATE;
        break;
    // translate camera's position in z
    case 'b':
        if (camPos[2] > 0)
            camPos[2] -= SPEED_TRANSLATE;
        break;
    case 'n':
        if (camPos[2] < 150)
            camPos[2] += SPEED_TRANSLATE;
        break;
    }
    glutPostRedisplay();
}

// arrow up/down: translate; arrow left/right: rotate
// a lot of printing warning to make sure arrow keys work
void special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        if (selectedObj == -1 && selectedLight == -1)
            printf("[WARNING] You need to select an object or a light to translate.\n");
        else if (selectedObj == -1 && selectedLight == 0)
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to translate the selected light.\n");
            else if (selectedAxis == 0)
            {
                if (light_pos0[0] < 90)
                    light_pos0[0] += 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction +x any more.\n");
            }
            else if (selectedAxis == 1)
            {
                if (light_pos0[1] < 90)
                    light_pos0[1] += 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction +y any more.\n");
            }
            else if (selectedAxis == 2)
            {
                if (light_pos0[2] < 90)
                    light_pos0[2] += 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction +z any more.\n");
            }
        }
        else if (selectedObj == -1 && selectedLight == 1)
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to translate the selected light.\n");
            else if (selectedAxis == 0)
            {
                if (light_pos1[0] < 90)
                    light_pos1[0] += 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction +x any more.\n");
            }
            else if (selectedAxis == 1)
            {
                if (light_pos1[1] < 90)
                    light_pos1[1] += 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction +y any more.\n");
            }
            else if (selectedAxis == 2)
            {
                if (light_pos1[2] < 90)
                    light_pos1[2] += 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction +z any more.\n");
            }
        }
        else if (selectedObj != -1)
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to translate.\n");
            else if (selectedAxis == 0)
            {
                double dx = objects.at(selectedObj).getX();
                if (dx < 70)
                    objects.at(selectedObj).setX(dx + SPEED_TRANSLATE);
                else
                    printf("[WARNING] The selected object cannot move in the direction +x any more.\n");
            }
            else if (selectedAxis == 1)
            {
                double dy = objects.at(selectedObj).getY();
                if (dy < 70)
                    objects.at(selectedObj).setY(dy + SPEED_TRANSLATE);
                else
                    printf("[WARNING] The selected object cannot move in the direction +y any more.\n");
            }
            else if (selectedAxis == 2)
            {
                double dz = objects.at(selectedObj).getZ();
                if (dz < 70)
                    objects.at(selectedObj).setZ(dz + SPEED_TRANSLATE);
                else
                    printf("[WARNING] The selected object cannot move in the direction +z any more.\n");
            }
        }
        break;
    case GLUT_KEY_DOWN:
        if (selectedObj == -1 && selectedLight == -1)
            printf("[WARNING] You need to select an object or a light to translate.\n");
        else if (selectedObj == -1 && selectedLight == 0)
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to translate the selected light.\n");
            else if (selectedAxis == 0)
            {
                if (light_pos0[0] > 20)
                    light_pos0[0] -= 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction -x any more.\n");
            }
            else if (selectedAxis == 1)
            {
                if (light_pos0[1] > 20)
                    light_pos0[1] -= 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction -y any more.\n");
            }
            else if (selectedAxis == 2)
            {
                if (light_pos0[2] > 20)
                    light_pos0[2] -= 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction -z any more.\n");
            }
        }
        else if (selectedObj == -1 && selectedLight == 1)
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to translate the selected light.\n");
            else if (selectedAxis == 0)
            {
                if (light_pos1[0] > 20)
                    light_pos1[0] -= 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction -x any more.\n");
            }
            else if (selectedAxis == 1)
            {
                if (light_pos1[1] > 20)
                    light_pos1[1] -= 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction -y any more.\n");
            }
            else if (selectedAxis == 2)
            {
                if (light_pos1[2] > 20)
                    light_pos1[2] -= 5;
                else
                    printf("[WARNING] The selected light cannot move in the direction -z any more.\n");
            }
        }
        else if (selectedObj != -1)
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to translate.\n");
            else if (selectedAxis == 0)
            {
                double dx = objects.at(selectedObj).getX();
                if (dx > 20)
                    objects.at(selectedObj).setX(dx - SPEED_TRANSLATE);
                else
                    printf("[WARNING] The selected object cannot move in the direction -x any more.\n");
            }
            else if (selectedAxis == 1)
            {
                double dy = objects.at(selectedObj).getY();
                if (dy > 20)
                    objects.at(selectedObj).setY(dy - SPEED_TRANSLATE);
                else
                    printf("[WARNING] The selected object cannot move in the direction -y any more.\n");
            }
            else if (selectedAxis == 2)
            {
                double dz = objects.at(selectedObj).getZ();
                if (dz > 20)
                    objects.at(selectedObj).setZ(dz - SPEED_TRANSLATE);
                else
                    printf("[WARNING] The selected object cannot move in the direction -z any more.\n");
            }
        }
        break;
    case GLUT_KEY_LEFT:
        if (selectedObj == -1)
            printf("[WARNING] You need to select an object to rotate.\n");
        else
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to rotate.\n");
            else if (selectedAxis == 0)
            {
                double rx = objects.at(selectedObj).getRotateX();
                if (rx > -45)
                    objects.at(selectedObj).setRotateX(rx - SPEED_ROTATE);
                else
                    printf("[WARNING] The selected object cannot be rotated along the axis x in this direction any more.\n");
            }
            else if (selectedAxis == 1)
            {
                double ry = objects.at(selectedObj).getRotateY();
                if (ry > -45)
                    objects.at(selectedObj).setRotateY(ry - SPEED_ROTATE);
                else
                    printf("[WARNING] The selected object cannot be rotated along the axis y in this direction any more.\n");
            }
            else if (selectedAxis == 2)
            {
                double rz = objects.at(selectedObj).getRotateZ();
                if (rz > -45)
                    objects.at(selectedObj).setRotateZ(rz - SPEED_ROTATE);
                else
                    printf("[WARNING] The selected object cannot be rotated along the axis z in this direction any more.\n");
            }
        }
        break;
    case GLUT_KEY_RIGHT:
        if (selectedObj == -1)
            printf("[WARNING] You need to select an object to rotate.\n");
        else
        {
            if (selectedAxis == -1)
                printf("[WARNING] You need to select an axis to rotate.\n");
            else if (selectedAxis == 0)
            {
                double rx = objects.at(selectedObj).getRotateX();
                if (rx < 45)
                    objects.at(selectedObj).setRotateX(rx + SPEED_ROTATE);
                else
                    printf("[WARNING] The selected object cannot be rotated along the axis x in this direction any more.\n");
            }
            else if (selectedAxis == 1)
            {
                double ry = objects.at(selectedObj).getRotateY();
                if (ry < 45)
                    objects.at(selectedObj).setRotateY(ry + SPEED_ROTATE);
                else
                    printf("[WARNING] The selected object cannot be rotated along the axis y in this direction any more.\n");
            }
            else if (selectedAxis == 2)
            {
                double rz = objects.at(selectedObj).getRotateZ();
                if (rz < 45)
                    objects.at(selectedObj).setRotateZ(rz + SPEED_ROTATE);
                else
                    printf("[WARNING] The selected object cannot be rotated along the axis z in this direction any more.\n");
            }
        }
        break;
    }

    glutPostRedisplay();
}

void init(void)
{
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 1000);
}

/**
 * Ray-box intersection test (Slab algorithm)
 * 
 * Reference: https://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
 */
bool rayBox(double *low, double *high, double *origin, double *destination)
{
    double direction[3];
    // for each slab (X plane, Y plane, Z plane)
    double tnear = std::numeric_limits<double>::min();
    double tfar = std::numeric_limits<double>::max();
    for (int i = 0; i < 3; i++)
    {
        direction[i] = destination[i] - origin[i];
        if (direction[i] == 0) // parallel
        {
            if (origin[i] < low[i] || origin[i] > high[i])
                return false;
        }
        else
        {
            double t1 = (low[i] - origin[i]) / direction[i];
            double t2 = (high[i] - origin[i]) / direction[i];
            if (t1 > t2) // swap t1, t2
            {
                double tmp = t1;
                t1 = t2;
                t2 = tmp;
            }
            if (t1 > tnear) // want largest tnear
                tnear = t1;
            if (t2 < tfar) // want smallest tfar
                tfar = t2;
            if (tnear > tfar) // box is missed
                return false;
            if (tfar < 0) // box behind ray origin
                return false;
        }
    }
    return true;
}

void setRay(int mouseX, int mouseY)
{
    int viewport[4];
    double matModelView[16], matProjection[16];

    glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
    glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // window pos of mouse, Y is inverted on windows
    double winX = (double)mouseX;
    double winY = viewport[3] - (double)mouseY;
    // get point on the 'near' plan
    gluUnProject(winX, winY, 0.0, matModelView, matProjection, viewport, &m_start.x, &m_start.y, &m_start.z);
    //get point on the 'far' plan
    gluUnProject(winX, winY, 1.0, matModelView, matProjection, viewport, &m_end.x, &m_end.y, &m_end.z);

    // std::cout << "Ray start: " << m_start.x << "  " << m_start.y << "  " << m_start.z << std::endl;
    // std::cout << "Ray end: " << m_end.x << "  " << m_end.y << "  " << m_end.z << std::endl;
}

// ray test with objects and lights
void rayTestObjects(int mouseX, int mouseY)
{
    setRay(mouseX, mouseY);

    double ray_origin[] = {m_start.x, m_start.y, m_start.z};
    double ray_destination[] = {m_end.x, m_end.y, m_end.z};

    double minDistance = std::numeric_limits<double>::max();
    int indexMin = -1;
    selectedLight = -1;
    for (int i = 0; i < objects.size(); i++)
    {
        Object o = objects.at(i);
        double o_low[] = {o.getX() + SIZE_BOX / 2 * o.getScaleX(), o.getY() + SIZE_BOX / 2 * o.getScaleY(), o.getZ() + SIZE_BOX / 2 * o.getScaleY()};
        double o_high[] = {o.getX() - SIZE_BOX / 2 * o.getScaleX(), o.getY() - SIZE_BOX / 2 * o.getScaleY(), o.getZ() - SIZE_BOX / 2 * o.getScaleY()};

        if (rayBox(o_low, o_high, ray_origin, ray_destination))
        {
            double distance = sqrt(pow(o.getX() - m_start.x, 2) + pow(o.getY() - m_start.y, 2) + pow(o.getZ() - m_start.z, 2));
            if (distance < minDistance)
            {
                minDistance = distance;
                indexMin = i;
                selectedLight = -1;
            }
        }
    }

    // test if light is selected
    // it should be ray sphere but it's ray box now
    double l0_low[] = {light_pos0[0] + 3, light_pos0[1] + 3, light_pos0[2] + 3};
    double l0_high[] = {light_pos0[0] - 3, light_pos0[1] - 3, light_pos0[2] - 3};
    if (rayBox(l0_low, l0_high, ray_origin, ray_destination))
    {
        double distance = sqrt(pow(light_pos0[0] - m_start.x, 2) + pow(light_pos0[1] - m_start.y, 2) + pow(light_pos0[2] - m_start.z, 2));
        if (distance < minDistance)
        {
            minDistance = distance;
            indexMin = -1;
            selectedLight = 0;
        }
    }

    double l1_low[] = {light_pos1[0] + 3, light_pos1[1] + 3, light_pos1[2] + 3};
    double l1_high[] = {light_pos1[0] - 3, light_pos1[1] - 3, light_pos1[2] - 3};
    if (rayBox(l1_low, l1_high, ray_origin, ray_destination))
    {
        double distance = sqrt(pow(light_pos1[0] - m_start.x, 2) + pow(light_pos1[1] - m_start.y, 2) + pow(light_pos1[2] - m_start.z, 2));
        if (distance < minDistance)
        {
            minDistance = distance;
            indexMin = -1;
            selectedLight = 1;
        }
    }
    selectedObj = indexMin;
    selectedAxis = -1;
}

void mouse(int button, int state, int mouseX, int mouseY)
{

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
            rayTestObjects(mouseX, mouseY);
        }
        break;
    case GLUT_RIGHT_BUTTON: // right click delect selected object, but light will not be deleted
        if (state == GLUT_DOWN)
        {
            rayTestObjects(mouseX, mouseY);
            if (selectedObj != -1)
            {
                objects.erase(objects.begin() + selectedObj);
                selectedObj = -1;
            }
        }
        break;
    }

    glutPostRedisplay();
}

void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ((w + 0.0f) / h), 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

void callBackInit()
{
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
}

void printGuide()
{
	std::cout << "###############################################################################\n"
			  << "################################### Controls ##################################\n"
			  << "###############################################################################\n"
              << "q/esc => quit\n"
			  << "SPACE => create an object\n"
              << "Left click => select an object if possible\n"
              << "Right click => delete the clicked object\n"
              << "d => delete the selected object, not working if no object is selected\n"
              << "r => reset the scene, clear all objects\n"
              << "s => save objects to a file, need to type the file name (without .txt) in the console.\n"
              << "l => load the file and restore objects, need to type the file name (without .txt) in the console\n"
              << "u/i => rotate the scene about y\n"
              << "o/p => rotate the scene about z\n"
              << "g/h => translate the camera in x\n"
              << "j/k => translate the camera in y\n"
              << "b/n => translate the camera in z\n"
              << "x => select/unselect x axis of the selected object, not working if no object is selected\n"
              << "y => select/unselect y axis of the selected object, not working if no object is selected\n"
              << "z => select/unselect z axis of the selected object, not working if no object is selected\n"
              << "UP/DOWN => When an object or a light is selected, translate it in the selected axis, not working if no axis is selected\n"
              << "LEFT/RIGHT => When an object or a light is selected, rotate it in the selected axis, not working if no axis is selected\n"
              << "+/- => When an object or a light is selected, scale it in the selected axis, not working if no axis is selected\n"
              << "1/2/3/4/5 => change current drawing material\n"
              << "m = > apply the current drawing material to the selected object, not working if no object is selected\n"
			  << "###############################################################################\n"
              << "Example: if you want to translate an object in axis x, you should left click this object then press x then press UP or DOWN.\n"
              << "For some raisons (limits, operations...), the translation is according to the scene's xyz not the selected object itself.\n"
              << "The selected object will be covered by a wireframe cube and shows its axes x (red), y (green) and z (blue) and the selected axis is cyan.\n";
}

/* Main function */
int main(int argc, char **argv)
{
    printGuide();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Modeller");
    callBackInit();

    glEnable(GL_DEPTH_TEST);
    init();
    glutMainLoop(); //starts the event glutMainLoop
    return (0);     //return may not be necessary on all compilers
}

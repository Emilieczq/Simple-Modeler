#include "Object.h"
Object::Object()
{
}
Object::Object(int newType, int newMaterial)
{
    type = newType;
    material = newMaterial;
    x = 0.0;
    y = 0.0;
    z = 0.0;
    rotateX = 0.0;
    rotateY = 0.0;
    rotateZ = 0.0;
    scaleX = 1.0;
    scaleY = 1.0;
    scaleZ = 1.0;
}
Object::~Object()
{
}

int Object::getType()
{
    return type;
}
int Object::getMaterial()
{
    return material;
}
double Object::getX()
{
    return x;
}
double Object::getY()
{
    return y;
}
double Object::getZ()
{
    return z;
}
double Object::getRotateX()
{
    return rotateX;
}
double Object::getRotateY()
{
    return rotateY;
}
double Object::getRotateZ()
{
    return rotateZ;
}
double Object::getScaleX()
{
    return scaleX;
}
double Object::getScaleY()
{
    return scaleY;
}
double Object::getScaleZ()
{
    return scaleZ;
}
void Object::setType(int newType)
{
    type = newType;
}
void Object::setMaterial(int newMaterial)
{
    material = newMaterial;
}
void Object::setX(double newX)
{
    x = newX;
}
void Object::setY(double newY)
{
    y = newY;
}
void Object::setZ(double newZ)
{
    z = newZ;
}
void Object::setRotateX(double newRotateX)
{
    rotateX = newRotateX;
}
void Object::setRotateY(double newRotateY)
{
    rotateY = newRotateY;
}
void Object::setRotateZ(double newRotateZ)
{
    rotateZ = newRotateZ;
}
void Object::setScaleX(double newScaleX)
{
    scaleX = newScaleX;
}
void Object::setScaleY(double newScaleY)
{
    scaleY = newScaleY;
}
void Object::setScaleZ(double newScaleZ)
{
    scaleZ = newScaleZ;
}
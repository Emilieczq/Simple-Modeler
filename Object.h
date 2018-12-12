#ifndef OBJECT_H
#define OBJECT_H

class Object
{
public:
    Object();
    Object(int, int);
    ~Object();
    int getType();
    int getMaterial();
    double getX();
    double getY();
    double getZ();
    double getRotateX();
    double getRotateY();
    double getRotateZ();
    double getScaleX();
    double getScaleY();
    double getScaleZ();
    void setType(int);
    void setMaterial(int);
    void setX(double);
    void setY(double);
    void setZ(double);
    void setRotateX(double);
    void setRotateY(double);
    void setRotateZ(double);
    void setScaleX(double);
    void setScaleY(double);
    void setScaleZ(double);
private:
    int type;
    int material; // 1: copper; 2: silver; 3: emerald; 4: pearl; 5: turquoise; 6: jade
    double x;
    double y;
    double z;
    double rotateX;
    double rotateY;
    double rotateZ;
    double scaleX;
    double scaleY;
    double scaleZ;
};

#endif
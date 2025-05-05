#ifndef Entity_H
#define Entity_H

class Entity {
protected:
    float x, y;
    float width, height;

public:
    Entity(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {
    }

    virtual ~Entity() {}

    virtual void update(float collectible) = 0; // Pure virtual function

    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
};

#endif

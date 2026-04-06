#include <iostream>
#include <cmath>
using namespace std;

class Shape {
        
    private:
        static int shapeCount;

    public:

    Shape() 
    {
        shapeCount++;
    }

    virtual ~Shape() 
    {
        shapeCount--;
    }

    virtual double area() const 
    {
        return 0;
    }

    virtual double perimeter() const 
    {
        return 0;
    }

    static int getShapeCount() 
    {
        return shapeCount;
    }
};
int Shape::shapeCount = 0;

class Rectangle : public Shape 
{
    private:
        double width;
        double height;

    public:
    Rectangle(double w, double h);
    ~Rectangle();
    double area() const override;
    double perimeter() const override;
};
Rectangle::Rectangle(double w, double h) 
{
    width = w;
    height = h;
}

Rectangle::~Rectangle() { }

double Rectangle::area() const
{
    return width * height;
}
double Rectangle::perimeter() const
{
    return 2 * (width + height);
}
class Triangle : public Shape 
{
    private:
        double base;
        double height;

    public:
    Triangle(double b, double h);
    ~Triangle();
    double area() const override;
    double perimeter() const override;
};
Triangle::Triangle(double b, double h) 
{
    base = b;
    height = h;
}
Triangle::~Triangle() { }
double Triangle::area() const 
{
    return 0.5 * base * height;
}
double Triangle::perimeter() const 
{
    double hyp = sqrt(base * base + height * height);
    return base + height + hyp;
}
class Circle : public Shape 
{
    private:
        double radius;
        static constexpr double PI = 3.14159;

    public:
    Circle(double r);
    ~Circle();
    double area() const override;
    double perimeter() const override;
};
Circle::Circle(double r) 
{
    radius = r;
}
Circle::~Circle() { }
double Circle::area() const 
{
    return PI * radius * radius;
}
double Circle::perimeter() const 
{
    return 2 * PI * radius;
}
int main() 
{
    double width, height, base, triangleHeight, radius;
    
    // Read rectangle width and height
    cin >> width >> height;
    Shape* rect = new Rectangle(width, height);
    cout << "Rectangle object created" << endl;
    cout << "Number of Shape's objects created: " << Shape::getShapeCount() <<
    endl;
    // Read triangle base and height
    cin >> base >> triangleHeight;
    Shape* tri = new Triangle(base, triangleHeight);
    cout << "Triangle object created" << endl;
    cout << "Number of Shape's objects created: " << Shape::getShapeCount() <<
    endl;
    // Read circle radius
    cin >> radius;
    Shape* circ = new Circle(radius);
    cout << "Circle object created" << endl;
    cout << "Number of Shape's objects created: " << Shape::getShapeCount() <<
    endl;
    cout << "Rectangle area: " << rect->area() << endl;
    cout << "Triangle area: " << tri->area() << endl;
    cout << "Circle area: " << circ->area() << endl;
    cout << "Rectangle perimeter: " << rect->perimeter() << endl;
    cout << "Triangle perimeter: " << tri->perimeter() << endl;
    cout << "Circle perimeter: " << circ->perimeter() << endl;
    cout << "Total number of Shape's objects created: " << Shape::getShapeCount() << endl;
    delete rect;
    cout << "Rectangle object deleted" << endl;
    delete tri;
    cout << "Triangle object deleted" << endl;
    delete circ;
    cout << "Circle object deleted" << endl;
    cout << "Total number of Shape's objects left after deletion: " << Shape::getShapeCount() << endl;
    return 0;
}
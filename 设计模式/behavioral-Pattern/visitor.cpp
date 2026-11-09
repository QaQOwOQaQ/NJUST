// 我们仍然需要对原始类 Dot, Rectangle 和 CompoundShape 进行修改
// 但仅仅只需要在其中加一个 accept 方法
// 使用双分派方法来执行正确的方法，每次添加新的形状接口
// 我们只需要添加新的类和方法而不需要修改已有的方法
// 从而确保 open-close 原则


#include <iostream>
#include <vector>
#include <memory>

// 先声明访问者类
class Dot;
class Circle;
class Rectangle;
class CompoundShape;

// 访问者接口
class Visitor {
public:
    virtual void visitDot(Dot* d) = 0;
    virtual void visitCircle(Circle* c) = 0;
    virtual void visitRectangle(Rectangle* r) = 0;
    virtual void visitCompoundShape(CompoundShape* cs) = 0;
    virtual ~Visitor() = default;
};

// 形状接口
class Shape {
public:
    virtual void move(int x, int y) = 0;
    virtual void draw() = 0;
    virtual void accept(Visitor* v) = 0;  // 关键的 accept 方法
    virtual ~Shape() = default;
};

// 具体形状类：点
class Dot : public Shape {
public:
    void move(int x, int y) override {
        std::cout << "移动点到 (" << x << ", " << y << ")\n";
    }
    
    void draw() override {
        std::cout << "绘制点\n";
    }

    void accept(Visitor* v) override {
        v->visitDot(this);
    }
};

// 具体形状类：圆
class Circle : public Shape {
public:
    void move(int x, int y) override {
        std::cout << "移动圆到 (" << x << ", " << y << ")\n";
    }
    
    void draw() override {
        std::cout << "绘制圆\n";
    }

    void accept(Visitor* v) override {
        v->visitCircle(this);
    }
};

// 具体形状类：矩形
class Rectangle : public Shape {
public:
    void move(int x, int y) override {
        std::cout << "移动矩形到 (" << x << ", " << y << ")\n";
    }
    
    void draw() override {
        std::cout << "绘制矩形\n";
    }

    void accept(Visitor* v) override {
        v->visitRectangle(this);
    }
};

// 复合形状：可以包含多个形状
class CompoundShape : public Shape {
private:
    std::vector<std::unique_ptr<Shape>> children;
public:
    void addShape(std::unique_ptr<Shape> shape) {
        children.push_back(std::move(shape));
    }

    void move(int x, int y) override {
        std::cout << "移动复合形状到 (" << x << ", " << y << ")\n";
    }

    void draw() override {
        std::cout << "绘制复合形状\n";
        for (const auto& shape : children) {
            shape->draw();
        }
    }

    void accept(Visitor* v) override {
        v->visitCompoundShape(this);
        for (const auto& shape : children) {
            shape->accept(v);
        }
    }
};

// 具体访问者：XML 导出
class XMLExportVisitor : public Visitor {
public:
    void visitDot(Dot* d) override {
        std::cout << "导出 Dot 为 XML\n";
    }

    void visitCircle(Circle* c) override {
        std::cout << "导出 Circle 为 XML\n";
    }

    void visitRectangle(Rectangle* r) override {
        std::cout << "导出 Rectangle 为 XML\n";
    }

    void visitCompoundShape(CompoundShape* cs) override {
        std::cout << "导出 CompoundShape 为 XML\n";
    }
};

// 客户端应用程序
class Application {
private:
    std::vector<std::unique_ptr<Shape>> allShapes;
public:
    void addShape(std::unique_ptr<Shape> shape) {
        allShapes.push_back(std::move(shape));
    }

    void exportShapes() {
        XMLExportVisitor exportVisitor;
        for (const auto& shape : allShapes) {
            shape->accept(&exportVisitor);
        }
    }
};

// 测试
int main() {
    Application app;
    
    app.addShape(std::make_unique<Dot>());
    app.addShape(std::make_unique<Circle>());
    app.addShape(std::make_unique<Rectangle>());

    auto compoundShape = std::make_unique<CompoundShape>();
    compoundShape->addShape(std::make_unique<Dot>());
    compoundShape->addShape(std::make_unique<Circle>());
    app.addShape(std::move(compoundShape));

    app.exportShapes();

    return 0;
}

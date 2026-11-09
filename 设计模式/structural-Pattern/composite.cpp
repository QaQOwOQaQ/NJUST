#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

// 组件接口
class Graphic {
public:
    virtual void move(int x, int y) = 0;
    virtual void draw() = 0;
    virtual ~Graphic() = default;
};

// 叶子节点：点
class Dot : public Graphic {
protected:
    int x, y;
public:
    Dot(int x, int y) : x(x), y(y) {}

    void move(int dx, int dy) override {
        x += dx;
        y += dy;
    }

    void draw() override {
        cout << "Draw Dot at (" << x << ", " << y << ")\n";
    }
};

// 叶子节点：圆
class Circle : public Dot {
    int radius;
public:
    Circle(int x, int y, int r) : Dot(x, y), radius(r) {}

    void draw() override {
        cout << "Draw Circle at (" << x << ", " << y << ") with radius " << radius << "\n";
    }
};

// 组合类
class CompoundGraphic : public Graphic {
    vector<shared_ptr<Graphic>> children;

public:
    void add(shared_ptr<Graphic> child) {
        children.push_back(child);
    }

    void remove(shared_ptr<Graphic> child) {
        children.erase(std::remove_if(children.begin(), children.end(),
                                      [&child](const shared_ptr<Graphic> &g) {
                                          return g == child; // 这里修正
                                      }),
                       children.end());
    }

    void move(int dx, int dy) override {
        for (auto &child : children) {
            child->move(dx, dy);
        }
    }

    void draw() override {
        cout << "Drawing CompoundGraphic:\n";
        for (auto &child : children) {
            child->draw();
        }
    }
};

// 客户端代码
int main() {
    auto group = make_shared<CompoundGraphic>();
    auto dot = make_shared<Dot>(1, 2);
    auto circle = make_shared<Circle>(5, 3, 10);

    group->add(dot);
    group->add(circle);

    cout << "Initial state:\n";
    group->draw();

    cout << "\nMoving graphics...\n";
    group->move(10, 10);
    group->draw();

    cout << "\nRemoving dot...\n";
    group->remove(dot);
    group->draw();

    return 0;
}

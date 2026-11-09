#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

// 享元类：存储树的颜色、纹理等共享数据
class TreeType {
public:
    string name;
    string color;
    string texture;

    TreeType(const string &name, const string &color, const string &texture)
        : name(name), color(color), texture(texture) {}

    void draw(int x, int y) {
        cout << "Drawing tree '" << name << "' at (" << x << ", " << y
             << ") with color " << color << " and texture " << texture << "\n";
    }
};

// 享元工厂：管理并复用 TreeType 实例
class TreeFactory {
private:
    static unordered_map<string, shared_ptr<TreeType>> treeTypes;

public:
    static shared_ptr<TreeType> getTreeType(const string &name, const string &color, const string &texture) {
        string key = name + "_" + color + "_" + texture;
        if (treeTypes.find(key) == treeTypes.end()) {
            treeTypes[key] = make_shared<TreeType>(name, color, texture);
        }
        return treeTypes[key];
    }
};

// 定义静态成员变量
unordered_map<string, shared_ptr<TreeType>> TreeFactory::treeTypes;

// 具体的树类：包含位置和共享的 TreeType
class Tree {
public:
    int x, y;
    shared_ptr<TreeType> type;

    Tree(int x, int y, shared_ptr<TreeType> type) : x(x), y(y), type(type) {}

    void draw() {
        type->draw(x, y);
    }
};

// 客户端：森林，存储大量的树
class Forest {
private:
    vector<shared_ptr<Tree>> trees;

public:
    void plantTree(int x, int y, const string &name, const string &color, const string &texture) {
        shared_ptr<TreeType> type = TreeFactory::getTreeType(name, color, texture);
        trees.push_back(make_shared<Tree>(x, y, type));
    }

    void draw() {
        for (const auto &tree : trees) {
            tree->draw();
        }
    }
};

// 测试代码
int main() {
    Forest forest;
    forest.plantTree(1, 2, "Oak", "Green", "Rough");
    forest.plantTree(3, 4, "Pine", "Dark Green", "Smooth");
    forest.plantTree(5, 6, "Oak", "Green", "Rough"); // 复用已有的 "Oak" 类型

    cout << "Drawing forest:\n";
    forest.draw();

    return 0;
}

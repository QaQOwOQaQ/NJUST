#include <iostream>
#include <vector>
#include <memory>

// 抽象类 GameAI
class GameAI {
public:
    // 模板方法
    void turn() {
        collectResources();
        buildStructures();
        buildUnits();
        attack();
    }

protected:
    // 某些步骤可在基类中实现
    virtual void collectResources() {
        std::cout << "Collecting resources...\n";
    }

    // 抽象方法，由子类实现
    virtual void buildStructures() = 0;
    virtual void buildUnits() = 0;

    // 一个类可包含多个模板方法
    virtual void attack() {
        std::cout << "Attacking enemy...\n";
    }

    virtual void sendScouts(const std::string& position) = 0;
    virtual void sendWarriors(const std::string& position) = 0;
};

// 具体类：OrcsAI
class OrcsAI : public GameAI {
protected:
    void buildStructures() override {
        std::cout << "Orcs are building structures...\n";
    }

    void buildUnits() override {
        std::cout << "Orcs are building units...\n";
    }

    void sendScouts(const std::string& position) override {
        std::cout << "Orc scouts are heading to " << position << ".\n";
    }

    void sendWarriors(const std::string& position) override {
        std::cout << "Orc warriors are attacking " << position << ".\n";
    }
};

// 具体类：MonstersAI
class MonstersAI : public GameAI {
protected:
    void collectResources() override {
        std::cout << "Monsters do not collect resources.\n";
    }

    void buildStructures() override {
        std::cout << "Monsters do not build structures.\n";
    }

    void buildUnits() override {
        std::cout << "Monsters do not build units.\n";
    }

    void sendScouts(const std::string& position) override {
        std::cout << "Monster scouts are roaming around " << position << ".\n";
    }

    void sendWarriors(const std::string& position) override {
        std::cout << "Monster horde is moving towards " << position << ".\n";
    }
};

// 测试代码
int main() {
    std::cout << "Testing OrcsAI:\n";
    std::unique_ptr<GameAI> orcAI = std::make_unique<OrcsAI>();
    orcAI->turn();  // 运行兽人 AI 逻辑

    std::cout << "\nTesting MonstersAI:\n";
    std::unique_ptr<GameAI> monsterAI = std::make_unique<MonstersAI>();
    monsterAI->turn();  // 运行怪物 AI 逻辑

    return 0;
}
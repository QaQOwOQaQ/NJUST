#include <iostream>
#include <memory>

// 策略接口
class Strategy {
public:
    virtual int execute(int a, int b) = 0;
    virtual ~Strategy() = default;
};

// 具体策略：加法
class ConcreteStrategyAdd : public Strategy {
public:
    int execute(int a, int b) override {
        return a + b;
    }
};

// 具体策略：减法
class ConcreteStrategySubtract : public Strategy {
public:
    int execute(int a, int b) override {
        return a - b;
    }
};

// 具体策略：乘法
class ConcreteStrategyMultiply : public Strategy {
public:
    int execute(int a, int b) override {
        return a * b;
    }
};

// 上下文类
class Context {
private:
    std::unique_ptr<Strategy> strategy; // 使用智能指针管理策略对象
public:
    void setStrategy(std::unique_ptr<Strategy> newStrategy) {
        strategy = std::move(newStrategy);
    }

    int executeStrategy(int a, int b) {
        if (strategy) {
            return strategy->execute(a, b);
        } else {
            std::cerr << "错误：尚未设置策略！" << std::endl;
            return 0;
        }
    }
};

// 客户端代码
int main() {
    Context context;
    int a, b;
    char action;

    std::cout << "输入第一个数: ";
    std::cin >> a;
    std::cout << "输入第二个数: ";
    std::cin >> b;
    std::cout << "选择操作 (+, -, *): ";
    std::cin >> action;

    // 选择策略
    if (action == '+') {
        context.setStrategy(std::make_unique<ConcreteStrategyAdd>());
    } else if (action == '-') {
        context.setStrategy(std::make_unique<ConcreteStrategySubtract>());
    } else if (action == '*') {
        context.setStrategy(std::make_unique<ConcreteStrategyMultiply>());
    } else {
        std::cerr << "错误：未知的操作。" << std::endl;
        return 1;
    }

    // 执行策略
    int result = context.executeStrategy(a, b);
    std::cout << "计算结果: " << result << std::endl;

    return 0;
}

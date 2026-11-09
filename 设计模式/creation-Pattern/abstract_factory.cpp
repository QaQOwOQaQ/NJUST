#include <iostream>
#include <memory>
#include <stdexcept>

class Button;
class Checkbox;
class WinButton;
class MacButton;
class WinCheckbox;
class MacCheckbox;

// 产品接口：Button
class Button {
public:
    virtual void render() = 0;
    virtual void onClick(void(*f)()) = 0;
    virtual ~Button() = default;
};

// 具体产品：WinButton
class WinButton : public Button {
public:
    void render() override {
        std::cout << "Render Windows Button" << std::endl;
    }

    void onClick(void(*f)()) override {
        std::cout << "Windows Button Clicked!" << std::endl;
        f(); // 执行事件回调
    }
};

// 产品接口：Checkbox
class Checkbox {
public:
    virtual void render() = 0;
    virtual void onClick(void(*f)()) = 0;
    virtual ~Checkbox() = default;
};

// 具体产品：WinCheckbox
class WinCheckbox : public Checkbox {
public:
    void render() override {
        std::cout << "Render Windows Checkbox" << std::endl;
    }

    void onClick(void(*f)()) override {
        std::cout << "Windows Checkbox Clicked!" << std::endl;
        f(); // 执行事件回调
    }
};

// 产品接口：MacCheckbox
class MacCheckbox : public Checkbox {
public:
    void render() override {
        std::cout << "Render Mac Checkbox" << std::endl;
    }

    void onClick(void(*f)()) override {
        std::cout << "Mac Checkbox Clicked!" << std::endl;
        f(); // 执行事件回调
    }
};

// 抽象工厂接口：GUIFactory
class GUIFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
    virtual ~GUIFactory() = default;
};

// 具体工厂：WinFactory
class WinFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WinButton>();  // 返回 unique_ptr<Button>
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WinCheckbox>();  // 返回 unique_ptr<Checkbox>
    }
};

// 产品接口：MacButton
class MacButton : public Button {
public:
    void render() override {
        std::cout << "Render Mac Button" << std::endl;
    }

    void onClick(void(*f)()) override {
        std::cout << "Mac Button Clicked!" << std::endl;
        f(); // 执行事件回调
    }
};

// 具体工厂：MacFactory
class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();  // 返回 unique_ptr<Button>
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::unique_ptr<Checkbox>(new MacCheckbox());  // 使用 new 来创建
    }
};



// 客户端代码
class Application {
private:
    std::unique_ptr<GUIFactory> factory;

public:
    Application(std::unique_ptr<GUIFactory> _factory) : factory(std::move(_factory)) {}

    void render() {
        auto button = factory->createButton();
        auto checkbox = factory->createCheckbox();
        button->render();
        checkbox->render();
    }
};

int main() {
    // 选择工厂类型：WinFactory 或 MacFactory
    std::unique_ptr<GUIFactory> factory = std::make_unique<WinFactory>();
    Application app(std::move(factory));
    app.render();

    return 0;
}

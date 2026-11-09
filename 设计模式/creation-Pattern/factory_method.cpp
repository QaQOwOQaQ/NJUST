#include <iostream>
#include <memory>
#include <stdexcept>


// 产品接口（Button）
class Button {
public:
    virtual void render() = 0;  // 渲染按钮
    virtual void onClick(void(*f)()) = 0;  // 按钮点击事件
    virtual ~Button() = default;
};

// 具体产品 WindowsButton
class WindowsButton : public Button {
public:
    void render() override {
        std::cout << "渲染 Windows 样式的按钮" << std::endl;
    }

    void onClick(void(*f)()) override {
        std::cout << "绑定 Windows 操作系统点击事件" << std::endl;
        f();  // 调用事件
    }
};

// 具体产品 HTMLButton
class HTMLButton : public Button {
public:
    void render() override {
        std::cout << "渲染 HTML 风格的按钮" << std::endl;
    }

    void onClick(void(*f)()) override {
        std::cout << "绑定 Web 浏览器点击事件" << std::endl;
        f();  // 调用事件
    }
};

// 创建者类（Dialog）
class Dialog {
public:
    virtual std::unique_ptr<Button> createButton() = 0;  // 工厂方法，必须返回一个 Button 对象
    void render() {
        // 调用工厂方法创建一个产品对象
        auto okButton = createButton();
        // 使用产品
        okButton->onClick([]() { std::cout << "关闭对话框" << std::endl; });
        okButton->render();
    }

    virtual ~Dialog() = default;
};

// 具体创建者 WindowsDialog
class WindowsDialog : public Dialog {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
};

// 具体创建者 WebDialog
class WebDialog : public Dialog {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<HTMLButton>();
    }
};

// 应用程序类（Application）
class Application {
private:
    std::unique_ptr<Dialog> dialog;  // 创建者对象

public:
    void initialize() {
        // 假设从配置中读取操作系统类型
        std::string configOS = "Windows";  // 可修改为其他 OS 类型

        if (configOS == "Windows") {
            dialog = std::make_unique<WindowsDialog>();
        } else if (configOS == "Web") {
            dialog = std::make_unique<WebDialog>();
        } else {
            throw std::runtime_error("错误！未知的操作系统。");
        }
    }

    void main() {
        this->initialize();
        dialog->render();
    }
};

// 主函数
int main() {
    Application app;
    app.main();
    return 0;
}

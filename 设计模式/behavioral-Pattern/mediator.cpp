// 这是 Mediator Pattern 的实现，它用于减少组件之间的直接依赖关系，而是通过 Mediator 进行通信

#include <iostream>
#include <memory>
#include <string>

// 预声明 Mediator 接口
class Mediator;

// 组件基类
class Component {
protected:
    Mediator* dialog;

public:
    Component(Mediator* d) : dialog(d) {}
    virtual void click() = 0; // event
};

// 中介者接口
class Mediator {
public:
    virtual void notify(Component* sender, const std::string& event) = 0;
};

// 按钮组件
class Button : public Component {
public:
    Button(Mediator* d) : Component(d) {}

    void click() override {
        std::cout << "Button clicked.\n";
        dialog->notify(this, "click");
    }
};

// 复选框组件
class Checkbox : public Component {
public:
    bool checked = false;

    Checkbox(Mediator* d) : Component(d) {}

    void check() {
        checked = !checked;
        std::cout << "Checkbox toggled, now: " << (checked ? "Login Mode" : "Register Mode") << "\n";
        dialog->notify(this, "check");
    }

    void click() override {
        check();
    }
};

// 文本框组件
class Textbox : public Component {
public:
    std::string text;

    Textbox(Mediator* d) : Component(d) {}

    void setText(const std::string& newText) {
        text = newText;
    }

    void click() override {
        std::cout << "Textbox focused.\n";
    }
};

// 具体的中介者：身份验证对话框
class AuthenticationDialog : public Mediator {
private:
    std::string title;
    std::unique_ptr<Checkbox> loginOrRegisterChkBx;
    std::unique_ptr<Textbox> loginUsername, loginPassword;
    std::unique_ptr<Textbox> registrationUsername, registrationPassword, registrationEmail;
    std::unique_ptr<Button> okBtn, cancelBtn;

public:
    AuthenticationDialog() {
        loginOrRegisterChkBx = std::make_unique<Checkbox>(this);
        loginUsername = std::make_unique<Textbox>(this);
        loginPassword = std::make_unique<Textbox>(this);
        registrationUsername = std::make_unique<Textbox>(this);
        registrationPassword = std::make_unique<Textbox>(this);
        registrationEmail = std::make_unique<Textbox>(this);
        okBtn = std::make_unique<Button>(this);
        cancelBtn = std::make_unique<Button>(this);
    }

    void notify(Component* sender, const std::string& event) override {
        if (sender == loginOrRegisterChkBx.get() && event == "check") {
            if (loginOrRegisterChkBx->checked) {
                title = "登录";
                std::cout << "Switched to Login Mode.\n";
            } else {
                title = "注册";
                std::cout << "Switched to Register Mode.\n";
            }
        }

        if (sender == okBtn.get() && event == "click") {
            if (loginOrRegisterChkBx->checked) {
                std::cout << "Attempting to login...\n";
            } else {
                std::cout << "Attempting to register...\n";
            }
        }
    }

    // 获取组件（为了测试）
    Checkbox* getCheckbox() { return loginOrRegisterChkBx.get(); }
    Button* getOkButton() { return okBtn.get(); }
};

// 测试代码
int main() {
    AuthenticationDialog dialog;

    std::cout << "Initial State: Register Mode\n";
    
    // 模拟点击复选框切换模式
    dialog.getCheckbox()->click();  // 切换到登录模式
    dialog.getCheckbox()->click();  // 切换回注册模式

    // 模拟点击 OK 按钮
    dialog.getOkButton()->click();

    return 0;
}

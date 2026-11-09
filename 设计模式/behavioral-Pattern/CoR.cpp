// 在本例中， 责任链模式负责为活动的 GUI 元素显示上下文帮助信息
// 当用户将鼠标指针移动到某个元素并按下 F1键时， 程序检测到指针下的组件并对其发送帮助请求。 
// 该请求不断向上传递到该元素所有的容器， 直至某个元素能够显示帮助信息。
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 处理者接口声明了一个创建处理者链的方法。还声明了一个执行请求的方法。
class ComponentWithContextualHelp {
public:
    virtual void showHelp() = 0;
    virtual ~ComponentWithContextualHelp() = default;
};

// 简单组件的基础类。
class Component : public ComponentWithContextualHelp {
protected:
    std::string tooltipText;
    Component* container = nullptr; // 组件容器在处理者链中作为“下一个”链接。

public:
    void setTooltipText(const std::string& text) { tooltipText = text; }
    void setContainer(Component* _container) { container = _container; }

    // 如果组件设定了帮助文字，那它将会显示提示信息。
    // 如果组件没有帮助文字且其容器存在，那它会将调用传递给容器。
    void showHelp() override {
        if (!tooltipText.empty()) {
            // 显示提示信息。
            std::cout << "Tooltip: " << tooltipText << std::endl;
        } else if (container) {
            container->showHelp();
        }
    }
};

// 容器可以将简单组件和其他容器作为其子项目。链关系将在这里建立。
// 该类将从其父类处继承 showHelp（显示帮助）的行为。
class Container : public Component {
protected:
    std::vector<Component*> children;

public:
    void add(Component* child) {
        children.push_back(child);
        child->setContainer(this);
    }
};

// 原始组件应该能够使用帮助操作的默认实现……
class Button : public Component {
    // ……
};

// 但复杂组件可能会对默认实现进行重写。
// 如果无法以新的方式来提供帮助文字，那组件总是还能调用基础实现的（参见 Component 类）。
class Panel : public Container {
private:
    std::string modalHelpText;

public:
    void setModalHelpText(const std::string& text) { modalHelpText = text; }

    void showHelp() override {
        if (!modalHelpText.empty()) {
            // 显示包含帮助文字的模态窗口。
            std::cout << "Panel Help: " << modalHelpText << std::endl;
        } else {
            Component::showHelp();
        }
    }
};

// ……同上……
class Dialog : public Container {
private:
    std::string wikiPageURL;

public:
    void setWikiPageURL(const std::string& url) { wikiPageURL = url; }

    void showHelp() override {
        if (!wikiPageURL.empty()) {
            // 打开百科帮助页面。
            std::cout << "Opening help page: " << wikiPageURL << std::endl;
        } else {
            Component::showHelp();
        }
    }
};

// 客户端代码。
class Application {
private:
    Dialog* dialog;
    Panel* panel;
    Button* ok;
    Button* cancel;

public:
    // 每个程序都能以不同方式对链进行配置。
    void createUI() {
        dialog = new Dialog();
        dialog->setWikiPageURL("http://……");

        panel = new Panel();
        panel->setModalHelpText("本面板用于……");

        ok = new Button();
        ok->setTooltipText("这是一个确认按钮……");

        cancel = new Button();
        // ……

        panel->add(ok);
        panel->add(cancel);
        dialog->add(panel);
    }

    // 想象这里会发生什么。
    void onF1KeyPress(Component* component) {
        component->showHelp();
    }

    void testPanel() {
        onF1KeyPress(panel);
    }

    void testDialog() {
        onF1KeyPress(dialog);
    }

    void testOKButton() {
        onF1KeyPress(ok);
    }

    void testCancelButton() {
        onF1KeyPress(cancel);
    }
};

int main() {
    Dialog *d = new Dialog;

    Application app;
    app.createUI();
    
    app.testPanel();
    std::cout << "-------------------------------" << std::endl;
    
    app.testDialog();
    std::cout << "--------------------------------" << std::endl;
    
    app.testOKButton();
    std::cout << "--------------------------------" << std::endl;
    
    app.testCancelButton();
    std::cout << "--------------------------------" << std::endl;
    
    return 0;
}
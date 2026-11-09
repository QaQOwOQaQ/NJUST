#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stack>

// 命令基类
    // 命令相当于 Application 和 Editor 之间的中介
class Command {
protected:
    class Application* app;
    class Editor* editor;
    std::string backup;

public:
    Command(Application* app, Editor* editor) : app(app), editor(editor) {}
    virtual ~Command() = default;

    void saveBackup() { backup = editor->text; }
    void undo() { editor->text = backup; }
    virtual bool execute() = 0; // 如果修改了编辑器的状态,返回true,表示需要存入历史记录
};

// 具体命令
class CopyCommand : public Command {
public:
    using Command::Command; // 继承父类的构造函数，减少代码重复
    bool execute() override {
        app->clipboard = editor->getSelection();
        return false;
    }
};

class CutCommand : public Command {
public:
    using Command::Command;
    bool execute() override {
        saveBackup();
        app->clipboard = editor->getSelection();
        editor->deleteSelection();
        return true;
    }
};

class PasteCommand : public Command {
public:
    using Command::Command;
    bool execute() override {
        saveBackup();
        editor->replaceSelection(app->clipboard);
        return true;
    }
};

class UndoCommand : public Command {
public:
    using Command::Command;
    bool execute() override {
        app->undo();
        return false;
    }
};

// 编辑器类
class Editor {
public:
    std::string text;
    std::string getSelection() const { return text; }
    void deleteSelection() { text.clear(); }
    void replaceSelection(const std::string& newText) { text = newText; }
};

// 命令历史记录
class CommandHistory {
private:
    std::stack<std::shared_ptr<Command>> history;

public:
    void push(std::shared_ptr<Command> command) { history.push(command); }
    std::shared_ptr<Command> pop() {
        if (history.empty()) return nullptr;
        auto command = history.top();
        history.pop();
        return command;
    }
};

// 应用程序
class Application {
public:
    std::string clipboard;
    std::vector<std::shared_ptr<Editor>> editors;
    std::shared_ptr<Editor> activeEditor;
    CommandHistory history;

    void executeCommand(std::shared_ptr<Command> command) {
        if (command->execute()) {
            history.push(command);
        }
    }
    void undo() { 
        auto command = history.pop();
        if (command) command->undo();
    }
};

int main() 
{
   return 0; 
}
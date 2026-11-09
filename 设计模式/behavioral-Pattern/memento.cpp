#include <iostream>
#include <memory>
#include <string>

// 前向声明
class Snapshot;

// 编辑器类（原发器）
class Editor {
private:
    std::string text;
    int curX, curY, selectionWidth;

public:
    void setText(const std::string& newText) { text = newText; }
    void setCursor(int x, int y) { curX = x; curY = y; }
    void setSelectionWidth(int width) { selectionWidth = width; }

    void printState() const {
        std::cout << "Text: " << text << ", Cursor: (" << curX << ", " << curY
                  << "), Selection Width: " << selectionWidth << std::endl;
    }

    // 创建快照（备忘录）
    std::shared_ptr<Snapshot> createSnapshot();
};

// 备忘录类（不可变）
class Snapshot {
private:
    Editor* editor;
    std::string text;
    int curX, curY, selectionWidth;

public:
    Snapshot(Editor* editor, std::string text, int x, int y, int width)
        : editor(editor), text(std::move(text)), curX(x), curY(y), selectionWidth(width) {}

    void restore() {
        editor->setText(text);
        editor->setCursor(curX, curY);
        editor->setSelectionWidth(selectionWidth);
    }
};

// 实现 `Editor::createSnapshot()`
std::shared_ptr<Snapshot> Editor::createSnapshot() {
    return std::make_shared<Snapshot>(this, text, curX, curY, selectionWidth);
}

// 命令类（管理备忘录）
class Command {
private:
    std::shared_ptr<Snapshot> backup;
    Editor* editor;

public:
    Command(Editor* editor) : editor(editor) {}

    void makeBackup() {
        backup = editor->createSnapshot();
    }

    void undo() {
        if (backup) {
            std::cout << "Undoing changes...\n";
            backup->restore();
        }
    }
};

// 测试代码
int main() {
    Editor editor;
    Command command(&editor);

    // 设置初始状态
    editor.setText("Hello, World!");
    editor.setCursor(5, 10);
    editor.setSelectionWidth(3);
    std::cout << "Initial State:\n";
    editor.printState();

    // 备份当前状态
    command.makeBackup();

    // 修改状态
    editor.setText("Modified Text");
    editor.setCursor(15, 20);
    editor.setSelectionWidth(5);
    std::cout << "\nAfter Modification:\n";
    editor.printState();

    // 撤销操作
    command.undo();
    std::cout << "\nAfter Undo:\n";
    editor.printState();

    return 0;
}

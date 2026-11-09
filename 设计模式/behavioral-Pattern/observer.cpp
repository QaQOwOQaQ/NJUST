#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <fstream>

// 订阅者接口
class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void update(const std::string& filename) = 0;
};

// 事件管理器（发布者）
class EventManager {
private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<EventListener>>> listeners;

public:
    void subscribe(const std::string& eventType, std::shared_ptr<EventListener> listener) {
        listeners[eventType].push_back(listener);
    }

    void unsubscribe(const std::string& eventType, std::shared_ptr<EventListener> listener) {
        auto& lst = listeners[eventType];  
        lst.erase(std::remove(lst.begin(), lst.end(), listener), lst.end()); // 修正 std::remove
    }

    void notify(const std::string& eventType, const std::string& data) {
        if (listeners.find(eventType) != listeners.end()) {
            for (auto& listener : listeners[eventType]) {
                listener->update(data);
            }
        }
    }
};

// 具体发布者（文件编辑器）
class Editor {
public:
    EventManager events;
    std::string file;

    void openFile(const std::string& path) {
        file = path;
        events.notify("open", file);
    }

    void saveFile() {
        events.notify("save", file);
    }
};

// 具体订阅者：日志监听器
class LoggingListener : public EventListener {
private:
    std::string logFilename;
    std::string message;

public:
    LoggingListener(const std::string& logFile, const std::string& msg)
        : logFilename(logFile), message(msg) {}

    void update(const std::string& filename) override {
        std::ofstream logFile(logFilename, std::ios::app);
        if (logFile.is_open()) {
            logFile << message << ": " << filename << std::endl;
        }
    }
};

// 具体订阅者：邮件通知监听器
class EmailAlertsListener : public EventListener {
private:
    std::string email;
    std::string message;

public:
    EmailAlertsListener(const std::string& emailAddr, const std::string& msg)
        : email(emailAddr), message(msg) {}

    void update(const std::string& filename) override {
        std::cout << "发送邮件至 " << email << "：" << message << ": " << filename << std::endl;
    }
};

// 应用程序
class Application {
public:
    void config() {
        Editor editor;

        auto logger = std::make_shared<LoggingListener>(
            "log.txt", "有人打开了文件"
        );
        editor.events.subscribe("open", logger);

        auto emailAlerts = std::make_shared<EmailAlertsListener>(
            "admin@example.com", "有人更改了文件"
        );
        editor.events.subscribe("save", emailAlerts);

        // 模拟操作
        editor.openFile("test.txt");
        editor.saveFile();
    }
};

int main() {
    Application app;
    app.config();
    return 0;
}

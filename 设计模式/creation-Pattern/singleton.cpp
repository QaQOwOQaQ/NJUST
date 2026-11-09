#include <iostream>
#include <mutex>

class Database {
private:
    static Database* instance;
    static std::mutex _mutex;

    // 私有构造函数，防止外部实例化
    Database() {
        // 初始化数据库连接
    }

public:
    // 删除拷贝构造和赋值运算符，确保单例
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    static Database* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> _lock(_mutex);
            if (instance == nullptr) {
                instance = new Database();
            }
        }
        return instance;
    }

    void query(const std::string& sql) {
        std::cout << "Executing query: " << sql << std::endl;
    }
};

// 初始化静态成员
Database* Database::instance = nullptr;
std::mutex Database::_mutex;

int main() {
    Database* foo = Database::getInstance();
    foo->query("SELECT * FROM users");

    Database* bar = Database::getInstance();
    bar->query("SELECT * FROM orders");

    // foo 和 bar 指向同一个实例
    return 0;
}

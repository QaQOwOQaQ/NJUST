// 当数据即将被写入磁盘前，装饰对数据进行加密和压缩。在原始类对改变毫无察觉的情况下，将加密后的受保护数据写入文件。
// 当数据刚从磁盘读出后，同样通过装饰对数据进行解压和解密。
#include <iostream>
#include <memory>
#include <string>

using namespace std;

// 数据源接口
class DataSource {
public:
    virtual void writeData(const string& data) = 0;
    virtual string readData() = 0;
    virtual ~DataSource() = default;
};

// 具体组件：文件数据源
class FileDataSource : public DataSource {
private:
    string filename;
public:
    FileDataSource(string filename) : filename(filename) {}

    void writeData(const string& data) override {
        cout << "Writing data to file: " << filename << endl;
        // 实际写入文件的逻辑（这里省略）
    }

    string readData() override {
        cout << "Reading data from file: " << filename << endl;
        return "file_data"; // 模拟返回数据
    }
};

// 装饰基类
class DataSourceDecorator : public DataSource {
protected:
    shared_ptr<DataSource> wrappee;
public:
    DataSourceDecorator(shared_ptr<DataSource> source) : wrappee(source) {}

    void writeData(const string& data) override {
        wrappee->writeData(data);
    }

    string readData() override {
        return wrappee->readData();
    }
};

// 具体装饰：加密
class EncryptionDecorator : public DataSourceDecorator {
public:
    EncryptionDecorator(shared_ptr<DataSource> source) : DataSourceDecorator(source) {}

    void writeData(const string& data) override {
        string encryptedData = "Encrypted(" + data + ")";
        cout << "Encrypting data before writing\n";
        wrappee->writeData(encryptedData);
    }

    string readData() override {
        string data = wrappee->readData();
        cout << "Decrypting data after reading\n";
        return "Decrypted(" + data + ")";
    }
};

// 具体装饰：压缩
class CompressionDecorator : public DataSourceDecorator {
public:
    CompressionDecorator(shared_ptr<DataSource> source) : DataSourceDecorator(source) {}

    void writeData(const string& data) override {
        string compressedData = "Compressed(" + data + ")";
        cout << "Compressing data before writing\n";
        wrappee->writeData(compressedData);
    }

    string readData() override {
        string data = wrappee->readData();
        cout << "Decompressing data after reading\n";
        return "Decompressed(" + data + ")";
    }
};

// 客户端代码示例
class Application {
public:
    void dumbUsageExample() {
        shared_ptr<DataSource> source = make_shared<FileDataSource>("somefile.dat");

        source->writeData("salaryRecords");  // 直接写入

        source = make_shared<CompressionDecorator>(source);
        source->writeData("salaryRecords");  // 先压缩再写入

        source = make_shared<EncryptionDecorator>(source);
        source->writeData("salaryRecords");  // 先加密再压缩再写入
    }
};

// 运行示例
int main() {
    Application app;
    app.dumbUsageExample();
    return 0;
}
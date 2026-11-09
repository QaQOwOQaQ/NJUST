#include <iostream>
#include <memory>

using namespace std;

// “实现部分”接口：设备
class Device {
public:
    virtual bool isEnabled() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual int getVolume() = 0;
    virtual void setVolume(int percent) = 0;
    virtual int getChannel() = 0;
    virtual void setChannel(int channel) = 0;
    virtual ~Device() = default;
};

// “具体实现”类：电视
class Tv : public Device {
private:
    bool power = false;
    int volume = 50;
    int channel = 1;
public:
    bool isEnabled() override { return power; }
    void enable() override { power = true; cout << "TV is ON\n"; }
    void disable() override { power = false; cout << "TV is OFF\n"; }
    int getVolume() override { return volume; }
    void setVolume(int percent) override { volume = percent; cout << "TV Volume: " << volume << "\n"; }
    int getChannel() override { return channel; }
    void setChannel(int ch) override { channel = ch; cout << "TV Channel: " << channel << "\n"; }
};

// “具体实现”类：收音机
class Radio : public Device {
private:
    bool power = false;
    int volume = 30;
    int channel = 88;
public:
    bool isEnabled() override { return power; }
    void enable() override { power = true; cout << "Radio is ON\n"; }
    void disable() override { power = false; cout << "Radio is OFF\n"; }
    int getVolume() override { return volume; }
    void setVolume(int percent) override { volume = percent; cout << "Radio Volume: " << volume << "\n"; }
    int getChannel() override { return channel; }
    void setChannel(int ch) override { channel = ch; cout << "Radio Frequency: " << channel << " MHz\n"; }
};

// “抽象部分”：基础遥控器
class RemoteControl {
protected:
    shared_ptr<Device> device;
public:
    RemoteControl(shared_ptr<Device> dev) : device(dev) {}

    void togglePower() {
        if (device->isEnabled()) {
            device->disable();
        } else {
            device->enable();
        }
    }

    void volumeDown() { device->setVolume(device->getVolume() - 10); }
    void volumeUp() { device->setVolume(device->getVolume() + 10); }
    void channelDown() { device->setChannel(device->getChannel() - 1); }
    void channelUp() { device->setChannel(device->getChannel() + 1); }
};

// “扩展抽象部分”：高级遥控器，增加静音功能
class AdvancedRemoteControl : public RemoteControl {
public:
    AdvancedRemoteControl(shared_ptr<Device> dev) : RemoteControl(dev) {}

    void mute() { 
        device->setVolume(0);
        cout << "Muted\n";
    }
};

// 客户端代码
int main() {
    shared_ptr<Device> tv = make_shared<Tv>();
    shared_ptr<RemoteControl> remote = make_shared<RemoteControl>(tv);

    remote->togglePower();  // 开启电视
    remote->volumeUp();     // 调高音量
    remote->channelUp();    // 切换频道

    shared_ptr<Device> radio = make_shared<Radio>();
    shared_ptr<AdvancedRemoteControl> advRemote = make_shared<AdvancedRemoteControl>(radio);

    advRemote->togglePower(); // 开启收音机
    advRemote->mute();        // 静音收音机
    advRemote->channelDown(); // 调整频道

    return 0;
}
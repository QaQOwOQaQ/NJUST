// 客户端仅通过接口与集合（社交网络：朋友，同事）和迭代器交互，也就不会同具体类耦合。 
// 如果你决定将应用连接到全新的社交网络，只需提供新的集合和迭代器类即可，无需修改现有代码。

#include <iostream>
#include <vector>
#include <memory>
using namespace std;

// 前向声明
class Profile;
class ProfileIterator;
class SocialNetwork;

// 具体的 Profile 类
class Profile {
    string email;
public:
    explicit Profile(string email) : email(move(email)) {}
    string getEmail() const { return email; }
};

// 迭代器接口
class ProfileIterator {
public:
    virtual ~ProfileIterator() = default;
    virtual bool hasMore() = 0;
    virtual shared_ptr<Profile> getNext() = 0;
};

// 具体迭代器类
class WeChatIterator : public ProfileIterator {
    vector<shared_ptr<Profile>> profiles;
    size_t currentPosition = 0;

public:
    explicit WeChatIterator(vector<shared_ptr<Profile>> profiles) : profiles(move(profiles)) {}

    bool hasMore() override {
        return currentPosition < profiles.size();
    }

    shared_ptr<Profile> getNext() override {
        if (!hasMore()) return nullptr;
        return profiles[currentPosition++];
    }
};

// 社交网络接口
class SocialNetwork {
public:
    virtual ~SocialNetwork() = default;
    virtual unique_ptr<ProfileIterator> createFriendsIterator(const string& profileId) = 0;
    virtual unique_ptr<ProfileIterator> createCoworkersIterator(const string& profileId) = 0;
};

// 具体的社交网络类
class WeChat : public SocialNetwork {
    vector<shared_ptr<Profile>> friends;
    vector<shared_ptr<Profile>> coworkers;

public:
    WeChat() {
        friends.emplace_back(make_shared<Profile>("friend1@wechat.com"));
        friends.emplace_back(make_shared<Profile>("friend2@wechat.com"));
        coworkers.emplace_back(make_shared<Profile>("coworker1@wechat.com"));
    }

    unique_ptr<ProfileIterator> createFriendsIterator(const string&) override {
        return make_unique<WeChatIterator>(friends);
    }

    unique_ptr<ProfileIterator> createCoworkersIterator(const string&) override {
        return make_unique<WeChatIterator>(coworkers);
    }
};


// 发送垃圾邮件的类
class SocialSpammer {
public:
    void send(ProfileIterator& iterator, const string& message) {
        while (iterator.hasMore()) {
            auto profile = iterator.getNext();
            cout << "Sending message to " << profile->getEmail() << ": " << message << endl;
        }
    }
};

// 应用程序
class Application {
    unique_ptr<SocialNetwork> network;
    SocialSpammer spammer;

public:
    void config(bool useWeChat) {
        if (useWeChat) {
            network = make_unique<WeChat>();
        }
    }

    void sendSpamToFriends(const string& profileId) {
        auto iterator = network->createFriendsIterator(profileId);
        spammer.send(*iterator, "非常重要的消息");
    }

    void sendSpamToCoworkers(const string& profileId) {
        auto iterator = network->createCoworkersIterator(profileId);
        spammer.send(*iterator, "非常重要的消息");
    }
};

int main() {
    Application app;
    app.config(true);
    app.sendSpamToFriends("user123");
    app.sendSpamToCoworkers("user123");
    return 0;
}

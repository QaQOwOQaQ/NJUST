#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>

using namespace std;

// 远程服务接口
class ThirdPartyTVLib {
public:
    virtual vector<string> listVideos() = 0;
    virtual string getVideoInfo(int id) = 0;
    virtual void downloadVideo(int id) = 0;
    virtual ~ThirdPartyTVLib() = default;
};

// 真实的第三方服务
class ThirdPartyTVClass : public ThirdPartyTVLib {
public:
    vector<string> listVideos() override {
        cout << "Fetching video list from Tencent Video...\n";
        return {"Video1", "Video2", "Video3"};
    }

    string getVideoInfo(int id) override {
        cout << "Fetching video info for ID: " << id << " from Tencent Video...\n";
        return "Video Info for ID " + to_string(id);
    }

    void downloadVideo(int id) override {
        cout << "Downloading video with ID: " << id << " from Tencent Video...\n";
    }
};

// 代理类：缓存数据以减少网络请求
class CachedTVClass : public ThirdPartyTVLib {
private:
    shared_ptr<ThirdPartyTVLib> service;
    vector<string> listCache;
    unordered_map<int, string> videoCache;
    bool needReset = true;

public:
    CachedTVClass(shared_ptr<ThirdPartyTVLib> service) : service(service) {}

    vector<string> listVideos() override {
        if (listCache.empty() || needReset) {
            listCache = service->listVideos();
            needReset = false;
        }
        return listCache;
    }

    string getVideoInfo(int id) override {
        if (videoCache.find(id) == videoCache.end() || needReset) {
            videoCache[id] = service->getVideoInfo(id);
            needReset = false;
        }
        return videoCache[id];
    }

    void downloadVideo(int id) override {
        if (videoCache.find(id) == videoCache.end() || needReset) {
            service->downloadVideo(id);
        }
    }
};

// GUI 类
class TVManager {
protected:
    shared_ptr<ThirdPartyTVLib> service;

public:
    TVManager(shared_ptr<ThirdPartyTVLib> service) : service(service) {}

    void renderVideoPage(int id) {
        string info = service->getVideoInfo(id);
        cout << "Rendering video page: " << info << endl;
    }

    void renderListPanel() {
        vector<string> list = service->listVideos();
        cout << "Rendering video thumbnails:\n";
        for (const auto &video : list) {
            cout << " - " << video << endl;
        }
    }

    void reactOnUserInput() {
        renderListPanel();
        renderVideoPage(1);
    }
};

// 客户端应用
class Application {
public:
    void init() {
        shared_ptr<ThirdPartyTVLib> aTVService = make_shared<ThirdPartyTVClass>();
        shared_ptr<ThirdPartyTVLib> aTVProxy = make_shared<CachedTVClass>(aTVService);
        TVManager manager(aTVProxy);

        // 用户请求视频列表和视频详情
        manager.reactOnUserInput();
    }
};

// 运行示例
int main() {
    Application app;
    app.init();
    return 0;
}

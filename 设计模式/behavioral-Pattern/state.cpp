// 用状态类的形式替代满是 if-else 的状态机
#include <iostream>
#include <memory>

using namespace std;

// 预先声明 AudioPlayer 类
class AudioPlayer;

// ======================= 抽象状态类 =======================
class State {
protected:
    AudioPlayer* player;

public:
    State(AudioPlayer* player) : player(player) {}
    virtual ~State() = default;

    virtual void clickLock() = 0;
    virtual void clickPlay() = 0;
    virtual void clickNext() = 0;
    virtual void clickPrevious() = 0;
};

// ======================= 具体状态：LockedState =======================
class LockedState : public State {
public:
    LockedState(AudioPlayer* player) : State(player) {}

    void clickLock() override;
    void clickPlay() override { cout << "Locked: Ignoring Play\n"; }
    void clickNext() override { cout << "Locked: Ignoring Next\n"; }
    void clickPrevious() override { cout << "Locked: Ignoring Previous\n"; }
};

// ======================= 具体状态：ReadyState =======================
class ReadyState : public State {
public:
    ReadyState(AudioPlayer* player) : State(player) {}

    void clickLock() override;
    void clickPlay() override;
    void clickNext() override;
    void clickPrevious() override;
};

// ======================= 具体状态：PlayingState =======================
class PlayingState : public State {
public:
    PlayingState(AudioPlayer* player) : State(player) {}

    void clickLock() override;
    void clickPlay() override;
    void clickNext() override;
    void clickPrevious() override;
};

// ======================= 上下文类：AudioPlayer =======================
class AudioPlayer {
private:
    unique_ptr<State> state;
    bool playing = false;

public:
    AudioPlayer() {
        state = make_unique<ReadyState>(this);
    }

    void changeState(unique_ptr<State> newState) {
        state = move(newState);
    }

    void clickLock() { state->clickLock(); }
    void clickPlay() { state->clickPlay(); }
    void clickNext() { state->clickNext(); }
    void clickPrevious() { state->clickPrevious(); }

    void startPlayback() {
        playing = true;
        cout << "Playing music...\n";
    }

    void stopPlayback() {
        playing = false;
        cout << "Stopped playing.\n";
    }

    void nextSong() { cout << "Next song.\n"; }
    void previousSong() { cout << "Previous song.\n"; }

    void fastForward(int seconds) { cout << "Fast forwarding " << seconds << " seconds.\n"; }
    void rewind(int seconds) { cout << "Rewinding " << seconds << " seconds.\n"; }

    bool isPlaying() const { return playing; }
};

// =================== LockedState 方法实现 ===================
void LockedState::clickLock() {
    if (player->isPlaying())
        player->changeState(make_unique<PlayingState>(player));
    else
        player->changeState(make_unique<ReadyState>(player));
    cout << "Unlocked player.\n";
}

// =================== ReadyState 方法实现 ===================
void ReadyState::clickLock() {
    player->changeState(make_unique<LockedState>(player));
    cout << "Locked player.\n";
}

void ReadyState::clickPlay() {
    player->startPlayback();
    player->changeState(make_unique<PlayingState>(player));
}

void ReadyState::clickNext() {
    player->nextSong();
}

void ReadyState::clickPrevious() {
    player->previousSong();
}

// =================== PlayingState 方法实现 ===================
void PlayingState::clickLock() {
    player->changeState(make_unique<LockedState>(player));
    cout << "Locked while playing.\n";
}

void PlayingState::clickPlay() {
    player->stopPlayback();
    player->changeState(make_unique<ReadyState>(player));
}

void PlayingState::clickNext() {
    cout << "Single click: Fast forward 5s, Double click: Next song\n";
    player->fastForward(5);
}

void PlayingState::clickPrevious() {
    cout << "Single click: Rewind 5s, Double click: Previous song\n";
    player->rewind(5);
}

// ======================= 测试代码 =======================
int main() {
    AudioPlayer player;

    player.clickPlay();  // 进入播放状态
    player.clickNext();  // 快进 5 秒
    player.clickLock();  // 锁定
    player.clickPlay();  // 忽略（已锁定）
    player.clickLock();  // 解锁（恢复播放）
    player.clickPlay();  // 停止播放（回到 ReadyState）

    return 0;
}

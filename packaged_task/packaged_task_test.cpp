#include <functional>
#include <future>
#include <iostream>
#include <thread>

int add(int a, int b) {
    return a + b;
}

int main()
{
    std::packaged_task<int(int,int)> task(add);
    std::future<int> res = task.get_future();

    std::thread t(std::move(task), 10, 20);
    int sum = res.get();
    std::cout << "sum: " << sum << std::endl;
    t.join();

    return 0;
}
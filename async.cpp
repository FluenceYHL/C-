#include <iostream>
#include <functional>
#include <chrono>
// thread
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>

int main() {

    std::promise<int> b;
    std::future<int> f2 = b.get_future();
    std::thread([&b]{
        std::cout << "promise\n";
        b.set_value_at_thread_exit(1023);
    }).detach();


    std::packaged_task<int()> a([]{
        std::cout << "packaged_task\n";
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        return 1022;
    });
    std::future<int> f1 = a.get_future();
    std::thread(std::move(a)).detach();

    std::future<int> f3 = std::async(std::launch::deferred, []{  // async
        std::cout << "async\n";
        return 219;
    });

    f1.wait();
    f2.wait();
    f3.wait();
    return 0;
}
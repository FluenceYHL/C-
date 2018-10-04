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
    std::promise<int> promiseParam;
    std::thread t([](std::promise<int>& p){
        std::this_thread::sleep_for(std::chrono::seconds(2));
        p.set_value_at_thread_exit(1022);
    }, std::ref(promiseParam));
    std::future<int> futureParam = promiseParam.get_future();
    auto r = futureParam.get();
    std::cout << "r 的值 " << r << std::endl;

    std::promise<std::string> a;
    std::thread t2([](std::promise<std::string>& p){
    	std::this_thread::sleep_for(std::chrono::seconds(2));
    	p.set_value_at_thread_exit("YHL 0219");
    }, std::ref(a));
    std::future<std::string> arg = a.get_future();
    auto e = arg.get();
    std::cout << "e = " << e << std::endl;

    
    t2.join();
    t.join();
    return 0;
}
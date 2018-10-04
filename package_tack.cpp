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
    std::packaged_task<std::string()> task([]{
    	std::cout << "thread starts !\n";
    	std::this_thread::sleep_for(std::chrono::seconds(2));
    	std::cout << "thread ends !\n";
    	return "YHL make it\n";
    });
    std::thread t(std::ref(task));
    std::future<std::string> futureParam = task.get_future();
    auto r = futureParam.get();
    std::cout << r << std::endl;
    t.join();	
    return 0;
}
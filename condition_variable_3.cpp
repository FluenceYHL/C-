#include <iostream>
#include <functional>
#include <chrono>
// thread
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
int value;

int main() {
	std::thread t([&]{
		std::cin >> value;
		cv.notify_all();
	});
	std::unique_lock<std::mutex> lck(mtx);
	while(cv.wait_for(lck, std::chrono::seconds(1)) == std::cv_status::timeout)
		std::cout << "YHL\n";
	std::cout << "\nValue  :  " << value << std::endl;
	t.join();
	return 0;
}
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
bool ready = false;

void printId(const int id) {
	std::unique_lock<std::mutex> lck(mtx);
	while(ready == false)
		cv.wait(lck);
	std::cout << "thread  " << id << std::endl;
}

inline void go() { // {} 局部代码段, 不用担心重名
	std::unique_lock<std::mutex> lck(mtx);
	ready = true;
	cv.notify_all();
}

int main() {
	std::thread threads[10];
	for(int i = 0; i < 10; ++i)
		threads[i] = std::thread(printId, i + 1);
	std::cout << "go\n\n";
	go();
	for(auto &it : threads)
		it.join();
	return 0;
}
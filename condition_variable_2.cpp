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
int cargo = 0;
static bool Ok() { return cargo != 0; }

void consume(const int n) {
	for(int i = 0; i < n; ++i) {
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, Ok);
		std::cout << "cargo = " << cargo << std::endl;
		cargo = 0;
	}
}

int main() {
	std::thread comsumeThread(consume, 10);
	std::cout << "here\n";
	for(int i = 0;i < 10; ++i) {
		while( Ok() ) 
			std::this_thread::yield();
		std::unique_lock<std::mutex> lck(mtx);
		cargo = i + 1;
		cv.notify_all();
	}
	comsumeThread.join();
	return 0;
}
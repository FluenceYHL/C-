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
std::condition_variable produce, consume;
int cargo = 0;

void consumer() {
	std::unique_lock<std::mutex> lck(mtx);
	while(cargo == 0) 
		consume.wait(lck);
	// cargo 不为 0 才进入
	std::cout << "cargo = " << cargo << std::endl;
	cargo = 0;
	produce.notify_one(); // cargo = 0, 通知生产者
}

void producer(const int id) {
	std::unique_lock<std::mutex> lck(mtx);
	while(cargo != 0)
		produce.wait(lck);
	// cargo = 0, 才可以进入
	cargo = id;
	consume.notify_one(); // cargo 不为 0, 通知消费者
}

int main() {
	std::thread cons[10], pros[10];
	for(int i = 0;i < 10; ++i) {
		cons[i] = std::thread(consumer);
		pros[i] = std::thread(producer, i + 1);
	}
	for(int i = 0; i < 10; ++i)
		cons[i].join(), pros[i].join();
	return 0;
}

// https://blog.csdn.net/fengbingchun/article/details/73695596
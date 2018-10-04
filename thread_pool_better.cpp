#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <functional>
#include <chrono>
// thread
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace YHL {

	class thread_pool {
	private:
		// 一个线程池 + 一个任务队列, 线程不断检查是否可以执行任务
		std::vector< std::thread > pool;
		std::queue< std::function<void()> > tasks;
		// sunchronization
		std::mutex mtx;
		std::condition_variable cv;
		bool stop;
	public:
		thread_pool(const size_t);
		~thread_pool();

		// 从任务队列中获取一个任务
		std::function<void()> get_task();

		// 拓展线程池的容量
		void add_thread(const size_t);

		void scheche();

		void pause() { this->stop = true; }

		void reStart() { this->stop = false; }

		template<typename F, class... Args>
		auto enqueue(F&& fun, Args&& ...args)
			-> std::future<typename std::result_of<F(Args...)>::type>;
	};

	thread_pool::thread_pool(const size_t init_size) 
			:stop(false) {
		add_thread(init_size);
		// std::thread([&]{ this->scheche();}).detach();
	}


	// 拓展线程池的容量
	void thread_pool::add_thread(const size_t extend) {
		for(size_t i = 0;i < extend; ++i)
			this->pool.emplace_back(get_task());
	}

	void thread_pool::scheche() {
		constexpr static size_t addSize = 5;

		while(stop == false) {
			std::unique_lock<std::mutex> lck(mtx);

			cv.wait(lck, [this]{ return this->tasks.size() > pool.size(); });

			this->add_thread(addSize);

			cv.notify_one();
		}
	}

	// 从任务队列中获取一个任务
	std::function<void()> thread_pool::get_task() {
		auto task = [this] {
			for(;;) {    // 实现线程池的关键 : 每个线程轮询队列是否有未处理的任务
				std::function<void()> cur;
				do{
					std::unique_lock<std::mutex> lck(this->mtx);
					this->cv.wait(lck, [this]{ return !this->tasks.empty() || this->stop; });
					
					if(this->stop and this->tasks.empty())
						return;

					cur = std::move(this->tasks.front());
					this->tasks.pop();
				} while(0);
				
				cur();  // 本次任务结束, 继续轮询任务队列，把可以执行的任务放到线程中
			}
		};
		return task;
	}

	// 放入新的任务到队列中去
	template<typename F, class... Args>
	auto thread_pool::enqueue(F&& fun, Args&& ...args)
			-> std::future< typename std::result_of<F(Args...)>::type > {
		using return_type = typename std::result_of<F(Args...)>::type;

		auto packed_task = std::make_shared< std::packaged_task<return_type()> >(
				std::bind(std::forward<F>(fun), std::forward<Args>(args)...)
			);

		{
			std::unique_lock<std::mutex> lck(this->mtx);

			if(stop == true)
				throw std::runtime_error("enqueue task on stopped pool\n");

			this->tasks.emplace([packed_task](){ (*packed_task)(); });
		}

		this->cv.notify_one();

		std::future<return_type> res = packed_task->get_future();
		return res;
	}

	inline thread_pool::~thread_pool() {
		{
			std::unique_lock<std::mutex> lck(this->mtx);
			stop = true;
		}
		this->cv.notify_all();
		for(auto &it : pool)
			it.detach();
		pool.clear();
		pool.shrink_to_fit();
	}

}

namespace test {
	int cnt = 0;
	std::mutex m;

	int fun() {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		std::cout << "id  :  " << std::this_thread::get_id() << std::endl;
		std::lock_guard<std::mutex> lck(m);
		++cnt;
		return cnt;
	}
}

int main() {
	YHL::thread_pool pool(4);
	for(int i = 0;i < 10; ++i) {
		auto result = pool.enqueue(test::fun);
		std::cout << "answer  :  " << result.get() << std::endl;
	}

	// pool.pause();

	pool.add_thread(2);
	for(int i = 0;i < 20; ++i) {
		auto result = pool.enqueue(test::fun);
		std::cout << "answer  :  " << result.get() << std::endl;
	}
	return 0;
}
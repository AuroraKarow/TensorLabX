#ifndef Thread_Pool
#define Thread_Pool

#include <atomic>
#include <future>
#include <queue>
#include <stack>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace Tool
{
	class ThreadPool
	{
	private:
		static const int threadMaximumNum = 1024;
		using Task = std::function<void()>;

		int threadNum;
		std::atomic_int32_t idleNum;
		std::mutex syncRoot;
		std::condition_variable wakeupLock;

		std::vector<std::thread> innerPool;
		std::queue<Task> tasks;

		/* status */
		bool running = false;

	public:
		ThreadPool(int threadNum);
		~ThreadPool();

		void WaitAll();
		void WaitOne();

		template<class F, class... Args>
		auto AddTask(F&& f, Args&&... args)->std::future<decltype(f(args...))>;

	private:
		void SetIdle();
		void SetWorking();
		void AddThread(int size);
	};

	ThreadPool::ThreadPool(int threadNum)
	{
		this->threadNum = threadNum < threadMaximumNum ? threadNum : threadMaximumNum;
		running = true;
		AddThread(threadNum);
	}

	ThreadPool::~ThreadPool()
	{
		running = false;
		this->wakeupLock.notify_all();
		for (auto& thread : innerPool) {
			if (thread.joinable())
				thread.join();
		}
		std::cout << "all thread exited" << std::endl;
	}

	void ThreadPool::AddThread(int size) {
		for (; innerPool.size() < threadMaximumNum && size > 0; --size)
		{
			innerPool.emplace_back([this] {
				while (true)
				{
					Task task;
					{
						std::unique_lock<std::mutex> lock{ syncRoot };
						wakeupLock.wait(lock, [this] {
							return !running || !tasks.empty();
							});
						if (!running && tasks.empty())
							return;
						idleNum--;
						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
					{
						std::unique_lock<std::mutex> lock{ syncRoot };
						idleNum++;
					}
				}
				});
			{
				std::unique_lock<std::mutex> lock{ syncRoot };
				idleNum++;
			}
		}
	}
	void ThreadPool::SetIdle() {}
	void ThreadPool::SetWorking() {}
	void ThreadPool::WaitAll() {}
	void ThreadPool::WaitOne() {}

	template<class F, class... Args>
	auto ThreadPool::AddTask(F&& f, Args&&... args)
		->std::future<decltype(f(args...))> {
		if (!running) {
			throw std::runtime_error("commit on ThreadPool is stopped.");
		}


		using RetType = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<RetType()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);
		std::future<RetType> future = task->get_future();
		{
			std::lock_guard<std::mutex> lock(syncRoot);
			tasks.push([task] { (*task)(); });
		}

		wakeupLock.notify_one();

		return future;
	}
}

#endif
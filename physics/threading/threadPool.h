#pragma once

#include <shared_mutex>
#include <functional>
#include <vector>
#include <thread>

struct shared_lock_guard {
	std::shared_mutex& mut;
	shared_lock_guard(std::shared_mutex& mut) : mut(mut) { mut.lock_shared(); }
	~shared_lock_guard() { mut.unlock_shared(); }
};

struct inverse_lock_guard {
	std::shared_mutex& mut;
	inverse_lock_guard(std::shared_mutex& mut) : mut(mut) { mut.unlock(); }
	~inverse_lock_guard() { mut.lock(); }
};

class ThreadPool {
	std::vector<std::thread> threads;
	std::shared_mutex threadsDone;
	std::function<void()> funcToRun = []() {};
	bool shouldExit = false;

public:
	ThreadPool() : threads(std::thread::hardware_concurrency() - 1) {
		threadsDone.lock();
		for(std::thread& t : threads) {
			t = std::thread([this]() {
				while(true) {
					shared_lock_guard lg(threadsDone); // wait for work
					if(shouldExit) break;
					funcToRun();
				}
			});
		}
	}

	// cleanup
	~ThreadPool() {
		shouldExit = true;
		threadsDone.unlock(); // let threads exit
		for(std::thread& t : threads) t.join();
	}

	// this work function may only return once all work has been completed
	void doInParallel(std::function<void()>&& work) {
		funcToRun = std::move(work);
		inverse_lock_guard lg(threadsDone); // all threads start running
		funcToRun();
		// lg destructor: wait for all threads to finish
	}
};

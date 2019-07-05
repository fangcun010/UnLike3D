/*
MIT License

Copyright(c) 2019 fangcun

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <threadpool.h>

namespace unlike3d {
	void ThreadPool::InitThreadPool(unsigned int thread_count) {
		thread_count_ = thread_count;
		thread_ids_ = new unsigned int[thread_count + 1];
		thread_ids_[0] = 0;
		thread_lock_free_queues_ = new LockFreeQueue<Task> *[thread_count+1];
		thread_lock_free_queues_[0] = new LockFreeQueue<Task>();
		threads_ = new std::thread *[thread_count];
		for (unsigned int i = 0; i < thread_count; i++) {
			thread_ids_[i+1] = i + 1;
			thread_lock_free_queues_[i+1] = new LockFreeQueue<Task>();
			threads_[i] =new std::thread([&thread_pool = *this,thread_id=i+1](){
				Task task;
				for (;;) {
					if (thread_pool.GetTask(thread_id,task)) {
						thread_pool.running_thread_count_++;
						thread_pool.status_ = RUNNING;
						task.function_point(&thread_pool.thread_ids_[thread_id],task.data1_point, task.data2_point, task.data3_point,
							task.data4_point);
						thread_pool.running_thread_count_--;
						thread_pool.task_count_--;
					}
					if(thread_pool.GetTask(task)){
						thread_pool.running_thread_count_++;
						thread_pool.status_ = RUNNING;
						task.function_point(&thread_pool.thread_ids_[thread_id],task.data1_point, task.data2_point, task.data3_point,
							task.data4_point);
						thread_pool.running_thread_count_--;
						thread_pool.task_count_--;
					}
					if (thread_pool.running_thread_count_ == 0 && thread_pool.task_count_ == 0)
						thread_pool.status_ = ENDING;
					else if (thread_pool.running_thread_count_)
						thread_pool.status_ = RUNNING;
					else
						thread_pool.status_ = WAITING;
				}
			});
		}
	}

	unsigned int ThreadPool::GetStatus() {
		return status_;
	}

	void ThreadPool::ResetStatus() {
		status_ = WAITING;
	}

	bool ThreadPool::GetTask(Task &task) {
		return lock_free_queue_.Pop(task);
	}

	bool ThreadPool::GetTask(unsigned int thread_id, Task &task) {
		return thread_lock_free_queues_[thread_id]->Pop(task);
	}

	void ThreadPool::AddTask(unsigned int thread_id,const Task &task) {
		task_count_++;
		thread_lock_free_queues_[thread_id]->Push(task);
	}

	void ThreadPool::AddTask(const Task &task) {
		task_count_++;
		lock_free_queue_.Push(task);
	}

	unsigned int ThreadPool::GetThreadCount() {
		return thread_count_;
	}

	ThreadPool::ThreadPool():thread_count_(0),task_count_(0),running_thread_count_(0),status_(WAITING),thread_ids_(nullptr) {
	}


	ThreadPool::~ThreadPool() {
		if (thread_ids_) delete[]thread_ids_;
	}
}
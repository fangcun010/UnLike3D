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
#ifndef _THREADPOOL_H_INCLUDED
#define _THREADPOOL_H_INCLUDED

#include <thread>
#include <atomic>
#include <lockfree.hpp>

namespace unlike3d {
	class ThreadPool {;
	public:
		enum {
			WAITING,RUNNING, ENDING
		};
		typedef void (*VoidFunc5Point) (void *,void *,void *,void *,void *);
		struct Task {
			VoidFunc5Point function_point;
			void *data1_point;
			void *data2_point;
			void *data3_point;
			void *data4_point;
		};
	public:
		LockFreeQueue<Task> lock_free_queue_;
		std::atomic_uint task_count_;
		std::atomic_uint thread_count_;
		std::atomic_uint running_thread_count_;
		std::thread **threads_;
		LockFreeQueue<Task> **thread_lock_free_queues_;
		std::atomic_uint status_;
		unsigned int *thread_ids_;
	public:
		bool GetTask(Task &task);
		bool ThreadPool::GetTask(unsigned int thread_id, Task &task);
		void AddTask(const Task &task);
		void ThreadPool::AddTask(unsigned int thread_id, const Task &task);

		unsigned int GetStatus();
		void ResetStatus();

		unsigned int GetThreadCount();

		void InitThreadPool(unsigned int thread_count);
		ThreadPool();
		~ThreadPool();
	};
}

#endif
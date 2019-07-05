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
#ifndef _LOCKFREE_H_INCLUDED
#define _LOCKFREE_H_INCLUDED

#include <lockfreequeue.hpp>

namespace unlike3d {
	template<typename T>
	class LockFreeQueue {
	private:
		lock_free::LockFreeQueue<T> *lock_free_queue_;
		lock_free::FreeList<typename lock_free::LockFreeQueue<T>::Node> *free_list_;
	public:
		void SetAlloc(const std::function<typename lock_free::LockFreeQueue<T>::Node *()> &alloc) {
			lock_free_queue_->SetAlloc(alloc);
		}
		void SetFree(const std::function<void(typename lock_free::LockFreeQueue<T>::Node *)> &free) {
			lock_free_queue_->SetFree(free);
		}
		void Push(const T &value) {
			lock_free_queue_->Push(value);
		}
		bool Pop(T &var) {
			return lock_free_queue_->Pop(var);
		}
		void Clear() {
			lock_free_queue_->Clear();
		}
		bool Empty() {
			return lock_free_queue_->Empty();
		}
		LockFreeQueue() {
			lock_free_queue_ = new lock_free::LockFreeQueue<T>();
			free_list_ = new lock_free::FreeList<typename lock_free::LockFreeQueue<T>::Node>();
			std::function<lock_free::LockFreeQueue<T>::Node *()> alloc = [&free_list = free_list_]()
				->lock_free::LockFreeQueue<T>::Node * {
				return free_list->Alloc();
			};
			std::function<void(lock_free::LockFreeQueue<T>::Node *)> free = [&free_list = free_list_](
				lock_free::LockFreeQueue<T>::Node *node) {
					free_list->Free(node);
			};
			lock_free_queue_->SetAlloc(alloc);
			lock_free_queue_->SetFree(free);

			lock_free_queue_->Init();
		}
		~LockFreeQueue() {
			delete lock_free_queue_;
			delete free_list_;
		}
	};
}

#endif //LOCK_FREE_QUEUE_HPP_INCLUDED
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


********You need to implement your own memory manager********
********Or you will get memory leak
********
*/
#ifndef LOCK_FREE_QUEUE_HPP_INCLUDED
#define LOCK_FREE_QUEUE_HPP_INCLUDED

#ifdef _WIN64

#define LOCK_FREE_TAG_TYPE		unsigned long long
#define	LOCK_FREE_ALIGN_SIZE	8

#else

#define LOCK_FREE_TAG_TYPE		unsigned int
#define	LOCK_FREE_ALIGN_SIZE	8

#define LOCK_FREE_CACHE_ALIGN __declspec(align(LOCK_FREE_ALIGN_SIZE))

#endif

#include <Windows.h>
#include <functional>

namespace lock_free {
	template<typename T>
	class FreeList {
	public:
		struct Node {
			Node *next;
		};
	private:
		Node *top_;
		unsigned int node_sz_;
		std::function<Node *(unsigned int)> alloc_;
		std::function<void(Node *)> free_;
	public:
		FreeList() {
			node_sz_ = sizeof(T);
			if (node_sz_ < sizeof(Node)) node_sz_ = sizeof(Node);
			alloc_ = [](unsigned int sz)->Node * {
				Node *node=reinterpret_cast<Node *>(new char[sz]);
				return node;
			};
			free_ = [](Node * node) { delete[]reinterpret_cast<unsigned char *>(node); };
			top_ = nullptr;
		}
		~FreeList() {
			Clear();
		}
		void SetAlloc(const std::function<Node *(unsigned int)> &alloc) {
			alloc_ = alloc;
		}
		void SetFree(const std::function<void(Node *)> &free) {
			free_ = free;
		}
		Node *Pop() {
			Node *top;
			Node *next;
			do {
				top = top_;
				if (top == nullptr)
					return nullptr;
				next = top->next;
			} while (*(unsigned int *)&top != InterlockedCompareExchange((unsigned int *)&top_, *(unsigned int *)&next, *(unsigned int *)&top));
			return top;
		}
		T *Alloc() {
			Node *node = Pop();
			if (node == nullptr)
				return reinterpret_cast<T *>(alloc_(node_sz_));
			return reinterpret_cast<T *>(node);
		}
		void Free(T *free_node) {
			Node *node;
			node = (Node *)free_node;
			Node *top;
			do {
				top = top_;
				node->next = top;
			} while (*(unsigned int *)&top !=InterlockedCompareExchange((unsigned int *)&top_, *(unsigned int *)&node, *(unsigned int *)&top ));
		}
		void Clear() {
			Node *node;
			for (;;) {
				node = Pop();
				if (node == nullptr) break;
				free_(node);
			} 
		}
	};
	template<typename T>
		class LockFreeQueue {
		public:
			struct Node ;
			struct Ptr {
				Node *ptr;
				LOCK_FREE_TAG_TYPE tag;
			};
			bool PtrEqual(const Ptr &p1, const Ptr &p2) {
				Ptr tp1, tp2;
				tp1 = p1;
				tp2 = p2;
				return tp1.ptr == tp2.ptr && tp1.tag == tp2.tag ;
			}
			struct Node {
				Ptr next;
				T value;
			} ;
		private:
			Ptr head_;
			Ptr tail_;
			std::function<Node *()> alloc_;
			std::function<void(Node *)> free_;
		public:
			LockFreeQueue() {
				alloc_ = nullptr;
				free_ = nullptr;
			}
			~LockFreeQueue() {
				if (free_ == nullptr) return ;
				Clear();
				if (head_.ptr != nullptr)
					free_(head_.ptr);
			}
			void SetAlloc(const std::function<Node *()> &alloc) {
				alloc_ = alloc;
			}
			void SetFree(const std::function<void(Node *)> &free) {
				free_ = free;
			}
			void Init() {
				if (alloc_ == nullptr)
					alloc_ = []()->Node * { return new Node() ; };
				if (free_ == nullptr)
					free_ = [](Node * node) { delete node; };
				Node * node = alloc_();
				node->next.ptr = nullptr;
				head_.tag = 0;
				head_.ptr = node;
				tail_ = head_;
			}
			void Push(const T &value) {
				Ptr new_ptr;
				Ptr tail, next;
				Node * node = alloc_();
				node->value = value;
				node->next.ptr = nullptr;
				for (;  ;) {
					InterlockedExchange64((__int64 *)&tail, *(__int64 *)&tail_);
					InterlockedExchange64((__int64 *)&next, *(__int64 *)&tail.ptr->next);
					if (PtrEqual(tail, tail_)) {
						if (next.ptr == nullptr) {
							new_ptr = { node, next.tag + 1 };
							if (*(__int64 *)&next == InterlockedCompareExchange64((__int64 *)&tail.ptr->next, *(__int64 *)&new_ptr, *(__int64 *)&next)) {
								new_ptr = { node, tail.tag + 1 };
								InterlockedCompareExchange64((__int64 *)&tail_, *(__int64 *)&new_ptr, *(__int64 *)&tail);
								return;
							}
						}
						else {
							new_ptr = { next.ptr, tail.tag + 1 };
							InterlockedCompareExchange64((__int64 *)&tail_, *(__int64 *)&new_ptr, *(__int64 *)&tail);
						}
					}
				}
			}
			bool Pop(T &var) {
				Ptr new_ptr;
				Ptr head, tail, next;
				for (;  ;) {
					InterlockedExchange64((__int64 *)&head, *(__int64 *)&head_);
					InterlockedExchange64((__int64 *)&tail, *(__int64 *)&tail_);
					InterlockedExchange64((__int64 *)&next, *(__int64 *)&head.ptr->next);
					if (PtrEqual(head, head_)) {
						if (head.ptr == tail.ptr) {
							if (next.ptr == nullptr)
								return false ;
							new_ptr = { next.ptr, tail.tag + 1 };
							InterlockedCompareExchange64((__int64 *)&tail_, *(__int64 *)&new_ptr, *(__int64 *)&tail);
						}
						else {
							var = next.ptr->value;
							new_ptr = { next.ptr, head.tag + 1 };
							if (*(__int64 *)& head ==InterlockedCompareExchange64((__int64 *)&head_, *(__int64 *)&new_ptr, *(__int64 *)& head)) {
								free_(head.ptr);
								return true ;
							}
						}
					}
				}
			}
			void Clear() {
				T var;
				while (Pop(var)) ;
			}
			bool Empty() {
				return PtrEqual(head_, tail_) ;
			}
		};
}

#undef LOCK_FREE_TAG_TYPE
#undef LOCK_FREE_ALIGN_SIZE

#endif //LOCK_FREE_QUEUE_HPP_INCLUDED
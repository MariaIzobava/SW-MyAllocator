
#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <new>
#include <typeinfo>
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>

class MyClass {
private:
	const int x;
public:
	MyClass(int x) : x(x) {
		printf("Element with id=%d is created\n", x);
	}

	MyClass(const MyClass& x) : x(x.x) {
		printf("Element with id=%d is copied\n", x);
	}

	~MyClass() {
		printf("Element with id=%d is deleted\n", x);
	}

	int get_ID() const {
		return x;
	}
};

// __func__ macro to get function name string c++

class MemoryManager {
private:
	static constexpr int Nbytes = 100;
	int8_t memory[Nbytes];
	std::vector<bool> free_map;
	void print() const {
		for (auto memory_byte : free_map)
			printf("%d ", memory_byte);
		printf("\n");
	}

	void mark_as(int left_range, int right_range, bool flag) {
		std::fill(free_map.begin() + left_range, free_map.begin() + right_range, flag);
	}

public:
	MemoryManager() {
		free_map.resize(Nbytes);
		printf("Constructor for MyBuffer\n");

	}
	~MemoryManager() {
		printf("BUFFER IS DELETED!!\n");
	}

	MemoryManager(const MemoryManager& b) = delete;

	void free_memory(void* p, size_t n) {
		auto k = (int8_t *)p - memory;
		if (k + n - 1 >= Nbytes) throw std::bad_alloc();
		mark_as(k, k + n, 0);
		printf("%d bytes were freed in buffer\n", n);
		MemoryManager::print();
	}

	void* find_sufficient_block(const size_t& n) {
		int k = 0;
		std::vector<bool>::iterator first = free_map.begin();
		std::vector<bool>::iterator last = free_map.end();
		while (first != last) {
			std::vector<bool>::iterator first_free = std::find(first, last, 0);
			std::vector<bool>::iterator last_expected = first_free + n;
			if (last_expected > last)
				break;
			std::vector<bool>::iterator first_taken_in_range_n = std::find(first_free, last_expected, 1);
			if (first_taken_in_range_n == last_expected)
				return memory + (first_free - free_map.begin());
			else
				first = std::find(first_taken_in_range_n, last, 0);
		}

		printf("Couldn't find place for %d bytes in buffer\n", n);
		MemoryManager::print();
		return nullptr;
	}

	void fill_memory(void* p, size_t n) {
		auto k = (int8_t *)p - memory;
		if (k + n - 1 >= Nbytes) throw std::bad_alloc();
		mark_as(k, k + n, 1);
		printf("Filled space for %d bytes in buffer\n", n);
		MemoryManager::print();
	}

};

template <class T>
struct MyAllocator {

	MemoryManager* memory_man;

	typedef T value_type;

	MyAllocator() {
		printf("Constructor for MyAllocator\n");
		memory_man = new MemoryManager();
	}

	MyAllocator(MemoryManager* memory_man) : memory_man(memory_man) {
		printf("Constructor for MyAllocator\n");
	}

	~MyAllocator() {
		printf("Destructor for MyAllocator\n");
	}
	template <class U> constexpr MyAllocator(const MyAllocator<U>& a) : memory_man(a.memory_man) {
		printf("Copy constructor for MyAllocator\n");
	}

	T* allocate(std::size_t n) {
		void* p = memory_man->find_sufficient_block(n * sizeof(T));
		if (p != nullptr) {
			memory_man->fill_memory(p, n * sizeof(T));
			printf("Allocated memory for %d elements of type %s\n", n, typeid(T).name());
			return static_cast<T*>(p);
		}
		else {
			printf("ATTENTION! There is no memory for %d elements of type %s\n", n, typeid(T).name());
			throw std::bad_alloc();
		}
	}
	void deallocate(T* arr, std::size_t n) {
		memory_man->free_memory(arr, n * sizeof(T));
		printf("Memory is freed for %d elements of type %s\n", n, typeid(T).name());
	}

	template <typename U, typename ... Args>
	void construct(U * p, Args... args) {
		new((void *)p) U(std::forward<Args>(args)...);
		printf("Memory was successfuly initialized with new object of type %s\n", typeid(U).name());

	}

	void destroy(T* p) {
		p->~T();
		printf("Object was successfuly destroyed\n");
	}
};

int main() {
	freopen("output.txt", "w", stdout);
	/**MyClass a(30);
	MyClass *p = new MyClass(40);
	delete p;
	const int N = 10;
	MyClass * arr = static_cast<MyClass*>(malloc(N * sizeof(MyClass)));
	for (std::size_t i = 0; i < N; ++i) {
	auto q = new (arr + i*sizeof(MyClass)) MyClass(i+1);
	}
	for (std::size_t i = 0; i < N; ++i) {
	(arr + i*sizeof(MyClass)) -> ~MyClass();
	}
	delete arr;**/

	//MyBuffer buf;

	//MyAllocator<MyClass> allocator;

	//MyClass* p1, *p2, *p3;
	//MyClass* p = allocator.allocate(5);
	//allocator.construct(p, MyClass(13));
	//allocator.deallocate(p, 4);
	MemoryManager buf;
	MyAllocator<MyClass> my_allocator(&buf);
	std::vector<MyClass, MyAllocator<MyClass> > v(my_allocator);
	//v.push_back(5);
	printf("%d\n", v.size());
	v.push_back(MyClass(34));
	v.push_back(MyClass(123));
	v.push_back(MyClass(6));
	v.pop_back();
	v.push_back(MyClass(56));
	v.push_back(MyClass(8));
	printf("pop_back\n");
	v.pop_back();
	printf("Vector size = %d\n", v.size());
	for (auto q : v)
		printf("ID=%d ", q.get_ID());
	printf("\n");
	
}
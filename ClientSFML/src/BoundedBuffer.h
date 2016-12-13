#ifndef __BOUNDED_BUFFER_H__
#define __BOUNDED_BUFFER_H__

#include <string>
#include <mutex>
#include <condition_variable>

struct BoundedBuffer
{
	std::string* buffer_;
	int capacity_;

	int front_;
	int back_;
	int count_;

	std::mutex lock_;
	std::condition_variable notfullcon_;
	std::condition_variable notemptycon_;

	BoundedBuffer(int capacity);
	~BoundedBuffer();

	void Deposit(const std::string& data);
	std::string Fetch();
};

#endif

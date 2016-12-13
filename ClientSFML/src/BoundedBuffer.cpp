#include "BoundedBuffer.h"


BoundedBuffer::BoundedBuffer(int capacity) :
	capacity_(capacity),
	front_(0),
	back_(0),
	count_(0)
{
	buffer_ = new std::string[capacity_];
}

BoundedBuffer::~BoundedBuffer()
{
	delete[] buffer_;
}

void BoundedBuffer::Deposit(const std::string& data)
{
	std::unique_lock<std::mutex> l(lock_);

	// Block if full and wait until there is room
	notfullcon_.wait(l, [&]() {return count_ != capacity_; });

	buffer_[back_] = data;
	back_ = (back_ + 1) % capacity_;
	++count_;

	// Inform another potentially blocked thread that they are not empty and can fetch
	notemptycon_.notify_one();
}

std::string BoundedBuffer::Fetch()
{
	std::unique_lock<std::mutex> l(lock_);

	if (count_ == 0)
	{
		return nullptr;
	}

	notemptycon_.wait(l, [&]() {return count_ != 0; });

	std::string result = buffer_[front_];
	//size_t result = (size_t)front_;
	front_ = (front_ + 1) % capacity_;
	--count_;

	notfullcon_.notify_one();
	return result;// &buffer_[result];
}
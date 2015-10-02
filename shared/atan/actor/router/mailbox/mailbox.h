#pragma once


template<typename T>
class mailbox {
public:
	virtual ~mailbox()
	{
	}

private:
	virtual void push_message(T&& message) = 0;
    virtual T&& pop_message() = 0;
};


/*
 * queue.h
 *
 *  Created on: Jan 24, 2019
 *      Author: HB
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

/*
 * basic fifo queue
 */
template<typename T>
class queue {
public:
	queue();
	virtual ~queue();

	template<typename V>
	bool push(V&& value);

	T pop();

	uint32_t size() {
		return num;
	}

	uint32_t capacity() {
		return cap;
	}

	bool isEmpty() {
		return head == tail;
	}

private:

	struct element {
		T item;
		element* next;
		element() : item{}, next{this} {}
		element(T data, element* next) : item(data), next(next) {

		}
	};

	element* head;
	element* tail;
	// start of allocated list
	element* start;

	uint32_t num;
	uint32_t cap;
};

template<typename T>
queue<T>::queue() : head{new element()}, tail{head}, start{head}, num{}, cap{} {

}

template<typename T>
queue<T>::~queue() {
	element* e = start->next;
	while(e != start) {
		delete e;
		e = e->next;
	}
	delete start;
}

template<typename T> template<typename V>
bool queue<T>::push(V&& value) {
	//m.lock()
	if (tail->next == head) {
		// need new element
		element* container = new element(static_cast<V&&>(value), head);
		tail->next = container;
		tail = container;
		cap++;
	} else {
		// use existing element
		element* container = tail->next;
		tail = tail->next;
		container->item = static_cast<V&&>(value);
	}

	num++;
	//m.unlock()
	return true;
}

template<typename T>
T queue<T>::pop() {
	//m.lock()
	if(head == tail) {
		// error
		return {};
	}
	num--;
	element* e = head;
	head = head->next;
	//m.unlock()
	return e->item;
}


#endif /* QUEUE_H_ */

/*
 * Service.h
 *
 *  Created on: Oct 3, 2018
 *      Author: oneso
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include "stdio.h"
#include <utility>
#include <assert.h>


class Service {
public:
	virtual ~Service() {}
};


template<class T, typename ...Args>
class StaticService : public Service {
public:

	// called after construction, do hardware setup here
	virtual void init() {}

	static void ConstructStatic(Args&&... ar) {
		// std::forward is a wrapper for static_cast<T&&>
		static T in(std::forward<Args>(ar)...);
		inst = &in;
	}

	static T& StaticClass() {
		assert(inst);
		return *inst;
	}

	StaticService& operator=(const StaticService&) = delete;

private:
	static T* inst;

protected:
	StaticService() {}
};

template<class T, typename ...Args>
T* StaticService<T, Args...>::inst;


#endif /* SERVICE_H_ */

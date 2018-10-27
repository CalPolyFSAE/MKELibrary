/*
 * Service.h
 *
 *  Created on: Oct 3, 2018
 *      Author: oneso
 */

#pragma once

#ifndef SERVICE_H_
#define SERVICE_H_

#include "stdio.h"
#include <utility>
#include <assert.h>

class Service {
public:
	virtual void tick() = 0;
	virtual ~Service() {}
};

template<class T, typename ...Args>
class StaticService : public Service {
public:
	virtual void tick() = 0;

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



///////////////////////////
// TESTING
///////////////////////////
//#include <iostream>
//
//class DOB{
//public:
//	DOB() : a(1) {
//		echo();
//	}
//	DOB(const DOB& other) : a(other.a) {
//		echo();
//	}
//
//private:
//	void echo() {
//		std::cout << "DOB echo" << std::endl;
//	}
//
//	int a;
//};

/*class TestService : public StaticService<TestService, int> {
public:
	TestService(int a){
		printf("TESTSERVICE INITIALIZED WITH %d\n", a);
	}

	virtual void tick() override {
		printf("tick\n");
	}
};*/

#endif /* SERVICE_H_ */

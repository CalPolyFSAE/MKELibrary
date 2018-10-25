/*
 * Event.h
 *
 *  Created on: Oct 3, 2018
 */

#ifndef EVENT_H_
#define EVENT_H_

namespace SE {

template<typename T>
class Event;

template<typename Ret, typename ...Params>
class Event<Ret(Params...)> {
public:
	using stub_type = Ret(*)(void* this_ptr, Params&&...);

	Event() = default;

	Event(const Event& other) {
		this->object = other.object;
		this->method_ptr = other.method_ptr;
	}

	bool operator==(const Event& other){
		return other.method_ptr == method_ptr && other.object == object;
	}
	bool operator!=(const Event& other){
		return other.method_ptr != method_ptr || other.object != object;
	}

	template<class T, Ret(T::*TMethod)(Params...)>
	static Event create(T* object){
		return Event(object, method_stub<T, TMethod>);
	}

	template<class T, Ret(T::*TMethod)(Params...) const>
	static Event create(const T* object){
		return Event(object, method_stub_const<T, TMethod>);
	}

	template<Ret(*TMethod)(Params...)>
	static Event create(){
		return Event(nullptr, function_stub<TMethod>);
	}


	Ret operator()(Params&&... params){
		return method_ptr(object, static_cast<Params&&>(params)...);
	}

private:

	Event(void* this_ptr, stub_type stub) :
			object(this_ptr), method_ptr(stub) {
	}

	template<class T, Ret(T::*TMethod)(Params...)>
	static Ret method_stub(void* this_ptr, Params&&... params){
		T* obj = static_cast<T*>(this_ptr);
		return (obj->*TMethod)(static_cast<Params&&>(params)...);
	}

	template<class T, Ret(T::*TMethod)(Params...) const>
	static Ret method_stub_const(void* this_ptr, Params&&... params){
		T* const obj = static_cast<T*>(this_ptr);
		return (obj->*TMethod)(static_cast<Params&&>(params)...);
	}

	template<Ret(*TMethod)(Params...)>
	static Ret function_stub(Params&&... params){
		return (*TMethod)(static_cast<Params&&>(params)...);
	}

	void* object = nullptr;
	stub_type method_ptr = nullptr;
};

}

#endif /* EVENT_H_ */

/*
 * Event.h
 *
 *  Created on: Oct 3, 2018
 */

#ifndef EVENT_H_
#define EVENT_H_

namespace se {

template<typename T>
class Event;

template<typename Ret, typename ...Params>
class Event<Ret(Params...)> {
public:
	using stub_type = Ret(*)(void* this_ptr, Params&&...);

	Event() = default;
	/*!
	 * @brief copy constructor
	 */
	Event(const Event& other) {
		this->object = other.object;
		this->method_ptr = other.method_ptr;
	}

	/*!
	 * @brief Check if the function pointer is valid.
	 *
	 * @retval Is the Event pointer null
	 */
	bool isNull() const {
		return method_ptr == nullptr;
	}

	bool operator==(const Event& other) const {
		return other.method_ptr == method_ptr && other.object == object;
	}
	bool operator!=(const Event& other) const {
		return other.method_ptr != method_ptr || other.object != object;
	}

	/*!
	 * @brief create an Event from a member function
	 *
	 * @retval Event created
	 */
	template<class T, Ret(T::*TMethod)(Params...)>
	static Event create(T* object){
		return Event(object, method_stub<T, TMethod>);
	}

	/*!
	 * @brief create an Event from a const qualified member function
	 *
	 * @retval Event created
	 */
	template<class T, Ret(T::*TMethod)(Params...) const>
	static Event create(const T* object){
		return Event(object, method_stub_const<T, TMethod>);
	}

	/*!
	 * @brief create an Event from a non-member function
	 *
	 * @retval Event created
	 */
	template<Ret (*TMethod)(Params...)>
	static Event create() {
		return Event(nullptr, function_stub<TMethod>);
	}

	/*!
	 * @brief executes function pointer
	 *
	 * @param params variadic arguments
	 * @retval specified function return value
	 */
	inline Ret operator()(Params... params) const {
		return (*method_ptr)(object, static_cast<Params&&>(params)...);
	}

private:

	Event(void* this_ptr, stub_type stub) :
			object(this_ptr), method_ptr(stub) {
	}

	template<class T, Ret (T::*TMethod)(Params...)>
	static Ret method_stub(void* this_ptr, Params&&... params) {
		T* obj = static_cast<T*>(this_ptr);
		return (obj->*TMethod)(static_cast<Params&&>(params)...);
	}

	template<class T, Ret (T::*TMethod)(Params...) const>
	static Ret method_stub_const(void* this_ptr, Params&&... params) {
		T* const obj = static_cast<T*>(this_ptr);
		return (obj->*TMethod)(static_cast<Params&&>(params)...);
	}

	template<Ret (*TMethod)(Params...)>
	static Ret function_stub(void* this_ptr, Params&&... params) {
		return (TMethod)(static_cast<Params&&>(params)...);
	}

	void* object = nullptr;
	stub_type method_ptr = nullptr;
};

}

#endif /* EVENT_H_ */

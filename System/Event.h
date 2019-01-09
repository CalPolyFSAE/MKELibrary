/*
 * Event.h
 *
 *  Created on: Oct 3, 2018
 *
 *  Description: This is a faster drop in replacement for std::function
 *  TODO: rename to delegate and namespace should be util
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
	Event(const Event& other) : object(other.object), method_ptr(other.method_ptr) {

	}

	/*!
	 * @brief assignment operator
	 */
	Event& operator=(Event rhs) {
		object = std::move(rhs.object);
		method_ptr = std::move(rhs.method_ptr);
		return *this;
	}

	/*!
	 * @brief Check if the function pointer is valid.
	 *
	 * @retval Is the Event pointer null
	 */
	bool isNull() const noexcept {
		return method_ptr == nullptr;
	}

	/*!
	 * @brief Check if the object pointer is valid.
	 */
	bool operator==(std::nullptr_t) const noexcept {
		return object == nullptr;
	}
	bool operator!=(std::nullptr_t) const noexcept {
		return object != nullptr;
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
	template<Ret(*TMethod)(Params...)>
	static Event create(){
		return Event(nullptr, function_stub<TMethod>);
	}

	/*!
	 * @brief executes function pointer
	 *
	 * @param params variadic arguments
	 * @retval specified function return value
	 */
	template<typename ...T>
	inline Ret operator()(T&&... params) const {
		// static cast <Params&&>(params)... is the same as std::forward
		return (*method_ptr)(object, static_cast<Params&&>(params)...);
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

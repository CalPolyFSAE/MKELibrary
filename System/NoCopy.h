/*
 * NoCopy.h
 *
 *  Created on: Jan 23, 2019
 *      Author: HB
 */

#ifndef NOCOPY_H_
#define NOCOPY_H_

/*
 * templated base type allows empty base optimization as
 * 	it can be unique to each base class
 */
template<typename T>
class NoCopy<T> {
protected:
	// do not allow construction or destruction of object outside class
	NoCopy(){}
	~NoCopy(){}

private:
	NoCopy(const NoCopy&);

	NoCopy& operator=(const NoCopy&);
};


#endif /* NOCOPY_H_ */

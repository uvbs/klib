/*******************************************************************************
*
* FileName : Auto_ptr.h
* Comment  : smart pointer auto pointer, it must point to an object on heap memory
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-10 1:25
*
*******************************************************************************/

#ifndef _AUTO_PTR_H_INCLUDED
#define _AUTO_PTR_H_INCLUDED

template<typename T>
class Auto_ptr {
public:
	explicit Auto_ptr(T* p = 0);
	template<typename Y> Auto_ptr(Auto_ptr<Y>& rhs);
	~Auto_ptr();

	template<typename Y>
	Auto_ptr<T>& operator=(Auto_ptr<Y>& rhs);
	T& operator*() const;
	T* operator->() const;
	
	T* get() const;
	T* release();
	void reset(T* p = 0);

private:
	T* pointee;
};

template<typename T>
inline Auto_ptr<T>::Auto_ptr(T *p) : pointee(p)
{
}

template<typename T>
template<typename Y>
inline Auto_ptr<T>::Auto_ptr(Auto_ptr<Y> &rhs) : pointee(rhs.release)
{
}

template<typename T>
inline Auto_ptr<T>::~Auto_ptr()
{
	delete pointee;
}

template<typename T>
template<typename Y>
inline Auto_ptr<T>& Auto_ptr<T>::operator=(Auto_ptr<Y>& rhs)
{
	if (this != &rhs)
		reset(rhs.release);
	return *this;
}

template<typename T>
inline T& Auto_ptr<T>::operator*() const
{
	return *pointee;
}

template<typename T>
inline T* Auto_ptr<T>::operator->() const
{
	return pointee;
}

template<typename T>
inline T* Auto_ptr<T>::get() const
{
	return pointee;
}

template<typename T>
inline T* Auto_ptr<T>::release()
{
	T* old_pointee = pointee;
	pointee = 0;
	return old_pointee;
}

template<typename T>
inline void Auto_ptr<T>::reset(T *p = 0)
{
	if (pointee != p) {
		delete pointee;
		pointee = p;
	}
}

#endif
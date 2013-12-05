/*******************************************************************************
*
* FileName : reference_count.h
* Comment  : for reference count. RC class should be publicly inherited. RCptr is
*			 a smart pointer which should point to class inherit from RC
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-10 11:36
*
*******************************************************************************/

#ifndef _REFERENCE_COUNT_H_INCLUDED
#define _REFERENCE_COUNT_H_INCLUDED

//base class for reference counted object
class RC {
public:
	RC();
	RC(const RC& other);
	RC& operator=(const RC& other);
	virtual ~RC() = 0;

	void add_reference();
	void remove_reference();

	void mark_unsharealbe();
	bool is_shareable() const;
	bool is_shared() const;

private:
	int ref_count;
	bool shareable;
};

RC::RC() : ref_count(0), shareable(true)
{
}

RC::RC(const RC &other) : ref_count(0), shareable(true)
{
}

RC& RC::operator =(const RC &other)
{
	return *this;
}

RC::~RC()
{
}

void RC::add_reference()
{
	++ref_count;
}

void RC::remove_reference()
{
	if (--ref_count == 0)
		delete this;
}

void RC::mark_unsharealbe()
{
	shareable = false;
}

bool RC::is_shareable() const
{
	return shareable;
}

bool RC::is_shared() const
{
	return ref_count > 1;
}

//T must be inherit from a RC
template<class T>
class RCptr {
public:
	RCptr(T* pointee = 0);
	RCptr(const RCptr& other);
	~RCptr();

	RCptr& operator=(const RCptr& other);

	T& operator*() const;
	T* operator->() const;

private:
	void init();

	T* hb_pointee;
};

template<class T>
RCptr<T>::RCptr(T *pointee = 0) : hb_pointee(pointee)
{
	init();
}

template<class T>
RCptr<T>::RCptr(const RCptr<T> &other) : hb_pointee(other.hb_pointee)
{
	init()
}

template<class T>
void RCptr<T>::init()
{
	if (0 == hb_pointee)
		return;
	if (pointee->is_shareable() == false) {
		hb_pointee = new T(*hb_pointee);
	}
	hb_pointee->add_reference();
}

template<class T>
RCptr<T>::~RCptr()
{
	if (hb_pointee != 0)
		hb_pointee->remove_reference();
}

template<class T>
RCptr<T>& RCptr<T>::operator =(const RCptr<T> &other)
{
	if (hb_pointee != other.hb_pointee) {
		T* old_pointee = hb_pointee;
		hb_pointee = other.hb_pointee;
		init();
		if (old_pointee != 0) {
			old_pointee->remove_reference();
		}
	}
	return *this;
}

template<class T>
T& RCptr<T>::operator *() const
{
	return *hb_pointee;
}

template<class T>
T* RCptr<T>::operator ->() const
{
	return hb_pointee;
}

#endif
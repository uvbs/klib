/*******************************************************************************
*
* FileName : uncopyable.h
* Comment  : uncopyable class, make the derived class uncopyable
* Version  : 1.0
* Author   : haibindev.cnblogs.com
* Date     : 2007-10-10 19:06
*
*******************************************************************************/

#ifndef _UNCOPYABLE_H_INCLUDED
#define _UNCOPYABLE_H_INCLUDED

//avoid copying
class Uncopyable {
public:
	Uncopyable() {}
	~Uncopyable() {}

private:
	Uncopyable(const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);
};

#endif
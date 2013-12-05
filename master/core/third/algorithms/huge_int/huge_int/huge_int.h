/*******************************************************************************
*
* FileName : huge_int.h
* Comment  : huge int, support all the arithmetic operators, I/O operators,
*			 relational operators, assignment operators, increment and 
*			 decrement operators. It will throw an exception when divided by zero
* Author   : 
* Date     : 2007-10-9 10:50
*
*******************************************************************************/

#ifndef _HUGE_INT_H_INCLUDED
#define _HUGE_INT_H_INCLUDED

#include <deque>
#include <vector>
#include <iostream>
#include <string>

using std::deque;
using std::vector;
using std::istream;
using std::ostream;
using std::string;

//empty class for exception
class Error_divided_by_zero  {};

class Huge_int {
public:
	Huge_int() : sign(true) {}
	Huge_int(int);
	Huge_int(string&) ;
	Huge_int (const Huge_int&);
	Huge_int& operator=(const Huge_int& rhs);

	Huge_int abs() const;
	Huge_int pow(int n);
    
	Huge_int& operator+=(const Huge_int& rhs);
	Huge_int& operator-=(const Huge_int& rhs);
	Huge_int& operator*=(const Huge_int& rhs);
	Huge_int& operator/=(const Huge_int& rhs);
	Huge_int& operator%=(const Huge_int& rhs);

	Huge_int& operator++();
	const Huge_int operator++(int);
	Huge_int& operator--();
	const Huge_int operator--(int);
	Huge_int operator-() const;

	//friend function
	friend Huge_int operator+(const Huge_int&,const Huge_int&);
	friend Huge_int operator-(const Huge_int&,const Huge_int&);
	friend Huge_int operator*(const Huge_int&,const Huge_int&);
	friend Huge_int operator/(const Huge_int&,const Huge_int&);
	friend Huge_int operator%(const Huge_int&,const Huge_int&);

	friend bool operator>(const Huge_int&,const Huge_int&);
	friend bool operator<(const Huge_int&,const Huge_int&);
	friend bool operator==(const Huge_int&,const Huge_int&);
	friend bool operator!=(const Huge_int&,const Huge_int&);
	friend bool operator>=(const Huge_int&,const Huge_int&);
	friend bool operator<=(const Huge_int&,const Huge_int&);

	friend ostream& operator<<(ostream&,const Huge_int&);
	friend istream& operator>>(istream&, Huge_int&);

	static const Huge_int ZERO;
	static const Huge_int ONE;
	static const Huge_int TEN;

private:
	vector<char> digits; 
	bool sign;          //  true for positive, false for negitive
	void trim();        //  remove zeros in the tail
};

const Huge_int Huge_int::ZERO=Huge_int(0);
const Huge_int Huge_int::ONE =Huge_int(1);
const Huge_int Huge_int::TEN =Huge_int(10);

//-------------------implementation------------------------
Huge_int::Huge_int(int val)
{
	if (val >= 0)
		sign = true;
	else {
		sign = false;
		val = -val;
	}
	while(true) {
		digits.push_back( static_cast<char>(val%10) );
		val /= 10;
		if (val == 0)
			break;
	}
}

Huge_int::Huge_int(string& str)
{
	sign=true;
	for ( string::reverse_iterator iter=str.rbegin() ;
		iter != str.rend(); ++iter) {
		char ch = (*iter);
		if (iter == str.rend()-1) {
			if(ch == '-') {
				sign = false;
				break;
			}
		}//if
		digits.push_back( static_cast<char>( (*iter) - '0') );
    }
    trim();
}

void Huge_int::trim()
{
	vector<char>::reverse_iterator iter = digits.rbegin();
	while(!digits.empty() && (*iter) == 0) {
		digits.pop_back();
		iter=digits.rbegin();
	}
	if(digits.empty()) {
		sign = true;
		digits.push_back(0);
	}
}

Huge_int::Huge_int(const Huge_int& rhs)
{
	sign = rhs.sign;
	digits = rhs.digits;
}

Huge_int& Huge_int::operator=(const Huge_int& rhs)
{
	digits = rhs.digits;
	sign = rhs.sign;
	return (*this);
}

Huge_int Huge_int::abs() const 
{
	if(sign)
		return *this;
	else
		return -(*this);
}

//if n is a even number: x^n = (x*x)^(n/2)
//then x^n = (x*x)^( (n-1)/2 )*x
Huge_int Huge_int::pow(int n) 
{
	Huge_int res(1);
	if (0 == n)
		return res;
	if (1 == n)
		return *this;
	if (n%2 == 0) {	//even number
		res = (*this)*(*this);
		return res.pow(n/2);
	}
	else {	//odd number
		res = (*this)*(*this);
		return res.pow(n/2)*(*this);
	}
}

Huge_int& Huge_int::operator+=(const Huge_int& rhs)
{
	if( sign == rhs.sign ) {
		vector<char>::iterator iter1 = digits.begin();
		vector<char>::const_iterator iter2 = rhs.digits.begin();

		char carry = 0;
		while ( iter1 != digits.end() && iter2 != rhs.digits.end()) {
			(*iter1) = (*iter1) + (*iter2) + carry;
			carry = ((*iter1) / 10);
			(*iter1) = (*iter1) % 10;
			iter1++;
			iter2++;
		}
		while ( iter1 != digits.end() ) {
			(*iter1) = (*iter1) + carry;
			carry = ( (*iter1) / 10 );
			(*iter1) %= 10;
			iter1++;
		}
		while ( iter2 != rhs.digits.end() ) {
			char val = (*iter2) + carry;
			carry = (val / 10) ;
			val %= 10;
			digits.push_back(val);
			iter2++;
		}
		if( carry != 0 )
			digits.push_back(carry);
	}
	else {
		if (sign)
			operator-=( -rhs );
		else {
			(*this)= -(-(*this) - rhs);
		}
	}
	return (*this);
}

Huge_int& Huge_int::operator-=(const Huge_int& rhs)
{
	if( sign == rhs.sign ) {
		if(sign)  { 
			if(*this < rhs)
				return  (*this)=-(rhs - (*this));
		} 
		else  {
			if(-(*this) > -rhs)
				return *this=-((-*this)-(-rhs));
			else
				return *this= (-rhs) - (-(*this));
		}

		vector<char>::iterator iter1 = digits.begin();
		vector<char>::const_iterator iter2 = rhs.digits.begin();
		char borrow = 0;

		while (iter2 != rhs.digits.end()) {
			(*iter1) = (*iter1) - (*iter2) - borrow;
			borrow = 0;
			if( (*iter1) < 0 ) {
				borrow=1;
				(*iter1) += 10;
			}
			iter1++;
			iter2++;
		}
		while ( iter1 != digits.end() ) {
			(*iter1) = (*iter1) - borrow;
			borrow = 0;
			if( (*iter1) < 0 ) {
				borrow=1;
				(*iter1) += 10;
			}
			else 
				break;
			iter1++;
		}
		trim();
		return *this;
	}
	else {
		if ((*this) > Huge_int::ZERO) {
			operator+=(-rhs);
			return (*this);
		}
		else {
			(*this) = -(rhs + (*this).abs());
			return (*this);
		}
	}//else
}

Huge_int& Huge_int::operator*=(const Huge_int& rhs)
{
	Huge_int result(0);
	if ((*this) == Huge_int::ZERO || rhs==Huge_int::ZERO)
		result = Huge_int::ZERO;
	else {
		vector<char>::const_iterator iter2 = rhs.digits.begin();
		while ( iter2 != rhs.digits.end() ) {
			if (*iter2 != 0) {
				deque<char> temp(digits.begin(), digits.end());
				char carry = 0;
				deque<char>::iterator iter1 = temp.begin();
				while ( iter1 != temp.end() ) {
					(*iter1) *= (*iter2);
					(*iter1) += carry;
					carry = (*iter1) / 10;
					(*iter1) %= 10;
					iter1++;
				}
				if ( carry != 0)
					temp.push_back( carry );
				int num_of_zeros = static_cast<int>( iter2 - rhs.digits.begin() );
				while (num_of_zeros--)
					temp.push_front(0);
				//convert temp type to Huge_int
				Huge_int temp2;
				temp2.digits.insert(temp2.digits.end(),temp.begin(),temp.end());
				temp2.trim();
				result += temp2;
			}
			iter2++;
		}
		result.sign = ( (sign && rhs.sign) || (!sign && !rhs.sign) );
	}   
	return (*this) = result;
}

Huge_int& Huge_int::operator/=(const Huge_int& rhs )
{
	if( rhs == Huge_int::ZERO )
		throw Error_divided_by_zero();
	Huge_int t1 = abs(), t2 = rhs.abs();
	if ( t1 < t2 ) {
		*this = Huge_int::ZERO;
		return *this;
	}

	deque<char> temp;
	vector<char>::reverse_iterator iter = t1.digits.rbegin();

	Huge_int temp2(0);
	while( iter != t1.digits.rend() ) {
		temp2 = temp2 * Huge_int::TEN + Huge_int( static_cast<int>(*iter) );
		char s = 0;
		while( temp2 >= t2 ) {
			temp2 -= t2;
			s = s + 1;
		}
		temp.push_front( s );
		iter++;
	}
	digits.clear();
	digits.insert( digits.end() , temp.begin() , temp.end() );
	trim();
	sign = ( (sign && rhs.sign) || (!sign && !rhs.sign) );
	return *this;
}

Huge_int& Huge_int::operator%=(const Huge_int& rhs)
{
	return *this -= ((*this / rhs)*rhs);
}

Huge_int Huge_int::operator-() const
{
	Huge_int temp;
	temp.digits = digits;
	temp.sign = !sign;
	return temp;
}

Huge_int& Huge_int::operator++()
{
	(*this) += Huge_int::ONE;
	return *this;
}

const Huge_int Huge_int::operator++(int)
{
	Huge_int temp(*this);
	++(*this);
	return temp;
}

Huge_int& Huge_int::operator--()
{
	return (*this) -=  Huge_int::ONE;
}

const Huge_int Huge_int::operator--(int)
{
	Huge_int temp(*this);
	--(*this);
	return temp;
}

//--------------------friend function-------------------------
Huge_int operator+(const Huge_int& lhs, const Huge_int& rhs)
{
	Huge_int temp(lhs);
	temp += rhs;
	return temp;
}
Huge_int operator-(const Huge_int& lhs, const Huge_int& rhs)
{
	Huge_int temp(lhs);
	temp -= rhs;
	return temp;
}

Huge_int operator*(const Huge_int& lhs, const Huge_int& rhs)
{
	Huge_int temp(lhs);
	temp *= rhs;
	return temp;
}

Huge_int operator/(const Huge_int& lhs, const Huge_int& rhs)
{
	Huge_int temp(lhs);
	temp /= rhs;
	return temp;
}

Huge_int operator%(const Huge_int& lhs, const Huge_int& rhs)
{
	Huge_int temp(lhs);
	temp %= rhs;
	return temp;
}

bool operator<(const Huge_int& lhs, const Huge_int& rhs)
{
	if( lhs.sign != rhs.sign )
		return !lhs.sign;
	else {
		if(lhs.digits.size() != rhs.digits.size())
			return (lhs.sign && lhs.digits.size()<rhs.digits.size())
                       || (!lhs.sign && lhs.digits.size()>rhs.digits.size());
		vector<char>::const_reverse_iterator iter1,iter2;
		iter1 = lhs.digits.rbegin();iter2 = rhs.digits.rbegin();
		while( iter1 != lhs.digits.rend() ) {
			if(  lhs.sign &&  *iter1 < *iter2 ) return true;
			if(  lhs.sign &&  *iter1 > *iter2 ) return false;
			if( !lhs.sign &&  *iter1 > *iter2 ) return true;
			if( !lhs.sign &&  *iter1 < *iter2 ) return false;
			iter1++;
			iter2++;
		}
		return false;
	}
}
bool operator==(const Huge_int& lhs, const Huge_int& rhs)
{
	if( lhs.sign != rhs.sign  || lhs.digits.size() != rhs.digits.size() )
		return false;
	vector<char>::const_iterator iter1,iter2;
	iter1 = lhs.digits.begin();
	iter2 = rhs.digits.begin();
	while( iter1!= lhs.digits.end() ) {
		if( *iter1 != *iter2 )  return false;
		iter1++;
		iter2++;
	}
	return true;
}

bool operator!=(const Huge_int& lhs, const Huge_int& rhs)
{
	return !(lhs==rhs);
}

bool operator>=(const Huge_int& lhs, const Huge_int& rhs)
{
	return (lhs>rhs) || (lhs==rhs);
}

bool operator<=(const Huge_int& lhs, const Huge_int& rhs)
{
	return (lhs<rhs) || (lhs==rhs);
}

bool operator>(const Huge_int& lhs, const Huge_int& rhs)
{
	return !(lhs<=rhs);
}

ostream& operator<<(ostream& output, const Huge_int& val)
{
	if (!val.sign)
		output<< "-";
	for ( vector<char>::const_reverse_iterator iter = val.digits.rbegin();
		iter != val.digits.rend() ; iter++)
		output<< static_cast<char>((*iter) + '0');
	return output;
}

istream& operator>>(istream& input, Huge_int& value)
{
	string str;
	input>> str;
	value = Huge_int(str);
	return input;
}

#endif
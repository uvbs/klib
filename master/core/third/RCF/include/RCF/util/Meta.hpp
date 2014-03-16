
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_META_HPP_
#define _UTIL_META_HPP_

namespace Meta {

    // Null
    struct Null {};
    
    struct True { enum { Val=true }; char a[1]; };
    struct False { enum { Val=false }; char a[2]; };

    // Boolean operations
    template<bool Cond>
    struct Bool;

    template<>
    struct Bool<true> { typedef True Val; };

    template<>
    struct Bool<false> { typedef False Val; };

    template<int Cond>
    struct ToBool { typedef True Val; };

    template<>
    struct ToBool<0> { typedef False Val; };

    template<typename T>
    struct Not;

    template<>
    struct Not<True> { typedef False Val; };

    template<>
    struct Not<False> { typedef True Val; };

    template<typename T, typename U>
    struct Or;

    template<>
    struct Or<False, False> { typedef False Val; };

    template<>
    struct Or<False, True> { typedef True Val; };

    template<>
    struct Or<True, False> { typedef True Val; };

    template<>
    struct Or<True, True> { typedef True Val; };

    template<typename T, typename U>
    struct And;

    template<>
    struct And<False, False> { typedef False Val; };

    template<>
    struct And<False, True> { typedef False Val; };

    template<>
    struct And<True, False> { typedef False Val; };

    template<>
    struct And<True, True> { typedef True Val; };

    template<typename T>
    struct NonNull { typedef True Val; };

    template<>
    struct NonNull<Meta::Null> { typedef False Val; };

    // Integers
    template<int N>
    struct Int { 
        enum { Val = N }; 
    };

    // Assert
    template<typename T>
    struct Assert;

    template<>
    struct Assert<True> { typedef Meta::Null Val; };

    // Select - type selection based on a Bool condition
    template <typename Cond, typename A, typename B>
    struct Select;

    template <typename A, typename B>
    struct Select<True, A, B> { typedef A Val; };

    template <typename A, typename B>
    struct Select<False, A, B> { typedef B Val; };

} // namespace Meta

#endif // ! _UTIL_META_HPP_

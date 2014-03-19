
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

// NOTE: when using the build system to supply definitions for OUTPUT_DIR_UNQUOTED
// and OPENSSL_CERTIFICATE_DIR_UNQUOTED, the paths should not contain any symbols which
// might be replaced by the preprocessor. Eg if OUTPUT_DIR_UNQUOTED is
//
// \\A\\B\\C\\_MSC_VER\\D\\E
//
// then the resulting OUTPUT_DIR will be "\\A\\B\\C\\0x1310\\D\\E", or something like that.

#ifndef _UTIL_AUTOBUILD_HPP_
#define _UTIL_AUTOBUILD_HPP_

#define UTIL_AUTOBUILD_HELPER_MACRO_QUOTE(x) UTIL_AUTOBUILD_HELPER_MACRO_QUOTE_(x)
#define UTIL_AUTOBUILD_HELPER_MACRO_QUOTE_(x) #x

#ifdef TEMP_DIR_UNQUOTED 
#define TEMP_DIR UTIL_AUTOBUILD_HELPER_MACRO_QUOTE( TEMP_DIR_UNQUOTED ) "/"
#else
#define TEMP_DIR ""
#endif

#ifdef OPENSSL_CERTIFICATE_DIR_UNQUOTED 
#define OPENSSL_CERTIFICATE_DIR UTIL_AUTOBUILD_HELPER_MACRO_QUOTE( OPENSSL_CERTIFICATE_DIR_UNQUOTED ) "/"
#else
#define OPENSSL_CERTIFICATE_DIR ""
#endif

#endif //! _UTIL_AUTOBUILD_HPP_

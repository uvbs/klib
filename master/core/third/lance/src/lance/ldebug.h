/***

Copyright 2006 bsmith@qq.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

/**
@author bsmith.zhao, bsmith@qq.com
@date 2005-10-12
@version 0.1 debug interface.
@desc base on C++ _DEBUG MACRO.
*/

#pragma once

#include <stdio.h>

/**
assert exp is true or not, if not, output information and throws (int)-1.
@module lance assert
@author bsmith
*/
#ifdef _DEBUG
#define lassert(exp) (void)( (exp) || (printf("assert failed: %s, file %s, line %d\n", #exp, __FILE__, __LINE__), throw (int)-1, 0))
#else
	#define lassert(exp) ((void)0)
#endif

/**
output debug information.
@module lance debug
@author bsmith
*/
#ifdef _DEBUG
#define ldebug printf
#else
	#define ldebug(exp) ((void)0)
#endif



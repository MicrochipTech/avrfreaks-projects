/*
   Copyright 2011 Elijah M. Merkin aka Ellioh

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

#ifndef AVRPP_AVR_GCC_RTASSERT_HPP___
#define AVRPP_AVR_GCC_RTASSERT_HPP___

#ifdef AUTOTEST
#define RUNTIME_ASSERT(expr,msg) {if(!(expr)) {__asm__ __volatile__( "jmp 0x04\n" );}}
#define EXIT_PROGRAM() {__asm__ __volatile__( "jmp 0x08\n" );}
#define RUNTIME_ASSERT(expr,msg) {if(!(expr)) {__asm__ __volatile__( "jmp 0x04\n" );}}
#else
#define RUNTIME_ASSERT(expr,msg) while(!(expr));
#define EXIT_PROGRAM() {while(1);}
#endif


#endif //AVRPP_KTEST_RTASSERT_HPP___

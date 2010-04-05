#ifndef ZO_STRING_H
#define ZO_STRING_H

#include "zoTypes.h"

inline u16 strToU16(u08* str)
{
	U16 *num;
	num = (U16*)str;
	return num->all;
}

inline s16 strToS16(u08* str)
{
	S16* num;
	num = (S16*)str;
	return num->all;
}

inline u32 strToU32(u08* str)
{
	U32 *num;
	num = (U32*)str;
	return num->all;
}

inline s32 strToS32(u08* str)
{
	S32 *num;
	num = (S32*)str;
	return num->all;
}

inline u64 strToU64(u08* str)
{
	U64 *num;
	num = (U64*)str;
	return num->all;
}

inline s64 strToS64(u08* str)
{
	S64 *num;
	num = (S64*)str;
	return num->all;
}

inline void u16ToStr(u16 data, u08* str)
{
	U16* num;
	num = (U16*)str;
	num->all = data;
}

inline void s16ToStr(s16 data, u08* str)
{
	S16* num;
	num = (S16*)str;
	num->all = data;
}

inline void u32ToStr(u32 data, u08* str)
{
	U32* num;
	num = (U32*)str;
	num->all = data;
}

inline void s32ToStr(s32 data, u08* str)
{
	S32* num;
	num = (S32*)str;
	num->all = data;
}

inline void u64ToStr(u64 data, u08* str)
{
	U64* num;
	num = (U64*)str;
	num->all = data;
}

inline void s64ToStr(s64 data, u08* str)
{
	S64* num;
	num = (S64*)str;
	num->all = data;
}

#endif //ZO_STRING_H

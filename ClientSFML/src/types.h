#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>

// This only
#include <SFML/Graphics.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#define GE_TRUE		1
#define GE_FALSE	0

#define GE_OK			0x00000000
#define GE_FATAL_ERROR	0x00000001
#define GE_MAJOR_ERROR	0x00000002
#define GE_MINOR_ERROR	0X00000003

typedef sf::Vector2f Vec2;
typedef sf::IntRect	 Rect;

typedef unsigned char			byte;
typedef unsigned short			word;
typedef unsigned int			dword;

typedef unsigned char			uint8;
typedef unsigned char			int8;
typedef unsigned short			uint16;
typedef short					int16;
typedef unsigned int			uint32;
typedef int						int32;
typedef unsigned long long		uint64;
typedef long long				int64;

#define INLINE	inline

// ------- MACROS ---------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = nullptr; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p) = nullptr; } }
#endif
#ifndef SAFE_CLOSE
#define SAFE_CLOSE(p)      { if (p) { (p)->Close();   delete (p);   (p) = nullptr; } }
#endif

#define MAX_TYPE( x )			( ( ( ( 1 << ( ( sizeof( x ) - 1 ) * 8 - 1 ) ) - 1 ) << 8 ) | 255 )
#define MIN_TYPE( x )			( - MAX_TYPE( x ) - 1 )
#define MAX_UNSIGNED_TYPE( x )	( ( ( ( 1U << ( ( sizeof( x ) - 1 ) * 8 ) ) - 1 ) << 8 ) | 255U )
#define MIN_UNSIGNED_TYPE( x )	0

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_UNIFORM_LOCATION 0xffffffff

template<typename T> void IncEnum(T& variable)
{
	using IntType = std::underlying_type<T>::type;
	variable = static_cast<T>(static_cast<IntType>(variable) + 1);
}

template<typename T> void DecEnum(T& variable)
{
	using IntType = std::underlying_type<T>::type;
	variable = static_cast<T>(static_cast<IntType>(variable) - 1);
}

template< typename _type_ >
bool IsSignedType(const _type_ t)
{
	return _type_(-1) < 0;
}

#endif
#ifndef __UTILS_H__
#define __UTILS_H__

#include <sstream>
#include "types.h"
#include "Point.h"

namespace util
{
	INLINE std::string get_file_extension(const std::string& fileIn)
	{
		size_t index = 0;

		// Determine what type it is
		for ( size_t i = fileIn.size() - 1; i >= 0; --i )
		{
			if ( fileIn[i] == '.' )
			{
				index = i;
				break;
			}
		}

		if ( index <= 0 )
		{
			return "";
		}

		std::string file_type;
		for ( size_t i = index + 1; i < fileIn.size(); ++i )
		{
			file_type += fileIn[i];
		}

		return file_type;
	}

	INLINE int32 str_hash(const std::string& str)
	{
		int32 hash = 0;

		for ( int32 i = 0; i < (int32)str.size(); ++i )
		{
			hash += (str[i] * (i + 119));
		}

		return hash;
	}

	INLINE std::string str_to_lower(const std::string& str)
	{
		std::string s_out;
		s_out.resize(str.size());

		for ( size_t i = 0; i < str.size(); ++i )
		{
			char c = str[i];

			if ( c >= 0x41 && c <= 0x5A )
				c += 32;

			s_out[i] = c;
		}

		return s_out;
	}

	INLINE std::string bool_to_str(bool b)
	{
		return b ? "true" : "false";
	}

	INLINE bool str_contains(const std::string& str, char c)
	{
		for ( size_t i = 0; i < str.size(); ++i )
		{
			if ( str[i] == c )
				return true;
		}

		return false;
	}

	INLINE int32 str_to_int(char* str)
	{
		int32 value = 0;
		while ( *str != NULL )
		{
			value = (value * 10) + *str - '0';
			++str;
		}
		return value;
	}

	INLINE float str_to_float(const std::string& str)
	{
		return (float)atof(str.c_str());
	}

	INLINE std::vector<std::string> split_str(const std::string& s, char token)
	{
		std::stringstream ss(s);
		std::vector<std::string> split_line;

		while ( ss.good() )
		{
			std::string substr;
			getline(ss, substr, token);
			split_line.push_back(substr);
		}

		return split_line;
	}

	template <typename Type> INLINE std::string to_str(const Type& t)
	{
		std::stringstream os;
		os << t;
		return os.str();
	}

	INLINE std::string vec2_to_str(const Vec2& v)
	{
		return "(" + util::to_str(v.x) + "," + util::to_str(v.y) + ")";
	}

	template <class T> INLINE void quick_removal(size_t index_to_remove, std::vector<T>& list_, bool isPointers)
	{
		if ( index_to_remove != list_.size() - 1 )
		{
			std::swap(list_[index_to_remove], list_.back());
			if ( isPointers )
				delete list_[list_.size() - 1];
			list_.pop_back();
		}
		else
		{
			if ( isPointers )
				delete list_[list_.size() - 1];
			list_.pop_back();
		}
	}
}

namespace rando
{
	int RandomRange(int32 min, int32 max);
	float RandFloat(float min, float max);
}

namespace REC
{
	INLINE sf::FloatRect Set(float left, float top, float width, float height)
	{
		return sf::FloatRect( left, top, width, height);
	}
}

namespace Maths
{
#define EPSILON  1.192092896e-07f

	INLINE float  ToDegrees(float radians)
	{
		return float(radians * (180.0f / 3.1415926535f));
	}

	INLINE float  ToRadians(float degrees)
	{
		return float(degrees * (3.1415926535f / 180.0f));
	}

	template<class T> T	Max(T x, T y)
	{
		return (x > y) ? x : y;
	}

	template<class T> T	Min(T x, T y)
	{
		return (x < y) ? x : y;
	}

	template<class T> INLINE T	Square(T x)
	{
		return x * x;
	}

	template<class T> INLINE T	Cube(T x)
	{
		return x * x * x;
	}

	template <typename Type> INLINE Type Clamp(const Type& value, const Type& min, const Type& max)
	{
		if ( value <= min )
			return min;
		else if ( value >= max )
			return max;
		else
			return value;
	}

	INLINE float LinearStep(float animStartTime, float animStopTime, float currentTime)
	{
		float anim_clip_time = Clamp(currentTime, animStartTime, animStopTime);
		return (anim_clip_time - animStartTime) / (animStopTime - animStartTime);
	}

	INLINE float Fade(float start_colour, float end_colour, float elapsed_time, float total_time)
	{
		if ( elapsed_time >= total_time )
			return end_colour;
		return start_colour + ((end_colour - start_colour) * (elapsed_time / total_time));
	}

	INLINE float LerpF(float from, float to, float time)
	{
		if ( time <= 0.0f )
			return from;
		else if ( time >= 1.0f )
			return to;
		else
			return (from + time * (to - from));
	}

	INLINE float Clamp(float time, float min, float max)
	{
		float value{ 0.0f };

		if ( time < min )
			value = min;
		else if ( time > max )
			value = max;
		else
			value = time;

		return value;
	}

	INLINE bool AreSame(float a, float b)
	{
		return fabs(a - b) < EPSILON;
	}

	INLINE Vec2 To_NDS(float screenWidth, float screenHeight, const Vec2& pos, bool flipY = true)
	{
		float x_norm = (1.0f / screenWidth) * pos.x;
		float y_norm = (1.0f / screenHeight) * pos.y;
		return Vec2((x_norm - 0.5f) / 0.5f,
			flipY ? -((y_norm - 0.5f) / 0.5f) : (y_norm - 0.5f) / 0.5f);
	}

	INLINE float Distance(const Vec2& v1, const Vec2& v2)
	{
		return sqrt(Square(v2.x - v1.x) + Square(v2.y - v1.y));
	}

	INLINE float Length(const Vec2& v)
	{
		return sqrtf(v.x*v.x + v.y*v.y);
	}

	INLINE Vec2 Snapped(const Vec2& v)
	{
		int x = static_cast<int>(v.x);
		int y = static_cast<int>(v.y);
		return Vec2(static_cast<float>(x), static_cast<float>(y));
	}

	INLINE sf::Vector2i Vec2CastInt(const Vec2& v)
	{
		return sf::Vector2i((int)v.x, (int)v.y);
	}

	INLINE Vec2 Normalize(const Vec2& v)
	{
		float l = Length(v);
		bool z = AreSame(l, 0.0f);
		return Vec2(z ? 0.0f : v.x / l, z ? 0.0f : v.y / l);
	}

	INLINE Vec2 Lerp(const Vec2& v1, const Vec2& v2, float time)
	{
		if ( time <= 0.0f )
			return v1;
		else if ( time >= 1.0f )
			return v2;
		else
			return Vec2(v1 + time * (v2 - v1));
	}

	INLINE Vec2 Zero()
	{
		return Vec2(0.0f, 0.0f);
	}

	INLINE Vec2 Set(float x, float y)
	{
		return Vec2(x, y);
	}

	INLINE bool RectCompletelyContains(const Rect& rec, const Rect& test)
	{
		if ( (test.left > rec.left) &&
			((test.left + test.width) < (rec.left + rec.width)) &&
			(test.top > rec.top) &&
			( (test.top + test.height) < (rec.top + rec.height) ) )
			return true;

		return false;
	}

	INLINE bool RectOutside(const Rect& rect, const Rect& test)
	{
		if ( rect.left > (test.left + test.width) || 
			(rect.left + rect.width) < test.left ||
			(rect.top + rect.height) < test.top || 
			rect.top > (test.top + test.height) )
			return true;
		return false;
	}

	INLINE Rect RectScale(const Rect& rect, int factor)
	{
		int w = rect.width * factor;
		int h = rect.height * factor;

		return Rect(
			rect.left-(rect.width/2),
			rect.top -(rect.height/2),
			w, h
		);

		/*
			 10,
			 **********

			 20
			 -10, -10

			 10 - (original width / 2 ) = 5
			 10 - (original height / 2 ) = 5
			 width = orginal * scalar
			 ../

			 should be 5, 25

		*/

		/*
		int w = this->Width() * scalar;
		int h = this->Height() * scalar;
		return Rect(
			this->left - (w / 2),
			this->right + (w / 2),
			this->top - (h / 2),
			this->bottom + (h / 2)
		);
		*/
	}

	INLINE Vec2 PointToVec2(const point& p)
	{
		return Vec2((float)p.x, (float)p.y);
	}
}


#endif
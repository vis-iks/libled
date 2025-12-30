#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <cstddef>
#include <typeinfo>
#include <unistd.h>
#include <sys/types.h>
#include "core/Defines.h"
#include "core/Assert.h"
#include "utils/String.h"
#include "core/RTTI.h"

// Silence tweeny warnings about integer conversion
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "tweeny/tweeny.h"
#pragma GCC diagnostic pop

// Types from Tweeny library
using tweeny::easing;
typedef tweeny::tween<float> TweenFloat;
typedef tweeny::tween<int> TweenInt;
typedef tweeny::tween<int, int> TweenXY;

// This outputs the given arguments to the specified output stream
template<typename... Args>
int ArgsToStream(std::ostream& s, Args&... args)
{
	using Expander = int[];
	return Expander{0, ((s << std::forward<Args>(args)), 0)...}[0];
}

// This builds a std::string from the given arguments by simply concatenating them
template<typename... Args>
std::string ConcatString(Args const&... args)
{
	std::stringstream msg;
	ArgsToStream(msg, args...);
	return msg.str();
}

// Implementation of streambuf for use with an existing char array
class membuf : public std::basic_streambuf<char>
{
public:
	membuf(const unsigned char *p, size_t l)
	{
		char* cp = const_cast<char*>(reinterpret_cast<const char*>(p));
		setg(cp, cp, cp + l);
	}
};

// Implementation of istream for use with an existing char array
class memstream : public std::istream
{
public:
	memstream(const unsigned char *p, size_t l) :
		std::istream(&_buffer),
		_buffer(p, l)
	{
		rdbuf(&_buffer);
	}

private:
	membuf _buffer;
};

// This performs a cast of numerical types and throws a runtime_error if the cast overflowed.
template<typename TargetT, typename SourceT>
TargetT checked_cast(SourceT v)
{
	auto r = static_cast<TargetT>(v);
	if(static_cast<SourceT>(r) != v)
		throw std::runtime_error("Cast overflowed.");
	return r;
}

// Generates a random integer number in the given range (inclusive)
int Random(int min, int max);
float Random(float min, float max);

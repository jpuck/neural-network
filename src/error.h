// This file is licensed under the CC0 license (See http://creativecommons.org/publicdomain/zero/1.0/).
// And just to make sure you get the idea, it is also licensed under the WTFPL (See http://en.wikipedia.org/wiki/WTFPL).

#ifndef error_h
#define error_h

#ifdef WINDOWS
// Throw out the min and max macros supplied by Microsoft that collide with std::min and std::max
#	define NOMINMAX
#	undef min
#	undef max
#	include <BaseTsd.h>
#endif
#include <string>


/// A simple exception object that wraps a string message
class Ex : public std::exception
{
protected:
	std::string m_message;

public:
	typedef std::string s;
	Ex(s a) { setMessage(a); }
	Ex(s a, s b) { setMessage(a + b); }
	Ex(s a, s b, s c) { setMessage(a + b + c); }
	Ex(s a, s b, s c, s d) { setMessage(a + b + c + d); }
	Ex(s a, s b, s c, s d, s e) { setMessage(a + b + c + d + e); }
	Ex(s a, s b, s c, s d, s e, s f) { setMessage(a + b + c + d + e + f); }
	Ex(s a, s b, s c, s d, s e, s f, s g) { setMessage(a + b + c + d + e + f + g); }
	Ex(s a, s b, s c, s d, s e, s f, s g, s h) { setMessage(a + b + c + d + e + f + g + h); }
	Ex(s a, s b, s c, s d, s e, s f, s g, s h, s i) { setMessage(a + b + c + d + e + f + g + h + i); }
	Ex(s a, s b, s c, s d, s e, s f, s g, s h, s i, s j) { setMessage(a + b + c + d + e + f + g + h + i + j); }

    virtual ~Ex() throw()
	{
	}

	/// Sets the message on the exception. (This method is called by all constructors of this object.)
	void setMessage(std::string message)
	{
		m_message = message; // (This is a really good place to put a breakpoint)
	}

	/// Returns the error message corresponding to this exception
	virtual const char* what() const throw();
};

#define INVALID_INDEX ((size_t)-1)

/// This method will cause an exception to be thrown when floating point
/// overflow, divide-by-zero, or NAN conditions occur.
void enableFloatingPointExceptions();


#endif // error_h

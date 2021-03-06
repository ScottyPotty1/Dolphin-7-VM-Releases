#pragma once
#include <windows.h>
#pragma warning(push,3)
#pragma warning(disable:4530)
#include <ostream>
#include <streambuf>
#pragma warning(pop)
#include "CritSect.h"

template<class charT, class traits = std::char_traits<charT> >
	class basic_tracestreambuf : public std::basic_streambuf<charT, traits>
{
public:
	// Public interface
	
	basic_tracestreambuf () : std::basic_streambuf<charT, traits>() 
	{
		setp(buffer, buffer+bufSize);
	}
	
	virtual ~basic_tracestreambuf ()
	{
		// Ensure buffer is flushed on destruct
		sync();
	}
	
protected:
	// Implementation
	
	int_type overflow(int_type ch = traits::eof())
	{
		if (output_buffer() < 0)
			return traits::eof();
		else
		{
			if (!traits::eq_int_type(traits::eof(), ch))
				return sputc(traits::to_char_type(ch));
			else
				return traits::not_eof(ch);
		}
	}
	
	int sync()
	{
		return output_buffer();
	}
	
private:
	int output_buffer()
	{
		char* pb = pbase();
		long count = pptr() - pb;
		if (count != 0)
		{
			pb[count] = 0;
			::OutputDebugString(pb);
		}
		
		// Empty the put area
		pbump(-count);
		return 0;
	}
	
private:
	// State
	enum {bufSize = 255};
	// Allow an extra char for null terminator
	char_type buffer[bufSize+1];
};

typedef basic_tracestreambuf<char> tracestreambuf;

template<class charT, class traits = std::char_traits<charT> >
	class basic_tracestream : public std::basic_ostream<charT, traits>
{
private:
	// Suppress autogenerated copy constructor and assignment operator
	basic_tracestream(const basic_tracestream&);
	basic_tracestream& operator=(const basic_tracestream&);
public:
	typedef basic_tracestreambuf<charT, traits> buf_type;

	basic_tracestream() : std::basic_ostream<charT, traits>(&m_buf), m_buf() {}

	virtual ~basic_tracestream()
	{}
	
	buf_type *rdbuf() const
	{
		return ((buf_type*)&m_buf); 
	}

	void Lock() {m_mutex.Lock(); }
	void Unlock() {m_mutex.Unlock();}
private:
	CMonitor m_mutex;

	buf_type m_buf;
};

typedef basic_tracestream<char> tracestream;
typedef CAutoLock<tracestream> tracelock;

struct _SYSTEMTIME;
std::ostream& operator<<(std::ostream& stream, const _SYSTEMTIME& st);
struct _CONTEXT;
std::ostream& operator<<(std::ostream& stream, const _CONTEXT* pCtx);

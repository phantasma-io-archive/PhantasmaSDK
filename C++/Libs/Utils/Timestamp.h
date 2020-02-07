#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#include <ctime>
#include <chrono>
//#include <iostream>
//#include <sstream>
//#include <locale>
//#include <iomanip>

namespace phantasma {

class Timespan
{
public:
	const Int32 Value;

	explicit Timespan( Int32 value = 0 )
		: Value( value )
	{
	}

	static Timespan FromSeconds( int seconds )
	{
		return Timespan{ (Int32)seconds };
	}
	static Timespan FromMinutes( int minutes )
	{
		return Timespan{ (Int32)(minutes * 60) };
	}
	static Timespan FromHours( int hours )
	{
		return Timespan{ (Int32)(hours * 60 * 60) };
	}
	static Timespan FromDays( int days )
	{
		return Timespan{ (Int32)(days * 60 * 60 * 24) };
	}
};

class Timestamp
{
public:
	UInt32 Value; // TODO - https://en.wikipedia.org/wiki/Year_2038_problem

	explicit Timestamp( UInt32 value = 0 )
		: Value( value )
	{
	}

	String ToString() const
	{
		//todo - not technically defined as the 1970 unix epoch... but probably is in practice...
		std::time_t value = (std::time_t)Value;
#ifdef _MSC_VER
		std::tm timeBuffer;
		gmtime_s(&timeBuffer, &value);
		const std::tm* dateTime = &timeBuffer;
#else
		const std::tm* dateTime = std::gmtime(&value);
#endif
		constexpr int BufferLength = 512;
		Char buffer[BufferLength];
		if(0 == StrFTime( buffer, BufferLength, PHANTASMA_LITERAL("%c"), dateTime ))
		{
			PHANTASMA_EXCEPTION("Could not format date/time");
			buffer[0] = '\0';
		}
		return String(buffer);
	}

	String ToISO8601() const
	{
		//todo - not technically defined as the 1970 unix epoch... but probably is in practice...
		std::time_t value = (std::time_t)Value;
#ifdef _MSC_VER
		std::tm timeBuffer;
		gmtime_s(&timeBuffer, &value);
		const std::tm* dateTime = &timeBuffer;
#else
		const std::tm* dateTime = std::gmtime(&value);
#endif
		constexpr int BufferLength = 512;
		Char buffer[BufferLength];
		if(0 == StrFTime( buffer, BufferLength, PHANTASMA_LITERAL("%FT%TZ"), dateTime ))
		{
			PHANTASMA_EXCEPTION("Could not format date/time");
			buffer[0] = '\0';
		}
		return String(buffer);
	}

	static Timestamp Now()
	{
		std::time_t now = std::time(0);
		std::tm time;
		gmtime_s(&time, &now);
		std::time_t value = _mkgmtime(&time);
		return Timestamp((UInt32)value);
	}

	void GetDateTimeElements(int& out_year, int& out_month, int& out_day, int& out_hour, int& out_minute, int& out_second)
	{
		std::tm time;
		const std::time_t value = Value;
		gmtime_s(&time, &value);
		out_year = time.tm_year + 1900;
		out_month = time.tm_mon + 1;
		out_day = time.tm_mday;
		out_hour = time.tm_hour;
		out_minute = time.tm_min;
		out_second = time.tm_sec;
	}

	static Timestamp FromDateTimeUTC(int year, int month, int day, int hour, int minute, int second)
	{
		if( month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 || 
			minute < 0 || minute > 59 || second < 0 || second > 60 )
		{
			PHANTASMA_EXCEPTION("Invalid date/time");
		}
		std::tm t = {};
		//std::stringstream ss;//("2011-2-18 23:12:34");
		//ss.imbue(std::locale("en_US.utf-8"));
		//ss << year << "-" << month << "-" << day << " " << hour24 << ":" << minute << ":" << second;
		//ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
		t.tm_hour = hour;
		t.tm_min = minute;
		t.tm_sec = second;
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		std::time_t time = _mkgmtime(&t);
		return Timestamp((UInt32)time);
	}

	static Timestamp FromISO8601(const Char* input, int inputLength=-1)
	{
		if(input && inputLength < 0)
		{
			inputLength = (int)PHANTASMA_STRLEN(input);
		}
		if(!input ||  inputLength < 19)
		{
			PHANTASMA_EXCEPTION("Invalid ISO8601 input string");
			return Timestamp{};
		}

		std::tm t = {};
		t.tm_year = (int)PHANTASMA_STRTOINT(&input[0]) - 1900;
		t.tm_mon  = (int)PHANTASMA_STRTOINT(&input[5]) - 1;
		t.tm_mday = (int)PHANTASMA_STRTOINT(&input[8]);
		t.tm_hour = (int)PHANTASMA_STRTOINT(&input[11]);
		t.tm_min  = (int)PHANTASMA_STRTOINT(&input[14]);
		t.tm_sec  = (int)PHANTASMA_STRTOINT(&input[17]);
		t.tm_isdst = 0;
		const int millis = inputLength > 20 ? (int)PHANTASMA_STRTOINT(&input[20]) : 0;
		std::time_t time = _mkgmtime(&t);
		return Timestamp((UInt32)time + (millis >= 500 ? 1 : 0));
	}

	int CompareTo( Timestamp other ) const
	{
		if(other.Value == Value)
		{
			return 0;
		}

		if(Value < other.Value)
		{
			return -1;
		}

		return 1;
	}

	int GetSize()
	{
		return sizeof(UInt32);
	}

	bool operator ==( Timestamp B ) const { return Value == B.Value; }

	bool operator !=( Timestamp B ) const { return !(Value == B.Value); }

	bool operator <( Timestamp B ) const { return Value < B.Value; }

	bool operator >( Timestamp B ) const { return Value > B.Value; }

	bool operator <=( Timestamp B ) const { return Value <= B.Value; }

	bool operator >=( Timestamp B ) const { return Value >= B.Value; }

	Timespan operator -( Timestamp B ) const { return Timespan{(Int32)(Value - B.Value)}; }

	Timestamp operator +( Timespan B ) const { return Timestamp{Value + B.Value}; }

private:
	static std::size_t StrFTime( char* str, std::size_t count, const char* format, const std::tm* time )
	{
		return strftime( str, count, format, time );
	}
	static std::size_t StrFTime( wchar_t* str, std::size_t count, const wchar_t* format, const std::tm* time )
	{
		return wcsftime( str, count, format, time );
	}       
};

}

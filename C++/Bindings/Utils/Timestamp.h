#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#include <ctime>

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
};

class Timestamp
{
public:
	const UInt32 Value;

	explicit Timestamp( UInt32 value = 0 )
		: Value( value )
	{
	}

	String ToString() const
	{
		//todo - not technically defined as the 1970 unix epoch... but probably is in practice...
		std::time_t value = (std::time_t)Value;
		const std::tm* dateTime = std::localtime(&value);
		constexpr int BufferLength = 512;
		Char buffer[BufferLength];
		if(0 == StrFTime( buffer, BufferLength, "%c", dateTime ))
		{
			PHANTASMA_EXCEPTION("Could not format date/time");
			buffer[0] = '\0';
		}
		return String(buffer);
	}

	static Timestamp Now()
	{
		std::time_t value = std::time(0);
		return Timestamp((UInt32)value);
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

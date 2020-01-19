#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

#include <ctype.h>
#include <math.h>
#include <utility>
#include "../Security/SecureString.h"

/*
* Implementation of BigInteger class, written for Phantasma project
* Author: Simão Pavlovich and Bernardo Pinho
* Ported from C# to C++ by Brooke Hodgman.
* I've left all declarations in the same order as the C# code for maintainability, so it jumps between private/public sections like this as a deliberate choice :)
*/
namespace phantasma {

template<bool B, class True, class False> struct SelectType                   { typedef True  Type; };
template<        class True, class False> struct SelectType<false,True,False> { typedef False Type; };

template<class T> class SecureVector;
template<bool S> class TBigInteger;

typedef TBigInteger<false> BigInteger;
typedef TBigInteger<true> SecureBigInteger;

template<bool UseSecureMemory=false>
class TBigInteger
{
private:
	int _sign = 0;
	typedef PHANTASMA_VECTOR<UInt32> Data_Fast;
	typedef SecureVector<UInt32>     Data_Secure;

	typedef typename SelectType<UseSecureMemory, Data_Secure, Data_Fast>::Type Data;
	Data _data;

	constexpr static int _Base = sizeof(UInt32) * 8;    //number of bits required for shift operations
	constexpr static UInt32 _MaxVal = 0xFFFFFFFFU;

	void Trim()
	{
		while( _data.size() > 1 && _data.back() == 0 )
			_data.pop_back();
		if( _data.empty() || (_data.size() == 1 && _data.back() == 0) )
			_sign = 0;
	}
public:
	static const TBigInteger Zero() { return TBigInteger{0LL}; }
	static const TBigInteger One()  { return TBigInteger{1LL}; }

	TBigInteger() : TBigInteger(0) {}

	TBigInteger(TBigInteger&& other)
		: _sign(other._sign)
		, _data(other._data)
	{
	}

	TBigInteger(const TBigInteger& other)
		: _sign(other._sign)
		, _data(other._data)
	{
	}

	TBigInteger& operator=(const TBigInteger& other)
	{
		_sign = other._sign;
		_data = other._data;
		return *this;
	}

	TBigInteger(const UInt32* words, int numWords, int sign = 1)
	{
		_sign = sign;
		InitFromArray(words, numWords);
	}

	TBigInteger(Data&& buffer, int sign = 1)
	{
		_sign = sign;
		InitFromArray(buffer.empty()?0:&buffer.front(), (int)buffer.size());
	}

	TBigInteger(const Data& buffer, int sign = 1)
	{
		_sign = sign;
		InitFromArray(buffer.empty()?0:&buffer.front(), (int)buffer.size());
	}

	TBigInteger(Int32 val) : TBigInteger((Int64)val)
	{
	}

	TBigInteger(UInt32 val) : TBigInteger((Int64)val)
	{
	}

	template<class Bytes>
	static TBigInteger FromUnsignedArray(const Bytes& unsignedArray, bool isPositive)
	{
		return BigInteger(unsignedArray, isPositive ? 1 : -1);
	}

	template<class Bytes>
	static TBigInteger FromSignedArray(const Bytes& signedArray)
	{
		return BigInteger(signedArray);
	}

	static TBigInteger FromSignedArray(const Byte* signedArray, int signedArrayLength)
	{//todo - do this without the extra copy
		ByteArray temp;
		if( signedArray && signedArrayLength > 0 )
		{
			temp.resize(signedArrayLength);
			memcpy(&temp.front(), signedArray, signedArrayLength);
		}
		return BigInteger(temp);
	}

    //this constructor assumes that the byte array is in Two's complement notation
	template<class Bytes>
	TBigInteger(const Bytes& bytes)
	{
		if( bytes.empty() )
		{
			_sign = 0;
			_data.push_back(0);
			return;
		}

		Byte msb = bytes[bytes.size() - 1];

		int sign = 0;

        switch (msb)
        {
            case 0xFF:
                sign = -1;
                break;

            case 0x00:
                sign = 1;
                break;

            default:
                PHANTASMA_EXCEPTION("unexpected sign byte value");
				break;
        }
		
		if (sign == -1)
		{
			*this = TBigInteger(ApplyTwosComplement(bytes), sign);
		}
		else
		{
			*this = TBigInteger(bytes, sign);
		}
	}

private:
	template<class Bytes>
	TBigInteger(const Bytes& bytes, int sign)
	{
		_sign = sign;

		if( !bytes.empty() && (bytes.size() % 4) == 0 )
		{
			InitFromArray((UInt32*)&bytes.front(), (int)bytes.size()/4);
		}
		else
		{
			Data uintArray;
			uintArray.resize((bytes.size()+3) / 4);

			int bytePosition = 0;
			for (int i = 0, j = -1, end = (int)bytes.size(); i < end; i++)
			{
				bytePosition = i % 4;
				if (bytePosition == 0)
					j++;
				uintArray[j] |= (UInt32)(bytes[i] << (bytePosition * 8));
			}

			InitFromArray(&uintArray.front(), (int)uintArray.size());
		}
	}
public:

	TBigInteger(const Byte* bytes, int numBytes, int sign = 1)
	{
		_sign = sign;

		if( (numBytes % 4) == 0 )
		{
			InitFromArray((UInt32*)bytes, numBytes/4);
		}
		else
		{
			Data uintArray;
			uintArray.resize((numBytes+3) / 4);

			int bytePosition = 0;
			for (int i = 0, j = -1, end = (int)numBytes; i < end; i++)
			{
				bytePosition = i % 4;
				if (bytePosition == 0)
					j++;
				uintArray[j] |= (UInt32)(bytes[i] << (bytePosition * 8));
			}

			InitFromArray(&uintArray.front(), (int)uintArray.size());
		}
	}

	TBigInteger(Int64 val)
	{
		if (val == 0)
		{
			_sign = 0;
			_data.push_back(0);
			return;
		}

		_sign = val < 0 ? -1 : 1;

		if (val < 0) val = -val;

		UInt32 uintBytes[2];
		memcpy(uintBytes, &val, 8);

		InitFromArray(uintBytes, 2);
	}

private:
	void InitFromArray(const UInt32* digits, int length)
	{
		int n = length;
		for (int i = n - 1; i >= 0; i--)
		{
			if (digits[i] == 0)
			{
				n--;
			}
			else
			{
				break;
			}
		}

		if (n <= 0)
		{
			_data.resize(1);
			_data[0] = 0;
			_sign = 0;
		}
		else
		{
			_data.resize(n);
			PHANTASMA_COPY(digits, digits+n, &_data.front());
		}
	}

public:
	TBigInteger(const String& str, int radix, bool* out_error=0)
		:TBigInteger(str.c_str(), (int)str.length(), radix, out_error)
	{
	}
	TBigInteger(const SecureString& str, int radix, bool* out_error=0)
		:TBigInteger(str.c_str(), (int)str.length(), radix, out_error)
	{
	}
	TBigInteger(const Char* str, int strLength, int radix, bool* out_error=0)
	{
		TBigInteger bigInteger = Zero();
		TBigInteger bi = One();

		if (strLength == 0 || str[0] == '\0' || (strLength == 1 && str[0] == '0'))
		{
			_sign = 0;
			_data.push_back(0);
			return;
		}

		const Char* first = str;
		const Char* last = first + strLength - 1;
		while( *first == '\r' || *first == '\n' )
			++first;
		while( last >= first && (*last == '\r' || *last == '\n') )
			--last;

		if( *first == '-' )
		{
			++first;
			_sign = -1;
		}
		else
		{
			_sign = 1;
		}

		int length = (int)(last+1 - first);

		for (int i = 0; i < length; i++)
		{
			int val = toupper(last[-i]);
			val = ((val >= '0' && val <= '9') ? (val - '0') : ((val < 'A' || val > 'Z') ? 9999999 : (val - 'A' + 10)));
			if( val >= radix )
			{
				if( out_error )
					*out_error = true;
				PHANTASMA_EXCEPTION("Invalid string in constructor.");
				return;
			}

			bigInteger += bi * val;

			if (i + 1 < length)
				bi *= radix;
		}

		InitFromArray(&bigInteger._data.front(), (int)bigInteger._data.size());
	}

	static TBigInteger FromHex(const String& p0)
	{
		return TBigInteger(p0, 16);
	}

	explicit operator int() const
	{
		if (_data.empty())
			return 0;

		int result = (int)(_data[0] & 0x7FFFFFFF);

		if (_sign < 0)
			result *= -1;

		return result;
	}

	explicit operator Int64() const
	{
		if (_data.empty())
			return 0;

		Int64 result = _data[0];

		if(_data.size() > 1)
			result |= (Int64)(((UInt64)_data[1]) << 32);

		if (_sign < 0)
			result *= -1;

		return result;
	}

	static TBigInteger Abs(const TBigInteger& x)
	{
		return TBigInteger(&x._data.front(), (int)x._data.size(), 1);
	}

	String ToString() const
	{
		return ToDecimal();
	}

	String ToDecimal() const
	{
		int radix = 10;
		if( radix < 2 || radix > 36 )
		{
			PHANTASMA_EXCEPTION("Radix must be >= 2 and <= 36");
			return String();
		}

		const Char* digits = PHANTASMA_LITERAL("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

		TBigInteger bi = TBigInteger(radix);
		if (_data.empty() || (_data.size() == 1 && _data[0] == 0))
		{
			return String(PHANTASMA_LITERAL("0"));
		}
		else
		{
			String text2;
			TBigInteger largeInteger = *this;
			largeInteger._sign = 1;
			TBigInteger largeInteger2;
			TBigInteger largeInteger3;
			while (largeInteger._data.size() > 1 || (largeInteger._data.size() == 1 && largeInteger._data[0] != 0))
			{
				DivideAndModulus(largeInteger, bi, largeInteger2, largeInteger3);
				if (largeInteger3._data.size() == 0)
				{
					String temp(PHANTASMA_LITERAL("0"));
					temp.append(text2);
					text2 = temp;
				}
				else
				{
					int idx = largeInteger3._data[0];
					String temp;
					temp.append({digits[idx]});
					temp.append(text2);
					text2 = temp;
				}
				largeInteger = largeInteger2;
			}
			if (_sign < 1 && 0!=text2.compare(PHANTASMA_LITERAL("0")))
			{
				String temp(PHANTASMA_LITERAL("-"));
				temp.append(text2);
				text2 = temp;
			}

			return text2;
		}
	}

	String ToHex() const
	{
		StringBuilder builder;

		for(UInt32 digit : _data)
		{
			Char buffer[10];
			sprintf_s(buffer, "%08x", digit);
			builder << buffer;
		}

		String result = String(builder.str());

		return result;
	}

private: 
	static Data Add(const Data& X, const Data& Y)
	{
		int sizeX = (int)X.size();
		int sizeY = (int)Y.size();
		int longest = PHANTASMA_MAX(sizeX, sizeY);
		Data r;
		if( longest == 0 )
			return r;
		r.resize(longest+1);

		UInt32 overflow = 0;
		for (int i = 0; i < longest; i++)
		{
			UInt32 x = i < sizeX ? X[i] : 0;
			UInt32 y = i < sizeY ? Y[i] : 0;
			UInt64 sum = (UInt64)overflow + x + y;

			r[i] = (UInt32)sum;
			overflow = (UInt32)(sum >> _Base);
		}

		r[longest] = (Byte)overflow;
		return r;
	}

	static Data Subtract(const Data& X, const Data& Y)
	{
		int sizeX = (int)X.size();
		int sizeY = (int)Y.size();
		int longest = PHANTASMA_MAX(sizeX, sizeY);
		Data r;
		if( longest == 0 )
			return r;
		r.resize(longest);

		Int64 carry = 0;

		for (int i = 0; i < longest; i++)
		{
			Int64 x = i < sizeX ? X[i] : 0;
			Int64 y = i < sizeY ? Y[i] : 0;
			Int64 tmpSub = x - y - carry;
			r[i] = (UInt32)(tmpSub & _MaxVal);
			carry = ((tmpSub >= 0) ? 0 : 1);
		}

		return r;
	}

	static Data Multiply(const Data& X, const Data& Y)
	{
		int sizeX = (int)X.size();
		int sizeY = (int)Y.size();
		Data output;
		if( sizeX == 0 || sizeY == 0 )
			return output;
		output.resize(sizeX + sizeY + 1);

		for (int i = 0; i < sizeX; i++)
		{
			if (X[i] == 0)
				continue;

			UInt64 carry = 0uL;
			Int32 k = i;

			for (int j = 0; j < sizeY; j++, k++)
			{
				UInt64 tmp = (UInt64)(X[i] * (Int64)Y[j] + output[k] + (Int64)carry);
				output[k] = (UInt32)(tmp);
				carry = tmp >> 32;
			}

			output[i + sizeY] = (UInt32)carry;
		}

		return output;
	}

public: 
	TBigInteger operator+(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		TBigInteger result;

		//all these if-else's are to make sure we don't attempt operations that would give a negative result,
		//allowing the large int operations to deal only in the scope of unsigned numbers
		if (a._sign < 0 && b._sign < 0)
		{
			result._data = Add(a._data, b._data);
			result._sign = (int)result == 0 ? 0 : -1;
		}
		else if (a._sign < 0)
		{
			if (Abs(a) < b)
			{
				result = TBigInteger(Subtract(b._data, a._data));
				result._sign = result == 0 ? 0 : 1;
			}
			else
			{
				result = TBigInteger(Subtract(a._data, b._data));
				result._sign = result == 0 ? 0 : -1;
			}
		}
		else if (b._sign < 0)
		{
			if (a < Abs(b))
			{
				result = TBigInteger(Subtract(b._data, a._data));
				result._sign = result == 0 ? 0 : -1;
			}
			else
			{
				result = TBigInteger(Subtract(a._data, b._data));
				result._sign = result == 0 ? 0 : 1;
			}
		}
		else
		{
			result = TBigInteger(Add(b._data, a._data));
			result._sign = result == 0 ? 0 : 1;
		}

		result.Trim();

		return result;
	}
	TBigInteger& operator +=(const TBigInteger& b)
	{
		return (*this = *this + b);
	}

	TBigInteger operator -(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		TBigInteger result;

		//all these if-else's are to make sure we don't attempt operations that would give a negative result,
		//allowing the large int operations to deal only in the scope of unsigned numbers
		if (a._sign < 0 && b._sign < 0)
		{
			if (Abs(a) < Abs(b))
			{
				result = TBigInteger(Subtract(b._data, a._data));
				result._sign = result == 0 ? 0 : 1;
			}
			else
			{
				result = TBigInteger(Subtract(a._data, b._data));
				result._sign = result == 0 ? 0 : -1;
			}
		}
		else
			if (a._sign < 0)
			{
				result = TBigInteger(Add(a._data, b._data));
				result._sign = result == 0 ? 0 : -1;
			}
			else if (b._sign < 0)
			{
				result = TBigInteger(Add(a._data, b._data));
				result._sign = result == 0 ? 0 : 1;
			}
			else
			{
				if (a < b)
				{
					result = TBigInteger(Subtract(b._data, a._data));
					result._sign = result == 0 ? 0 : -1;
				}
				else
				{
					result = TBigInteger(Subtract(a._data, b._data));
					result._sign = result == 0 ? 0 : 1;
				}
			}

		return result;
	}
	TBigInteger& operator -=(const TBigInteger& b)
	{
		return (*this = *this - b);
	}

	TBigInteger operator *(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		TBigInteger result;
		result._data = TBigInteger::Multiply(a._data, b._data);
		result._sign = a._sign * b._sign;
		result.Trim();
		return result;
	}
	TBigInteger& operator *=(const TBigInteger& b)
	{
		return (*this = *this * b);
	}

	TBigInteger operator /(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		TBigInteger quot, rem;
		TBigInteger::DivideAndModulus(Abs(a), Abs(b), quot, rem);
		quot._sign = quot._sign == 0 ? 0 : a._sign * b._sign;
		return quot;
	}
	TBigInteger& operator /=(const TBigInteger& b)
	{
		return (*this = *this / b);
	}

	TBigInteger operator %(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		TBigInteger quot, rem;
		TBigInteger::DivideAndModulus(a, b, quot, rem);
		return rem;
	}
	TBigInteger& operator %=(const TBigInteger& b)
	{
		return (*this = *this % b);
	}

	static void DivideAndModulus(const TBigInteger& a, const TBigInteger& b, TBigInteger& quot, TBigInteger& rem)
	{
		if ((int)b == 0)
		{
			quot = Zero();
			rem = Zero();
			return;
		}

		if (a._data.size() < b._data.size())
		{
			quot = Zero();
			rem = TBigInteger(a);
			return;
		}

		if (b._data.size() == 1)
			SingleDigitDivMod(a, b, quot, rem);
		else
			MultiDigitDivMod(a, b, quot, rem);


		rem._sign = a._sign;
		rem = (int)a >= 0 ? rem : b + rem;

		quot._sign = quot.GetBitLength() == 0 ? 0 : a._sign * b._sign;
		rem._sign = rem.GetBitLength() == 0 ? 0 : rem._sign;
	}

private:
	//do not access this function directly under any circumstances, always go through DivideAndModulus
	static void SingleDigitDivMod(const TBigInteger& numerator, const TBigInteger& denominator, TBigInteger& quotient, TBigInteger& remainder)
	{
		Data tmpQuotArray, remArray;
		tmpQuotArray.resize(numerator._data.size() - denominator._data.size() + 1);
		remArray.resize(numerator._data.size());

		for (int i = 0, end = (int)numerator._data.size(); i < end; i++)
		{
			remArray[i] = numerator._data[i];
		}

		int quotIter = 0;   //quotient array iterator index
		UInt64 quickDen = denominator._data[0];  //quick denominator
		int remIter = (int)remArray.size() - 1;  //remainder array iterator index
		UInt64 tmpRem = remArray[remIter];   //temporary remainder digit

		if (tmpRem >= quickDen)
		{
			UInt64 tmpQuot = tmpRem / quickDen;
			tmpQuotArray[quotIter++] = (UInt32)tmpQuot;
			remArray[remIter] = (UInt32)(tmpRem % quickDen);
		}

		remIter--;
		while (remIter >= 0)
		{
			tmpRem = ((UInt64)remArray[remIter + 1] << 32) + remArray[remIter];
			UInt64 tmpQuot = tmpRem / quickDen;
			tmpQuotArray[quotIter++] = (UInt32)tmpQuot;
			remArray[remIter + 1] = 0u;
			remArray[remIter--] = (UInt32)(tmpRem % quickDen);
		}

		Data quotArray;
		quotArray.resize(quotIter);
		for (int i = (int)quotArray.size() - 1, j = 0; i >= 0; i--, j++)
		{
			quotArray[j] = tmpQuotArray[i];
		}

		quotient._data = quotArray;
		quotient._sign = 1;
		remainder._data = remArray;
		remainder._sign = 1;

		quotient.Trim();
		remainder.Trim();
	}

	//do not access this function directly under any circumstances, always go through DivideAndModulus
	static void MultiDigitDivMod(const TBigInteger& numerator, TBigInteger denominator, TBigInteger& quot, TBigInteger& rem)
	{
		Data quotArray, remArray;
		quotArray.resize(numerator._data.size() - denominator._data.size() + 1);
		remArray.resize(numerator._data.size() + 1);

		UInt32 tmp = 0x80000000u;
		UInt32 tmp2 = denominator._data[denominator._data.size() - 1];    //denominator most significant digit
		int shiftCount = 0;

		while (tmp != 0 && (tmp2 & tmp) == 0)
		{
			shiftCount++;
			tmp >>= 1;
		}
		for (int i = 0, end = (int)numerator._data.size(); i < end; i++)
		{
			remArray[i] = numerator._data[i];
		}

		ShiftLeft(remArray, shiftCount);
		denominator <<= shiftCount;

		int j = (int)numerator._data.size() - (int)denominator._data.size() + 1;
		int remIter = (int)numerator._data.size(); //yes, numerator, not remArray
		UInt64 denMsd = denominator._data[(int)denominator._data.size() - 1];       //denominator most significant digit
		UInt64 denSubMsd = denominator._data[(int)denominator._data.size() - 2];    //denominator second most significant digit
		int denSize = (int)denominator._data.size() + 1;

		Data tmpRemSubArray;
		tmpRemSubArray.resize(denSize);

		while (j > 0)
		{
			UInt64 quickDenominator = ((UInt64)remArray[remIter] << 32) + remArray[remIter - 1];
			UInt64 tmpQuot = quickDenominator / denMsd;
			UInt64 tmpRem = quickDenominator % denMsd;
			bool flag = false;
			while (!flag)
			{
				flag = true;
				if (tmpQuot == 0x100000000LL || tmpQuot * denSubMsd > (tmpRem << 32) + remArray[remIter - 2])
				{
					tmpQuot--;
					tmpRem += denMsd;
					if (tmpRem < 0x100000000LL)
					{
						flag = false;
					}
				}
			}

			for (int k = 0; k < denSize; k++)
			{
				tmpRemSubArray[(tmpRemSubArray.size() - 1) - k] = remArray[remIter - k];
			}

			TBigInteger tmpRemBigInt(tmpRemSubArray);
			TBigInteger estimNumBigInt = denominator * (Int64)tmpQuot;  //current numerator estimate
			while (estimNumBigInt > tmpRemBigInt)
			{
				tmpQuot--;
				estimNumBigInt -= denominator;
			}
			TBigInteger estimRemBigInt = tmpRemBigInt - estimNumBigInt;    //current remainder estimate
			for (int k = 0; k < denSize; k++)
			{
				tmp = denominator._data.size() - k < estimRemBigInt._data.size()
					? estimRemBigInt._data[denominator._data.size() - k]
					: 0;
				remArray[remIter - k] = tmp;
			}

			remIter--;
			j--;
			quotArray[j] = (UInt32)tmpQuot;
		}

		quot = TBigInteger(quotArray);

		ShiftRight(remArray, shiftCount);

		rem = TBigInteger(remArray);

		quot.Trim();
		rem.Trim();
	}

public:
	static TBigInteger DivideAndRoundToClosest(const TBigInteger& numerator, const TBigInteger& denominator)
	{
		//from https://stackoverflow.com/a/2422723
		return (numerator + (denominator / 2)) / denominator;
	}

	TBigInteger operator >>(int bits) const
	{
		if (_data.empty())
			return *this;
		bits = bits < 0 ? -bits : bits;
		TBigInteger r = *this;
		ShiftRight(r._data, bits);
		if (r._data[0] == 0 && r._data.size() == 1)
			r._sign = 0;
		r.Trim();
		return r;
	}
	TBigInteger& operator >>=(int bits)
	{
		if (_data.empty())
			return *this;
		bits = bits < 0 ? -bits : bits;
		ShiftRight(_data, bits);
		if (_data[0] == 0 && _data.size() == 1)
			_sign = 0;
		Trim();
		return *this;
	}
private:
	static void ShiftRight(Data& buffer, int shiftBitCount)
	{
		int length = (int)buffer.size();
		if( length == 0 )
			return;

		int shrinkage = shiftBitCount / 32;  //amount of digits we need to cut from the buffer

		int quickShiftAmount = shiftBitCount % 32;


		UInt32 msd = buffer[length - 1] >> quickShiftAmount;  //shifts the most significant digit
		int extraShrinkage = (msd == 0) ? 1 : 0;    //if that shift goes to 0, it means we need to cut
													//an extra position of the array to account for an MSD == 0

		int newLength = length - shrinkage - extraShrinkage;

		if (newLength <= 0)
		{
			buffer.resize(0);
			return;
		}

		Data newBuffer;
		newBuffer.resize(newLength);

		quickShiftAmount = 32 - quickShiftAmount;   //we'll use this new shift amount to pre-left shift the applicable digits
													//so we have easy access to the potential underflow of any given digit's right shift operation

		if (extraShrinkage == 1)
			newBuffer[newLength - 1] = buffer[length - 1] << quickShiftAmount;

		for (int i = length - (1 + extraShrinkage), j = newLength - 1; j >= 1; i--, j--)
		{
			UInt64 upshiftedVal = (UInt64)buffer[i] << quickShiftAmount;

			UInt32 shiftMsd = (UInt32)(upshiftedVal >> 32);
			UInt32 shiftLsd = (UInt32)upshiftedVal;

			newBuffer[j] |= shiftMsd;
			newBuffer[j - 1] |= shiftLsd;
		}

		newBuffer[0] |= buffer[shrinkage] >> shiftBitCount;

		PHANTASMA_SWAP(buffer, newBuffer);
	}
public:
	TBigInteger operator <<(int bits) const
	{
		if (_data.empty())
			return *this;
		bits = bits < 0 ? -bits : bits;
		TBigInteger r = *this;
		ShiftLeft(r._data, bits);
		return r;
	}
	TBigInteger& operator <<=(int bits)
	{
		if (_data.empty())
			return *this;
		bits = bits < 0 ? -bits : bits;
		ShiftLeft(_data, bits);
		return *this;
	}

private:
	static void ShiftLeft(Data& buffer, int shiftBitCount)
	{
		auto length = buffer.size();
		if( length == 0 )
			return;

		int amountOfZeros = shiftBitCount / 32;  //amount of least significant digit zero padding we need
		int quickShiftAmount = shiftBitCount % 32;

		Int64 msd = ((Int64)buffer[length - 1]) << quickShiftAmount;  //shifts the most significant digit

		int extraDigit = (msd != (UInt32)msd) ? 1 : 0;  //if it goes above the UInt32 range, we need to add
														//a new position for the new MSD

		Data newBuffer;
		newBuffer.resize(length + amountOfZeros + extraDigit);

		for (UInt32 i = 0, j = amountOfZeros; i < length; i++, j++)
		{
			UInt64 shiftedVal = ((UInt64)buffer[i]) << quickShiftAmount;

			UInt32 shiftLsd = (UInt32)shiftedVal;
			UInt32 shiftMsd = (UInt32)(shiftedVal >> 32);

			newBuffer[j] |= shiftLsd;

			if (shiftMsd > 0)
				newBuffer[j + 1] |= shiftMsd;
		}

		PHANTASMA_SWAP(buffer, newBuffer);
	}

public:
	TBigInteger& operator ++()
	{
		return (*this = *this + 1);
	}
	TBigInteger operator ++(int)
	{
		TBigInteger pre = *this;
		*this = *this + 1;
		return pre;
	}

	TBigInteger& operator --()
	{
		return (*this = *this - 1);
	}
	TBigInteger operator --(int)
	{
		TBigInteger pre = *this;
		*this = *this - 1;
		return pre;
	}

	TBigInteger operator -() const
	{
		TBigInteger n = *this;
		n._sign = -n._sign;
		return n;
	}

	bool IsZero() const
	{
		return _sign == 0;
	}

	bool operator ==(const TBigInteger& b) const
	{
		return _data.size() == b._data.size() && _sign == b._sign && PHANTASMA_EQUAL(_data.begin(), _data.end(), b._data.begin());
	}

	bool operator !=(const TBigInteger& b) const
	{
		return _data.size() != b._data.size() || _sign != b._sign || !PHANTASMA_EQUAL(_data.begin(), _data.end(), b._data.begin());
	}

private:
	static bool LogicalCompare(const TBigInteger& a, const TBigInteger& b, bool op)
	{
		if (a._sign < b._sign)
		{
			return op;
		}

		if (a._sign > b._sign)
		{
			return !op;
		}

		if (a._data.size() < b._data.size())
		{
			return op;
		}

		if (a._data.size() > b._data.size())
		{
			return !op;
		}

		const Data& A = a._data;
		const Data& B = b._data;
		for (int i = (int)A.size() - 1; i >= 0; i--)
		{
			UInt32 x = A[i];
			UInt32 y = B[i];
			if (x < y)
			{
				return op;
			}

			if (x > y)
			{
				return !op;
			}
		}

		return false;
	}

public:
	bool operator <(const TBigInteger& b) const
	{
		return LogicalCompare(*this, b, true);
	}

	bool operator >(const TBigInteger& b) const
	{
		return LogicalCompare(*this, b, false);
	}

	bool operator <=(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		return (a == b || a < b);
	}

	bool operator >=(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		return (a == b || a > b);
	}

	TBigInteger operator ^(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		auto aSize = a._data.size();
		auto bSize = b._data.size();
		auto len = PHANTASMA_MAX(aSize, bSize);
		Data temp;
		temp.resize(len);

		for (UInt32 i = 0; i < len; i++)
		{
			UInt32 A = i < aSize ? a._data[i] : 0;
			UInt32 B = i < bSize ? b._data[i] : 0;
			temp[i] = (A ^ B);
		}

		return TBigInteger(std::move(temp));
	}

	TBigInteger operator |(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		auto aSize = a._data.size();
		auto bSize = b._data.size();
		auto len = PHANTASMA_MAX(aSize, bSize);
		Data temp;
		temp.resize(len);

		for (UInt32 i = 0; i < len; i++)
		{
			UInt32 A = i < aSize ? a._data[i] : 0;
			UInt32 B = i < bSize ? b._data[i] : 0;
			temp[i] = A | B;
		}

		return TBigInteger(std::move(temp));
	}

	TBigInteger operator ~() const
	{
		Data buffer;
		buffer.resize(_data.size());
		for (int i = 0, end = (int)buffer.size(); i < end; i++)
		{
			buffer[i] = ~_data[i];
		}

		return TBigInteger(std::move(buffer));
	}

	TBigInteger operator &(const TBigInteger& b) const
	{
		const TBigInteger& a = *this;
		auto aSize = a._data.size();
		auto bSize = b._data.size();
		auto len = PHANTASMA_MAX(aSize, bSize);
		Data temp;
		temp.resize(len);

		for (UInt32 i = 0; i < len; i++)
		{
			UInt32 A = i < aSize ? a._data[i] : 0;
			UInt32 B = i < bSize ? b._data[i] : 0;
			temp[i] = A & B;
		}

		return TBigInteger(std::move(temp));
	}

	bool Equals(TBigInteger other) const
	{
		//BH!!!
		//this doesn't compare _signs!?!?!?!
		if (other._data.size() != _data.size())
		{
			return false;
		}

		return PHANTASMA_EQUAL(_data.begin(), _data.end(), other._data.begin());
	}

	int CompareTo(const TBigInteger& other) const
	{
		if (Equals(other))
		{
			return 0;
		}

		if (*this < other)
		{
			return -1;
		}

		return 1;
	}

	static TBigInteger Pow(TBigInteger powBase, TBigInteger powExp)
	{
		TBigInteger val = One();
		TBigInteger i = Zero();

		while (i < powExp)
		{
			val *= powBase;
			i = i + One();
		}
		return val;
	}

	static TBigInteger ModPow(TBigInteger b, TBigInteger exp, TBigInteger mod)
	{
		return b.ModPow(exp, mod);
	}

	/// <summary>
	/// Modulo Exponentiation
	/// Ported from http://developer.classpath.org/doc/java/math/TBigInteger-source.html
	/// </summary>
	/// <param name="exp">Exponential</param>
	/// <param name="mod">Modulo</param>
	/// <returns>TBigInteger result of raising this to the power of exp and then modulo n </returns>
	TBigInteger ModPow(TBigInteger exp, TBigInteger mod) const
	{
		if( mod._sign == -1 || mod == 0 )
		{
			PHANTASMA_EXCEPTION("Non-positive modulo");
			return Zero();
		}

		if (exp._sign < 0)
			return ModInverse(mod).ModPow(-exp, mod);

		if (exp == 1)
			return *this % mod;

		TBigInteger s = One();
		TBigInteger t = *this;

		while (exp != Zero())
		{
			if ((exp & One()) == One())
				s = (s * t) % mod;

			exp = exp >> 1;
			t = (t * t) % mod;
		}

		return s;
	}

	TBigInteger ModInverse(TBigInteger modulus) const
	{
		TBigInteger array[2] =
		{
			Zero(),
			One()
		};
		TBigInteger array2[2] = {};
		TBigInteger array3[2] = 
		{
			Zero(),
			Zero()
		};
		int num = 0;
		TBigInteger bi = modulus;
		TBigInteger bigInteger = *this;
		while (bigInteger._data.size() > 1 || (bigInteger._data.size() == 1 && bigInteger._data[0] != 0))
		{
			TBigInteger bigInteger2;
			TBigInteger bigInteger3;
			if (num > 1)
			{
				TBigInteger bigInteger4 = (array[0] - array[1] * array2[0]) % modulus;
				array[0] = array[1];
				array[1] = bigInteger4;
			}

			DivideAndModulus(bi, bigInteger, bigInteger2, bigInteger3);

			array2[0] = array2[1];
			array3[0] = array3[1];
			array2[1] = bigInteger2;
			array3[1] = bigInteger3;
			bi = bigInteger;
			bigInteger = bigInteger3;
			num++;
		}

		if( array3[0]._data.size() > 1 || (array3[0]._data.size() == 1 && array3[0]._data[0] != 1) )
		{
			PHANTASMA_EXCEPTION("No inverse!");
			return Zero();
		}

		TBigInteger bigInteger5 = (array[0] - array[1] * array2[0]) % modulus;
		if (bigInteger5._sign < 0)
		{
			bigInteger5 += modulus;
		}
		return bigInteger5;
	}

	bool TestBit(int index) const
	{
		return (*this & (One() << index)) > Zero();
	}

	int GetLowestSetBit() const
	{
		if (_sign == 0)
			return -1;

		ByteArray b = ToSignedByteArray();
		int w = 0;
		while (b[w] == 0)
			w++;
		for (int x = 0; x < 8; x++)
			if ((b[w] & 1 << x) > 0)
				return x + w * 8;
		PHANTASMA_EXCEPTION("GetLowestSetBit error");
		return -1;
	}

	template<class String>
	static TBigInteger Parse(const String& input, int radix = 10)
	{
		return TBigInteger(input, radix);
	}

	static bool TryParse(const String& input, TBigInteger& output)
	{
		PHANTASMA_TRY
		{
			bool error = false;
			output = TBigInteger(input, 10, &error);
			return error;
		}
		PHANTASMA_CATCH_ALL()
		{
			output = Zero();
			return false;
		}
	}

	int GetBitLength() const
	{
		if (_data.empty() || (_data.size() == 1 && _data[0] == 0))
			return 0;

		auto result = (_data.size() - 1) * 32;

		result += (int) log2(_data[(int)_data.size() - 1]) + 1;

		return (int)result;
	}

	Data ToUintArray() const
	{
		return _data;
	}

	bool CalcIsEven() const
	{
		TBigInteger tmp = *this % 2;
		return tmp == 0;
	}

	TBigInteger Sqrt() const
	{
		if( *this < 0 )
		{
			PHANTASMA_EXCEPTION("cannot be negative");
			return Zero();
		}

		if (*this == 0)
		{
			return Zero();
		}

		UInt32 bitLength = (UInt32)GetBitLength();

		bitLength = (((bitLength & 1) == 0) ? (bitLength >> 1) : ((bitLength >> 1) + 1));
		UInt32 num2 = bitLength >> 5;
		Byte b = (Byte)(bitLength & 0x1F);

		UInt32 num3;

		if (b == 0)
		{
			num3 = 0x80000000u;
		}
		else
		{
			num3 = (UInt32)(1 << (int)b);
			num2++;
		}

		Data sqrtArray;
		sqrtArray.resize(num2);
		for (int num4 = (int)(num2 - 1); num4 >= 0; num4--)
		{
			while (num3 != 0)
			{
				sqrtArray[num4] ^= num3;
				TBigInteger tmp(sqrtArray);
				if (tmp * tmp > *this)
				{
					sqrtArray[num4] ^= num3;
				}
				num3 >>= 1;
			}
			num3 = 0x80000000u;
		}
		return TBigInteger(std::move(sqrtArray));
	}

    /// <summary>
    /// IF YOU USE THIS METHOD, DON'T FEED THE RESULTING BYTE ARRAY TO THE BigInteger(byte[] array) CONSTRUCTOR
    /// That constructor depends on having a byte array using the Two's Complement convention, where the MSB is either 0 or FF
    /// This method does not produce an extra byte for the sign, that only happens on the ToSignedByteArray method.
    ///
    /// tl;dr:  if the byte array will be used to reconstruct a bigint, use ToSignedByteArray
    ///         if you just need to manipulate the raw byte array without having to reconstruct a bigint, AND you don't care about sign, use ToUnsignedByteArray.
    /// </summary>
    /// <returns></returns>
	int ToUnsignedByteArray(Byte* result, int resultSize) const
	{
		int bitLength = GetBitLength();
		UInt32 byteArraySize = (bitLength / 8) + (UInt32)((bitLength % 8 > 0) ? 1 : 0);
		if(!result)
			return (int)byteArraySize;
		if(resultSize < 0 || (int)byteArraySize > resultSize )
		{
			PHANTASMA_EXCEPTION("invalid argument");
			return 0;
		}

		Byte bytes[4];
		if( UseSecureMemory )
		{
			PHANTASMA_LOCKMEM(bytes, 4);
		}
		for (UInt32 i = 0, j = 0, end = (UInt32)_data.size(); i < end; i++, j += 4)
		{
			memcpy(bytes, &_data[i], 4);
			for (int k = 0; k < 4; k++)
			{
				if (j + k >= byteArraySize)
					break;
				result[j + k] = bytes[k];
			}
		}
		if( UseSecureMemory )
		{
			PHANTASMA_UNLOCKMEM(bytes, 4);
		}

		return (int)byteArraySize;
	}
	
    /// <summary>
    /// IF YOU USE THIS METHOD, DON'T FEED THE RESULTING BYTE ARRAY TO THE BigInteger(byte[] array) CONSTRUCTOR
    /// That constructor depends on having a byte array using the Two's Complement convention, where the MSB is either 0 or FF
    /// This method does not produce an extra byte for the sign, that only happens on the ToSignedByteArray method.
    ///
    /// tl;dr:  if the byte array will be used to reconstruct a bigint, use ToSignedByteArray
    ///         if you just need to manipulate the raw byte array without having to reconstruct a bigint, AND you don't care about sign, use ToUnsignedByteArray.
    /// </summary>
    /// <returns></returns>
	ByteArray ToUnsignedByteArray() const
	{
		int bitLength = GetBitLength();
		UInt32 byteArraySize = (bitLength / 8) + (UInt32)((bitLength % 8 > 0) ? 1 : 0);
		ByteArray result;
		result.resize(byteArraySize);

		for (UInt32 i = 0, j = 0, end = (UInt32)_data.size(); i < end; i++, j += 4)
		{
			Byte bytes[4];
			memcpy(bytes, &_data[i], 4);
			for (int k = 0; k < 4; k++)
			{
				if (bytes[k] == 0)
					continue;

				result[j + k] = bytes[k];
			}
		}

		return result;
	}
	

    //The returned byte array is signed by applying the Two's complement technique for negative numbers
	ByteArray ToSignedByteArray() const
	{
		int bitLength = GetBitLength();
		UInt32 byteArraySize = (bitLength / 8) + (UInt32)((bitLength % 8 > 0) ? 1 : 0) + 1; //the extra byte is for sign carrying purposes
		ByteArray result;
		result.resize(byteArraySize);

		bool applyTwosComplement = _sign == -1;    //only apply two's complement if this number is negative

		for (UInt32 i = 0, j = 0, end = (UInt32)_data.size(); i < end; i++, j += 4)
		{
			Byte bytes[4];
			memcpy(bytes, &_data[i], 4);
			for (int k = 0; k < 4; k++)
			{
				if (!applyTwosComplement && bytes[k] == 0)
					continue;
				if (j + k >= byteArraySize)
					break;

				if (applyTwosComplement)
					result[j + k] = (Byte) ~bytes[k];
				else
					result[j + k] = bytes[k];
			}
		}

		//this could be optimized if needed, but likely not worth it for now
		if (applyTwosComplement)
		{
			TBigInteger tmp = TBigInteger(result, 1) + 1; //create a biginteger with the inverted bits but with positive sign, and add 1.

			result = tmp.ToSignedByteArray();     //when we call the ToByteArray, we will get an extra byte on the array to keep sign information while in byte[] format
												//but the twos complement logic won't get applied again given the bigint has positive sign.

			result[result.size() - 1] = 0xFF;      //sets the MSB to 1's, as this array represents a negative number.
		}

		return result;
	}

	template<class Bytes>
	static ByteArray ApplyTwosComplement(const Bytes& bytes)
	{
		ByteArray buffer;
		buffer.resize(bytes.size());

		for (int i = 0, end = (int)bytes.size(); i < end; i++)
		{
			buffer[i] = (Byte)~bytes[i];
		}

		TBigInteger tmp = TBigInteger(buffer, 1) + 1; //create a biginteger with the inverted bits but with positive sign, and add 1. result will remain with positive sign

		buffer = tmp.ToSignedByteArray(); //when we call the ToByteArray asking to include sign, we will get an extra Byte on the array to make sure sign is correct 
										//but the twos complement logic won't get applied again given the bigint has positive sign.

		return buffer;
	}

	//TODO: this probably needs looking into..
	int GetHashCode() const
	{
		Int64 hashCode = -1521134295 * _sign;

		// Rotate by 3 bits and XOR the new value
		for (UInt32 word : _data)
		{
			hashCode = (int)((hashCode << 3) | (hashCode >> (29)) ^ word);
		}

		return (int)hashCode;
	}

	TBigInteger Mod(const TBigInteger& b) const
	{
		return *this % b;
	}

	TBigInteger FlipBit(int bit) const
	{
		return *this ^ (One() << bit);
	}
};

template<bool S>
String DecimalConversion( const TBigInteger<S>& value, UInt32 decimals, Char decimalPoint='.', bool alwaysShowDecimalPoint=false )
{
	//todo - is it better to just convert value to text, and then insert the decimal point in the right place? :D
	if( decimals > 0 || alwaysShowDecimalPoint )
	{
		TBigInteger<S> q, r;
		if( decimals > 0 )
		{
			TBigInteger<S> radix = TBigInteger<S>::Pow(10, decimals);
			TBigInteger<S>::DivideAndModulus( value, radix, q, r );
		}
		else
		{
			q = value;
			r = TBigInteger<S>::Zero();
		}
		if( alwaysShowDecimalPoint || r != TBigInteger<S>::Zero() )
		{
			String rstr = r.ToString();
			UInt32 trailingZeros = 0;
			for( UInt32 i=(UInt32)rstr.length(); i-->0; ++trailingZeros )
				if( rstr[i] != '0' )
					break;
			String result = q.ToString();
			result.append({decimalPoint});
			for( UInt32 i=(UInt32)rstr.length(); i<decimals; ++i)
				result.append(PHANTASMA_LITERAL("0"));
			for( UInt32 i=0, end=(UInt32)rstr.length()-trailingZeros; i<end; ++i)
			{
				Char ch[2] = { rstr[i], '\0' };
				result.append(ch);
			}
			return result;
		}
		else
			return q.ToString();
	}
	else
		return value.ToString();
}

template<bool S, class CharArray, class String>
TBigInteger<S> _DecimalConversion( const String& value, UInt32 decimals, Char decimalPoint='.', Char toleratedSeparator='\0' )
{
	int decimalIdx = -1;
	UInt32 fractionalDecimals = 0;
	for( int i=0, end=(int)value.length(); i!=end; ++i )
	{
		Char c = value[i];
		if(c == decimalPoint)
		{
			if( decimalIdx < 0 )
				decimalIdx = i;
			else
			{
				PHANTASMA_EXCEPTION("Multiple decimal points present in string");
				return {};
			}
		}
		else if(c >= '0' && c <= '9')
		{
			if( decimalIdx >= 0 )
				++fractionalDecimals;
		}
		else if( c != toleratedSeparator && c != '\r' && c != '\n' && c != '-' )
		{
			PHANTASMA_EXCEPTION("Non-numeric characters in string");
			return {};
		}
	}
	if(decimalIdx < 0)//easy case, no decimal point in the input!
	{
		TBigInteger<S> parsed = TBigInteger<S>::Parse(value);
		if( decimals == 0 )//easy case, no fractional part allowed!
			return parsed;
		TBigInteger<S> multiplier = TBigInteger<S>::Pow(10, decimals);
		return parsed * multiplier;
	}
	else
	{
		//shift the fractional part over the decimal point
		CharArray copy;
		copy.resize(value.length());
		for( int i=0; i<decimalIdx; ++i )
		{
			copy[i] = value[i];
		}
		for( int i=decimalIdx, end=(int)value.length()-1; i<end; ++i )
		{
			copy[i] = value[i+1];
		}

		int newLength;
		if(fractionalDecimals > decimals)//user has provided a value that is too precise. Truncate their input.
		{
			int excess = fractionalDecimals - decimals;
			fractionalDecimals = decimals;
			newLength = (int)value.length()-(excess+1);
		}
		else // we shifted everything down one place, so insert a new null terminator in the last place
		{
			newLength = (int)value.length()-1;
		}
		copy[newLength] = '\0';

		TBigInteger<S> parsed = TBigInteger<S>::Parse(String(&copy.front(), newLength));
		if( decimals == fractionalDecimals )//easy case, the user input had the exact right number of decimal places! (or too many, but we truncated)
			return parsed;
		TBigInteger<S> multiplier = TBigInteger<S>::Pow(10, decimals-fractionalDecimals);
		return parsed * multiplier;
	}
}

BigInteger DecimalConversion( const String& value, UInt32 decimals, Char decimalPoint='.', Char toleratedSeparator='\0' )
{
	return _DecimalConversion<false, PHANTASMA_VECTOR<Char>>( value, decimals, decimalPoint, toleratedSeparator );
}

SecureBigInteger DecimalConversion( const SecureString& value, UInt32 decimals, Char decimalPoint='.', Char toleratedSeparator='\0' )
{
	return _DecimalConversion<true, SecureVector<Char>>( value, decimals, decimalPoint, toleratedSeparator );
}

}

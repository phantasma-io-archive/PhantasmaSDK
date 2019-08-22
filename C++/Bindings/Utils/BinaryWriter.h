#pragma once
#ifndef PHANTASMA_API_INCLUDED
#error "Configure and include PhantasmaAPI.h first"
#endif 

namespace phantasma {

class BinaryWriter
{
	PHANTASMA_VECTOR<Byte> stream;
public:
	BinaryWriter(UInt32 sizeHint = 4096)
	{
		stream.reserve(sizeHint);
	}

	UInt32 Position() const { return 0; }

	PHANTASMA_VECTOR<Byte> ToArray() { return PHANTASMA_VECTOR<Byte>{}; }

	void Write(const Byte* b, int size) {};
	void Write(const PHANTASMA_VECTOR<Byte>&) {};
	void Write(uint8_t b) {};
	void Write( int8_t b) {};
	void Write(uint16_t b) {};
	void Write( int16_t b) {};
	void Write(uint32_t b) {};
	void Write( int32_t b) {};
	void Write(uint64_t b) {};
	void Write( int64_t b) {};
	void WriteVarInt( int i ) {};
};

}
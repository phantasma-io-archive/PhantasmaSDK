#pragma once

#include "VMObject.h"
#include "VirtualMachine.h"
#include "Opcodes.h"
#include "../Utils/TextUtils.h"
#include "../Utils/BinaryWriter.h"

namespace phantasma {

class ScriptBuilder
{
private:
	BinaryWriter writer;

	PHANTASMA_MAP<int, String> _jumpLocations;
	PHANTASMA_MAP<String, int> _labelLocations;

public:

	ScriptBuilder& Emit( Opcode opcode, const Byte* bytes=0, int numBytes=0 )
	{
		//auto ofs = (int)stream.Position;
		writer.Write( (Byte)opcode );

		if(bytes)
		{
			writer.Write( bytes, numBytes );
		}

		return *this;
	}

	ScriptBuilder& EmitPush( Byte reg )
	{
		Emit( Opcode::PUSH );
		writer.Write( (Byte)reg );
		return *this;
	}

	ScriptBuilder& EmitPop( Byte reg )
	{
		Emit( Opcode::POP );
		writer.Write( (Byte)reg );
		return *this;
	}

	ScriptBuilder& EmitExtCall( const String& method, Byte reg = 0 )
	{
		EmitLoad( reg, method );
		Emit( Opcode::EXTCALL );
		writer.Write( (Byte)reg );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const Byte* bytes, int numBytes, VMType type = VMType::Bytes )
	{
		if( numBytes > 0xFFFF )
		{
			PHANTASMA_EXCEPTION("tried to load too much data");
			Emit( Opcode::NOP );//todo - set error flag for when exceptions are disabled?
			return *this;
		}

		Emit( Opcode::LOAD );
		writer.Write( (Byte)reg );
		writer.Write( (Byte)type );

		writer.WriteVarInt( numBytes );
		writer.Write( bytes, numBytes );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const String& val )
	{
		ByteBuffer temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( val, temp, numBytes );
		EmitLoad( reg, bytes, numBytes, VMType::String );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const BigInteger& val )
	{
		auto bytes = val.ToByteArray( true );
		EmitLoad( reg, &bytes.front(), bytes.size(), VMType::Number );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, bool val )
	{
		Byte byte = (Byte)(val ? 1 : 0);
		EmitLoad( reg, &byte, 1, VMType::Bool );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, int enum_val )
	{
		UInt32 temp = (UInt32)enum_val;
		Byte bytes[4];
		memcpy(bytes, &temp, 4);
		EmitLoad( reg, bytes, 4, VMType::Enum );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, Timestamp val )
	{
		UInt32 temp = val.Value;
		Byte bytes[4];
		memcpy(bytes, &temp, 4);
		EmitLoad( reg, bytes, 4, VMType::Timestamp );
		return *this;
	}

	template<class TSerializable>
	ScriptBuilder& EmitLoad( Byte reg, TSerializable val )
	{
		BinaryWriter temp; 
		val.SerializeData( writer );
	
		PHANTASMA_VECTOR<Byte> bytes = temp.ToArray();
		EmitLoad( reg, &bytes.front(), bytes.size(), VMType::Bytes );
		return *this;
	}

	ScriptBuilder& EmitMove( Byte src_reg, Byte dst_reg )
	{
		Emit( Opcode::MOVE );
		writer.Write( (Byte)src_reg );
		writer.Write( (Byte)dst_reg );
		return *this;
	}

	ScriptBuilder& EmitCopy( Byte src_reg, Byte dst_reg )
	{
		Emit( Opcode::COPY );
		writer.Write( (Byte)src_reg );
		writer.Write( (Byte)dst_reg );
		return *this;
	}

	ScriptBuilder& EmitLabel( const String& label )
	{
		Emit( Opcode::NOP );
		_labelLocations[label] = (int)writer.Position();
		return *this;
	}

	ScriptBuilder& EmitJump( Opcode opcode, const String& label, Byte reg = 0 )
	{
		switch(opcode)
		{
		case Opcode::JMP:
		case Opcode::JMPIF:
		case Opcode::JMPNOT:
			Emit( opcode );
			break;

		default:
			PHANTASMA_EXCEPTION( "Invalid jump opcode" );// todo format the string properly
			//throw new Exception( "Invalid jump opcode: " + opcode ); 
			return *this;
		}

		if(opcode != Opcode::JMP)
		{
			writer.Write( reg );
		}

		auto ofs = (int)writer.Position();
		writer.Write( (uint16_t)0 );
		_jumpLocations[ofs] = label;
		return *this;
	}

	ScriptBuilder& EmitCall( const String& label, Byte regCount )
	{
		if(regCount<1 || regCount > VirtualMachine::MaxRegisterCount)
		{
			PHANTASMA_EXCEPTION( "Invalid number of registers" );
			return *this;
		}

		int ofs = (int)writer.Position();
		ofs += 2;
		Emit( Opcode::CALL );
		writer.Write( (Byte)regCount );
		writer.Write( (uint16_t)0 );

		_jumpLocations[ofs] = label;
		return *this;
	}

	ScriptBuilder& EmitConditionalJump( Opcode opcode, Byte src_reg, const String& label )
	{
		if(opcode != Opcode::JMPIF && opcode != Opcode::JMPNOT)
		{
			PHANTASMA_EXCEPTION( "Opcode is not a conditional jump" );
			return *this;
		}

		int ofs = (int)writer.Position();
		ofs += 2;

		Emit( opcode );
		writer.Write( (Byte)src_reg );
		writer.Write( (ushort)0 );
		_jumpLocations[ofs] = label;
		return *this;
	}

	ScriptBuilder& EmitVarBytes( long value )
	{
		writer.WriteVarInt( value );
		return *this;
	}

	ScriptBuilder& EmitRaw( const Byte* bytes, int numBytes )
	{
		writer.Write( bytes, numBytes );
		return *this;
	}

	PHANTASMA_VECTOR<Byte> ToScript()
	{
		PHANTASMA_VECTOR<Byte> script = writer.ToArray();

		// resolve jump offsets
		for( const auto& entry : _jumpLocations )
		{
			const auto& label = entry.second;
			uint16_t labelOffset = (uint16_t)_labelLocations[label];
			auto targetOffset = entry.first;

			script[targetOffset + 0] =  labelOffset       & 0xFF;
			script[targetOffset + 1] = (labelOffset >> 8) & 0xFF;
		}

		return script;
	}
};

}

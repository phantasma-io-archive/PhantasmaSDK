#pragma once

#include "VMObject.h"
#include "VirtualMachine.h"
#include "Opcodes.h"
#include "../Utils/TextUtils.h"
#include "../Utils/BinaryWriter.h"
#include <type_traits>

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

	ScriptBuilder& EmitLoad( Byte reg, const PHANTASMA_VECTOR<Byte>& bytes )
	{
		return EmitLoad( reg, bytes.empty()?0:&bytes.front(), (int)bytes.size() );
	}
	ScriptBuilder& EmitLoad( Byte reg, const Byte* bytes, int numBytes, VMType type = VMType::Bytes )
	{
		if(numBytes > 0xFFFF)
		{
			PHANTASMA_EXCEPTION( "tried to load too much data" );
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

	ScriptBuilder& EmitLoad( Byte reg, const Char* val )
	{
		ByteBuffer temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( val, temp, numBytes );
		EmitLoad( reg, bytes, numBytes, VMType::String );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const BigInteger& val )
	{
		auto bytes = val.ToSignedByteArray();
		EmitLoad( reg, bytes.empty()?0:&bytes.front(), (int)bytes.size(), VMType::Number );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, bool val )
	{
		Byte byte = (Byte)(val ? 1 : 0);
		EmitLoad( reg, &byte, 1, VMType::Bool );
		return *this;
	}

	template<class T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
	ScriptBuilder& EmitLoad( Byte reg, T enum_val )
	{
		UInt32 temp = (UInt32)enum_val;
		Byte bytes[4];
		memcpy( bytes, &temp, 4 );
		EmitLoad( reg, bytes, 4, VMType::Enum );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const Timestamp& val )
	{
		UInt32 temp = val.Value;
		Byte bytes[4];
		memcpy( bytes, &temp, 4 );
		EmitLoad( reg, bytes, 4, VMType::Timestamp );
		return *this;
	}

	template<class TSerializable, typename std::enable_if<std::is_base_of<Serializable, TSerializable>::value>::type* = nullptr>
	ScriptBuilder& EmitLoad( Byte reg, const TSerializable& val )
	{
		BinaryWriter temp;
		val.SerializeData( temp );

		const PHANTASMA_VECTOR<Byte>& bytes = temp.ToArray();
		EmitLoad( reg, bytes.empty() ? 0 : &bytes.front(), (int)bytes.size(), VMType::Bytes );
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
		writer.Write( (uint16_t)0 );
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
		for(const auto& entry : _jumpLocations)
		{
			const auto& label = entry.second;
			uint16_t labelOffset = (uint16_t)_labelLocations[label];
			auto targetOffset = entry.first;

			script[targetOffset + 0] =  labelOffset       & 0xFF;
			script[targetOffset + 1] = (labelOffset >> 8) & 0xFF;
		}

		return script;
	}

//--------------------------------------------------------------
// ScriptUtils.cs port:
//--------------------------------------------------------------

	static ScriptBuilder BeginScript()
	{
		return ScriptBuilder();
	}

	PHANTASMA_VECTOR<Byte> EndScript()
	{
		Emit( Opcode::RET );
		return ToScript();
	}
	
	ScriptBuilder& AllowGas( const Address& from, const Address& to, const BigInteger& gasPrice, const BigInteger& gasLimit )
	{
		CallContract( PHANTASMA_LITERAL("gas"), PHANTASMA_LITERAL("AllowGas"), from, to, gasPrice, gasLimit );
		return *this;
	}

	ScriptBuilder& SpendGas( const Address& address )
	{
		CallContract( PHANTASMA_LITERAL("gas"), PHANTASMA_LITERAL("SpendGas"), address );
		return *this;
	}

private:
// NOTE when arg is an array, this method will corrupt the two registers adjacent to target_reg
// "corrupt" in the sense of any value being there previousvly will be lost
// this should be taken into account by methods that call this method
// also this was not tested with nested arrays, might work, might not work yet
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const String& arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const Char* arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, Int32 arg )
	{
		sb.EmitLoad( target_reg, BigInteger( arg ) );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, Int64 arg )
	{
		sb.EmitLoad( target_reg, BigInteger( arg ) );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const BigInteger& arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, bool arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const PHANTASMA_VECTOR<Byte>& arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	template<class T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, T arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const Timestamp& arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	template<class T, typename std::enable_if<std::is_base_of<Serializable, T>::value>::type* = nullptr>
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const T& arg )
	{
		sb.EmitLoad( target_reg, arg );
	}
	template<class T>
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const PHANTASMA_VECTOR<T>& array )
	{
		// this cast is required to clear any previous value that might be stored at target_reg
		Byte cast_bytes[3] = { target_reg, target_reg, (Byte)VMType::None };
		sb.Emit( Opcode.CAST, cast_bytes );
		for(int j = 0, jend=(int)array.size(); j < jend; j++)
		{
			const auto& element = array[j];
			Byte temp_regVal = (Byte)(target_reg + 1);
			Byte temp_regKey = (Byte)(target_reg + 2);
			LoadIntoReg( sb, temp_regVal, element );
			LoadIntoReg( sb, temp_regKey, j );
			Byte put_bytes[3] = { temp_regVal, target_reg , temp_regKey };
			sb.Emit( Opcode::PUT, put_bytes );
		}
	}

	template<class T>
	static void InsertMethodArgs( ScriptBuilder& sb, const T& arg )
	{
		Byte temp_reg = 0;
		LoadIntoReg( sb, temp_reg, arg );
		sb.EmitPush( temp_reg );
	}
	template<class T, class... Args>
	static void InsertMethodArgs( ScriptBuilder& sb, const T& arg, const Args&... args )
	{
		InsertMethodArgs(sb, args...);
		InsertMethodArgs(sb, arg);
	}
public:

	template<class... Args>
	ScriptBuilder& CallInterop( const Char* method, const Args&... args )
	{
		InsertMethodArgs( *this, args... );

		Byte dest_reg = 0;
		EmitLoad( dest_reg, method );

		Emit( Opcode::EXTCALL, &dest_reg, 1 );
		return *this;
	}

	template<class... Args>
	ScriptBuilder& CallContract( const Char* contractName, const Char* method, const Args&... args )
	{
		InsertMethodArgs( *this, args... );

		Byte temp_reg = 0;
		EmitLoad( temp_reg, method );
		EmitPush( temp_reg );

		Byte src_reg = 0;
		Byte dest_reg = 1;
		EmitLoad( src_reg, contractName );
		Byte bytes[2] = { src_reg, dest_reg };
		Emit( Opcode::CTX, bytes, 2 );

		Emit( Opcode::SWITCH, &dest_reg, 1 );
		return *this;
	}
};

}

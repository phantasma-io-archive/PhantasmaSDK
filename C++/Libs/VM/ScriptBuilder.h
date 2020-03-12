#pragma once

#include "VMObject.h"
#include "VirtualMachine.h"
#include "Opcodes.h"
#include "../Utils/TextUtils.h"
#include "../Utils/BinaryWriter.h"
#include "../Utils/Timestamp.h"
#include "../Cryptography/Address.h"
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

	ScriptBuilder& EmitCast( Byte src_reg, Byte dst_reg, VMType type )
	{
		Emit( Opcode::CAST );
		writer.Write( (Byte)src_reg );
		writer.Write( (Byte)dst_reg );
		writer.Write( (Byte)type );
		return *this;
	}

	ScriptBuilder& EmitCat( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::CAT, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitLeft( Byte src_reg, Byte dest_reg, int length )
	{
		if(length < 0 || length > 0xFFFF)
		{
			PHANTASMA_EXCEPTION("Input exceed max");
			//todo - set error flag for when exceptions are disabled?
		}
		Emit( Opcode::LEFT );
		writer.Write( (Byte)src_reg );
		writer.Write( (Byte)dest_reg );
		writer.WriteVarInt(length);
		return *this;
	}

	ScriptBuilder& EmitRight( Byte src_reg, Byte dest_reg, int length )
	{
		if(length < 0 || length > 0xFFFF)
		{
			PHANTASMA_EXCEPTION("Input exceed max");
			//todo - set error flag for when exceptions are disabled?
		}
		Emit( Opcode::RIGHT );
		writer.Write( (Byte)src_reg );
		writer.Write( (Byte)dest_reg );
		writer.WriteVarInt(length);
		return *this;
	}

	ScriptBuilder& EmitSize( Byte src_reg, Byte dest_reg )
	{
		return Emit2Arg( Opcode::SIZE, src_reg, dest_reg );
	}

	ScriptBuilder& EmitCount( Byte src_reg, Byte dest_reg )
	{
		return Emit2Arg( Opcode::COUNT, src_reg, dest_reg );
	}

	ScriptBuilder& EmitNot( Byte src_reg, Byte dest_reg )
	{
		return Emit2Arg( Opcode::NOT, src_reg, dest_reg );
	}

	ScriptBuilder& EmitAnd( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::AND, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitOr( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::OR, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitXor( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::XOR, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitEqual( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::EQUAL, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitLessThan( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::LT, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitLessThanEqual( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::LTE, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitGreaterThan( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::GT, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitGreaterThanEqual( Byte src_a, Byte src_b, Byte dst_reg )
	{
		return Emit3Arg( Opcode::GTE, src_a, src_b, dst_reg );
	}

	ScriptBuilder& EmitInc( Byte reg )
	{
		Emit( Opcode::INC );
		writer.Write( (Byte)reg );
		return *this;
	}

	ScriptBuilder& EmitDec( Byte reg )
	{
		Emit( Opcode::DEC );
		writer.Write( (Byte)reg );
		return *this;
	}

	ScriptBuilder& EmitSign( Byte src_reg, Byte dest_reg )
	{
		return Emit2Arg( Opcode::SIGN, src_reg, dest_reg );
	}

	ScriptBuilder& EmitNegate( Byte src_reg, Byte dest_reg )
	{
		return Emit2Arg( Opcode::NEGATE, src_reg, dest_reg );
	}

	ScriptBuilder& EmitAbs( Byte src_reg, Byte dest_reg )
	{
		return Emit2Arg( Opcode::ABS, src_reg, dest_reg );
	}


	ScriptBuilder& EmitPut( Byte src_reg, Byte dest_reg, Byte key_reg )
	{
		return Emit3Arg( Opcode::PUT, src_reg, dest_reg, key_reg );
	}

private:
	ScriptBuilder& Emit2Arg( Opcode opcode, Byte src_reg, Byte dst_reg )
	{
		Emit( opcode );
		writer.Write( (Byte)src_reg );
		writer.Write( (Byte)dst_reg );
		return *this;
	}
	ScriptBuilder& Emit3Arg( Opcode opcode, Byte src_a, Byte src_b, Byte dst_reg )
	{
		Emit( opcode );
		writer.Write( (Byte)src_a );
		writer.Write( (Byte)src_b );
		writer.Write( (Byte)dst_reg );
		return *this;
	}
public:

	ScriptBuilder& EmitExtCall( const Char* method, Byte reg = 0 )
	{
		EmitLoad( reg, method );
		Emit( Opcode::EXTCALL );
		writer.Write( (Byte)reg );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const ByteArray& bytes )
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
		ByteArray temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( val, temp, numBytes );
		EmitLoad( reg, bytes, numBytes, VMType::String );
		return *this;
	}

	ScriptBuilder& EmitLoad( Byte reg, const Char* val )
	{
		ByteArray temp;
		int numBytes = 0;
		const Byte* bytes = GetUTF8Bytes( val, 0, temp, numBytes );
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

		const ByteArray& bytes = temp.ToArray();
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

	ScriptBuilder& EmitThrow( const ByteArray& data )
	{
		Emit( Opcode::THROW );
		writer.WriteByteArray( data );
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
			Emit( Opcode::NOP );//todo - set error flag for when exceptions are disabled?
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
			Emit( Opcode::NOP );//todo - set error flag for when exceptions are disabled?
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
			Emit( Opcode::NOP );//todo - set error flag for when exceptions are disabled?
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

	ByteArray ToScript()
	{
		ByteArray script = writer.ToArray();

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
	// ScriptBuilderExtensions.cs port:
	//--------------------------------------------------------------
	constexpr static const Char* GasContract = PHANTASMA_LITERAL("gas");
	constexpr static const Char* NexusContract = PHANTASMA_LITERAL("nexus");
	constexpr static const Char* TokenContract = PHANTASMA_LITERAL("token");
	constexpr static const Char* EnergyContract = PHANTASMA_LITERAL("energy");
	constexpr static const Char* SwapContract = PHANTASMA_LITERAL("swap");


	ScriptBuilder& AllowGas( const Address& from, const Address& to, const BigInteger& gasPrice, const BigInteger& gasLimit )
	{
		return CallContract( GasContract, PHANTASMA_LITERAL("AllowGas"), from, to, gasPrice, gasLimit );
	}

	ScriptBuilder& SpendGas( const Address& address )
	{
		return CallContract( GasContract, PHANTASMA_LITERAL("SpendGas"), address );
	}

	ScriptBuilder& MintTokens(const String& tokenSymbol, const Address& from, const Address& target, const BigInteger& amount)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.MintTokens"), from, target, tokenSymbol, amount);
	}

	ScriptBuilder& MintToken(const String& tokenSymbol, const Address& from, const Address& target, const ByteArray& rom, const ByteArray& ram)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.MintToken"), from, target, tokenSymbol, rom, ram); 
	}

	ScriptBuilder& MintTokenContentsFromRegisters(const String& tokenSymbol, const Address& from, const Address& target, Byte rom_reg, Byte ram_reg)
	{
		return EmitPush( ram_reg )
			.EmitPush( rom_reg )
			.CallInterop(PHANTASMA_LITERAL("Runtime.MintToken"), from, target, tokenSymbol); 
	}

	ScriptBuilder& BurnTokens(const Address& from, const String& tokenSymbol, const BigInteger& amount)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.BurnTokens"), from, tokenSymbol, amount); 
	}

	ScriptBuilder& BurnToken(const Address& from, const String& tokenSymbol, const BigInteger& nftID)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.BurnToken"), from, tokenSymbol, nftID); 
	}

	ScriptBuilder& TransferTokens(const String& tokenSymbol, const Address& from, const String& to, const BigInteger& amount)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.TransferTokens"), from, Address::FromText(to), tokenSymbol, amount);
	}

	ScriptBuilder& TransferTokens(const String& tokenSymbol, const Address& from, const Address& to, const BigInteger& amount)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.TransferTokens"), from, to, tokenSymbol, amount);
	}

    ScriptBuilder& TransferBalance(const String& tokenSymbol, const Address& from, const Address& to)
    {
        return CallInterop(PHANTASMA_LITERAL("Runtime.TransferBalance"), from, to, tokenSymbol);
    }

	ScriptBuilder& TransferToken(const String& tokenSymbol, const Address& from, const Address& to, const BigInteger& tokenId)//todo check if this is valid
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.TransferToken"), from, to, tokenSymbol, tokenId);
	}

	ScriptBuilder& TransferToken(const String& tokenSymbol, const Address& from, const String& to, const BigInteger& tokenId)//todo check if this is valid
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.TransferToken"), from, Address::FromText(to), tokenSymbol, tokenId);
	}

	ScriptBuilder& ReadTokenToRegister( const String& tokenSymbol, const BigInteger& nftId, Byte dst_reg)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.ReadToken"), tokenSymbol, nftId)
			.EmitPop(dst_reg);
	}

	ScriptBuilder& WriteTokenFromRegister( const String& tokenSymbol, const BigInteger& nftId, Byte src_reg)
	{
		Byte temp_reg = 0;
		return EmitPush(src_reg)
			.EmitLoad(temp_reg, nftId)
			.EmitPush(temp_reg)
			.EmitLoad(temp_reg, tokenSymbol)
			.EmitPush(temp_reg)
			.EmitExtCall( PHANTASMA_LITERAL("Runtime.WriteToken") );
	}

	ScriptBuilder& WriteToken(const String& tokenSymbol, const BigInteger& id, const ByteArray& ram)
	{
		return CallInterop(PHANTASMA_LITERAL("Runtime.WriteToken"), tokenSymbol, id, ram); 
	}

//--------------------------------------------------------------
// ScriptUtils.cs port:
//--------------------------------------------------------------

	static ScriptBuilder BeginScript()
	{
		return ScriptBuilder();
	}

	ByteArray EndScript()
	{
		Emit( Opcode::RET );
		return ToScript();
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
	static void LoadIntoReg( ScriptBuilder& sb, Byte target_reg, const ByteArray& arg )
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
		sb.EmitCast( target_reg, target_reg, VMType::None );
		for(int j = 0, jend=(int)array.size(); j < jend; j++)
		{
			const auto& element = array[j];
			Byte temp_regVal = (Byte)(target_reg + 1);
			Byte temp_regKey = (Byte)(target_reg + 2);
			LoadIntoReg( sb, temp_regVal, element );
			LoadIntoReg( sb, temp_regKey, j );
			sb.EmitPut( temp_regVal, target_reg, temp_regKey );
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

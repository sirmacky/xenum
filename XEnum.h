#pragma once
//=========================================================================
// https://www.scs.stanford.edu/~dm/blog/va-opt.html
//=========================================================================
#ifndef FOR_EACH
#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))

#define FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))

#define FOR_EACH_AGAIN() FOR_EACH_HELPER
#endif


//=========================================================================
// Macro Prefix Variant
//=========================================================================
#ifndef FOR_EACH_MACRO
#define FOR_EACH_MACRO(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_MACRO_HELPER(macro, __VA_ARGS__)))

#define FOR_EACH_MACRO_HELPER(macro, a1, ...)                         \
  macro##a1                                                           \
  __VA_OPT__(FOR_EACH_MACRO_AGAIN PARENS (macro, __VA_ARGS__))

#define FOR_EACH_MACRO_AGAIN() FOR_EACH_MACRO_HELPER
#endif


//=========================================================================
// https://stackoverflow.com/a/77894709
//=========================================================================
#ifndef VALUE_IFNOT
#define VALUE_IFNOT_TEST(...) __VA_ARGS__
#define VALUE_IFNOT_TEST0(...) __VA_ARGS__
#define VALUE_IFNOT_TEST1(...)
#define VALUE_IFNOT(COND, ...) VALUE_IFNOT_TEST ## COND ( __VA_ARGS__ )
#endif

//=========================================================================
// EnumValue
//=========================================================================

#include <string>
#include <iostream>
#include <concepts>

namespace XEnum
{
	template<typename T>
	concept ValidEnumUnderylingType = std::integral<T>;

	template<typename _enum_type, ValidEnumUnderylingType _underlying_type>
	struct EnumValue
	{
		using underlying_type = _underlying_type;
		using EnumType = _enum_type;

		constexpr EnumValue() = default;
		constexpr EnumValue(EnumValue&& val) = default;
		constexpr EnumValue(const underlying_type& val) { Value = val; }
		constexpr EnumValue(const EnumValue& val) { Value = val.GetValue(); }

		constexpr operator std::string() const { return ToString(); }
		constexpr operator underlying_type() const { return Value; }

		friend constexpr auto operator<=>(const EnumValue&, const EnumValue&) = default;
		friend constexpr std::ostream& operator<<(std::ostream& os, const EnumValue& value) { return os << value.ToCString(); }

		constexpr std::string ToString() const { return EnumType::ToString(Value); }
		constexpr const char* ToCString() const { return EnumType::ToCString(Value); }
		constexpr underlying_type GetValue() const { return Value; }

		// Arithmetic operations
		constexpr EnumValue& operator+=(const underlying_type& val) { Value += val; return *this; }
		constexpr EnumValue& operator-=(const underlying_type& val) { Value -= val; return *this; }
		constexpr EnumValue& operator*=(const underlying_type& val) { Value *= val; return *this; }
		constexpr EnumValue& operator/=(const underlying_type& val) { Value /= val; return *this; }

		friend constexpr EnumValue operator+(EnumValue lhs, const EnumValue& rhs) { lhs += rhs; return lhs; }
		friend constexpr EnumValue operator-(EnumValue lhs, const EnumValue& rhs) { lhs -= rhs; return lhs; }
		friend constexpr EnumValue operator*(EnumValue lhs, const EnumValue& rhs) { lhs *= rhs; return lhs; }
		friend constexpr EnumValue operator/(EnumValue lhs, const EnumValue& rhs) { lhs /= rhs; return lhs; }

		// Bitwise Arithmetic Operators
		friend constexpr EnumValue operator~(const EnumValue& a) { return ~a.Value; }

		constexpr EnumValue& operator|=(const underlying_type& val) { Value |= val; return *this; }
		constexpr EnumValue& operator&=(const underlying_type& val) { Value &= val; return *this; }
		constexpr EnumValue& operator^=(const underlying_type& val) { Value ^= val; return *this; }

		friend constexpr EnumValue& operator&(EnumValue lhs, const EnumValue& rhs) { lhs &= rhs; return lhs; }
		friend constexpr EnumValue& operator|(EnumValue lhs, const EnumValue& rhs) { lhs |= rhs; return lhs; }
		friend constexpr EnumValue& operator^(EnumValue lhs, const EnumValue& rhs) { lhs ^= rhs; return lhs; }

		// Shift operators
		friend constexpr EnumValue operator<<(const EnumValue& lhs, const underlying_type& rhs) { return lhs.Value >> rhs; }
		friend constexpr EnumValue operator>>(const EnumValue& lhs, const underlying_type& rhs) { return lhs.Value << lhs; }

		// Flag functions
		constexpr bool IsSet(underlying_type index) const { return (Value & (1 << index)) != 0; }
		constexpr bool Set(underlying_type index, bool status = true)
		{
			auto val = Value;
			if (status)
				Value |= (1 << index);
			else
				Value &= ~(1 << index);
			return val != Value;
		}
		constexpr bool Clear(underlying_type index) { return Set(index, false); }

	protected:

		underlying_type Value = {};
	};
}


//=========================================================================
// XEnum Simple Construction
//=========================================================================
#define ImplementSimpleXEnumValueDeclaration(_name_) static constexpr auto _name_ = EnumValue{ static_cast<underlying_type>(underlying_enum :: _name_) };
#define ImplementSimpleXEnumUnderlyingIndexDeclaration(_name_)  _name_,
#define ImplementSimpleXEnumValArrayEntry(_name_) _name_,
#define ImplementSimpleXEnumStrArrayEntry(_name_) #_name_,
#define ImplementSimpleXEnumIndexOfCase(_name_) case _name_ : return static_cast<int>(underlying_enum_index :: _name_);		
#define ImplementSimpleXEnumUnderlyingFlagDefinition(_name_) _name_ = 1 << static_cast<size_t>(underlying_enum_index :: _name_),

#define ImplementSimpleXEnumUnderlyingEnumDeclarationFlags(_starting_val_, _first_val_, ...) 	\
enum class underlying_enum : size_t									        					\
{																								\
	FOR_EACH(ImplementSimpleXEnumUnderlyingFlagDefinition, _first_val_, __VA_ARGS__)			\
};

#define ImplementSimpleXEnumUnderlyingEnumDeclaration(_starting_val_, _first_val_, ...)	   	\
enum class underlying_enum : underlying_type												\
{																							\
	_first_val_ = _starting_val_, 															\
	__VA_ARGS__																				\
};

#define ImplementSimpleXEnum_FullDeclaration(_name_, _underlying_enum_macro_, _underlying_type_, _starting_val_, _first_val_, ...)			   \
struct _name_ : public XEnum::EnumValue<_name_, _underlying_type_>													   \
{																													   \
	using this_type = _name_;																						   \
	using EnumValue = XEnum::EnumValue<_name_, _underlying_type_>;													   \
																													   \
private:																											   \
																													   \
    enum class underlying_enum_index : size_t																		   \
	{																												   \
		FOR_EACH(ImplementSimpleXEnumUnderlyingIndexDeclaration, _first_val_, __VA_ARGS__)							   \
	};																												   \
	_underlying_enum_macro_(_starting_val_, _first_val_, __VA_ARGS__)												   \
																													   \
public:																												   \
																													   \
	_name_() = delete;																								   \
	constexpr _name_(const underlying_type& val) : EnumValue(val) {}												   \
	constexpr _name_(const this_type& val) : EnumValue(val) {}														   \
	constexpr _name_(const EnumValue& val) : EnumValue(val) {}														   \
																													   \
	FOR_EACH(ImplementSimpleXEnumValueDeclaration, _first_val_, __VA_ARGS__)              							   \
																													   \
	static constexpr std::array Values =																			   \
	{																												   \
		FOR_EACH(ImplementSimpleXEnumValArrayEntry, _first_val_, __VA_ARGS__)										   \
	};																												   \
	static constexpr const char* CStrValues[] = 																	   \
	{																												   \
		FOR_EACH(ImplementSimpleXEnumStrArrayEntry, _first_val_, __VA_ARGS__)									       \
	};																												   \
	static constexpr auto Count = Values.size();																	   \
																													   \
	static constexpr std::string ToString(const EnumValue& value) { return ToString(value.GetValue()); }			   \
	static constexpr std::string ToString(const this_type& value) { return ToString(value.GetValue()); }			   \
	static constexpr std::string ToString(underlying_type value, const std::string& fallback = "<unknown>")			   \
	{																												   \
		return ToCString(value, fallback.c_str());																	   \
	}																												   \
																													   \
	static constexpr const char* ToCString(underlying_type value, const char* fallback = "<unknown>")				   \
	{																												   \
		int index = IndexOf(value);																					   \
		if (index == -1)																							   \
			return fallback;																						   \
		return CStrValues[index];																					   \
	}																												   \
																													   \
	static constexpr int IndexOf(underlying_type value)																   \
	{																												   \
		switch (value)																								   \
		{																											   \
			FOR_EACH(ImplementSimpleXEnumIndexOfCase, _first_val_, __VA_ARGS__)								           \
		}																											   \
		return -1;																									   \
	}																												   \
};

//=========================================================================
// Implementation Macros
//=========================================================================
#define ImplementSimpleXEnum(_name_, ...) ImplementSimpleXEnum_FullDeclaration(_name_, ImplementSimpleXEnumUnderlyingEnumDeclaration, int, 0, ___VA_ARGS__)		   
#define ImplementSimpleXEnumWithUnderlyingType(_name_, _underlying_type_, ...) ImplementSimpleXEnum_FullDeclaration(_name_,ImplementSimpleXEnumUnderlyingEnumDeclaration, _underlying_type_, 0, __VA_ARGS__)
#define ImplementSimpleXEnumWithStartingValue(_name_, _start_value_, ...) ImplementSimpleXEnum_FullDeclaration(_name_, ImplementSimpleXEnumUnderlyingEnumDeclaration, int, _start_value_, __VA_ARGS__)
#define ImplementSimpleXEnumFlags(_name_, ...)  ImplementSimpleXEnum_FullDeclaration(_name_, ImplementSimpleXEnumUnderlyingEnumDeclarationFlags, size_t, 0, __VA_ARGS__)

//=========================================================================
// Examples
//=========================================================================
/*
	ImplementSimpleXEnum(Simple,
		Val1,
		Val2,
		Val3
	);

	ImplementSimpleXEnumWithUnderlyingType(SimpleTyped, unsigned int,
		Val1,
		Val2,
		Val3
	);

	ImplementSimpleXEnumWithStartingValue(SimpleWithInvalid, -1,
		Invalid,
		Val1,
		Val2,
		Val3
	);

	ImplementSimpleXEnumFlags(SimpleFlags,
		Flag1,
		Flag2,
		Flag3
	);
*/

//=========================================================================
// XValue is a standard value, defaults to whatever was before it +1
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXValue(_name_, ...) \
	VALUE_IFNOT(__VA_OPT__(1),  _name_)\
	__VA_OPT__(_name_ = __VA_ARGS__),

#define ImplementXEnumIndexAssignmentXValue(_name_, ...)   _name_,
#define ImplementXEnumValueDeclarationXValue(_name_, ...) static constexpr auto _name_ = EnumValue{ static_cast<underlying_type>(underlying_enum :: _name_) };
#define ImplementXEnumArrayEntryDeclarationXValue(_name_, ...) _name_,
#define ImplementXEnumToStringCaseXValue(_name_, ...) case _name_ : return static_cast<int>(underlying_index_enum :: _name_);	
#define ImplementXEnumToCStringDeclarationXValue(_name_, ...) #_name_,

//=========================================================================
// XFlag defaults to 1 << index, parameter allows regular assignment
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXFlag(_name_, ...)  \
	VALUE_IFNOT(__VA_OPT__(1),  _name_ = 1 << static_cast<int>(underlying_index_enum :: _name_ ))\
	__VA_OPT__(_name_ = 1 << static_cast<int>(__VA_ARGS__)),

#define ImplementXEnumIndexAssignmentXFlag(_name_, ...) ImplementXEnumIndexAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumValueDeclarationXFlag(_name_, ...) ImplementXEnumValueDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumArrayEntryDeclarationXFlag(_name_, ...) ImplementXEnumArrayEntryDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToStringCaseXFlag(_name_, ...) ImplementXEnumToStringCaseXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToCStringDeclarationXFlag(_name_, ...) ImplementXEnumToCStringDeclarationXValue(_name, __VA_ARGS__)

//=========================================================================
// XFlagState does not appear in the values array and defaults to 0 if no value is provided
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXFlagState(_name_, ...)  \
	VALUE_IFNOT(__VA_OPT__(1),  _name_ = 0)\
	__VA_OPT__(_name_ = __VA_ARGS__),

#define ImplementXEnumIndexAssignmentXFlagState(_name_, ...) ImplementXEnumIndexAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumValueDeclarationXFlagState(_name_, ...) ImplementXEnumValueDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumArrayEntryDeclarationXFlagState(_name_, ...)
#define ImplementXEnumToStringCaseXFlagState(_name_, ...) ImplementXEnumToStringCaseXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToCStringDeclarationXFlagState(_name_, ...) ImplementXEnumToCStringDeclarationXValue(_name, __VA_ARGS__)

//=========================================================================
// XAlias does not appear in the index (as it's covered) nor does it have a string, or an index value associated with it, just a reference and an underlying enum reference
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXAlias(_name_, ...)  ImplementXEnumUnderlyngAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumIndexAssignmentXAlias(_name_, ...) //ImplementXEnumIndexAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumValueDeclarationXAlias(_name_, ...) ImplementXEnumValueDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumArrayEntryDeclarationXAlias(_name_, ...) //ImplementXEnumArrayEntryDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToStringCaseXAlias(_name_, ...) 
#define ImplementXEnumToCStringDeclarationXAlias(_name_, ...) //ImplementXEnumToCStringDeclarationXValue(_name, __VA_ARGS__)

//=========================================================================
// Macro variant construction enum
//=========================================================================
#define ImplmenetXEnum_FullDeclaration(_name_, _underlying_type_, ...)												   \
struct _name_ : public XEnum::EnumValue<_name_, _underlying_type_>													   \
{																													   \
	using this_type = _name_;																						   \
	using EnumValue = XEnum::EnumValue<_name_, _underlying_type_>;													   \
																													   \
private:																											   \
																													   \
    enum class underlying_index_enum : size_t { FOR_EACH_MACRO(ImplementXEnumIndexAssignment, __VA_ARGS__) };		   \
	enum class underlying_enum : underlying_type { FOR_EACH_MACRO(ImplementXEnumUnderlyngAssignment, __VA_ARGS__) };   \
																													   \
public:																												   \
																													   \
	_name_() = delete;																								   \
	constexpr _name_(const underlying_type& val) : EnumValue(val) {}												   \
	constexpr _name_(const this_type& val) : EnumValue(val) {}														   \
	constexpr _name_(const EnumValue& val) : EnumValue(val) {}														   \
																													   \
	FOR_EACH_MACRO(ImplementXEnumValueDeclaration, __VA_ARGS__)              					  			     	   \
																													   \
	static constexpr std::array Values =																			   \
	{																												   \
		FOR_EACH_MACRO(ImplementXEnumArrayEntryDeclaration, __VA_ARGS__)					    				       \
	};																												   \
    static constexpr const char* CStrValues[] = 																	   \
	{																												   \
		FOR_EACH_MACRO(ImplementXEnumToCStringDeclaration, __VA_ARGS__)												   \
	};																												   \
	static constexpr auto Count = Values.size();																	   \
																													   \
	static constexpr std::string ToString(const EnumValue& value) { return ToString(value.GetValue()); }			   \
	static constexpr std::string ToString(const this_type& value) { return ToString(value.GetValue()); }			   \
	static constexpr std::string ToString(underlying_type value, const std::string& fallback = "<unknown>")			   \
	{																												   \
		return ToCString(value, fallback.c_str());																	   \
	}																												   \
																													   \
	static constexpr const char* ToCString(underlying_type value, const char* fallback = "<unknown>")				   \
	{																												   \
		int index = IndexOf(value);																					   \
		if (index == -1)																							   \
			return fallback;																						   \
		return CStrValues[index];																					   \
	}																												   \
																													   \
	static constexpr int IndexOf(underlying_type value)																   \
	{																												   \
		switch (value)																								   \
		{																											   \
			FOR_EACH_MACRO(ImplementXEnumToStringCase, __VA_ARGS__)											           \
		}																											   \
		return -1;																									   \
	}																												   \
};		

//=========================================================================
// Implementration Aliases
//=========================================================================
#define ImplementXEnum(_name_, ...) ImplmenetXEnum_FullDeclaration(_name_, int, __VA_ARGS__)			
#define ImplementXEnumWithUnderlyingType(_name_, _type_, ...) ImplmenetXEnum_FullDeclaration(_name_, _type_, __VA_ARGS__)
//=========================================================================
// EXAMPLES
//=========================================================================
/*
	ImplementXEnum(ExampleEnum,
		XValue(a),				// 0
		XValue(b, 3),			// 3
		XValue(d),				// 4
		XAlias(e, d),			// 4
		XFlag(f),				// 1 << 3
		XFlag(g),				// 1 << 4
		XFlag(h, 5),			// 1 << 5
		XValue(x),				// 33
		XFlagState(y, f | g),	// 24
	);
*/
//=========================================================================
// Safety Checks to help diagnose potential issues
//=========================================================================
#if defined(XValue) || defined(XFlagMacro) || defined(XFlagStateMacro) || defined(XAlias)
static_assert(false, "These macros must remain UNDEFINED for XEnum to work")
#endif
//=========================================================================
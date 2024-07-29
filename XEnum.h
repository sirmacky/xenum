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
// XEnum
//=========================================================================
#include <array>
#include <string>

template<typename EnumType> struct XEnumTraits { static constexpr bool is_valid = false; };

template<typename EnumType> concept XEnumValue = XEnumTraits<EnumType>::is_valid;

//=========================================================================
// Safety Checks to help diagnose potential issues
//=========================================================================
#if defined(XValue) || defined(XFlagMacro) || defined(XFlagStateMacro) || defined(XAlias)
static_assert(false, "These macros must remain UNDEFINED for XEnum to work")
#endif
//=========================================================================
// XValue is a standard value, defaults to whatever was before it +1
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXValue(_name_, ...) \
	VALUE_IFNOT(__VA_OPT__(1),  _name_)\
	__VA_OPT__(_name_ = __VA_ARGS__),

#define ImplementXEnumIndexAssignmentXValue(_name_, ...)   _name_,
#define ImplementXEnumArrayEntryDeclarationXValue(_name_, ...) EnumType :: _name_,
#define ImplementXEnumToStringCaseXValue(_name_, ...) case EnumType :: _name_ : return static_cast<int>(underlying_index_enum :: _name_);	
#define ImplementXEnumToCStringDeclarationXValue(_name_, ...) #_name_,

//=========================================================================
// XFlag defaults to 1 << index, parameter allows regular assignment
//=========================================================================
/* This variant is broken due to the underlying index being inaccessible with the information available
#define ImplementXEnumUnderlyngAssignmentXFlag(_name_, ...)  \
	 VALUE_IFNOT(__VA_OPT__(1),  _name_ = 1 << static_cast<int>(underlying_index_enum :: _name_ )) \
	__VA_OPT__(_name_ = 1 << static_cast<int>(__VA_ARGS__)),
*/

#define ImplementXEnumUnderlyngAssignmentXFlag(_name_, ...)  _name_ = 1 << static_cast<int>(__VA_ARGS__),
#define ImplementXEnumIndexAssignmentXFlag(_name_, ...) ImplementXEnumIndexAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumArrayEntryDeclarationXFlag(_name_, ...) ImplementXEnumArrayEntryDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToStringCaseXFlag(_name_, ...) ImplementXEnumToStringCaseXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToCStringDeclarationXFlag(_name_, ...) ImplementXEnumToCStringDeclarationXValue(_name_, __VA_ARGS__)
//=========================================================================
// XAlias does not appear in the index (as it's covered) nor does it have a string, or an index value associated with it, just a reference and an underlying enum reference
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXAlias(_name_, ...)  ImplementXEnumUnderlyngAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumIndexAssignmentXAlias(_name_, ...) //ImplementXEnumIndexAssignmentXValue(_name_, __VA_ARGS__)
#define ImplementXEnumArrayEntryDeclarationXAlias(_name_, ...) //ImplementXEnumArrayEntryDeclarationXValue(_name_, __VA_ARGS__)
#define ImplementXEnumToStringCaseXAlias(_name_, ...) 
#define ImplementXEnumToCStringDeclarationXAlias(_name_, ...) //ImplementXEnumToCStringDeclarationXValue(_name_, __VA_ARGS__)

//=========================================================================
// XFlagState is currently just an alias. It is intended not to appear in the values array and defaults to 0 if no value is provided
//=========================================================================
#define ImplementXEnumUnderlyngAssignmentXFlagState(_name_, ...) _name_ =  \
	VALUE_IFNOT(__VA_OPT__(1), 0)\
	__VA_OPT__(__VA_ARGS__),

#define ImplementXEnumIndexAssignmentXFlagState(_name_, ...) ImplementXEnumIndexAssignmentXAlias(_name_, __VA_ARGS__)
#define ImplementXEnumArrayEntryDeclarationXFlagState(_name_, ...)
#define ImplementXEnumToStringCaseXFlagState(_name_, ...) ImplementXEnumToStringCaseXAlias(_name_, __VA_ARGS__)
#define ImplementXEnumToCStringDeclarationXFlagState(_name_, ...) ImplementXEnumToCStringDeclarationXAlias(_name_, __VA_ARGS__)

//=========================================================================
// Macro variant construction enum
//=========================================================================
#define ImplmenetXEnum_FullDeclaration(_name_, _underlying_type_, ...)												   \
																													   \
enum class _name_ : _underlying_type_																				   \
{																													   \
	FOR_EACH_MACRO(ImplementXEnumUnderlyngAssignment, __VA_ARGS__)													   \
};																													   \
																													   \
template<>																											   \
struct XEnumTraits< _name_ >																						   \
{																													   \
	static constexpr bool is_valid = true;																			   \
    static constexpr const char* const name = #_name_ ;																   \
	using EnumType = _name_;																						   \
	enum class underlying_index_enum : size_t																		   \
	{																												   \
		FOR_EACH_MACRO(ImplementXEnumIndexAssignment, __VA_ARGS__)													   \
	};																												   \
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
	static constexpr std::string ToString(EnumType value, const std::string& fallback = "<unknown>")			       \
	{																												   \
		return ToCString(value, fallback.c_str());																	   \
	}																												   \
																													   \
	static constexpr const char* ToCString(EnumType value, const char* fallback = "<unknown>")				           \
	{																												   \
		int index = IndexOf(value);																					   \
		if (index == -1)																							   \
			return fallback;																						   \
		return CStrValues[index];																					   \
	}																												   \
																													   \
	static constexpr int IndexOf(EnumType value)																       \
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
		XFlag(f, 3),			// 1 << 3
		XFlag(g, 4),			// 1 << 4
		XFlag(h, 5),			// 1 << 5
		XValue(x),				// 33
		XFlagState(y, f | g)	// 24
	);
//*/
//=========================================================================


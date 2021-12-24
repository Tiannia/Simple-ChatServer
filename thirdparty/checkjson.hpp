#ifndef CHECKJSON_H
#define CHECKJSON_H
 
#include <iostream>
#include <string>
 
using namespace std;
 
class JsonString
{
public:
	typedef const char* Location;
	JsonString();
 
	/* 检测json字符串格式是否合法 */
	bool CheckFormat(const char *begin, const char *end);
 
private:
	enum TokenType
	{
		tokenEndOfStream = 0,
		tokenObjectBegin,
		tokenObjectEnd,
		tokenArrayBegin,
		tokenArrayEnd,
		tokenString,
		tokenNumber,
		tokenTrue,
		tokenFalse,
		tokenNull,
		tokenArraySeparator,
		tokenMemberSeparator,
		tokenError
	};
 
	class Token
	{
	public:
		TokenType type_;
	};
 
private:
	void SkipSpace();
	char GetNextChar();
	bool Match(Location pattern, int patternLength);
	bool ReadToken(Token &token);
 
	bool ReadString();
	void ReadNumber();
	bool ReadValue();
	bool ReadObject(Token &token);
	bool ReadArray(Token &token);
 
private:
	Location begin_;
	Location end_;
	Location current_;
	
	/* 用于记录对象的数量 */
	int objnum;
};
 
#endif	//CHECKJSON_H
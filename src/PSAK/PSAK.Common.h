#pragma once
#include <sstream>

namespace PSAK
{
	class PsRowReader;

	template<typename TS>
	TS& operator>>(TS& input, PsRowReader& reader);
	std::string& operator>>(std::string& input, PsRowReader& reader);

	// PSAK行加载器
	class PsRowReader
	{
	protected:
		std::stringstream ss; /*字符串流*/
		char delim; /*分隔符*/
		char note; /*注释符*/

		template<typename TS>
		friend TS& operator>>(TS& input, PsRowReader& reader);
		friend std::string& operator>>(std::string& input, PsRowReader& reader);
	public:
		operator bool() const; /*判断行读取器是否为空*/
		bool operator !() const; /*判断行读取器是否非空*/
		void removeStrSpace(std::string& str) const; /*移除字符串两侧空白字符*/
		bool equal(std::string lhs, std::string rhs); /*判断两个字符串是否相等(去除两侧空白，忽略大小写)*/
		std::string getBuffer(); /*获取缓冲区*/
		PsRowReader& operator>>(std::string& rhs); /*从行读取器中顺序加载一个字符串类型数据*/
		PsRowReader& operator>>(long long& rhs); /*从行读取器中顺序加载一个长整形类型数据*/
		PsRowReader& operator>>(unsigned long long& rhs); /*从行读取器中顺序加载一个无符号长整形类型数据*/
		PsRowReader& operator>>(int& rhs); /*从行读取器中顺序加载一个整形类型数据*/
		PsRowReader& operator>>(unsigned int& rhs); /*从行读取器中顺序加载一个无符号整形类型数据*/
		PsRowReader& operator>>(float& rhs); /*从行读取器中顺序加载一个单精度浮点类型数据*/
		PsRowReader& operator>>(double& rhs); /*从行读取器中顺序加载一个双精度浮点类型数据*/
		PsRowReader& operator>>(bool& rhs); /*从行读取器中顺序加载一个布尔类型数据*/
	public:
		PsRowReader(); /*默认构造*/
		PsRowReader(char delim, char note); /*构造时指定分隔符与注释符*/
	};

	template<typename TS>
	TS& operator>>(TS& input, PsRowReader& reader)
	{
		std::string buffer;
		bool status(false);
		while (getline(input, buffer))
		{
			for (auto& c : buffer)
			{
				if ((c != reader.note) && (c < 0 || !isspace(c)))
				{
					status = true;
					break;
				}
			}
			if (status)
				break;
		}
		reader.removeStrSpace(buffer);
		reader.ss.str(buffer);
		reader.ss.clear();
		return input;
	}
}

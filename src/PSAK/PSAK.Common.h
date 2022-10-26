#pragma once
#include <sstream>

namespace PSAK
{
	class PsRowReader;

	template<typename TS>
	TS& operator>>(TS& input, PsRowReader& reader);
	std::string& operator>>(std::string& input, PsRowReader& reader);

	// PSAK�м�����
	class PsRowReader
	{
	protected:
		std::stringstream ss; /*�ַ�����*/
		char delim; /*�ָ���*/
		char note; /*ע�ͷ�*/

		template<typename TS>
		friend TS& operator>>(TS& input, PsRowReader& reader);
		friend std::string& operator>>(std::string& input, PsRowReader& reader);
	public:
		operator bool() const; /*�ж��ж�ȡ���Ƿ�Ϊ��*/
		bool operator !() const; /*�ж��ж�ȡ���Ƿ�ǿ�*/
		void removeStrSpace(std::string& str) const; /*�Ƴ��ַ�������հ��ַ�*/
		bool equal(std::string lhs, std::string rhs); /*�ж������ַ����Ƿ����(ȥ������հף����Դ�Сд)*/
		std::string getBuffer(); /*��ȡ������*/
		PsRowReader& operator>>(std::string& rhs); /*���ж�ȡ����˳�����һ���ַ�����������*/
		PsRowReader& operator>>(long long& rhs); /*���ж�ȡ����˳�����һ����������������*/
		PsRowReader& operator>>(unsigned long long& rhs); /*���ж�ȡ����˳�����һ���޷��ų�������������*/
		PsRowReader& operator>>(int& rhs); /*���ж�ȡ����˳�����һ��������������*/
		PsRowReader& operator>>(unsigned int& rhs); /*���ж�ȡ����˳�����һ���޷���������������*/
		PsRowReader& operator>>(float& rhs); /*���ж�ȡ����˳�����һ�������ȸ�����������*/
		PsRowReader& operator>>(double& rhs); /*���ж�ȡ����˳�����һ��˫���ȸ�����������*/
		PsRowReader& operator>>(bool& rhs); /*���ж�ȡ����˳�����һ��������������*/
	public:
		PsRowReader(); /*Ĭ�Ϲ���*/
		PsRowReader(char delim, char note); /*����ʱָ���ָ�����ע�ͷ�*/
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

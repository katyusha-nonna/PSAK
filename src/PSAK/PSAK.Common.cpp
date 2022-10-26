#include "PSAK.Common.h"
#include <exception>
#include <algorithm>

PSAK::PsRowReader::operator bool() const
{
	return ss.rdbuf()->in_avail();
}

bool PSAK::PsRowReader::operator!() const
{
	return !ss.rdbuf()->in_avail();
}

void PSAK::PsRowReader::removeStrSpace(std::string& str) const
{
	auto wsEndsAt = std::find_if(str.begin(), str.end(), [](char c) {return c < 0 || !isspace(c); });
	str.erase(str.begin(), wsEndsAt);
	auto wsStartsAt = std::find_if(str.rbegin(), str.rend(), [](char c) {return c < 0 || !isspace(c); });
	str.erase(wsStartsAt.base(), str.end());
}

bool PSAK::PsRowReader::equal(std::string lhs, std::string rhs)
{
	if (&lhs == &rhs) return true;
	removeStrSpace(lhs);
	removeStrSpace(rhs);
	if (lhs.size() != rhs.size())
		return false;
	for (auto& s : lhs)
		s = tolower(s);
	for (auto& s : rhs)
		s = tolower(s);
	for (size_t i = 0; i < lhs.size(); i++)
		if (lhs[i] != rhs[i])
			return false;
	return true;
}

std::string PSAK::PsRowReader::getBuffer()
{
	return ss.str();
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(std::string& rhs)
{
	getline(ss, rhs, delim);
	removeStrSpace(rhs);
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(long long& rhs)
{
	std::string buffer;
	*this >> buffer;
	try
	{
		rhs = std::stoll(buffer);
	}
	catch (...)
	{
		auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to long long failed ( " + buffer + " )");
		throw std::exception(msg.c_str());
	}
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(unsigned long long& rhs)
{
	std::string buffer;
	*this >> buffer;
	try
	{
		rhs = std::stoull(buffer);
	}
	catch (...)
	{
		auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to unsigned long long failed ( " + buffer + " )");
		throw std::exception(msg.c_str());
	}
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(int& rhs)
{
	std::string buffer;
	*this >> buffer;
	try
	{
		rhs = std::stoi(buffer);
	}
	catch (...)
	{
		auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to int failed ( " + buffer + " )");
		throw std::exception(msg.c_str());
	}
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(unsigned int& rhs)
{
	// 无法转换为unsigned int
	std::string buffer;
	*this >> buffer;
	auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to unsigned int failed ( " + buffer + " )");
	throw std::exception(msg.c_str());
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(float& rhs)
{
	std::string buffer;
	*this >> buffer;
	try
	{
		rhs = std::stof(buffer);
	}
	catch (...)
	{
		auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to float failed ( " + buffer + " )");
		throw std::exception(msg.c_str());
	}
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(double& rhs)
{
	std::string buffer;
	*this >> buffer;
	try
	{
		rhs = std::stod(buffer);
	}
	catch (...)
	{
		auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to double failed ( " + buffer + " )");
		throw std::exception(msg.c_str());
	}
	return *this;
}

PSAK::PsRowReader& PSAK::PsRowReader::operator>>(bool& rhs)
{
	std::string buffer;
	*this >> buffer;
	try
	{
		if (equal(buffer, "true"))
			rhs = true;
		else if (equal(buffer, "false"))
			rhs = false;
		else
		{
			try
			{
				rhs = static_cast<bool>(std::stoi(buffer));
			}
			catch (const std::exception&)
			{
				throw std::invalid_argument("");
			}
		}
	}
	catch (...)
	{
		auto msg("[PSAK::PsRowReader::operator>>]: Exception->convert string to bool failed ( " + buffer + " )");
		throw std::exception(msg.c_str());
	}
	return *this;
}

PSAK::PsRowReader::PsRowReader()
	: ss(),
	delim('\t'),
	note('#')
{

}

PSAK::PsRowReader::PsRowReader(char delim, char note)
	: ss(),
	delim(delim),
	note(note)
{

}

std::string& PSAK::operator>>(std::string& input, PsRowReader& reader)
{
	std::string buffer(input);
	bool status(false);
	for (auto& c : buffer)
	{
		if ((c != reader.note) && (c < 0 || !isspace(c)))
		{
			status = true;
			break;
		}
	}
	reader.removeStrSpace(buffer);
	reader.ss.str(buffer);
	reader.ss.clear();
	return input;
}

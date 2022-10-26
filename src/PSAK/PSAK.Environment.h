#pragma once

namespace PSAK
{
	template<typename Derive>
	class PsCounter;

	// ʵ����������(CRTP)
	template<typename Derive>
	class PsCounter
	{
	protected:
		static long long objc_; /*���ഴ������*/
		static long long obja_; /*��������*/
	public:
		auto getObjCreated() { return objc_; }
		auto getObjAlive() { return obja_; }
	public:
		PsCounter() : idx_(objc_) { ++objc_; ++obja_; }
		PsCounter(const PsCounter& base) { ++objc_; ++obja_; }
		~PsCounter() { --obja_; }
	};

	template<typename Derive> long long PsCounter<Derive>::objc_(0);
	template<typename Derive> long long PsCounter<Derive>::obja_(0);

}
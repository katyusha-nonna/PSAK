#pragma once

namespace PSAK
{
	template<typename Derive>
	class PsCounter;

	// 实例化计数器(CRTP)
	template<typename Derive>
	class PsCounter
	{
	protected:
		static long long objc_; /*子类创建计数*/
		static long long obja_; /*子类活动计数*/
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
#pragma once
#include "PSAK.PF.Model.h"
#include "PSAK.PF.Config.h"

namespace PSAK
{
	namespace PF
	{
		class PsPFNodeOpt;
		class PsPFSolver;

		// PSAK.PF�ڵ����Ż���
		class PsPFNodeOpt
		{
		protected:
			PsPFMdl* mdl_;
		public:
			void extract(PsPFMdl* model); /*��ȡ����ģ��*/
			void random(); /*������(������Ԫ��˳��)*/
			void tinney1(); /*Tinney-1�����Ż����(��Ҫ��ִ��random)*/
			void tinney2(); /*Tinney-2�����Ż����(��Ҫ��ִ��random)*/
			void tinney3(); /*Tinney-3�����Ż����(��Ҫ��ִ��random)*/
		};

		// PSAK.PF���������
		class PsPFSolver
		{
		protected:
			PsPFMdl* mdl_;

			struct PsPFNtIterInfo;
		public:
			void extract(PsPFMdl* model); /*��ȡ����ģ��*/
			void init(const PsPFConfig& config); /*��ʼ��*/
			bool gauss(const PsPFConfig& config); /*ִ�и�˹����������*/
			bool newton(const PsPFConfig& config); /*ִ��ţ�ٷ���������*/
			bool fast(const PsPFConfig& config); /*ִ�п��ٽ����������*/
		};
	}
}
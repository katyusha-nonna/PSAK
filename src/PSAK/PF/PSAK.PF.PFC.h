#pragma once
#include "PSAK.PF.Model.h"
#include "PSAK.PF.Config.h"

namespace PSAK
{
	namespace PF
	{
		class PsPFNodeOpt;
		class PsPFSolver;

		// PSAK.PF节点编号优化器
		class PsPFNodeOpt
		{
		protected:
			PsPFMdl* mdl_;
		public:
			void extract(PsPFMdl* model); /*抽取潮流模型*/
			void random(); /*随机编号(按表中元件顺序)*/
			void tinney1(); /*Tinney-1方案优化编号(需要先执行random)*/
			void tinney2(); /*Tinney-2方案优化编号(需要先执行random)*/
			void tinney3(); /*Tinney-3方案优化编号(需要先执行random)*/
		};

		// PSAK.PF潮流求解器
		class PsPFSolver
		{
		protected:
			PsPFMdl* mdl_;

			struct PsPFNtIterInfo;
		public:
			void extract(PsPFMdl* model); /*抽取潮流模型*/
			void init(const PsPFConfig& config); /*初始化*/
			bool gauss(const PsPFConfig& config); /*执行高斯法潮流计算*/
			bool newton(const PsPFConfig& config); /*执行牛顿法潮流计算*/
			bool fast(const PsPFConfig& config); /*执行快速解耦法潮流计算*/
		};
	}
}
#pragma once
#include "../PSAK.Environment.h"
#include <unordered_map>

namespace PSAK
{
	namespace PF
	{
		// 静态-输入时由输入参数决定
		// 半静态-输入时由模型对象决定
		// 动态-计算时由求解器对象决定

		class PsPFMdlBase;
		class PsPFMdlBus;
		class PsPFMdlBranchPi;
		class PsPFMdlGen;
		class PsPFMdlLoad;
		class PsPFMdlComp;
		class PsPFMdlInfo;
		class PsPFMdl;

		// PSAK.PF节点类型
		enum class PsPFBusType
		{
			PQ, /*P-Q节点*/
			PV, /*P-V节点*/
			VT /*V-Theta节点*/
		};

		// PSAK.PF支路类型
		enum class PsPFBranchType
		{
			Line, /*线路*/
			Tran, /*变压器*/
		};

		class PsPFMdlBase
		{
		public:
			long long idx; /*元件编号*/
		public:
			virtual void initStatic() = 0; /*初始化静态数据*/
			virtual void initDynamic() = 0; /*初始化动态数据*/
		protected:
			PsPFMdlBase();
			virtual ~PsPFMdlBase();
		};

		// PSAK.PF节点模型
		class PsPFMdlBus
			: public PsPFMdlBase
		{
		public:
			PsPFBusType type; /*静态-节点类型*/
			long long no; /*动态-节点计算编号*/
			double va; /*动态-电压幅值(极坐标)*/
			double vt; /*动态-电压相角(极坐标)*/
			double ve; /*动态-电压实部(直角坐标)*/
			double vf; /*动态-电压虚部(直角坐标)*/
			double pi; /*动态-总有功注入功率*/
			double qi; /*动态-总无功注入功率*/
			double pg; /*动态-总有功发电功率*/
			double qg; /*动态-总无功发电功率*/
			double pd; /*动态-总有功负荷功率*/
			double qd; /*动态-总无功负荷功率*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*初始化静态数据*/
			virtual void initDynamic(); /*初始化动态数据*/
		protected:
			PsPFMdlBus();
			PsPFMdlBus(const PsPFMdlBus&) = delete;
			virtual ~PsPFMdlBus();
		};

		// PSAK.PF支路模型-Pi型
		class PsPFMdlBranchPi
			: public PsPFMdlBase
		{
		public:
			bool mode; /*静态-非标准变比侧(true-首端 / false-末端)*/
			PsPFBranchType type; /*静态-支路类型*/
			long long no; /*动态-支路编号*/
			double r; /*静态-支路电阻*/
			double x; /*静态-支路电抗*/
			double bk; /*静态-支路对地电纳 / 变压器非标准变比*/
			double ije; /*动态-支路电流实部*/
			double ijf; /*动态-支路电流虚部*/
			double pij; /*动态-支路首端有功功率*/
			double qij; /*动态-支路首端无功功率*/
			double pji; /*动态-支路末端有功功率*/
			double qji; /*动态-支路末端无功功率*/
			double pl; /*动态-支路有功损耗*/
			double ql; /*动态-支路无功损耗*/
			PsPFMdlBus* bus1; /*静态-支路首端节点*/
			PsPFMdlBus* bus2; /*静态-支路末端节点*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*初始化静态数据*/
			virtual void initDynamic(); /*初始化动态数据*/
		protected:
			PsPFMdlBranchPi();
			PsPFMdlBranchPi(const PsPFMdlBranchPi&) = delete;
			virtual ~PsPFMdlBranchPi();
		};

		// PSAK.PF发电机模型
		class PsPFMdlGen
			: public PsPFMdlBase
		{
		public:
			long long no; /*动态-发电机编号*/
			double v; /*静态-电压幅值*/
			double pg; /*静态-有功出力*/
			double qg; /*静态-无功出力*/
			double qmin; /*静态-最小无功出力*/
			double qmax; /*静态-最大无功出力*/
			PsPFMdlBus* bus; /*静态-发电机节点*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*初始化静态数据*/
			virtual void initDynamic(); /*初始化动态数据*/
		protected:
			PsPFMdlGen();
			PsPFMdlGen(const PsPFMdlGen&) = delete;
			virtual ~PsPFMdlGen();
		};
		
		// PSAK.PF负荷模型
		class PsPFMdlLoad
			: public PsPFMdlBase
		{
		public:
			long long no; /*动态-负荷编号*/
			double v; /*静态-电压幅值*/
			double pd; /*静态-有功负荷*/
			double qd; /*静态-无功负荷*/
			PsPFMdlBus* bus; /*静态-负荷节点*/

			friend class PsPFMdl;
		public:
			void initStatic(); /*初始化静态数据*/
			void initDynamic(); /*初始化动态数据*/
		protected:
			PsPFMdlLoad();
			PsPFMdlLoad(const PsPFMdlLoad&) = delete;
			virtual ~PsPFMdlLoad();
		};

		// PSAK.PF补偿装置模型
		class PsPFMdlComp
			: public PsPFMdlBase
		{
		public:
			long long no; /*动态-补偿装置编号*/
			double cap; /*静态-补偿容量 / 电纳*/
			PsPFMdlBus* bus; /*静态-补偿装置节点*/

			friend class PsPFMdl;
		public:
			void initStatic(); /*初始化静态数据*/
			void initDynamic(); /*初始化动态数据*/
		protected:
			PsPFMdlComp();
			PsPFMdlComp(const PsPFMdlComp&) = delete;
			virtual ~PsPFMdlComp();
		};

		// PSAK.PF统计信息
		class PsPFMdlInfo
			: public PsPFMdlBase
		{
		public:
			long long nb; /*半静态-节点数目*/
			long long nl; /*半静态-支路数目*/
			long long ng; /*半静态-发电机数目*/
			long long nd; /*半静态-负荷数目*/
			long long nc; /*半静态-补偿装置数目*/
			long long npq; /*半静态-PQ节点数目*/
			long long npv; /*半静态-PV节点数目*/
			long long idxvt; /*半静态-V-Theta节点索引*/
			long long nvt; /*动态-V-Theta节点计算编号*/
			double pg; /*动态-总有功发电*/
			double qg; /*动态-总无功发电*/
			double pd; /*动态-总有功负荷*/
			double qd; /*动态-总无功负荷*/
			double pl; /*动态-总有功网损*/
			double ql; /*动态-总无功网损*/
			double sparsity; /*动态-稀疏程度*/
			std::string name; /*算例名称*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*初始化静态数据*/
			virtual void initDynamic(); /*初始化动态数据*/
		protected:
			PsPFMdlInfo();
			PsPFMdlInfo(const PsPFMdlInfo&) = delete;
			virtual ~PsPFMdlInfo();
		};

		// PSAK.PF潮流计算模型
		// addBus格式 ： [节点索引] [节点类型(pq/pv/vt)]
		// addBranch格式：[支路索引] [支路类型(l/t)] [首端节点索引] [末端节点索引] [支路电阻] [支路电抗] [对地半电纳/非标准变比]
		// addGen格式：[发电机索引] [发电机节点索引] [有功功率] [无功功率] [电压] [无功下限] [无功上限]
		// addLoad格式：[负荷索引] [负荷节点索引] [有功功率] [无功功率] [电压]
		// addComp格式：[补偿索引] [补偿节点索引] [补偿容量]
		class PsPFMdl
		{
		public:
			std::unordered_map<long long, PsPFMdlBus*> busTab; /*节点表*/
			std::unordered_map<long long, PsPFMdlBranchPi*> branchTab; /*支路表*/
			std::unordered_map<long long, PsPFMdlGen*> genTab; /*发电机表*/
			std::unordered_map<long long, PsPFMdlLoad*> loadTab; /*负荷表*/
			std::unordered_map<long long, PsPFMdlComp*> compTab; /*补偿装置表*/
			PsPFMdlInfo* info;  /*统计信息表*/
		public:
			void init(); /*初始化模型(重复初始化前要先执行clear)*/
			void clear(); /*清空模型*/
			PsPFMdlBase* addBus(long long idx); /*添加节点*/
			PsPFMdlBase* addBus(std::string param, char delim = '\t'); /*添加节点*/
			PsPFMdlBase* addBranch(long long idx); /*添加支路*/
			PsPFMdlBase* addBranch(std::string param, char delim = '\t'); /*添加支路*/
			PsPFMdlBase* addGen(long long idx); /*添加发电机*/
			PsPFMdlBase* addGen(std::string param, char delim = '\t'); /*添加发电机*/
			PsPFMdlBase* addLoad(long long idx); /*添加负荷*/
			PsPFMdlBase* addLoad(std::string param, char delim = '\t');  /*添加负荷*/
			PsPFMdlBase* addComp(long long idx);  /*添加补偿*/
			PsPFMdlBase* addComp(std::string param, char delim = '\t');  /*添加补偿*/
			void removeBus(long long idx); /*移除节点*/
			void removeBranch(long long idx); /*移除支路*/
			void removeGen(long long idx); /*移除发电机*/
			void removeLoad(long long idx); /*移除负荷*/
			void removeComp(long long idx); /*移除补偿*/
			void update(); /*更新统计信息表(在添加完毕所有元件后执行)*/
			void read(std::string path); /*读取潮流模型*/
			void write(std::string path); /*输出潮流模型*/
			void report(std::string path); /*输出潮流报告*/
		public:
			PsPFMdl();
			~PsPFMdl();
		};
	}
}

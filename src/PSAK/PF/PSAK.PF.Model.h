#pragma once
#include "../PSAK.Environment.h"
#include <unordered_map>

namespace PSAK
{
	namespace PF
	{
		// ��̬-����ʱ�������������
		// �뾲̬-����ʱ��ģ�Ͷ������
		// ��̬-����ʱ��������������

		class PsPFMdlBase;
		class PsPFMdlBus;
		class PsPFMdlBranchPi;
		class PsPFMdlGen;
		class PsPFMdlLoad;
		class PsPFMdlComp;
		class PsPFMdlInfo;
		class PsPFMdl;

		// PSAK.PF�ڵ�����
		enum class PsPFBusType
		{
			PQ, /*P-Q�ڵ�*/
			PV, /*P-V�ڵ�*/
			VT /*V-Theta�ڵ�*/
		};

		// PSAK.PF֧·����
		enum class PsPFBranchType
		{
			Line, /*��·*/
			Tran, /*��ѹ��*/
		};

		class PsPFMdlBase
		{
		public:
			long long idx; /*Ԫ�����*/
		public:
			virtual void initStatic() = 0; /*��ʼ����̬����*/
			virtual void initDynamic() = 0; /*��ʼ����̬����*/
		protected:
			PsPFMdlBase();
			virtual ~PsPFMdlBase();
		};

		// PSAK.PF�ڵ�ģ��
		class PsPFMdlBus
			: public PsPFMdlBase
		{
		public:
			PsPFBusType type; /*��̬-�ڵ�����*/
			long long no; /*��̬-�ڵ������*/
			double va; /*��̬-��ѹ��ֵ(������)*/
			double vt; /*��̬-��ѹ���(������)*/
			double ve; /*��̬-��ѹʵ��(ֱ������)*/
			double vf; /*��̬-��ѹ�鲿(ֱ������)*/
			double pi; /*��̬-���й�ע�빦��*/
			double qi; /*��̬-���޹�ע�빦��*/
			double pg; /*��̬-���й����繦��*/
			double qg; /*��̬-���޹����繦��*/
			double pd; /*��̬-���й����ɹ���*/
			double qd; /*��̬-���޹����ɹ���*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*��ʼ����̬����*/
			virtual void initDynamic(); /*��ʼ����̬����*/
		protected:
			PsPFMdlBus();
			PsPFMdlBus(const PsPFMdlBus&) = delete;
			virtual ~PsPFMdlBus();
		};

		// PSAK.PF֧·ģ��-Pi��
		class PsPFMdlBranchPi
			: public PsPFMdlBase
		{
		public:
			bool mode; /*��̬-�Ǳ�׼��Ȳ�(true-�׶� / false-ĩ��)*/
			PsPFBranchType type; /*��̬-֧·����*/
			long long no; /*��̬-֧·���*/
			double r; /*��̬-֧·����*/
			double x; /*��̬-֧·�翹*/
			double bk; /*��̬-֧·�Եص��� / ��ѹ���Ǳ�׼���*/
			double ije; /*��̬-֧·����ʵ��*/
			double ijf; /*��̬-֧·�����鲿*/
			double pij; /*��̬-֧·�׶��й�����*/
			double qij; /*��̬-֧·�׶��޹�����*/
			double pji; /*��̬-֧·ĩ���й�����*/
			double qji; /*��̬-֧·ĩ���޹�����*/
			double pl; /*��̬-֧·�й����*/
			double ql; /*��̬-֧·�޹����*/
			PsPFMdlBus* bus1; /*��̬-֧·�׶˽ڵ�*/
			PsPFMdlBus* bus2; /*��̬-֧·ĩ�˽ڵ�*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*��ʼ����̬����*/
			virtual void initDynamic(); /*��ʼ����̬����*/
		protected:
			PsPFMdlBranchPi();
			PsPFMdlBranchPi(const PsPFMdlBranchPi&) = delete;
			virtual ~PsPFMdlBranchPi();
		};

		// PSAK.PF�����ģ��
		class PsPFMdlGen
			: public PsPFMdlBase
		{
		public:
			long long no; /*��̬-��������*/
			double v; /*��̬-��ѹ��ֵ*/
			double pg; /*��̬-�й�����*/
			double qg; /*��̬-�޹�����*/
			double qmin; /*��̬-��С�޹�����*/
			double qmax; /*��̬-����޹�����*/
			PsPFMdlBus* bus; /*��̬-������ڵ�*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*��ʼ����̬����*/
			virtual void initDynamic(); /*��ʼ����̬����*/
		protected:
			PsPFMdlGen();
			PsPFMdlGen(const PsPFMdlGen&) = delete;
			virtual ~PsPFMdlGen();
		};
		
		// PSAK.PF����ģ��
		class PsPFMdlLoad
			: public PsPFMdlBase
		{
		public:
			long long no; /*��̬-���ɱ��*/
			double v; /*��̬-��ѹ��ֵ*/
			double pd; /*��̬-�й�����*/
			double qd; /*��̬-�޹�����*/
			PsPFMdlBus* bus; /*��̬-���ɽڵ�*/

			friend class PsPFMdl;
		public:
			void initStatic(); /*��ʼ����̬����*/
			void initDynamic(); /*��ʼ����̬����*/
		protected:
			PsPFMdlLoad();
			PsPFMdlLoad(const PsPFMdlLoad&) = delete;
			virtual ~PsPFMdlLoad();
		};

		// PSAK.PF����װ��ģ��
		class PsPFMdlComp
			: public PsPFMdlBase
		{
		public:
			long long no; /*��̬-����װ�ñ��*/
			double cap; /*��̬-�������� / ����*/
			PsPFMdlBus* bus; /*��̬-����װ�ýڵ�*/

			friend class PsPFMdl;
		public:
			void initStatic(); /*��ʼ����̬����*/
			void initDynamic(); /*��ʼ����̬����*/
		protected:
			PsPFMdlComp();
			PsPFMdlComp(const PsPFMdlComp&) = delete;
			virtual ~PsPFMdlComp();
		};

		// PSAK.PFͳ����Ϣ
		class PsPFMdlInfo
			: public PsPFMdlBase
		{
		public:
			long long nb; /*�뾲̬-�ڵ���Ŀ*/
			long long nl; /*�뾲̬-֧·��Ŀ*/
			long long ng; /*�뾲̬-�������Ŀ*/
			long long nd; /*�뾲̬-������Ŀ*/
			long long nc; /*�뾲̬-����װ����Ŀ*/
			long long npq; /*�뾲̬-PQ�ڵ���Ŀ*/
			long long npv; /*�뾲̬-PV�ڵ���Ŀ*/
			long long idxvt; /*�뾲̬-V-Theta�ڵ�����*/
			long long nvt; /*��̬-V-Theta�ڵ������*/
			double pg; /*��̬-���й�����*/
			double qg; /*��̬-���޹�����*/
			double pd; /*��̬-���й�����*/
			double qd; /*��̬-���޹�����*/
			double pl; /*��̬-���й�����*/
			double ql; /*��̬-���޹�����*/
			double sparsity; /*��̬-ϡ��̶�*/
			std::string name; /*��������*/

			friend class PsPFMdl;
		public:
			virtual void initStatic(); /*��ʼ����̬����*/
			virtual void initDynamic(); /*��ʼ����̬����*/
		protected:
			PsPFMdlInfo();
			PsPFMdlInfo(const PsPFMdlInfo&) = delete;
			virtual ~PsPFMdlInfo();
		};

		// PSAK.PF��������ģ��
		// addBus��ʽ �� [�ڵ�����] [�ڵ�����(pq/pv/vt)]
		// addBranch��ʽ��[֧·����] [֧·����(l/t)] [�׶˽ڵ�����] [ĩ�˽ڵ�����] [֧·����] [֧·�翹] [�Եذ����/�Ǳ�׼���]
		// addGen��ʽ��[���������] [������ڵ�����] [�й�����] [�޹�����] [��ѹ] [�޹�����] [�޹�����]
		// addLoad��ʽ��[��������] [���ɽڵ�����] [�й�����] [�޹�����] [��ѹ]
		// addComp��ʽ��[��������] [�����ڵ�����] [��������]
		class PsPFMdl
		{
		public:
			std::unordered_map<long long, PsPFMdlBus*> busTab; /*�ڵ��*/
			std::unordered_map<long long, PsPFMdlBranchPi*> branchTab; /*֧·��*/
			std::unordered_map<long long, PsPFMdlGen*> genTab; /*�������*/
			std::unordered_map<long long, PsPFMdlLoad*> loadTab; /*���ɱ�*/
			std::unordered_map<long long, PsPFMdlComp*> compTab; /*����װ�ñ�*/
			PsPFMdlInfo* info;  /*ͳ����Ϣ��*/
		public:
			void init(); /*��ʼ��ģ��(�ظ���ʼ��ǰҪ��ִ��clear)*/
			void clear(); /*���ģ��*/
			PsPFMdlBase* addBus(long long idx); /*��ӽڵ�*/
			PsPFMdlBase* addBus(std::string param, char delim = '\t'); /*��ӽڵ�*/
			PsPFMdlBase* addBranch(long long idx); /*���֧·*/
			PsPFMdlBase* addBranch(std::string param, char delim = '\t'); /*���֧·*/
			PsPFMdlBase* addGen(long long idx); /*��ӷ����*/
			PsPFMdlBase* addGen(std::string param, char delim = '\t'); /*��ӷ����*/
			PsPFMdlBase* addLoad(long long idx); /*��Ӹ���*/
			PsPFMdlBase* addLoad(std::string param, char delim = '\t');  /*��Ӹ���*/
			PsPFMdlBase* addComp(long long idx);  /*��Ӳ���*/
			PsPFMdlBase* addComp(std::string param, char delim = '\t');  /*��Ӳ���*/
			void removeBus(long long idx); /*�Ƴ��ڵ�*/
			void removeBranch(long long idx); /*�Ƴ�֧·*/
			void removeGen(long long idx); /*�Ƴ������*/
			void removeLoad(long long idx); /*�Ƴ�����*/
			void removeComp(long long idx); /*�Ƴ�����*/
			void update(); /*����ͳ����Ϣ��(������������Ԫ����ִ��)*/
			void read(std::string path); /*��ȡ����ģ��*/
			void write(std::string path); /*�������ģ��*/
			void report(std::string path); /*�����������*/
		public:
			PsPFMdl();
			~PsPFMdl();
		};
	}
}

#include "PSAK.PF.PFC.h"
#include <set>
#include <algorithm>

/*PSAK::PF::PsPFNodeOpt*/

// ���˹�ϵͼ
class PsPFGraph
{
public:
	std::vector<std::set<long long> > link; /*�ڽӱ�*/
	std::vector<long long> out; /*���ȱ�(����ߺϲ�)*/
public:
	void connect(long long no1, long long no2);
	void disconnect(long long no1, long long no2);
	void init(PSAK::PF::PsPFMdl* mdl);
public:
	PsPFGraph();
};

void PsPFGraph::connect(long long no1, long long no2)
{
	if (link[no1].emplace(no2).second)
		++(out[no1]);
	if (link[no2].emplace(no1).second)
		++(out[no2]);
}

void PsPFGraph::disconnect(long long no1, long long no2)
{
	auto iter1 = link[no1].find(no2);
	if (iter1 != link[no1].end())
	{
		--(out[no1]);
		link[no1].erase(no2);
	}
	auto iter2 = link[no2].find(no1);
	if (iter2 != link[no2].end())
	{
		--(out[no2]);
		link[no2].erase(no1);
	}
}

void PsPFGraph::init(PSAK::PF::PsPFMdl* mdl)
{
	link = std::vector<std::set<long long> >(mdl->info->nb);
	out = std::vector<long long>(mdl->info->nb, 0);
	for (auto& cp : mdl->branchTab)
		connect(cp.second->bus1->no, cp.second->bus2->no);
}

PsPFGraph::PsPFGraph()
{

}

void PSAK::PF::PsPFNodeOpt::extract(PsPFMdl* model)
{
	mdl_ = model;
}

void PSAK::PF::PsPFNodeOpt::random()
{
	long long counter(0);
	for (auto& cp : mdl_->busTab)
	{
		cp.second->no = counter++;
		if (cp.second->type == PsPFBusType::VT)
			mdl_->info->nvt = cp.second->no;
	}
	counter = 0;
	for (auto& cp : mdl_->branchTab)
		cp.second->no = counter++;
	counter = 0;
	for (auto& cp : mdl_->genTab)
		cp.second->no = counter++;
	counter = 0;
	for (auto& cp : mdl_->loadTab)
		cp.second->no = counter++;
	counter = 0;
	for (auto& cp : mdl_->compTab)
		cp.second->no = counter++;
}

void PSAK::PF::PsPFNodeOpt::tinney1()
{
	PsPFGraph graph /*���˹�ϵͼ*/;
	std::vector<std::pair<long long, long long> > order; /*����-ԭʼ���˳���*/
	std::vector<long long> o2n(mdl_->info->nb, -1); /*ԭʼ���->�Ż����ӳ���*/
	graph.init(mdl_);
	order.reserve(graph.out.size());
	for (long long i = 0; i < mdl_->info->nb; i++)
		order.emplace_back(std::make_pair(graph.out[i], i));
	std::sort(order.begin(), order.end(), /*�����ȴ�С��������(������ͬ��ȡ��С���)*/
		[](auto& p1, auto& p2) {return p1.first < p2.first ? true : (p1.first == p2.first ? p1.second < p2.second : false); });
	for (long long count = 0; count < mdl_->info->nb; count++)
		o2n[order[count].second] = count;
	for (auto& cp : mdl_->busTab)
	{
		cp.second->no = o2n[cp.second->no];
		if (cp.second->type == PsPFBusType::VT)
			mdl_->info->nvt = cp.second->no;
	}
}

void PSAK::PF::PsPFNodeOpt::tinney2()
{
	PsPFGraph graph /*���˹�ϵͼ*/;
	std::vector<long long> wait; /*����������*/
	std::vector<long long> o2n(mdl_->info->nb, -1); /*ԭʼ���->�Ż����ӳ���*/
	graph.init(mdl_);
	for (long long count = 0; count < mdl_->info->nb; count++)
	{
		// S1��Ѱ�ҳ������ٵĽڵ���Ϊ����ȥ�ڵ�(������ͬ���Զ�ȡ��С���)
		long long cur(std::min_element(graph.out.begin(), graph.out.end()) - graph.out.begin());
		// S2�������ڽӱ�ͳ��ȱ�
		if (count < count < mdl_->info->nb - 1)
		{
			wait.clear();
			wait.reserve(graph.link[cur].size());
			for (auto& n : graph.link[cur])
				wait.emplace_back(n);
			// S2.1������ȥ�ڵ�����ô���
			graph.out[cur] = LLONG_MAX;
			// S2.2����ȥ�ýڵ������֧·
			for (auto& n : wait)
			{
				--(graph.out[n]);
				graph.link[n].erase(cur);
			}
			// S2.3���жϸýڵ�����ڽڵ�֮���Ƿ����ڣ�������ڣ������в��������򲹳�֧·
			if (wait.size() > 1)
				for (size_t i = 0; i < wait.size() - 1; i++)
					for (size_t j = i + 1; j < wait.size(); j++)
						graph.connect(wait[i], wait[j]);
		}
		// S3����¼�Ż����
		o2n[cur] = count;
	}
	for (auto& cp : mdl_->busTab)
	{
		cp.second->no = o2n[cp.second->no];
		if (cp.second->type == PsPFBusType::VT)
			mdl_->info->nvt = cp.second->no;
	}
}

void PSAK::PF::PsPFNodeOpt::tinney3()
{
	// ��δʵ��
}

/*PSAK::PF::PsPFNodeOpt*/

void PSAK::PF::PsPFSolver::extract(PsPFMdl* model)
{
	mdl_ = model;
}

void PSAK::PF::PsPFSolver::init(const PsPFConfig& config)
{
	// ��ʼ��ͳ����Ϣ
	mdl_->info->pg = 0;
	mdl_->info->qg = 0;
	mdl_->info->pd = 0;
	mdl_->info->qd = 0;
	mdl_->info->pl = 0;
	mdl_->info->ql = 0;
	mdl_->info->sparsity = 0;
	// ��ʼ���ڵ���Ϣ
	for (auto& cp : mdl_->busTab)
	{
		cp.second->va = 1;
		cp.second->vt = 0;
		cp.second->ve = 1;
		cp.second->vf = 0;
		cp.second->pi = 0;
		cp.second->qi = 0;
		cp.second->pg = 0;
		cp.second->qg = 0;
		cp.second->pd = 0;
		cp.second->qd = 0;
	}
	// ��ʼ��֧·��Ϣ
	for (auto& cp : mdl_->branchTab)
	{
		cp.second->ije = 0;
		cp.second->ijf = 0;
		cp.second->pij = 0;
		cp.second->qij = 0;
		cp.second->pji = 0;
		cp.second->qji = 0;
		cp.second->pl = 0;
		cp.second->ql = 0;
	}
	// ���ݸ�����Ϣ��ʼ���ڵ���Ϣ
	for (auto& cp : mdl_->loadTab)
	{
		auto bus(cp.second->bus);
		bus->pd += cp.second->pd;
		bus->qd += cp.second->qd;
		bus->pi -= cp.second->pd;
		bus->qi -= cp.second->qd;
		if (bus->type == PsPFBusType::PV || bus->type == PsPFBusType::VT)
		{
			bus->va = cp.second->v;
			bus->ve = cp.second->v;
		}
	}
	// ���ݷ������Ϣ��ʼ���ڵ���Ϣ
	for (auto& cp : mdl_->genTab)
	{
		auto bus(cp.second->bus);
		bus->pg += cp.second->pg;
		bus->qg += cp.second->qg;
		bus->pi += cp.second->pg;
		bus->qi += cp.second->qg;
		if (bus->type == PsPFBusType::PV || bus->type == PsPFBusType::VT)
		{
			bus->va = cp.second->v;
			bus->ve = cp.second->v;
		}
	}
}


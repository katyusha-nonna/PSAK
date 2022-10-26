#include "PSAK.PF.Model.h"
#include "../PSAK.Common.h"
#include <fstream>

/* PSAK::PF::PsPFMdlBase */

PSAK::PF::PsPFMdlBase::PsPFMdlBase()
	: idx(-1)
{

}

PSAK::PF::PsPFMdlBase::~PsPFMdlBase()
{

}

/* PSAK::PF::PsPFMdlBus */

void PSAK::PF::PsPFMdlBus::initStatic()
{
	type = PsPFBusType::PQ;
}

void PSAK::PF::PsPFMdlBus::initDynamic()
{
	no = -1;
	va = 1;
	vt = 0;
	ve = 1;
	vf = 0;
	pi = 0;
	qi = 0;
	pg = 0;
	qg = 0;
	pd = 0;
	qd = 0;
}

PSAK::PF::PsPFMdlBus::PsPFMdlBus()
{
	
}

PSAK::PF::PsPFMdlBus::~PsPFMdlBus()
{

}

/* PSAK::PF::PsPFMdlBranchPi */

void PSAK::PF::PsPFMdlBranchPi::initStatic()
{
	mode = true;
	type = PsPFBranchType::Line;
	r = 0;
	x = 0;
	bk = 1;
	bus1 = nullptr;
	bus2 = nullptr;
}

void PSAK::PF::PsPFMdlBranchPi::initDynamic()
{
	no = -1;
	ije = 0;
	ijf = 0;
	pij = 0;
	qij = 0;
	pji = 0;
	qji = 0;
	pl = 0;
	ql = 0;
}

PSAK::PF::PsPFMdlBranchPi::PsPFMdlBranchPi()
{

}

PSAK::PF::PsPFMdlBranchPi::~PsPFMdlBranchPi()
{

}

/* PSAK::PF::PsPFMdlGen */

void PSAK::PF::PsPFMdlGen::initStatic()
{
	v = 1;
	pg = 0;
	qg = 0;
	qmin = -INFINITY;
	qmax = INFINITY;
	bus = nullptr;
}

void PSAK::PF::PsPFMdlGen::initDynamic()
{
	no = -1;
}

PSAK::PF::PsPFMdlGen::PsPFMdlGen()
{

}

PSAK::PF::PsPFMdlGen::~PsPFMdlGen()
{

}

/* PSAK::PF::PsPFMdlLoad */

void PSAK::PF::PsPFMdlLoad::initStatic()
{
	v = 1;
	pd = 0;
	qd = 0;
	bus = nullptr;
}

void PSAK::PF::PsPFMdlLoad::initDynamic()
{
	no = -1;
}

PSAK::PF::PsPFMdlLoad::PsPFMdlLoad()
{

}

PSAK::PF::PsPFMdlLoad::~PsPFMdlLoad()
{

}

/* PSAK::PF::PsPFMdlComp */

void PSAK::PF::PsPFMdlComp::initStatic()
{
	cap = 0;
	bus = nullptr;
}

void PSAK::PF::PsPFMdlComp::initDynamic()
{
	no = -1;
}

PSAK::PF::PsPFMdlComp::PsPFMdlComp()
{

}

PSAK::PF::PsPFMdlComp::~PsPFMdlComp()
{

}


/* PSAK::PF::PsPFMdlInfo */

void PSAK::PF::PsPFMdlInfo::initStatic()
{
	nb = 0;
	nl = 0;
	ng = 0;
	nd = 0;
	nc = 0;
	npq = 0;
	npv = 0;
	idxvt = -1;
	name = "PF";
}

void PSAK::PF::PsPFMdlInfo::initDynamic()
{
	nvt = -1;
	pg = 0;
	qg = 0;
	pd = 0;
	qd = 0;
	pl = 0;
	ql = 0;
	sparsity = 0;
}

PSAK::PF::PsPFMdlInfo::PsPFMdlInfo()
{
	
}

PSAK::PF::PsPFMdlInfo::~PsPFMdlInfo()
{

}

/* PSAK::PF::PsPFMdl */

void PSAK::PF::PsPFMdl::init()
{
	info = new PsPFMdlInfo();
	info->initStatic();
	info->initDynamic();
}

void PSAK::PF::PsPFMdl::clear()
{
	delete info;
	info = nullptr;
	for (auto& cp : busTab)
		delete cp.second;
	busTab.clear();
	for (auto& cp : branchTab)
		delete cp.second;
	branchTab.clear();
	for (auto& cp : genTab)
		delete cp.second;
	genTab.clear();
	for (auto& cp : loadTab)
		delete cp.second;
	loadTab.clear();
	for (auto& cp : compTab)
		delete cp.second;
	compTab.clear();
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addBus(long long idx)
{
	auto& tmp = busTab.try_emplace(idx).first->second;
	if (tmp)
		delete tmp;
	tmp = new PsPFMdlBus();
	tmp->initStatic();
	tmp->idx = idx;
	tmp->initDynamic();
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addBus(std::string param, char delim)
{
	long long idx(-1);
	std::string type;
	PsRowReader reader;
	param >> reader;
	reader >> idx >> type;
	PsPFMdlBus* tmp(static_cast<PsPFMdlBus*>(addBus(idx)));
	if (reader.equal(type, "pq"))
		tmp->type = PsPFBusType::PQ;
	else if (reader.equal(type, "pv"))
		tmp->type = PsPFBusType::PV;
	else if (reader.equal(type, "vt"))
		tmp->type = PsPFBusType::VT;
	else
	{
		std::string msg("[PSAK::PF::PsPFMdl::addBus]: Exception->invalid type ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addBranch(long long idx)
{
	auto& tmp = branchTab.try_emplace(idx).first->second;
	if (tmp)
		delete tmp;
	tmp = new PsPFMdlBranchPi();
	tmp->initStatic();
	tmp->idx = idx;
	tmp->initDynamic();
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addBranch(std::string param, char delim)
{
	long long idx(-1);
	long long bus1(-1), bus2(-1);
	std::string type;
	PsRowReader reader;
	param >> reader;
	reader >> idx >> type >> bus1 >> bus2;
	PsPFMdlBranchPi* tmp(static_cast<PsPFMdlBranchPi*>(addBranch(idx)));
	if (reader.equal(type, "l"))
		tmp->type = PsPFBranchType::Line;
	else if (reader.equal(type, "t"))
		tmp->type = PsPFBranchType::Tran;
	else
	{
		std::string msg("[PSAK::PF::PsPFMdl::addBranch]: Exception->invalid type ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	auto iter1(busTab.find(bus1));
	if (iter1 == busTab.end())
	{
		std::string msg("[PSAK::PF::PsPFMdl::addBranch]: Exception->invalid bus1 ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	tmp->bus1 = iter1->second;
	auto iter2(busTab.find(bus2));
	if (iter2 == busTab.end())
	{
		std::string msg("[PSAK::PF::PsPFMdl::addBranch]: Exception->invalid bus2 ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	tmp->bus2 = iter2->second;
	reader >> tmp->r >> tmp->x >> tmp->bk;
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addGen(long long idx)
{
	auto& tmp = genTab.try_emplace(idx).first->second;
	if (tmp)
		delete tmp;
	tmp = new PsPFMdlGen();
	tmp->initStatic();
	tmp->idx = idx;
	tmp->initDynamic();
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addGen(std::string param, char delim)
{
	long long idx(-1);
	long long bus(-1);
	PsRowReader reader;
	param >> reader;
	reader >> idx >> bus;
	PsPFMdlGen* tmp(static_cast<PsPFMdlGen*>(addGen(idx)));
	auto iter(busTab.find(bus));
	if (iter == busTab.end())
	{
		std::string msg("[PSAK::PF::PsPFMdl::addGen]: Exception->invalid bus ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	tmp->bus = iter->second;
	reader >> tmp->pg >> tmp->qg >> tmp->v >> tmp->qmin >> tmp->qmax;
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addLoad(long long idx)
{
	auto& tmp = loadTab.try_emplace(idx).first->second;
	if (tmp)
		delete tmp;
	tmp = new PsPFMdlLoad();
	tmp->initStatic();
	tmp->idx = idx;
	tmp->initDynamic();
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addLoad(std::string param, char delim)
{
	long long idx(-1);
	long long bus(-1);
	PsRowReader reader;
	param >> reader;
	reader >> idx >> bus;
	PsPFMdlLoad* tmp(static_cast<PsPFMdlLoad*>(addLoad(idx)));
	auto iter(busTab.find(bus));
	if (iter == busTab.end())
	{
		std::string msg("[PSAK::PF::PsPFMdl::addLoad]: Exception->invalid bus ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	tmp->bus = iter->second;
	reader >> tmp->pd >> tmp->qd >> tmp->v;
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addComp(long long idx)
{
	auto& tmp = compTab.try_emplace(idx).first->second;
	if (tmp)
		delete tmp;
	tmp = new PsPFMdlComp();
	tmp->initStatic();
	tmp->idx = idx;
	tmp->initDynamic();
	return tmp;
}

PSAK::PF::PsPFMdlBase* PSAK::PF::PsPFMdl::addComp(std::string param, char delim)
{
	long long idx(-1);
	long long bus(-1);
	PsRowReader reader;
	param >> reader;
	reader >> idx >> bus;
	PsPFMdlComp* tmp(static_cast<PsPFMdlComp*>(addComp(idx)));
	auto iter(busTab.find(bus));
	if (iter == busTab.end())
	{
		std::string msg("[PSAK::PF::PsPFMdl::addComp]: Exception->invalid bus ( " + std::to_string(idx) + " )");
		throw std::exception(msg.c_str());
	}
	tmp->bus = iter->second;
	reader >> tmp->cap;
	return tmp;
}

void PSAK::PF::PsPFMdl::removeBus(long long idx)
{
	auto iter(busTab.find(idx));
	if (iter != busTab.end())
	{
		delete iter->second;
		busTab.erase(iter);
	}
}

void PSAK::PF::PsPFMdl::removeBranch(long long idx)
{
	auto iter(branchTab.find(idx));
	if (iter != branchTab.end())
	{
		delete iter->second;
		branchTab.erase(iter);
	}
}

void PSAK::PF::PsPFMdl::removeGen(long long idx)
{
	auto iter(genTab.find(idx));
	if (iter != genTab.end())
	{
		delete iter->second;
		genTab.erase(iter);
	}
}

void PSAK::PF::PsPFMdl::removeLoad(long long idx)
{
	auto iter(loadTab.find(idx));
	if (iter != loadTab.end())
	{
		delete iter->second;
		loadTab.erase(iter);
	}
}

void PSAK::PF::PsPFMdl::removeComp(long long idx)
{
	auto iter(compTab.find(idx));
	if (iter != compTab.end())
	{
		delete iter->second;
		compTab.erase(iter);
	}
}

void PSAK::PF::PsPFMdl::update()
{
	info->initStatic();
	info->initDynamic();
	for (auto& cp : busTab)
	{
		++info->nb;
		switch (cp.second->type)
		{
		case PsPFBusType::PQ:
			info->npq++;
			break;
		case PsPFBusType::PV:
			info->npv++;
			break;
		case PsPFBusType::VT:
			info->idxvt = cp.second->idx;
			break;
		default:
			break;
		}
	}
	info->nl = branchTab.size();
	info->ng = genTab.size();
	info->nd = loadTab.size();
	info->nc = compTab.size();
}

void PSAK::PF::PsPFMdl::read(std::string path)
{
	std::ifstream input(path);
	if (!input)
	{
		std::string msg("[PSAK::PF::PsPFMdl::read]: Exception->open file failed ( " + path + " )");
		throw std::exception(msg.c_str());
	}
	PsRowReader reader;
	long long nb(0), nl(0), ng(0), nd(0), npv(0), nc(0);
	input >> reader;
	reader >> info->name;
	input >> reader;
	reader >> nb >>  nl >> ng >> nd >> nc;
	for (; nb > 0; nb--)
	{
		input >> reader;
		addBus(reader.getBuffer(), '\t');
	}
	for (; nl > 0; nl--)
	{
		input >> reader;
		addBranch(reader.getBuffer(), '\t');
	}
	for (; ng > 0; ng--)
	{
		input >> reader;
		addGen(reader.getBuffer(), '\t');
	}
	for (; nd > 0; nd--)
	{
		input >> reader;
		addLoad(reader.getBuffer(), '\t');
	}
	for (; nc > 0; nc--)
	{
		input >> reader;
		addComp(reader.getBuffer(), '\t');
	}
	update();
}

void PSAK::PF::PsPFMdl::write(std::string path)
{

}

void PSAK::PF::PsPFMdl::report(std::string path)
{
	std::ofstream output(path);
	if (!output)
	{
		std::string msg("[PSAK::PF::PsPFMdl::report]: Exception->open file failed ( " + path + " )");
		throw std::exception(msg.c_str());
	}
	output.setf(std::ios::showpoint);
	output.precision(8);
	output << "Name:\t" << info->name << std::endl;
	output << "PG:\t" << info->pg << std::endl;
	output << "QG:\t" << info->qg << std::endl;
	output << "PD:\t" << info->pd << std::endl;
	output << "QD:\t" << info->qd << std::endl;
	output << "PL:\t" << info->pl << std::endl;
	output << "QL:\t" << info->ql << std::endl;
	output << "Sparsity:\t" << info->sparsity << std::endl << std::endl;
	output << "#Bus" << std::endl;
	output << "#Idx\tNo\tMod\tTheta\tReal\tImag\tP\tQ\tPG\tQG\tPD\tQD" << std::endl;
	for (auto& cp : busTab)
		output << cp.second->idx << '\t'
		<< cp.second->no << '\t'
		<< cp.second->va << '\t'
		<< cp.second->vt << '\t'
		<< cp.second->ve << '\t'
		<< cp.second->vf << '\t'
		<< cp.second->pi << '\t'
		<< cp.second->qi << '\t'
		<< cp.second->pg << '\t'
		<< cp.second->qg << '\t'
		<< cp.second->pd << '\t'
		<< cp.second->qd << '\t' << std::endl;
	output << std::endl;
	output << "#Branch" << std::endl;
	output << "#Idx\tNo\tFrom\tTo\tIij(Real)\tIij(Imag)\tPij\tQij\tPji\tQji\tPL\tQL" << std::endl;
	for (auto& cp : branchTab)
		output << cp.second->idx << '\t'
		<< cp.second->no << '\t'
		<< cp.second->bus1->idx << '\t'
		<< cp.second->bus2->idx << '\t'
		<< cp.second->ije << '\t'
		<< cp.second->ijf << '\t'
		<< cp.second->pij << '\t'
		<< cp.second->qij << '\t'
		<< cp.second->pji << '\t'
		<< cp.second->qji << '\t'
		<< cp.second->pl << '\t'
		<< cp.second->ql << '\t' << std::endl;
}

PSAK::PF::PsPFMdl::PsPFMdl()
	: info(nullptr)
{
	init();
}

PSAK::PF::PsPFMdl::~PsPFMdl()
{
	clear();
}
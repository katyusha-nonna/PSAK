#include "PSAK.PF.PFC.h"
#include <algorithm>
#include <iostream>

/*PSAK::PF::PsPFSolver*/

// 稀疏矩阵-行压缩格式
template<typename TV>
struct PsSpraseMat
{
	std::vector<TV> val; /*元素*/
	std::vector<long long> col; /*列号*/
	std::vector<long long> off; /*行首地址*/

	void clear()
	{
		val.clear();
		col.clear();
		off.clear();
	}

	void reserve(long long rows, long long nsize)
	{
		val.reserve(nsize);
		col.reserve(nsize);
		off = std::vector<long long>(rows + 1, 0);
	}
};

// 导纳矩阵元素
struct PsYMatEle
{
	double g; /*电导*/ 
	double b; /*电纳*/

	PsYMatEle();
	PsYMatEle(double g, double b);
};

PsYMatEle::PsYMatEle()
	: g(0),
	b(0)
{

}

PsYMatEle::PsYMatEle(double g, double b)
	: g(g),
	b(b)
{

}

// 牛顿法迭代信息
struct PSAK::PF::PsPFSolver::PsPFNtIterInfo
{
	bool quit; /*退出标志*/
	long long iter; /*当前迭代次数*/
	long long iterMax; /*最大迭代次数*/
	double maxErr; /*当前最大修正量*/
	double maxNo; /*当前最大修正量对应编号*/
	double gap; /*收敛判据*/
	double zero; /*零值判据*/
	std::vector<PsPFBusType> btype; /*节点类型*/
	std::vector<double> va; /*节点电压幅值(稠密)*/
	std::vector<double> vt; /*节点电压相角(稠密)*/
	std::vector<double> ve; /*节点电压实部(稠密)*/
	std::vector<double> vf; /*节点电压虚部(稠密)*/
	std::vector<double> pi; /*节点总有功注入功率(稠密)*/
	std::vector<double> qi; /*节点总无功注入功率(稠密)*/
	std::vector<double> pg; /*节点总有功发电功率(稠密)*/
	std::vector<double> qg; /*节点总无功发电功率(稠密)*/
	std::vector<double> pd; /*节点总有功负荷功率(稠密)*/
	std::vector<double> qd; /*节点总无功负荷功率(稠密)*/
	std::vector<double> dv; /*节点电压修正量(稠密)*/
	std::vector<double> dp; /*节点功率不平衡量(稠密)*/
	std::vector<double> jac1; /*雅可比矩阵2n-1行(稠密)*/
	std::vector<double> jac2; /*雅可比矩阵2n行(稠密)*/
	std::vector<PsYMatEle> diag; /*导纳矩阵对角向量(稠密)*/
	PsSpraseMat<PsYMatEle> up; /*导纳矩阵上三角阵(为方便迭代实际存储的是全矩阵)(稀疏)*/
	PsSpraseMat<double> jac; /*雅可比矩阵1到2n-2行(不含对角)(稀疏)*/

	void init(PsPFMdl* mdl, const PsPFConfig& cfg); /*初始化*/
	void clear(); /*清空*/
};

void PSAK::PF::PsPFSolver::PsPFNtIterInfo::init(PsPFMdl* mdl, const PsPFConfig& cfg)
{
	quit = false;
	iter = 0;
	iterMax = 1000; /*TODO：暂时设置1000*/
	maxErr = -INFINITY;
	maxNo = -1;
	gap = 1e-5; /*TODO：暂时设置为1e-6*/
	zero = 1e-8; /*TODO：暂时设置为1e-8*/
	btype = std::vector<PsPFBusType>(mdl->info->nb, PsPFBusType::PQ);
	va = std::vector<double>(mdl->info->nb, 1);
	vt = std::vector<double>(mdl->info->nb, 0);
	ve = std::vector<double>(mdl->info->nb, 1);
	vf = std::vector<double>(mdl->info->nb, 0);
	pi = std::vector<double>(mdl->info->nb, 0);
	qi = std::vector<double>(mdl->info->nb, 0);
	pg = std::vector<double>(mdl->info->nb, 0);
	qg = std::vector<double>(mdl->info->nb, 0);
	pd = std::vector<double>(mdl->info->nb, 0);
	qd = std::vector<double>(mdl->info->nb, 0);
	dv = std::vector<double>(mdl->info->nb * 2, 0);
	dp = std::vector<double>(mdl->info->nb * 2, 0);
	jac1 = std::vector<double>(mdl->info->nb * 2, 0);
	jac2 = std::vector<double>(mdl->info->nb * 2, 0);
	diag = std::vector<PsYMatEle>(mdl->info->nb);
	up.reserve(mdl->info->nb, mdl->info->nl);
	//jac.reserve(mdl->info->nb * 2, mdl->info->nl * 4);
	// 初始化节点信息
	for (auto& cp : mdl->busTab)
	{
		btype[cp.second->no] = cp.second->type;
		va[cp.second->no] = cp.second->va;
		vt[cp.second->no] = cp.second->vt;
		ve[cp.second->no] = cp.second->ve;
		vf[cp.second->no] = cp.second->vf;
		pi[cp.second->no] = cp.second->pi;
		qi[cp.second->no] = cp.second->qi;
		pg[cp.second->no] = cp.second->pg;
		qg[cp.second->no] = cp.second->qg;
		pd[cp.second->no] = cp.second->pd;
		qd[cp.second->no] = cp.second->qd;
	}
	// 初始化导纳矩阵
	std::vector<std::tuple<long long , long long, long long, bool> > reorder;
	reorder.reserve(mdl->info->nl);
	for (auto& cp : mdl->branchTab)
	{
		reorder.emplace_back(std::make_tuple(cp.second->bus1->no, cp.second->bus2->no, cp.second->idx, true));
		reorder.emplace_back(std::make_tuple(cp.second->bus2->no, cp.second->bus1->no, cp.second->idx, false));
	}
	// 首先对支路按照首端节点、末端节点编号升序排序
	std::sort(reorder.begin(), reorder.end(),
		[](auto& p1, auto& p2) {return std::get<0>(p1) < std::get<0>(p2) ? true : (std::get<0>(p1) == std::get<0>(p2) ? std::get<1>(p1) < std::get<1>(p2) : false); });
	// 按顺序压入导纳矩阵非零元素(多回线对应的非零元素保留)
	for (auto& order : reorder)
	{
		auto no1(std::get<0>(order)), no2(std::get<1>(order));
		auto& cp(mdl->branchTab.at(std::get<2>(order)));
		double z(cp->r * cp->r + cp->x * cp->x);
		double gij(cp->r / z), bij(-cp->x / z);
		double gii(0), bii(0), gjj(0), bjj(0);
		if (cp->type == PsPFBranchType::Line)
		{
			gii = gij;
			bii = bij + cp->bk;
			gjj = gij;
			bjj = bij + cp->bk;
		}
		else if (cp->type == PsPFBranchType::Tran)
		{
			if (cp->mode)
			{
				gii = gij / cp->bk / cp->bk;
				bii = bij / cp->bk / cp->bk;
				gjj = gij;
				bjj = bij;
			}
			else
			{
				gii = gij;
				bii = bij;
				gjj = gij / cp->bk / cp->bk;
				bjj = bij / cp->bk / cp->bk;
			}
			gij /= cp->bk;
			bij /= cp->bk;
		}
		if (std::get<3>(order))
		{
			diag[no1].g += gii;
			diag[no1].b += bii;
			diag[no2].g += gjj;
			diag[no2].b += bjj;
		}
		up.col.emplace_back(no2);
		up.val.emplace_back(PsYMatEle(-gij, -bij));
		++(up.off[no1]);
	}
	// 更新行首地址表
	std::swap(up.off[0], up.off[mdl->info->nb]);
	for (long long i = 1; i < mdl->info->nb; i++)
	{
		auto tmp(up.off[i]);
		up.off[i] = up.off[i - 1] + up.off[mdl->info->nb];
		up.off[mdl->info->nb] = tmp;
	}
	up.off[mdl->info->nb] += mdl->info->nb > 0 ? up.off[mdl->info->nb - 1] : 0;
	// 补偿装置
	for (auto& cp : mdl->compTab)
		diag[cp.second->bus->no].b += cp.second->cap;
}

void PSAK::PF::PsPFSolver::PsPFNtIterInfo::clear()
{

}

bool PSAK::PF::PsPFSolver::newton(const PsPFConfig& config)
{
	bool successful = false; /*计算成功标志*/
	PsPFNtIterInfo info; /*基本迭代信息*/
	info.init(mdl_, config);

	// lambda，用途：形成雅可比矩阵的2n-1和2n行
	auto formJac = [&](long long row) {
		long long i(row); /*行号*/
		long long j(-1); /*列号*/
		double g(0), b(0); /*电导/电纳*/
		double ai(0), bi(0); /*注入电流实部/虚部*/
		// 计算非对角元素
		for (long long k = info.up.off[i]; k < info.up.off[i + 1]; k++)
		{
			j = info.up.col[k];
			g = info.up.val[k].g;
			b = info.up.val[k].b;
			if (info.btype[i] == PsPFBusType::PQ)
			{
				info.jac1[2 * j] = b * info.ve[i] - g * info.vf[i]; /*Jij*/
				info.jac1[2 * j + 1] = g * info.ve[i] + b * info.vf[i]; /*Lij*/
				info.jac2[2 * j] = -g * info.ve[i] - b * info.vf[i]; /*Hij*/
				info.jac2[2 * j + 1] = b * info.ve[i] - g * info.vf[i]; /*Nij*/
			}
			else
			{
				info.jac2[2 * j] = -g * info.ve[i] - b * info.vf[i]; /*Hij*/
				info.jac2[2 * j + 1] = b * info.ve[i] - g * info.vf[i]; /*Nij*/
			}
			ai += g * info.ve[j] - b * info.vf[j];
			bi += b * info.ve[j] + g * info.vf[j];
		}
		// 计算对角元素
		j = i;
		g = info.diag[j].g;
		b = info.diag[j].b;
		ai += g * info.ve[j] - b * info.vf[j];
		bi += b * info.ve[j] + g * info.vf[j];
		if (info.btype[i] == PsPFBusType::PQ)
		{
			info.jac1[2 * j] = bi + b * info.ve[i] - g * info.vf[i]; /*Jii*/
			info.jac1[2 * j + 1] = -ai + g * info.ve[i] + b * info.vf[i]; /*Lii*/
			info.jac2[2 * j] = -ai - g * info.ve[i] - b * info.vf[i]; /*Hii*/
			info.jac2[2 * j + 1] = -bi + b * info.ve[i] - g * info.vf[i]; /*Nii*/
		}
		else
		{
			info.jac1[2 * j] = -2 * info.ve[i]; /*Rii*/
			info.jac1[2 * j + 1] = -2 * info.vf[i]; /*Sii*/
			info.jac2[2 * j] = -ai - g * info.ve[i] - b * info.vf[i]; /*Hii*/
			info.jac2[2 * j + 1] = -bi + b * info.ve[i] - g * info.vf[i]; /*Nii*/
		}
		// 计算电压/功率修正量
		if (info.btype[i] == PsPFBusType::PQ)
			info.dp[2 * i] = info.qi[i] + bi * info.ve[i] - ai * info.vf[i];
		else
			info.dp[2 * i] = info.va[i] * info.va[i] - info.ve[i] * info.ve[i] - info.vf[i] * info.vf[i];
		info.dp[2 * i + 1] = info.pi[i] - ai * info.ve[i] - bi * info.vf[i];
		// 更新误差统计
		if (std::abs(info.dp[2 * i]) > info.maxErr)
		{
			info.maxErr = std::abs(info.dp[2 * i]);
			info.maxNo = i;
		}
		if (std::abs(info.dp[2 * i + 1]) > info.maxErr)
		{
			info.maxErr = std::abs(info.dp[2 * i + 1]);
			info.maxNo = i;
		}
	};
	// lambda，用途：消去雅可比矩阵的最后一行
	auto eliminateJac = [&](long long row) {
		long long i(row); /*行号*/
		long long nnz(0); /*非零元素计数器*/
		// 消去第2i-1行(消去1~2i-2)
		for (long long ii = 0; ii < 2 * i; ii++)
		{
			if (std::abs(info.jac1[ii]) > info.zero)
			{
				for (long long k = info.jac.off[ii]; k < info.jac.off[ii + 1]; k++)
					info.jac1[info.jac.col[k]] -= info.jac1[ii] * info.jac.val[k];
				info.dp[2 * i] -= info.jac1[ii] * info.dp[ii];
			}
		}
		// 用主元归一化第2i-1行(归一化2i~2n)
		for (long long j = 2 * i + 1; j < mdl_->info->nb * 2; j++)
		{
			if (std::abs(info.jac1[j]) > info.zero)
			{
				info.jac1[j] /= info.jac1[2 * i];       
				info.jac.val.emplace_back(info.jac1[j]);
				info.jac.col.emplace_back(j);
				++nnz;
			}
		}
		info.dp[2 * i] /= info.jac1[2 * i];
		info.jac.off[2 * i + 1] = info.jac.off[2 * i] + nnz;
		nnz = 0;
		// 消去第2i行(消去1~2i-1)
		for (long long ii = 0; ii < 2 * i + 1; ii++)
		{
			if (std::abs(info.jac2[ii]) > info.zero)
			{
				for (long long k = info.jac.off[ii]; k < info.jac.off[ii + 1]; k++)
					info.jac2[info.jac.col[k]] -= info.jac2[ii] * info.jac.val[k];
				info.dp[2 * i + 1] -= info.jac2[ii] * info.dp[ii];
			}
		}
		// 用主元归一化第2i行(归一化2i+1~2n)
		for (long long j = 2 * i + 2; j < mdl_->info->nb * 2; j++)
		{
			if (std::abs(info.jac2[j]) > info.zero)
			{
				info.jac2[j] /= info.jac2[2 * i + 1];
				info.jac.val.emplace_back(info.jac2[j]);
				info.jac.col.emplace_back(j);
				++nnz;
			}
		}
		info.dp[2 * i + 1] /= info.jac2[2 * i + 1];
		info.jac.off[2 * i + 2] = info.jac.off[2 * i + 1] + nnz;
	};
	// lambda，用途：前推求解修正方程组
	auto backwardJac = [&]() {
		// 回代
		for (long long i = mdl_->info->nb * 2 - 1; i > -1; i--)
		{
			if ((i == mdl_->info->nvt * 2) || (i == mdl_->info->nvt * 2 + 1))
				continue;
			double tmp(0);
			for (long long k = info.jac.off[i]; k < info.jac.off[i + 1]; k++)
				tmp += info.jac.val[k] * info.dv[info.jac.col[k]];
			info.dv[i] = info.dp[i] - tmp;
		}
		// 更新电压
		for (long long i = 0; i < mdl_->info->nb; i++)
		{
			if (i == mdl_->info->nvt)
				continue;
			info.ve[i] -= info.dv[2 * i];
			info.vf[i] -= info.dv[2 * i + 1];
		}
	};
	// lambda，用途：计算节点注入电流
	auto getInj = [&](long long no) {
		long long j(-1);
		double g(0), b(0);
		double ai(0), bi(0);
		for (long long k = info.up.off[no]; k < info.up.off[no + 1]; k++)
		{
			j = info.up.col[k];
			g = info.up.val[k].g;
			b = info.up.val[k].b;
			ai += g * info.ve[j] - b * info.vf[j];
			bi += b * info.ve[j] + g * info.vf[j];
		}
		j = no;
		g = info.diag[j].g;
		b = info.diag[j].b;
		ai += g * info.ve[j] - b * info.vf[j];
		bi += b * info.ve[j] + g * info.vf[j];
		return std::make_pair(ai, bi);
	};

	// 初始化统计信息
	mdl_->info->pg = 0;
	mdl_->info->qg = 0;
	mdl_->info->pd = 0;
	mdl_->info->qd = 0;
	mdl_->info->pl = 0;
	mdl_->info->ql = 0;
	mdl_->info->sparsity = 0;
	// 执行牛顿法迭代求解
	while (!info.quit)
	{
		// S1：初始化
		info.jac.clear();
		info.jac.reserve(mdl_->info->nb * 2, mdl_->info->nl * 4);
		for (long long i = 0; i < mdl_->info->nb * 2; i++)
		{
			info.dv[i] = 0;
			info.dp[i] = 0;
		}
		info.maxErr = -INFINITY;
		info.maxNo = -1;
		// S2：边形成边消去雅可比矩阵
		for (long long row = 0; row < mdl_->info->nb; row++)
		{
			// S2.1：跳过平衡节点
			if (info.btype[row] == PsPFBusType::VT)
			{
				info.jac.off[2 * row + 1] = info.jac.off[2 * row];
				info.jac.off[2 * row + 2] = info.jac.off[2 * row + 1];
				continue;
			}
			// S2.2：初始化雅可比矩阵2n-1和2n行
			for (long long i = 0; i < mdl_->info->nb * 2; i++)
			{
				info.jac1[i] = 0;
				info.jac2[i] = 0;
			}
			// S2.3：形成雅可比矩阵的2n-1和2n行
			formJac(row);
			// S2.4：消去雅可比矩阵的2n-1和2n行
			eliminateJac(row);
		}
		// S3：前推求解
		backwardJac();
		// 更新迭代信息
		if (info.maxErr < info.gap)
			info.quit = true;
		++info.iter;
	}
	// 更新节点电压和功率结果
	for (long long i = 0; i < mdl_->info->nb; i++)
	{
		info.va[i] = std::sqrt(info.ve[i] * info.ve[i] + info.vf[i] * info.vf[i]);
		info.vt[i] = std::atan(info.vf[i] / info.ve[i]);
	}
	for (auto& cp : mdl_->busTab)
	{
		auto ptr(cp.second);
		auto no(ptr->no);
		ptr->va = info.va[no];
		ptr->vt = info.vt[no];
		ptr->ve = info.ve[no];
		ptr->vf = info.vf[no];
		if (info.btype[no] == PsPFBusType::PV)
		{
			auto inj(getInj(no));
			ptr->qi = inj.first * info.vf[no] - inj.second * info.ve[no];
		}
		else if (info.btype[no] == PsPFBusType::VT)
		{
			auto inj(getInj(no));
			ptr->pi = inj.first * info.ve[no] + inj.second * info.vf[no];
			ptr->qi = inj.first * info.vf[no] - inj.second * info.ve[no];
		}
		ptr->pg = ptr->pi + ptr->pd;
		ptr->qg = ptr->qi + ptr->qd;
		mdl_->info->pg += ptr->pg;
		mdl_->info->qg += ptr->qg;
		mdl_->info->pd += ptr->pd;
		mdl_->info->qd += ptr->qd;
	}
	// 计算支路潮流
	for (auto& cp : mdl_->branchTab)
	{
		auto ptr(cp.second);
		auto no1(ptr->bus1->no), no2(ptr->bus2->no);
		bool nstd(ptr->type == PsPFBranchType::Tran);
		double bk1(nstd&& ptr->mode ? ptr->bk : 1), bk2(nstd && !ptr->mode ? ptr->bk : 1), bk0(ptr->bk);
		double z(ptr->r* ptr->r + ptr->x * ptr->x);
		double gij(ptr->r / z), bij(-ptr->x / z);
		double vei(info.ve[no1] / bk1), vej(info.ve[no2] / bk2), vfi(info.vf[no1] / bk1), vfj(info.vf[no2] / bk2);
		double de(vei - vej);
		double df(vfi - vfj);
		double dqi(nstd ? 0 : bk0 * (info.ve[no1] * info.ve[no1] + info.vf[no1] * info.vf[no1]));
		double dqj(nstd ? 0 : bk0 * (info.ve[no2] * info.ve[no2] + info.vf[no2] * info.vf[no2]));
		ptr->ije = gij * de - bij * df;
		ptr->ijf = gij * df + bij * de;
		ptr->pij = vei * ptr->ije + vfi * ptr->ijf;
		ptr->qij = vfi * ptr->ije - vei * ptr->ijf - dqi;
		ptr->pji = -vej * ptr->ije - vfj * ptr->ijf;
		ptr->qji = -vfj * ptr->ije + vej * ptr->ijf - dqj;
		ptr->pl = ptr->pij + ptr->pji;
		ptr->ql = ptr->qij + ptr->qji;
		mdl_->info->pl += ptr->pl;
		mdl_->info->ql += ptr->ql;
	}
	// 统计稀疏程度
	mdl_->info->sparsity = (double)(info.up.val.size() + info.diag.size()) / (double)(mdl_->info->nb * mdl_->info->nb);

	successful = true;
	info.clear();
	return successful;
}
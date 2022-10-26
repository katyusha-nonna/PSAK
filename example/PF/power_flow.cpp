#include "PSAK.h"

int main()
{
    PSAK::PF::PsPFMdl mdl;
    mdl.read("PF.IEEE14.in.txt");
    PSAK::PF::PsPFNodeOpt opt;
    opt.extract(&mdl);
    opt.random();
    opt.tinney2();
    PSAK::PF::PsPFSolver sol;
    PSAK::PF::PsPFConfig cfg;
    sol.extract(&mdl);
    sol.init(cfg);
    sol.newton(cfg);
    mdl.report("PF.IEEE14.out.txt");
    return 0;
}
#ifndef IEVMDLCREATER_HPP_
#define IEVMDLCREATER_HPP_

#include "bcs/CLoadMan.hpp"
#include "game/msw/CEvDemoRes.hpp"

namespace msw {
    class CEventModel;

    // (name guessed; the class has no named function) What msw::CEvDemoRes
    // makes its models with: it keeps one of these per kind of model and
    // calls it through the vtable (CEvDemoRes::_CreateRes in the DOL calls
    // slots 0x0C, 0x08 and 0x14 in that order). The order of the functions
    // is the order of the slots in __vt__Q23msw21CEvMdlCreaterTmMdlChr;
    // return types are read from the bodies in TourF. Implemented by the
    // CEvMdlCreaterTm* classes (TourF) and CEvMdlCreaterGm* (BossF).
    class IEvMdlCreater {
        public:
            // Prepares the files LoadRes named, once they are loaded.
            // Declared without "= 0" and never defined: with all four pure,
            // a `new` of a derived class emits a weak vtable of this class
            // that BossF's CEventDemoBoss.cpp does not have (see
            // notes/toolchain.md, HockeyF CStgGmk_WaterWorks_Hockey).
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            // Decides which msw::CLoadData entries the model needs and asks
            // for the ones not loaded yet, starting at entry `nFile`.
            // Returns the first entry still free.
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile) = 0;
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo) = 0;
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo) = 0;
    };
}

#endif /* !IEVMDLCREATER_HPP_ */

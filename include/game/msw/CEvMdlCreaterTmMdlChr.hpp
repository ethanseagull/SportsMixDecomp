#ifndef CEVMDLCREATERTMMDLCHR_HPP_
#define CEVMDLCREATERTMMDLCHR_HPP_

#include "bcs/CLoadMan.hpp"
#include "game/msw/CEvDemoRes.hpp"

namespace msw {
    class CEventModel;

    // (name guessed; the class has no named function) What msw::CEvDemoRes
    // makes its models with: it keeps one of these per kind of model and
    // calls it through the vtable (CEvDemoRes::_CreateRes in the DOL calls
    // slots 0x0C, 0x08 and 0x14 in that order). The order of the functions
    // is the order of the slots in __vt__Q23msw21CEvMdlCreaterTmMdlChr;
    // return types are read from the bodies in TourF.
    class IEvMdlCreater {
        public:
            // Prepares the files LoadRes named, once they are loaded.
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo) = 0;
            // Decides which msw::CLoadData entries the model needs and asks
            // for the ones not loaded yet, starting at entry `nFile`.
            // Returns the first entry still free.
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile) = 0;
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo) = 0;
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo) = 0;
    };

    // Makes the player characters of the tournament map (module TourF,
    // game/tour/TournamentData.cpp). No data members.
    class CEvMdlCreaterTmMdlChr : public IEvMdlCreater {
        public:
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile);
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo);
    };

    // Makes the objects of the event demos on the tournament map (module
    // TourF, game/tour/CTournamentChip.cpp): models from the demo's own
    // archive. No data members.
    class CEvMdlCreaterTmMdlObj : public IEvMdlCreater {
        public:
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile);
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo);
    };

    // Makes the non-player characters of the event demos on the tournament
    // map (module TourF, game/tour/CTournamentChip.cpp). No data members.
    class CEvMdlCreaterTmMdlNpc : public IEvMdlCreater {
        public:
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile);
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo);
    };
}

#endif /* !CEVMDLCREATERTMMDLCHR_HPP_ */

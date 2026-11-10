#ifndef CEVMDLCREATERTMMDLCHR_HPP_
#define CEVMDLCREATERTMMDLCHR_HPP_

#include "game/msw/IEvMdlCreater.hpp"

namespace msw {

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

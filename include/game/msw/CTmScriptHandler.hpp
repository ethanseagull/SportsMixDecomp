#ifndef MSW_CTMSCRIPTHANDLER_HPP_
#define MSW_CTMSCRIPTHANDLER_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // INCOMPLETE: only the virtuals and the word msw::CTmScript reads.
    //
    // What the tournament scripts (msw::CTmScript, TourF) act on: the map
    // task gives itself to CTmScript::SetupInitial and receives the
    // scripts' requests through these virtuals. Their bodies in this class
    // (TourF 0x15448 on) are empty or return a fixed value; the vtable
    // (TourF .data 0x930, 30 words) ends in seven null words not mapped
    // here. Parameter names guessed.
    class CTmScriptHandler : public bcs::CTask {
        public:
            virtual ~CTmScriptHandler();
            virtual void Main();
            virtual void Disp();

            // Puts map object nObj at vPos, turned by fRotY radians.
            virtual void MapObjSet(long nObj, const bcs::FVEC3 &vPos, float fRotY, const char *pName, int nFlag);
            // Route nRoute was shown or hidden; bChanged says whether its
            // visibility bit changed.
            virtual void RouteWrite(long nRoute, int bChanged);
            virtual void ChipWrite(long nPoint, unsigned long nUnk1, unsigned long nUnk2, long nUnk3, int nUnk4);
            virtual void SendRouteEvent(long nRoute, long nEvent, float fStart, unsigned long nUnk, float fEnd);
            // The script's route list (CTmScript 0x68), nNum entries.
            virtual void SetRoutePoints(long nPoint, const unsigned char *pRoute, long nNum);
            virtual void RouteSelectSetup(unsigned char nPoint, const unsigned char *pRoute, long nUnk1, long nUnk2,
                                          long nUnk3);
            // Not zero once the player has chosen; rSelect receives the
            // choice.
            virtual int RouteSelect(long &rSelect);
            virtual void TeamCreate(long nTeam, long nUnk, float fUnk1, float fUnk2);
            virtual void TeamErase(long nTeam, long nUnk);
            virtual void TeamPos(long nTeam, long nPoint, long nUnk);
            virtual void MoveTarget(long nTeam, long nPoint, const unsigned char *pRoute);
            virtual void MoveResult(long nTeam, int nResult, float fUnk);
            virtual void PointCamera(long nPoint, long nCamera, float fUnk1, float fUnk2);
            virtual void FadeIn(float fSpeed, int nUnk);
            virtual int IsFadeInReq() const;
            virtual void SetMatchMake(long nMatch, long nTeam0, long nTeam1);
            virtual void VsMatchPause(long nMatch, long nTeam0, long nTeam1);

            // (name guessed) While it is not zero the map camera is busy,
            // and CTmScript's PointCamera waits.
            long IsCameraBusy() const { return m_nCameraBusy; }

        protected:
            long m_nCameraBusy; // 0x34
    };
}

#endif /* !MSW_CTMSCRIPTHANDLER_HPP_ */

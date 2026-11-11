#ifndef MSW_CCHRMANDODGE_HPP_
#define MSW_CCHRMANDODGE_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    // The character manager of the dodgeball game (DodgeF), size 0x8C: no
    // members of its own. It makes the eight characters, their drivers and
    // markers, and picks the characters to pass to or to switch to.
    //
    // The virtual functions follow __vt__Q23msw12CChrManDodge. The six after
    // Is1on1CameraDirection_ (0xE8 to 0xFC) are new in this class: the DOL's
    // CChrMan vtable ends before them.
    class CChrManDodge : public CChrMan {
        public:
            // (guessed inline: CreateInstance builds the object in place)
            CChrManDodge(bcs::CTask *pParent) : CChrMan(pParent, 8) {}
            virtual ~CChrManDodge();

            // Makes the instance under pParent and registers it.
            static CChrMan *CreateInstance(bcs::CTask *pParent);
            // ReconnectDefaultDriver_ and CanbeExitLoop_ on the instance, if
            // there is one (CanbeExitLoop gives zero without one).
            static void ReconnectDefaultDriver();
            static int CanbeExitLoop();
            // The first character of team nTeam that takes part and is an
            // extra outfielder (CGame::IsCharEx_DodgeOutfielder), or null.
            static CChrCore *GetExclusiveOuterChrCore(long nTeam);

            virtual void DispFunc();
            virtual void Create_();
            virtual void AllReset_();
            virtual void InitLeader_(int bConnect);
            virtual int CanbeTurnOver_();
            virtual CChrCore *Util_SearchChgLeader_(long nBall, long nTeam, CChrCore *pExclude);
            virtual CChrCore *Util_SearchNearOwnGoal_(long nTeam, int, CChrCore *pExclude);
            virtual CChrCore *Util_SearchPassTo_(long nChr, float fRotY);
            virtual CChrCore *Util_SearchPassmeetTo_(long nChr, float fRotY, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_SearchArtsPassTo_(long nChr);
            virtual int Util_GetSubChrTargetPos_(long nChr, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_GetSubChrMarkChrCore_(CChrCore *);
            virtual void Util_SetChgLeaderOrder_(long);
            virtual long Util_GetNextChgLeader_(long);

            virtual void CreateObj_();                  // 0xE8
            virtual void CreateDriver_();               // 0xEC
            virtual void CreateMarker_();               // 0xF0
            virtual void CreateMarkerOne_(long nChr);   // 0xF4
            virtual int CanbeExitLoop_() const;         // 0xF8
            virtual void ReconnectDefaultDriver_();     // 0xFC
    };

    // The manager of the party game "dodge or die" (DodgeF), size 0x8C like
    // its base. Its destructor is the implicit one.
    class CChrManDodgeDodgeOrDie : public CChrManDodge {
        public:
            // (guessed inline, as CChrManDodge's)
            CChrManDodgeDodgeOrDie(bcs::CTask *pParent) : CChrManDodge(pParent) {}

            static CChrMan *CreateInstance(bcs::CTask *pParent);

            virtual void InitLeader_(int bConnect);
            virtual void CreateObj_();
            virtual void CreateDriver_();
            virtual void CreateMarkerOne_(long nChr);
            virtual void ReconnectDefaultDriver_();
    };
}

#endif /* !MSW_CCHRMANDODGE_HPP_ */

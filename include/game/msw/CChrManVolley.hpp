#ifndef MSW_CCHRMANVOLLEY_HPP_
#define MSW_CCHRMANVOLLEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    // The character manager of the volleyball game (VolleyF), size 0xA4. It
    // makes the characters (six; eight in the rhythm receive party game,
    // CHR_MAN_VOLLEY::CChrManVolleyRhythmReceive), their drivers and
    // markers, the ghost of the haunted stage, and shows the in-game guides.
    //
    // The virtual functions follow __vt__Q23msw13CChrManVolley. The five with
    // a body here are emitted as weak functions after the file's code, so
    // they are inline; the last four (CheckGenChrCoreNum_ to
    // GameGuideDispCheck_) are new in this class.
    class CChrManVolley : public CChrMan {
        public:
            // (guessed inline: CreateInstance builds the object in place)
            CChrManVolley(bcs::CTask *pParent, long nChrMax = 6) : CChrMan(pParent, nChrMax)
            {
                for (long i = 0; i < 6; i++) {
                    m_nDrvId[i] = 0;
                }
            }
            virtual ~CChrManVolley();

            // Makes the instance under pParent (the rhythm receive manager in
            // the party game) and registers it.
            static CChrMan *CreateInstance(bcs::CTask *pParent);

            virtual void MainFunc_();
            virtual void DispFunc();
            virtual void Create_();
            virtual void AllReset_();
            virtual void InitLeader_(int bConnect);
            virtual int CanbeTurnOver_();
            virtual void StartArtsEvent_(long nChr);
            virtual CChrCore *Util_SearchChgLeader_(long, long, CChrCore *) { return 0; }
            virtual CChrCore *Util_SearchNearOwnGoal_(long, int, CChrCore *) { return 0; }
            virtual CChrCore *Util_SearchPassTo_(long nChr, float fRate);
            virtual CChrCore *Util_SearchPassmeetTo_(long nChr, float fRate, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_SearchArtsPassTo_(long nChr);
            virtual int Util_GetSubChrTargetPos_(long nChr, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_GetSubChrMarkChrCore_(CChrCore *) { return 0; }
            virtual void Util_SetChgLeaderOrder_(long) {}
            virtual long Util_GetNextChgLeader_(long) { return 0; }
            // How many characters the game has.
            virtual long CheckGenChrCoreNum_();
            virtual void CreateDriver_();
            virtual void CreateMarker_();
            // Shows or ends the guide telop and line message of the first
            // pad-driven character that asks for one (every frame, from
            // MainFunc_).
            virtual void GameGuideDispCheck_();

        protected:
            // Makes the character objects (not virtual: Create_ calls it
            // directly).
            void CreateObj_();

            // 0x8C (name guessed): the driver kind of each character, from
            // SetDrvIdAssign_. 1 starts on the pad driver; 3 and the teams
            // of control type 1 use the computer driver.
            long m_nDrvId[6];
    };

    namespace CHR_MAN_VOLLEY {

        // The character manager of the rhythm receive party game: eight
        // characters on one team, each on a pad driver or the rainbow road
        // computer driver. Adds no members.
        class CChrManVolleyRhythmReceive : public CChrManVolley {
            public:
                // (guessed inline, as CChrManVolley's)
                CChrManVolleyRhythmReceive(bcs::CTask *pParent) : CChrManVolley(pParent, 8) {}
                virtual ~CChrManVolleyRhythmReceive();

                virtual void MainFunc_() {}
                virtual void DispFunc();
                virtual void Create_();
                virtual void SetDrvIdAssign_(long nNum, long *pId);
                virtual void AllReset_();
                virtual void InitLeader_(int bConnect);
                virtual int CanbeTurnOver_();
                virtual long CheckGenChrCoreNum_();
                virtual void CreateDriver_();
                virtual void CreateMarker_();
        };
    }
}

#endif /* !MSW_CCHRMANVOLLEY_HPP_ */

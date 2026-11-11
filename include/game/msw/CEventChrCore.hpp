#ifndef MSW_CEVENTCHRCORE_HPP_
#define MSW_CEVENTCHRCORE_HPP_

#include "game/msw/CEventModel.hpp"

namespace msw {

    class CChrCore;

    // A character of the game taking part in an event scene (DOL class,
    // 0xB4 bytes): the event model hands its position, rotation, display
    // and motions on to the playing character (msw::CChrCore) instead of a
    // model of its own. The motions play in animation slot 3 of the
    // character. Vtable __vt__Q23msw13CEventChrCore at 0x8046F318.
    class CEventChrCore : public CEventModel {
        public:
            // (parameter names guessed) nType is the character
            // (CGame::GetCharType), nPlayer its player, pMotTable the
            // scene's motion table.
            CEventChrCore(long nType, long nPlayer, const short *pMotTable);
            virtual ~CEventChrCore();
            virtual void _SetPos(const bcs::FVEC3 &rPos);
            virtual void _SetRot(const bcs::FVEC3 &rRot);
            virtual void _SetScale(const bcs::FVEC3 &rScale);
            virtual void SetDisp(int bDisp);
            virtual void UpdateFrame(float fFrame);
            virtual void SetLightSelector(long nSel);
            virtual void ResetLightSelector();
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual const bcs::FMTX34 *GetNodeWorldMtxPtr(unsigned long nNode) const;
            virtual long GetNodeId(const char *pName) const;
            virtual void ChangeAnimFile(nw4r::g3d::ResFile file);
            virtual void SetAnimFrame(float fFrame, int);
            virtual void SetAnimSpeed(float fSpeed);
            virtual int IsAnimResLoopType(long nMot) const;
            virtual float AnimResFrame(long nMot) const;
            virtual float AnimResTrgFrame(long nMot, unsigned long nTrg, float) const;
            virtual float AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq);
            virtual void _UpdateFlags();
            virtual void Create();
            virtual void Destroy();
            virtual int IsObj() const;

            // (fn_80117770 to fn_801177A8 unnamed in the DOL; names guessed)
            // Used by msw::CEventDemo's scripts. A number of -1 plays
            // nothing.
            void PlayVoice(long nVoice);
            void PlaySe(long nSe);
            void SetSeVolume(float fRate);
            void SetVoiceVolume(float fRate, float f);
            long GetTeamNo() const;

        private:
            CChrCore *m_pChr; // 0xB0 (name guessed) the character
    };
}

#endif /* !MSW_CEVENTCHRCORE_HPP_ */

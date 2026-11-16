#ifndef CTROPHYOBJ_HPP_
#define CTROPHYOBJ_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "game/msw/CEventMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CScnGroup;
}

namespace msw {

    class CTrophyInObj;

    // A cup model of the tournament. A DOL class, 0x1D0 bytes (the size
    // CreateTrophy allocates), vtable lbl_80480958, built on
    // msw::CEventMdlObj (its constructor and destructor are called; no
    // virtual of its own besides the destructor). msw::CTournamentTrophy
    // in TourF calls CMdlObj's functions on it and hands it to
    // bcs::CScnObj::AppendDelayDeleteObject. Which members are static is
    // read from the callers (the ones taking the model as their first
    // argument are static); return types are guessed.
    class CTrophyObj : public CEventMdlObj {
        public:
            CTrophyObj() : m_hIn(0) {}
            virtual ~CTrophyObj();

            // Makes the cup of cup `nCup` from the model in rFile; bLarge
            // picks the model "trophyL" over "trophy". (meaning of the last
            // argument guessed)
            static CTrophyObj *CreateTrophy(nw4r::g3d::ResFile file, long nCup, long nGameMode, int bLarge);
            // Adds the model inside the cup and returns it (null for an
            // unknown game mode).
            static CTrophyInObj *CreateTrophyIn(CTrophyObj *pTrophy, nw4r::g3d::ResFile file, long nCup, long nGameMode);
            static void SetCrystalColor(bcs::CMdlObj *pMdl, nw4r::g3d::ResFile file, long nGameMode, long nCup);
            // pName names the texture; nArc is the msw::CLoadData entry the
            // cup was loaded from.
            static void SetCrystalEnvTex(bcs::CMdlObj *pMdl, const char *pName, long nArc, long nStageGameMode, long nCup);

            void Destroy();
            void SetDisp(int bDisp);
            void _SetDrawPriority(long nPriority);
            void _SetLightSelector(long nSelector);
            void _ResetLightSelector();
            void _SetBaseColor(bcs::BCS_Color color);
            void _SetColorScale(bcs::BCS_Color color);
            // Puts the cup into a group, or into draw layer nLayer.
            void _PushBack(bcs::CScnGroup *pGroup);
            void _PushBack(long nLayer);

            // (MenuF msw::SelectMenu::CCupChr; argument meanings and return
            // types guessed) Makes the cup of cup nCup for game mode
            // nGameMode with its inner model, light selector nLightSel (none
            // when negative), in draw layer nLayer, with the environment
            // texture pEnvTex.
            static CTrophyObj *CreateModel(long nCup, long nGameMode, long nLightSel, long nLayer, const char *pEnvTex);
            bcs::CScnObj *GetParentObj();

            // (names guessed) The bodies of Destroy, _SetDrawPriority,
            // _SetLightSelector and _PushBack(long): the file is built
            // without auto-inlining, yet the destructor and CreateModel
            // inline them. Each acts on the inner model, then on the cup.
            void DestroyWithIn()
            {
                bcs::CScnObj *pIn = GetInObj();
                if (pIn != 0) {
                    DeleteScnObj(pIn);
                }
                m_hIn = 0;
                AttachToObject(0, 0, ATTACHMODE_0);
                DetachFromParent();
            }
            void SetDrawPriorityWithIn(long nPriority)
            {
                bcs::CScnObj *pIn = GetInObj();
                if (pIn != 0) {
                    pIn->SetDrawPriority(nPriority + 0x80, nPriority + 0x80);
                }
                SetDrawPriority(nPriority + 0x84, nPriority + 0x84);
            }
            void SetLightSelectorWithIn(long nSelector)
            {
                if (nSelector >= 0) {
                    bcs::CMdlObj *pIn = GetInObj();
                    if (pIn != 0) {
                        pIn->SetLightSelector(nSelector);
                    }
                    bcs::CMdlObj::SetLightSelector(nSelector);
                }
            }
            void PushBackWithIn(long nLayer)
            {
                bcs::CScnObj *pIn = GetInObj();
                if (pIn != 0) {
                    bcs::CRenderMan::PushBack(pIn, nLayer);
                }
                bcs::CRenderMan::PushBack(this, nLayer);
            }

        private:
            // Queues a scene object for deletion.
            static void DeleteScnObj(bcs::CScnObj *pObj)
            {
                if (pObj != 0) {
                    AppendDelayDeleteObject(pObj);
                }
            }

            // (name guessed) The inner model, or null.
            bcs::CMdlObj *GetInObj() const
            {
                return m_hIn != 0 ? static_cast<bcs::CMdlObj *>(GetScnObj(m_hIn)) : 0;
            }

            unsigned long m_hIn; // 0x1CC: handle of the inner model (name guessed)

            friend class CTrophyInObj;
    }; // size 0x1D0

    // (class name guessed; nothing names it) The model inside the cup
    // ("trophyIn" file, models m10 to m13), 0x1D0 bytes, vtable
    // lbl_80480840. It follows the cup's visibility and alpha.
    class CTrophyInObj : public CEventMdlObj {
        public:
            CTrophyInObj() : m_hParent(0) {}
            virtual ~CTrophyInObj();
            virtual void ObjProc(unsigned long nTask, unsigned long nArg, void *pArg);

        private:
            unsigned long m_hParent; // 0x1CC: handle of the cup (name guessed)

            friend class CTrophyObj;
    }; // size 0x1D0
}

#endif /* !CTROPHYOBJ_HPP_ */

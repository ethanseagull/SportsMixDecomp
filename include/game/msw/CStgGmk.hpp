#ifndef MSW_CSTGGMK_HPP_
#define MSW_CSTGGMK_HPP_

#include "bcs/CListNodeBase.hpp"
#include "game/msw/CStgObj.hpp"

namespace msw {

    class CStgGmkClusterImpl;

    // A stage gimmick. A DOL class, size 0x68 (msw::CStgGmk_Wave's members
    // start there), constructor __ct__Q23msw7CStgGmkFPQ23bcs5CTaskUl. The
    // order of the virtual functions follows __vt__Q23msw18CStgGmk_Wave_Dodge
    // (DodgeF); the bodies are the weak copies the modules emit (DodgeF has
    // them in game/dodge/CStgGmk_BossPackun_Dodge_Act.cpp).
    class CStgGmk : public CStgObj {
        public:
            // (meaning of the second argument not known: CStageNetVolley passes 7)
            CStgGmk(bcs::CTask *pParent, unsigned long arg);
            virtual ~CStgGmk();

            // The overrides of msw::CStgObj's virtuals (NewAct to OnSceneLeave)
            // are declared between GetClusterIndex and OnEntranceDemoStart:
            // that does not move a vtable slot, but the weak inline copies
            // come out in reverse declaration order, and every module has
            // ...OnEntranceDemoStart, OnPreCreate, GetClusterIndex... and
            // ...OnSceneLeave, OnSceneEntry, OnPreCreate, NewDrv, NewAct,
            // GetClusterIndex... (BasketF/DodgeF/VolleyF/HockeyF symbols).
            virtual void SetType(int type) { m_nType = type; }
            virtual int GetType() const { return m_nType; }
            virtual void SetCluster(CStgGmkClusterImpl *pCluster, long idx)
            {
                m_pCluster = pCluster;
                m_nClusterIdx = idx;
            }
            virtual CStgGmkClusterImpl *GetCluster() const { return m_pCluster; }
            virtual long GetClusterIndex() const { return m_nClusterIdx; }

            // Inline: BasketF's game/basket/CGoalObjBasket.cpp and HockeyF's
            // game/hockey/CGoalObjHockey.cpp have weak copies (their goals
            // keep these two of CStgGmk). Defined below the class.
            virtual CStgAct *NewAct(long heap);
            virtual CStgDrv *NewDrv(long heap);
            // A plain model, as CStgObj's. Inline: the vtables of CStgGmk
            // and msw::CStageNet both name the weak copy that
            // game/msw/CStageNet.cpp emits (DOL 0x8016AD10).
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);

            // Overrides of msw::CStgObj's hooks; CStgObj::OnCalcSum is not
            // overridden here.
            virtual void OnPreCreate() {}
            // Empty inline: CStgGmk's vtable names a 4-byte weak copy
            // (DOL 0x8014B4E4).
            virtual void OnPostCreate() {}
            virtual void OnSceneEntry() {}
            virtual void OnSceneLeave() {}

            virtual void OnEntranceDemoStart() {}
            virtual void OnGameInit() {}
            virtual void OnGameStart() {}
            virtual void OnGameEnd() {}
            virtual void OnPeriodInit() {}
            virtual void OnTurnInit() {}

            // (name guessed) The gimmick's node in CStgGmkMan's list: a list
            // node (vtable lbl_80474C98, deleting destructor fn_8016FFF0)
            // holding its gimmick at 0x10. The constructor sets the gimmick
            // to the CStgGmk itself and links the node in through
            // CStgGmkMan::Regist.
            class CNode : public bcs::CListNodeBase {
                public:
                    CStgGmk *GetGmk() const { return m_pGmk; }

                    CStgGmk *m_pGmk; // 0x10
            };

            CNode &GetNode() { return m_Node; }
            const CNode &GetNode() const { return m_Node; }

        private:
            CNode m_Node;                   // 0x48, 0x14 bytes
            int m_nType;                    // 0x5C
            CStgGmkClusterImpl *m_pCluster; // 0x60
            long m_nClusterIdx;             // 0x64
    };
}

#include "bcs/New.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    // A plain gimmick actor and driver.
    inline CStgAct *CStgGmk::NewAct(long heap) { return new (heap) CStgGmkAct(this); }
    inline CStgDrv *CStgGmk::NewDrv(long heap) { return new (heap) CStgDrv(this); }
    inline CStgMdl *CStgGmk::NewMdl(long heap, unsigned long idx) { return new (heap) CStgMdl(this); }
}

#endif /* !MSW_CSTGGMK_HPP_ */

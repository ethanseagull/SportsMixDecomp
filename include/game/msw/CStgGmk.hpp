#ifndef MSW_CSTGGMK_HPP_
#define MSW_CSTGGMK_HPP_

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
            CStgGmk(bcs::CTask *pParent, unsigned long arg);
            virtual ~CStgGmk();

            // Overrides of msw::CStgObj's hooks; CStgObj::OnCalcSum is not
            // overridden here.
            virtual void OnPreCreate() {}
            virtual void OnPostCreate();
            virtual void OnSceneEntry() {}
            virtual void OnSceneLeave() {}
            virtual void SetType(int type) { m_nType = type; }
            virtual int GetType() const { return m_nType; }
            virtual void SetCluster(CStgGmkClusterImpl *pCluster, long idx)
            {
                m_pCluster = pCluster;
                m_nClusterIdx = idx;
            }
            virtual CStgGmkClusterImpl *GetCluster() const { return m_pCluster; }
            virtual long GetClusterIndex() const { return m_nClusterIdx; }
            virtual void OnEntranceDemoStart() {}
            virtual void OnGameInit() {}
            virtual void OnGameStart() {}
            virtual void OnGameEnd() {}
            virtual void OnPeriodInit() {}
            virtual void OnTurnInit() {}

        private:
            unsigned long m_Unk48;          // 0x48
            unsigned long m_Unk4C;          // 0x4C
            unsigned long m_Unk50;          // 0x50
            unsigned long m_Unk54;          // 0x54
            unsigned long m_Unk58;          // 0x58
            int m_nType;                    // 0x5C
            CStgGmkClusterImpl *m_pCluster; // 0x60
            long m_nClusterIdx;             // 0x64
    };
}

#endif /* !MSW_CSTGGMK_HPP_ */

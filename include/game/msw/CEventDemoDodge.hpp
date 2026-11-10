#ifndef CEVENTDEMODODGE_HPP_
#define CEVENTDEMODODGE_HPP_

#include "game/msw/CEventDemoBase.hpp"
#include "game/msw/CGameDodge.hpp"

namespace msw {

    // The dodge event demo (DodgeF, size 0x128): the base, the ids of the
    // telops it has shown (-1 for a free slot), and the damage demos: a
    // queue of hits waiting for their replay (character, life before and
    // after, demo type) and the hit being shown. Names of the damage members
    // are guessed from the symbols (PushDamageDemo, PopDamageLife, ...).
    class CEventDemoDodge : public CEventDemoBase {
        public:
            enum { TELOP_MAX = 32, DAMAGE_MAX = 8, CHG_FIELD_MAX = 8 };

            CEventDemoDodge(bcs::CTask *pParent);
            virtual ~CEventDemoDodge();
            virtual void CreateDemo(long nDemo, bcs::CTask *pParent);
            virtual int IsDemoPlay(long nDemo) const;
            virtual void TimerEvent(short, short, unsigned long);
            virtual void CameraEvent(const char *pName, long nArg, long, float);
            virtual void SendMessage(long nMsg, long nArg, long, long);
            virtual void CreateTelop(long nObjId, long nCloseAnim);
            virtual void CreateTelop(long nObjId, long nParam, long nCloseAnim);
            virtual void PlayEndTelop(long nObjId);
            virtual void RemoveTelop(long nObjId);
            virtual int IsEndTelop(long nObjId);
            virtual void PlayEndTelopAll();
            virtual void RemoveTelopAll();
            virtual long GetTelopId(long nTelop);
            virtual void SetCockpitState(long, long);

            static CEventDemoDodge *GetPtr()
            {
                return static_cast<CEventDemoDodge *>(CEventDemoBase::GetPtr());
            }

            // Takes the oldest hit off the queue and makes it the current
            // one: returns its character (-1 when there is none) and gives
            // its life after the hit and its demo type.
            static long PopDamageDemo(long &rLife, long &rType);
            // Queues a hit; a character already queued gets demo type -1.
            // A negative life is stored as 0.
            static void PushDamageDemo(long nChr, long nLife, long nPrevLife, long nType);
            // Shows the current hit on the life gauge; with bOut, also the
            // "out" telop when the hit took the last life, and the hit ends.
            static void PopDamageLife(int bOut);
            // Starts the camera task of a damage demo (the misspelling is
            // the symbol's).
            static void SetDamegeCamera(long nType, long nLife);
            // Deletes the damage demo's tasks.
            void _CleanDamageTask();

            // Task handles (bcs::CTask::GetId) of the damage demo.
            static unsigned long m_hDamageLifeGauge;
            static unsigned long m_hDamageVoice;
            static unsigned long m_hDamageTelop;
            static unsigned long m_hDamageSpeed;
            static unsigned long m_hDamageCamera;
            // How many change-field messages came in; queued hits; the
            // character of the current hit (-1 for none).
            static long m_nChangeCount;
            static long m_nDamageCount;
            static long m_nDamageIndexNow;

            // (names guessed) Empties the damage queue and deletes the
            // damage tasks; clears the change-field list.
            void InitDamage()
            {
                m_nDamageCount = 0;
                m_nDamageIndexNow = -1;
                m_nPrevLife = -1;
                for (int i = 0; i < DAMAGE_MAX; i++) {
                    m_nDamageChr[i] = -1;
                    m_nDamageTypeList[i] = 0;
                    m_nPrevLifeList[i] = CGameDodge::GetChrRestLife(i);
                    m_nLifeList[i] = CGameDodge::GetChrRestLife(i);
                }
                _CleanDamageTask();
            }

            void InitChgField()
            {
                m_nChangeCount = 0;
                for (int i = 0; i < CHG_FIELD_MAX; i++) {
                    m_nChgField[i] = -1;
                }
            }

        private:
            // (names guessed) Whether nObjId is in the list; puts it in the
            // first free slot; frees every slot that holds it.
            int IsTelop(long nObjId) const
            {
                for (int i = 0; i < TELOP_MAX; i++) {
                    if (nObjId == m_nTelop[i]) {
                        return 1;
                    }
                }
                return 0;
            }

            void RegistTelop(long nObjId)
            {
                for (int i = 0; i < TELOP_MAX; i++) {
                    if (m_nTelop[i] == -1) {
                        m_nTelop[i] = nObjId;
                        return;
                    }
                }
            }

            void ClearTelop(long nObjId)
            {
                for (int i = 0; i < TELOP_MAX; i++) {
                    if (nObjId == m_nTelop[i]) {
                        m_nTelop[i] = -1;
                    }
                }
            }

        public:
            long m_nTelop[TELOP_MAX];               // 0x40
            // The current hit (see PopDamageDemo).
            long m_nDamageType;                     // 0xC0
            long m_nPrevLife;                       // 0xC4, -1 once shown
            long m_nLife;                           // 0xC8
            long m_UnkCC;                           // 0xCC, not used here
            // The queue, oldest first.
            long m_nPrevLifeList[DAMAGE_MAX];       // 0xD0
            long m_nLifeList[DAMAGE_MAX];           // 0xF0
            signed char m_nDamageChr[DAMAGE_MAX];   // 0x110
            signed char m_nDamageTypeList[DAMAGE_MAX]; // 0x118
            // Set by SendMessage 0x40/0x41, passed to the change-field demo.
            char m_nChgField[CHG_FIELD_MAX];        // 0x120
    };

    // The dodge event demo's functions for the game (namespace guessed, as
    // for EvDemoVs).
    namespace EvDemoDodge {
        void CreateInstance(bcs::CTask *pParent);
        int IsChgFieldDemo();
        void ResetChangeFieldDemo();
        int IsDamageDemo();
        void PushDamageDemo(long nChr, long nLife, long nPrevLife, long nType);
        void SetDamageDemoType(long nChr, long nType);
        void StartDamageDemo();
        void ResetDamageDemo();
        // In another unit of DodgeF (CreateDemo 6).
        void CreateChgFieldDemo(bcs::CTask *pParent, char *pField, long nCount);
    }
}

#endif /* !CEVENTDEMODODGE_HPP_ */

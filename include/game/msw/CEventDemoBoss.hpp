#ifndef CEVENTDEMOBOSS_HPP_
#define CEVENTDEMOBOSS_HPP_

#include "game/msw/CEventDemoBase.hpp"

namespace msw {

    // The boss event demo (BossF, size 0xC8): the base plus the ids of the
    // telops it has shown, -1 for a free slot. Unlike the other sports it
    // plays its demos from files (msw::CDemoManager) with the players as
    // models it makes itself (the CEvMdlCreaterGm* classes in the .cpp).
    class CEventDemoBoss : public CEventDemoBase {
        public:
            enum { TELOP_MAX = 32 };

            CEventDemoBoss(bcs::CTask *pParent);
            virtual ~CEventDemoBoss();
            virtual void CreateDemo(long nDemo, bcs::CTask *pParent);
            virtual int IsDemoPlay(long nDemo) const;
            virtual void TimerEvent(short, short, unsigned long);
            // Inline: emitted as a weak function after Disp.
            virtual void CameraEvent(const char *pName, long, long, float) {}
            virtual void SendMessage(long, long, long, long);
            virtual void CreateTelop(long nObjId, long nCloseAnim);
            virtual void CreateTelop(long nObjId, long nParam, long nCloseAnim);
            virtual void PlayEndTelop(long nObjId);
            virtual void RemoveTelop(long nObjId);
            virtual int IsEndTelop(long nObjId);
            virtual void PlayEndTelopAll();
            virtual void RemoveTelopAll();
            virtual long GetTelopId(long nTelop);
            virtual void SetCockpitState(long, long);

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

            long m_nTelop[TELOP_MAX];    // 0x40
            unsigned char m_UnkC0[0x08]; // 0xC0: not used by this module
    };

    // Makes the boss event demo (namespace guessed, as for EvDemoVs).
    namespace EvDemoBoss {
        void CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !CEVENTDEMOBOSS_HPP_ */

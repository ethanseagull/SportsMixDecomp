#ifndef MSW_CSTAGEBASKETHUNGRYBOSSPACKUN72_HPP_
#define MSW_CSTAGEBASKETHUNGRYBOSSPACKUN72_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStageBasketHungryBossPackun.hpp"

namespace msw {

    // Hungry Boss Packun stage 72. Size 0x26C (from CStageFactoryBasket::
    // Create). A conveyor belt runs across the court, now one way, now the
    // other, with stops in between, and carries the players along Z.
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow.
    class CStageBasketHungryBossPackun72 : public CStageBasketHungryBossPackun {
        public:
            // (member names guessed) One conveyor belt, size 0x20.
            struct BELT {
                // Values of m_nDir.
                enum {
                    DIR_STOP,
                    DIR_BACK,   // the speed is negative
                    DIR_FRONT
                };

                BELT() : m_nDir(DIR_STOP), m_nPrevDir(DIR_FRONT), m_vDir(bcs::FVEC3(0.0f, 0.0f, 1.0f)),
                         m_fSpeed(0.0f), m_fTargetSpeed(0.0f), m_fMove(0.0f)
                {
                }

                long m_nDir;          // 0x00
                long m_nPrevDir;      // 0x04, the direction before the last stop
                bcs::FVEC3 m_vDir;    // 0x08, not read here
                float m_fSpeed;       // 0x14, per frame
                float m_fTargetSpeed; // 0x18
                float m_fMove;        // 0x1C, how far the belt moved in the last frame
            };

            CStageBasketHungryBossPackun72(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun72();

            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            // How far the belt moved in the last frame.
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnTurnInit();
            virtual void OnArtsShootStart();
            virtual void OnArtsShootEnd();
            virtual void OnGimmickReset();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // Runs the belt for `frame` frames: speeds it up or down
            // towards the speed of its direction, and switches between
            // running and stopping.
            virtual void ConveyorUpdate(float frame);
            // Sets belt i's speed and plays its model's animation to match.
            virtual void SetBeltSpeed(long i, float speed);

        private:
            enum {
                BELT_NUM = 1
            };

            // Values of m_nState.
            enum {
                STATE_RUN,
                STATE_STOP
            };

            float m_fTimer;                    // 0x23C, frames left in the state
            long m_nState;                     // 0x240
            long m_nBeltMdlNum;                // 0x244
            CStageMdl *m_apBeltMdl[BELT_NUM];  // 0x248, the belt model
            BELT m_aBelt[BELT_NUM];            // 0x24C
    };
}

#endif /* !MSW_CSTAGEBASKETHUNGRYBOSSPACKUN72_HPP_ */

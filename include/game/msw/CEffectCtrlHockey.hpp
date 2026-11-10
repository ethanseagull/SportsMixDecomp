#ifndef CEFFECTCTRLHOCKEY_HPP_
#define CEFFECTCTRLHOCKEY_HPP_

#include "game/msw/CEffectCtrl.hpp"

namespace msw {

    // The effect controller of the hockey map. Size 0x58: three words of its own.
    class CEffectCtrlHockey : public CEffectCtrl {
        public:
            CEffectCtrlHockey(bcs::CTask *pParent) : CEffectCtrl(pParent)
            {
                AttachResource();
                m_Unk4C = 0;
                m_Unk50 = 0;
                m_Unk54 = 0;
                InitializeDataTable();
            }

            virtual ~CEffectCtrlHockey();
            virtual void Main();
            virtual void Disp();
            virtual int InitPatch(long, bcs::EF::CEffect *);

            // Returns the instance, creating it if there is none yet.
            static CEffectCtrl *CreateInstance(bcs::CTask *pParent);

            // Attaches an effect with the base's fixed extra arguments.
            void CreateAttachEffect(long id, const bcs::CScnObj *pObj, const bcs::FVEC3 *pPos, unsigned long flags,
                                    int a, int b, unsigned long c);

        private:
            unsigned long m_Unk4C; // 0x4C
            unsigned long m_Unk50; // 0x50
            unsigned long m_Unk54; // 0x54
    };
}

#endif /* !CEFFECTCTRLHOCKEY_HPP_ */

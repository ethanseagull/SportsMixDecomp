#ifndef MSW_CCHROPTTASK_HPP_
#define MSW_CCHROPTTASK_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/CAnmObjMotBase.hpp"
#include "game/msw/CChrOptMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    // (class name guessed; nothing of it is named in the DOL) A task that
    // owns one option model of a character, size 0xC0 (what
    // CC_CMN::SLIME::CcSquadFactory allocates). Its vtable is lbl_8046C258:
    // a task part (destructor fn_800D3BD4, Main fn_800D4324, CTask::Disp,
    // CUnivTask::GetFrameRateF) and an msw::CChrOptMdlObj at 0x34. Only the
    // functions used so far are declared; the layout is not mapped.
    //
    // The CChrOptMdlObj base is the secondary vtable at lbl_8046C258+0x18;
    // the virtuals the class adds follow it (slots 30 to 46, from 0x78).
    class CChrOptTask : public bcs::CUnivTask, public CChrOptMdlObj {
        public:
            // fn_800D3AD0. nArg sets bit 1 of the word at 0x50 (meaning not
            // known).
            CChrOptTask(bcs::CTask *pParent, int nArg);

            virtual void Main();

            // fn_800D3E68 (name and argument meanings guessed). Makes model
            // pMdlName of res with the motions of mot, attaches it to node
            // nNode of pParent (when nNode >= 0), and the two optional
            // models pSubName0 and pSubName1.
            void Create(const bcs::CScnObj *pParent, long nNode, nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot,
                        const char *pMdlName, const char *pSubName0, const char *pSubName1,
                        const bcs::CAnmObjMotBase::NODE_EX_INFO *pNodeExInfo, long nArg8, long nArg9, long nArg10);
            // fn_800D3FF8 (name guessed). Plays motion nMot.
            void AnimPlay(long nMot, float fInterFrame, float fSpeed);

            // The virtuals the class adds (names and parameters guessed;
            // only 0x84 and 0x90 are called so far, by BasketF's
            // CC16_BASKET::CcSquadFactoryBasket::AnimPlay).
            virtual void VirtualUnk78();                               // 0x78, fn_800D43B4
            virtual void VirtualUnk7C();                               // 0x7C, fn_800D4708
            virtual void VirtualUnk80();                               // 0x80, fn_800D496C (makes a CMdlObj)
            // 0x84, fn_800D4A6C: plays motion nMotId on the model's
            // bcs::CAnmMotion.
            virtual void PlayMotion(long nMotId, int bLoop, float fInterFrame, float fStartFrame);
            virtual void VirtualUnk88();                               // 0x88, fn_800D4BD8 (an attached effect)
            virtual void VirtualUnk8C();                               // 0x8C, fn_800D4C84
            // 0x90, fn_800D4D78: effect nEff in slot nSlot, attached to the
            // model (CEffectCtrl::CreateAttachEffectAuto).
            virtual void Eff_CreateAttachAuto(long nSlot, long nEff, unsigned long);

        private:
            unsigned char m_Unk50[0xC0 - 0x50];
    };
}

#endif /* !MSW_CCHROPTTASK_HPP_ */

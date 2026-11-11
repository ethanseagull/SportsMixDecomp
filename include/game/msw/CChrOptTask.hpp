#ifndef MSW_CCHROPTTASK_HPP_
#define MSW_CCHROPTTASK_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/CAnmObjMotBase.hpp"
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
    class CChrOptTask : public bcs::CUnivTask {
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

        private:
            unsigned char m_Unk34[0xC0 - 0x34];
    };
}

#endif /* !MSW_CCHROPTTASK_HPP_ */

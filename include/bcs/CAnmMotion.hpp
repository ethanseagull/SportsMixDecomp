#ifndef BCS_CANMMOTION_HPP_
#define BCS_CANMMOTION_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {

    class CMdlObj;

    // Skeletal animation, the object in a model's first animation slot. A
    // DOL class, size 0x4C (the constructor writes up to 0x48; the volley
    // stage allocates 0x4C), probably derived through bcs::CAnmObjMotBase.
    // Only what the modules call is declared.
    class CAnmMotion : public CAnmObjMotBase {
        public:
            CAnmMotion();
            virtual ~CAnmMotion();

            // Binds the motions of `file` to pMdl. (meanings of the last two
            // arguments not known)
            void Create(const CMdlObj *pMdl, nw4r::g3d::ResFile file, const NODE_EX_INFO *pInfo, unsigned long,
                        long);
            // Plays motion number `nMot` of the file given to Create.
            // (parameter names guessed as for the other Play)
            void Play(long nMot, unsigned long flag, float rate, float blend, float frame);

            // The three floats are passed on unchanged in the DOL; their
            // names are guessed from msw::CMenuMdlTableSet::PlayImpl.
            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame);
            // Clears bit 0 of the word at 0x18.
            void Stop();

        private:
            unsigned char m_Unk34[0x18]; // 0x34
    };
}

#endif /* !BCS_CANMMOTION_HPP_ */

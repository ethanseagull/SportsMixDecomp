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

            // 0x44 to 0x98: slots past those of bcs::CAnmObj, read from the
            // DOL vtable of bcs::CAnmMotion, where none has a name. They are
            // probably bcs::CAnmObjMotBase's. Most forward to the motion
            // selected by the word at 0x2C. Only 0x58, 0x64 and 0x98 are called
            // from a module so far (msw::SelectMenu::CCourtCharaObj::
            // StateCheck in MenuF, both with (3, 0)); the others are
            // placeholders that keep the slots in place.
            virtual void VFunc44();
            virtual void VFunc48();
            virtual void VFunc4C();
            virtual void VFunc50();
            virtual void VFunc54();
            virtual float VFunc58(long nNode);          // 0x58, takes a node index (msw::SelectMenu::CBgMdl in MenuF divides the result by a frame count)
            virtual void VFunc5C();
            virtual void VFunc60();
            virtual long VFunc64(long, unsigned long);  // 0x64, compared with 2 by the caller
            virtual void VFunc68();
            virtual void VFunc6C();
            virtual void VFunc70();
            virtual void VFunc74();
            virtual void VFunc78();
            virtual void VFunc7C();
            virtual void VFunc80();
            virtual void VFunc84();
            // 0x88, 0x8C (meaning from msw::CEvDemoMdlChr, names not known):
            // the length in frames of animation nAnim of the file, and
            // whether it loops.
            virtual float VFunc88(long nAnim);
            virtual int VFunc8C(long nAnim);
            virtual void VFunc90();
            virtual void VFunc94();
            virtual int VFunc98(long, unsigned long);   // 0x98, tested for nonzero by the caller
            // Binds the motions of `file` to pMdl. (meanings of the last two
            // arguments not known)
            void Create(const CMdlObj *pMdl, nw4r::g3d::ResFile file, const NODE_EX_INFO *pInfo, unsigned long,
                        long);

            // The three floats are passed on unchanged in the DOL; their
            // names are guessed from msw::CMenuMdlTableSet::PlayImpl.
            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame);
            // The same for motion number idx of the file given to Create.
            void Play(long idx, unsigned long flag, float rate, float blend, float frame);
            // Clears bit 0 of the word at 0x18.
            void Stop();
            // (DOL) The frame of trigger nTrg of animation nAnim. (meaning
            // of the float not known)
            float SearchTrgFrame(long nAnim, unsigned long nTrg, float) const;
            // (DOL fn_80008D38, unnamed; name guessed) Takes the motions from
            // another file.
            void ChangeResFile(nw4r::g3d::ResFile file);
            // (DOL; parameter names guessed) Calls pFunc with pUser for each
            // trigger the motion passes.
            void SetTrgCallback(void (*pFunc)(const MOT_TRG *pTrg, void *pUser), void *pUser);

        private:
            unsigned char m_Unk34[0x18]; // 0x34
    };
}

#endif /* !BCS_CANMMOTION_HPP_ */

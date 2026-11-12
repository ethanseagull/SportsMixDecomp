#ifndef NW4R_G3D_RESANMCLR_HPP_
#define NW4R_G3D_RESANMCLR_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // Only the member read so far (msw::CEventModel::AnimResFrame, DOL).
        struct ResAnmClrData {
            unsigned char unk00[0x20];
            unsigned short numFrame; // 0x20 (name guessed)
        };
        // The colour result of one material. Size 0x5C
        // (bcs::CAnmMatClr::Create allocates them); the fields are not
        // mapped yet.
        struct ClrAnmResult {
            unsigned char m_Unk00[0x5C];
        };

        // Handle to a colour animation resource: one pointer, null when the
        // resource was not found. Minimal declaration written for this
        // project; the member and function names are guessed.
        class ResAnmClr {
            public:
                // (guessed) The default constructor holds a check that folds
                // away (a null pointer tested for alignment), like ResFile's.
                // It has no effect, but it keeps a member array of handles
                // (BossF CBallBoss::m_aPowerAnmClr) built by a plain store
                // loop as in the original instead of a peeled, unrolled one.
                // The original has no assertion strings for this class, so
                // the check here makes no call. Stand-in for whatever the
                // original constructor really held.
                ResAnmClr() : m_pData(0)
                {
                    ResAnmClrData *p = 0;
                    if ((unsigned long)p & 0x3) {
                        m_pData = 0;
                    }
                }
                explicit ResAnmClr(ResAnmClrData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResAnmClr"; }

                // File name and line from the original's inline copy in
                // msw::CEventModel::AnimResFrame (DOL).
                ResAnmClrData &ref() const
                {
                    if (!IsValid()) {
                        nw4r::db::Panic("g3d_resanmclr_ac.h", 39, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                int GetNumFrame() const { return ref().numFrame; }

            private:
                ResAnmClrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMCLR_HPP_ */

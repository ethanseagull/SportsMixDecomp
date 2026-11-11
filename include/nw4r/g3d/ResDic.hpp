#ifndef NW4R_G3D_RESDIC_HPP_
#define NW4R_G3D_RESDIC_HPP_

#include "nw4r/db/Panic.hpp"

namespace nw4r {
    namespace g3d {

        // A name in a resource file: its length, then the characters.
        struct ResNameData {
            unsigned long len; // 0x00
            char str[4];       // 0x04
        };

        // Handle to a resource name, passed by value to the lookups.
        // Minimal declaration written for this project.
        class ResName {
            public:
                explicit ResName(const ResNameData *pData = 0) : m_pData(pData) {}

            private:
                const ResNameData *m_pData; // 0x00
        };

        // One entry of a resource dictionary (a Patricia tree). Entry 0 is
        // the root; the named entries start at 1. Size 0x10.
        struct ResDicNodeData {
            unsigned short ref;      // 0x00
            unsigned short idxLeft;  // 0x02
            unsigned short idxRight; // 0x04
            long ofsString;          // 0x08
            long ofsData;            // 0x0C, from the start of the dictionary
        };

        struct ResDicData {
            unsigned long size;      // 0x00
            unsigned long numData;   // 0x04
            ResDicNodeData data[1];  // 0x08
        };

        // Handle to a resource dictionary. Minimal declaration written for
        // this project; the checks, their files and lines are the ones in the
        // DOL's inline copies (bcs::CAnmTexSrt::Bind).
        class ResDic {
            public:
                explicit ResDic(void *p) : m_pData(static_cast<ResDicData *>(p))
                {
                    if (reinterpret_cast<unsigned long>(p) & 0x3) {
                        db::Panic("g3d_resdict_ac.h", 84, "NW4R:Failed assertion !((u32)p & 0x3)");
                    }
                }

                bool IsValid() const { return m_pData != 0; }

                static const char *GetClassName() { return "ResDic"; }

                ResDicData &ref() const
                {
                    if (!IsValid()) {
                        db::Panic("g3d_resdict_ac.h", 84, "%s::%s: Object not valid.", GetClassName(), __FUNCTION__);
                    }
                    return *m_pData;
                }

                // Number of named entries, 0 when the dictionary is not valid.
                unsigned long GetNumData() const
                {
                    if (IsValid()) {
                        return m_pData->numData;
                    }
                    return 0;
                }

                // Index of the entry named `pName`, -1 when there is none (DOL).
                long GetIndex(const char *pName) const;

                // The data of the idx-th named entry; null when the
                // dictionary is not valid.
                void *operator[](int idx) const
                {
                    if (IsValid()) {
                        // The form of NW4R's range check macro.
                        bool bInRange = false;
                        if (0 <= idx && idx <= static_cast<int>(ref().numData - 1)) {
                            bInRange = true;
                        }
                        if (!bInRange) {
                            db::Panic("g3d_resdict_ac.h", 42, "idx is out of bounds(%d)\n%d <= idx <= %d not satisfied.", idx, 0, ref().numData - 1);
                        }
                        long ofs = ref().data[idx + 1].ofsData;
                        if (ofs != 0) {
                            return reinterpret_cast<unsigned char *>(m_pData) + ofs;
                        }
                        return 0;
                    }
                    return 0;
                }

            private:
                ResDicData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESDIC_HPP_ */

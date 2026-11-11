// msw::CEvNodeData, a node path read out of a skeletal animation: the
// position and rotation of one node of the animation at a frame.
#include "game/msw/CEvNodeData.hpp"

namespace msw {

    CEvNodeData::CEvNodeData() {}

    CEvNodeData::~CEvNodeData() {}

    nw4r::g3d::ResAnmChr CEvNodeData::_GetResAnm(const nw4r::g3d::ResFile &file, const char *pName)
    {
        if (file.IsValid() && file.HasResAnmChr() && file.GetResAnmChrNumEntries()) {
            return file.GetResAnmChr(pName);
        }
        return nw4r::g3d::ResAnmChr();
    }

    long CEvNodeData::_GetAnmId(const nw4r::g3d::ResAnmChr &anm, const char *pNode)
    {
        if (anm.IsValid()) {
            return anm.GetChrDic().GetIndex(pNode);
        }
        return -1;
    }

    float CEvNodeData::GetNumFrame() const
    {
        if (m_anm.IsValid()) {
            return m_anm.GetNumFrame();
        }
        return 0.0f;
    }

    bool CEvNodeData::IsEnable(long nId)
    {
        if (m_anm.IsValid() && nId >= 0) {
            if (nId < m_anm.GetChrDic().GetNumData()) {
                return true;
            }
        }
        return false;
    }
}

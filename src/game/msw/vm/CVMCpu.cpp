// One file (DOL .text 0x801F702C..0x801FB608): CVMData, the opcode handlers,
// CVMCpu and CVMScript.
//
// The opcode handler names (and most member names here) are guesses from
// what the code does; the original names are not known.

#include <string.h>

#include "game/msw/CLoadData.hpp"
#include "game/msw/vm/CVMScript.hpp"
#include "game/msw/vm/CVMCpu.hpp"

namespace msw {
    namespace vm {

        // ---- CVMData ----

        void CVMData::Load(long nFile)
        {
            CLoadData::Load(nFile);
        }

        int CVMData::IsLoaded(long nFile)
        {
            return CLoadData::IsComplete(nFile);
        }

        void CVMData::Release()
        {
        }

        void CVMData::BindData(long nFile)
        {
            m_pImage = static_cast<unsigned char *>(CLoadData::GetPtr(nFile));
            Setup();
        }

        void CVMData::Init()
        {
            m_pHeader = 0;
            m_pCode = 0;
            m_pStaticData = 0;
            m_pString = 0;
            m_pFunction = 0;
            m_pValueTag = 0;
            if (m_pValue != 0) {
                delete[] m_pValue;
            }
            m_pValue = 0;
        }

        // Unscrambles the static data of a version E or F image: in each
        // byte, low = low + high, then high = high - low (both mod 16).
        // (name guessed)
        static inline void Unscramble(unsigned char *pData, unsigned long nSize)
        {
            for (unsigned long i = 0; i < nSize; i++) {
                int nHigh = *pData & 0xF0;
                int nLow = *pData & 0x0F;
                nLow += nHigh >> 4;
                nLow &= 0x0F;
                nHigh += 0x100 - (nLow << 4);
                *pData++ = nLow | (nHigh & 0xF0);
            }
        }

        void CVMData::Setup()
        {
            m_pHeader = reinterpret_cast<const HEADER *>(m_pImage);
            m_nFunctionHash = 0;
            m_nValueHash = 0;
            if (m_pHeader->aMagic[0] == 'S' && m_pHeader->aMagic[1] == 'B' && m_pHeader->aMagic[2] == 'D') {
                unsigned char *pBody = m_pImage + sizeof(HEADER);
                if (m_pHeader->aMagic[3] == 'E' || m_pHeader->aMagic[3] == 'F') {
                    Unscramble(pBody, m_pHeader->nStaticSize);
                }
                const long *pHash = reinterpret_cast<const long *>(pBody + m_pHeader->nStaticSize + m_pHeader->nCodeSize +
                                                                   m_pHeader->nFunctionNum * sizeof(FUNCTION_TAG) +
                                                                   m_pHeader->nValueTagNum * sizeof(VALUE_TAG));
                if (m_pHeader->aMagic[3] == 'E') {
                    m_pImage[3] = 'D';
                } else if (m_pHeader->aMagic[3] == 'F' || m_pHeader->aMagic[3] == 'G') {
                    m_nValueHash = pHash[0];
                    m_nFunctionHash = pHash[1];
                    m_pImage[3] = 'G';
                }
            }
            m_pStaticData = m_pImage + sizeof(HEADER);
            m_pCode = m_pStaticData + m_pHeader->nStaticSize;
            m_pFunction = reinterpret_cast<FUNCTION_TAG *>(m_pCode + m_pHeader->nCodeSize);
            m_pValueTag = reinterpret_cast<VALUE_TAG *>(m_pFunction + m_pHeader->nFunctionNum);
            m_pString = reinterpret_cast<const char *>(m_pValueTag + m_pHeader->nValueTagNum);
            m_pStatic = reinterpret_cast<STACK_DATA *>(const_cast<char *>(m_pString) + 8);
            if (m_pValue != 0) {
                delete[] m_pValue;
            }
            m_pValue = 0;
            long nValueNum = m_pHeader->nValueNum;
            if (nValueNum > 0) {
                m_pValue = new STACK_DATA[nValueNum];
            }
        }

        // The hash loop is written out here and in FindValueTag rather than
        // in a shared inline helper: with the helper the compiler inlines
        // the whole function into SetCall, which the original does not
        // (-inline auto goes by statement count, see notes/toolchain.md).
        CVMData::FUNCTION_TAG *CVMData::FindFunctionTag(const char *pName, long nGroup)
        {
            // Names are compared as strings, or by a hash when the
            // multiplier is above 0x10.
            long nMul = m_nFunctionHash;
            const char *p = pName;
            long nHash;
            if (nMul <= 0x10) {
                nHash = 0;
            } else {
                long long nValue = 0;
                while (*p != 0) {
                    nValue = nValue * nMul + *p;
                    p++;
                    nValue = (nValue >> 32) ^ (nValue & 0xFFFFFFFF);
                }
                nHash = nValue;
            }
            for (long i = 0; i < m_pHeader->nFunctionNum; i++) {
                FUNCTION_TAG *pTag = &m_pFunction[i];
                if (nGroup == -1 || pTag->nGroup == nGroup) {
                    if (nMul == 0) {
                        if (strcmp(m_pString + pTag->nName, pName) == 0) {
                            return pTag;
                        }
                    } else if (nHash == pTag->nName) {
                        return pTag;
                    }
                }
            }
            return 0;
        }

        CVMData::FUNCTION_TAG *CVMData::FindFunctionTag(int nName, long nGroup)
        {
            for (long i = 0; i < m_pHeader->nFunctionNum; i++) {
                FUNCTION_TAG *pTag = &m_pFunction[i];
                if ((nGroup == -1 || pTag->nGroup == nGroup) && nName == pTag->nName) {
                    return pTag;
                }
            }
            return 0;
        }

        CVMData::VALUE_TAG *CVMData::FindValueTag(const char *pName, long nGroup)
        {
            // Names are compared as strings, or by a hash when the
            // multiplier is above 0x10.
            long nMul = m_nValueHash;
            const char *p = pName;
            long nHash;
            if (nMul <= 0x10) {
                nHash = 0;
            } else {
                long long nValue = 0;
                while (*p != 0) {
                    nValue = nValue * nMul + *p;
                    p++;
                    nValue = (nValue >> 32) ^ (nValue & 0xFFFFFFFF);
                }
                nHash = nValue;
            }
            for (long i = 0; i < m_pHeader->nValueTagNum; i++) {
                VALUE_TAG *pTag = &m_pValueTag[i];
                if (nGroup == -1 || pTag->nGroup == nGroup) {
                    if (nMul == 0) {
                        if (strcmp(m_pString + pTag->nName, pName) == 0) {
                            return pTag;
                        }
                    } else if (nHash == pTag->nName) {
                        return pTag;
                    }
                }
            }
            return 0;
        }

        // ---- CVMCpu: the opcode handlers ----

        long CVMCpu::OpPushInt()
        {
            Push(FetchOperand());
            return 0;
        }

        long CVMCpu::OpPushFloat()
        {
            long nOperand = FetchOperand();
            PushF(*reinterpret_cast<float *>(&nOperand));
            return 0;
        }

        long CVMCpu::OpPushStaticPtr()
        {
            NewTop() = reinterpret_cast<long>(m_pData->m_pStaticData + FetchOperand());
            return 0;
        }

        long CVMCpu::OpPushValue()
        {
            long nIndex = FetchOperand();
            NewTop() = m_pData->Value(nIndex);
            return 0;
        }

        long CVMCpu::OpPushLocal()
        {
            NewTop() = Stack(m_nFP + FetchOperand());
            return 0;
        }

        long CVMCpu::OpLoadValueIndex()
        {
            long nIndex = FetchOperand();
            Stack(m_nSP - 1) = m_pData->Value(nIndex + Stack(m_nSP - 1));
            return 0;
        }

        long CVMCpu::OpLoadLocalIndex()
        {
            long nIndex = FetchOperand();
            Stack(m_nSP - 1) = Stack(Stack(m_nSP - 1) + nIndex + m_nFP);
            return 0;
        }

        long CVMCpu::OpPushValueBlock()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nIndex = nOperand >> 16;
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = m_pData->Value(nIndex);
                m_nSP++;
                nIndex++;
            }
            return 0;
        }

        long CVMCpu::OpPushLocalBlock()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + m_nFP;
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = Stack(nIndex);
                m_nSP++;
                nIndex++;
            }
            return 0;
        }

        long CVMCpu::OpPushValueBlockIndex()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum * Stack(m_nSP);
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = m_pData->Value(nIndex);
                m_nSP++;
                nIndex++;
            }
            return 0;
        }

        long CVMCpu::OpPushStaticBlockIndex()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum * Stack(m_nSP);
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = m_pData->Static(nIndex);
                m_nSP++;
                nIndex++;
            }
            return 0;
        }

        long CVMCpu::OpPushLocalBlockIndex()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum * Stack(m_nSP) + m_nFP;
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = Stack(nIndex);
                m_nSP++;
                nIndex++;
            }
            return 0;
        }

        long CVMCpu::OpPushRef()
        {
            long nAddr = GetStack(m_nFP + FetchOperand());
            Push(Var(nAddr));
            return 0;
        }

        long CVMCpu::OpLoadRefIndex()
        {
            long nIndex = FetchOperand();
            long nAddr = Stack(m_nFP + nIndex) + Stack(m_nSP - 1);
            Stack(m_nSP - 1) = Var(nAddr);
            return 0;
        }

        long CVMCpu::OpPushRefBlock()
        {
            long nOperand = FetchOperand();
            long nAddr = Stack(m_nFP + (nOperand >> 16));
            long nNum = nOperand & 0xFFFF;
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = Var(nAddr);
                m_nSP++;
                nAddr++;
            }
            return 0;
        }

        long CVMCpu::OpPushRefBlockIndex()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nAddr = Stack(m_nFP + (nOperand >> 16)) + nNum * Stack(m_nSP - 1);
            m_nSP--;
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = Var(nAddr);
                m_nSP++;
                nAddr++;
            }
            return 0;
        }

        long CVMCpu::OpPushAddrBlock()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nAddr = Stack(m_nSP) + (nOperand >> 16);
            for (long i = 0; i < nNum; i++) {
                Stack(m_nSP) = Var(nAddr);
                m_nSP++;
                nAddr++;
            }
            return 0;
        }

        long CVMCpu::OpRemoveBlock()
        {
            long nOperand = FetchOperand();
            long nOffset = nOperand >> 16;
            long nNum = nOperand & 0xFFFF;
            long nDst = m_nSP - nOffset;
            long nSrc = nDst + nNum;
            for (long i = 0; i < nOffset - nNum; i++) {
                Stack(nDst) = Stack(nSrc);
                nDst++;
                nSrc++;
            }
            m_nSP -= nNum;
            return 0;
        }

        long CVMCpu::OpPushAddress()
        {
            long nAddr = FetchOperand();
            if (!(nAddr & 0x40000000) && !(nAddr & 0x20000000)) {
                nAddr += m_nFP;
            }
            Push(nAddr);
            return 0;
        }

        long CVMCpu::OpAddAddress()
        {
            long nAddr = FetchOperand();
            if (!(nAddr & 0x40000000) && !(nAddr & 0x20000000)) {
                nAddr += m_nFP;
            }
            Stack(m_nSP - 1) += nAddr;
            return 0;
        }

        long CVMCpu::OpPopValue()
        {
            m_pData->Value(FetchOperand()) = Stack(m_nSP - 1);
            m_nSP--;
            return 0;
        }

        long CVMCpu::OpPopLocal()
        {
            Stack(m_nFP + FetchOperand()) = Stack(m_nSP - 1);
            m_nSP--;
            return 0;
        }

        long CVMCpu::OpPopValueIndex()
        {
            long nIndex = FetchOperand();
            m_pData->Value(nIndex + Stack(m_nSP - 1)) = Stack(m_nSP - 2);
            m_nSP -= 2;
            return 0;
        }

        long CVMCpu::OpPopLocalIndex()
        {
            long nIndex = FetchOperand();
            Stack(Stack(m_nSP - 1) + nIndex + m_nFP) = Stack(m_nSP - 2);
            m_nSP -= 2;
            return 0;
        }

        long CVMCpu::OpPopRef()
        {
            long nAddr = Stack(m_nFP + FetchOperand());
            WritableVar(nAddr) = Stack(m_nSP - 1);
            m_nSP--;
            return 0;
        }

        long CVMCpu::OpPopRefIndex()
        {
            long nIndex = FetchOperand();
            long nAddr = Stack(m_nFP + nIndex) + Stack(m_nSP - 1);
            WritableVar(nAddr) = Stack(m_nSP - 2);
            m_nSP -= 2;
            return 0;
        }

        long CVMCpu::OpPopValueBlock()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum;
            for (long i = 0; i < nNum; i++) {
                m_nSP--;
                nIndex--;
                m_pData->Value(nIndex) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpPopLocalBlock()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum + m_nFP;
            for (long i = 0; i < nNum; i++) {
                m_nSP--;
                nIndex--;
                Stack(nIndex) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpPopValueBlockIndex()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum + nNum * Stack(m_nSP);
            for (long i = 0; i < nNum; i++) {
                m_nSP--;
                nIndex--;
                m_pData->Value(nIndex) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpPopLocalBlockIndex()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nIndex = (nOperand >> 16) + nNum + nNum * Stack(m_nSP) + m_nFP;
            for (long i = 0; i < nNum; i++) {
                m_nSP--;
                nIndex--;
                Stack(nIndex) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpPopRefBlock()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nAddr = Stack(m_nFP + (nOperand >> 16)) + nNum;
            for (long i = 0; i < nNum; i++) {
                nAddr--;
                m_nSP--;
                WritableVar(nAddr) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpPopRefBlockIndex()
        {
            long nOperand = FetchOperand();
            long nNum = nOperand & 0xFFFF;
            long nAddr = Stack(m_nFP + (nOperand >> 16)) + nNum;
            nAddr += nNum * Stack(m_nSP - 1);
            m_nSP--;
            for (long i = 0; i < nNum; i++) {
                nAddr--;
                m_nSP--;
                WritableVar(nAddr) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpPopAddrBlock()
        {
            long nOperand = FetchOperand();
            m_nSP--;
            long nNum = nOperand & 0xFFFF;
            long nAddr = Stack(m_nSP) + (nOperand >> 16);
            for (long i = 0; i < nNum; i++) {
                nAddr--;
                m_nSP--;
                WritableVar(nAddr) = Stack(m_nSP);
            }
            return 0;
        }

        long CVMCpu::OpFillInt()
        {
            long nNum = FetchOperand();
            long nBase = Stack(m_nSP - 1);
            m_nSP--;
            for (long i = 0; i < nNum; i++) {
                long nAddr = nBase + i;
                long nValue = FetchOperand();
                if ((nAddr & 0x40000000) == 0) {
                    Stack(nAddr) = nValue;
                } else {
                    m_pData->Value(nAddr & 0x3FFFFFFF) = nValue;
                }
            }
            return 0;
        }

        long CVMCpu::OpFillFloat()
        {
            long nNum = FetchOperand();
            long nBase = Stack(m_nSP - 1);
            m_nSP--;
            for (long i = 0; i < nNum; i++) {
                long nAddr = nBase + i;
                long nBits = FetchOperand();
                float fValue = *reinterpret_cast<float *>(&nBits);
                // Stored as a word, as CVMScript::SetValueF does.
                if ((nAddr & 0x40000000) == 0) {
                    Stack(nAddr) = *reinterpret_cast<long *>(&fValue);
                } else {
                    m_pData->Value(nAddr & 0x3FFFFFFF) = *reinterpret_cast<long *>(&fValue);
                }
            }
            return 0;
        }

        long CVMCpu::OpEnter()
        {
            m_nSP = m_nFP + FetchOperand();
            m_nMaxSP = m_nMaxSP > m_nSP ? m_nMaxSP : m_nSP;
            return 0;
        }

        long CVMCpu::OpDrop()
        {
            m_nSP--;
            return 0;
        }

        long CVMCpu::OpDropN()
        {
            m_nSP -= FetchOperand();
            return 0;
        }

        long CVMCpu::OpMulImm()
        {
            long nValue = FetchOperand();
            Stack(m_nSP - 1) *= nValue;
            return 0;
        }

        long CVMCpu::OpNeg()
        {
            Stack(m_nSP - 1) = -Stack(m_nSP - 1);
            return 0;
        }

        long CVMCpu::OpNegF()
        {
            StackF(m_nSP - 1) = -StackF(m_nSP - 1);
            return 0;
        }

        long CVMCpu::OpNot()
        {
            Stack(m_nSP - 1) = !Stack(m_nSP - 1);
            return 0;
        }

        long CVMCpu::OpNotF()
        {
            Stack(m_nSP - 1) = !StackF(m_nSP - 1);
            return 0;
        }

        long CVMCpu::OpInc()
        {
            Stack(m_nSP - 1)++;
            return 0;
        }

        long CVMCpu::OpIncF()
        {
            StackF(m_nSP - 1) += 1.0f;
            return 0;
        }

        long CVMCpu::OpDec()
        {
            Stack(m_nSP - 1)--;
            return 0;
        }

        long CVMCpu::OpDecF()
        {
            StackF(m_nSP - 1) -= 1.0f;
            return 0;
        }

        long CVMCpu::OpPostInc()
        {
            Stack(m_nSP - 1)++;
            return 0;
        }

        long CVMCpu::OpPostIncF()
        {
            StackF(m_nSP - 1) += 1.0f;
            return 0;
        }

        long CVMCpu::OpPostDec()
        {
            Stack(m_nSP - 1)--;
            return 0;
        }

        long CVMCpu::OpPostDecF()
        {
            StackF(m_nSP - 1) -= 1.0f;
            return 0;
        }

        // The binary operators: the left operand is under the right one;
        // the result replaces both. The original reads the operands in
        // three orders (through a copy of m_nSP, right then left, left then
        // right); which handler uses which is taken from its code and has
        // no effect on the result.

        long CVMCpu::OpEq()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) == Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpEqF()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = StackF(nSP - 2) == StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpNe()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) != Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpNeF()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = StackF(nSP - 2) != StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpGt()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) > Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpGtF()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = StackF(nSP - 2) > StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpGe()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) >= Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpGeF()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = StackF(nSP - 2) >= StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpLt()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) < Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpLtF()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = StackF(nSP - 2) < StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpLe()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) <= Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpLeF()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = StackF(nSP - 2) <= StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpLogicalAnd()
        {
            long nRight = Stack(m_nSP - 1);
            long nLeft = Stack(m_nSP - 2);
            m_nSP--;
            long &rResult = Stack(m_nSP - 1);
            rResult = nLeft != 0 && nRight != 0;
            return 0;
        }

        long CVMCpu::OpLogicalOr()
        {
            long nRight = Stack(m_nSP - 1);
            long nLeft = Stack(m_nSP - 2);
            m_nSP--;
            long &rResult = Stack(m_nSP - 1);
            rResult = nLeft != 0 || nRight != 0;
            return 0;
        }

        long CVMCpu::OpAnd()
        {
            long nRight = Stack(m_nSP - 1);
            long nLeft = Stack(m_nSP - 2);
            m_nSP--;
            Stack(m_nSP - 1) = nLeft & nRight;
            return 0;
        }

        long CVMCpu::OpOr()
        {
            long nRight = Stack(m_nSP - 1);
            long nLeft = Stack(m_nSP - 2);
            m_nSP--;
            Stack(m_nSP - 1) = nLeft | nRight;
            return 0;
        }

        long CVMCpu::OpShl()
        {
            long nRight = Stack(m_nSP - 1);
            long nLeft = Stack(m_nSP - 2);
            m_nSP--;
            Stack(m_nSP - 1) = nLeft << nRight;
            return 0;
        }

        long CVMCpu::OpShr()
        {
            long nRight = Stack(m_nSP - 1);
            long nLeft = Stack(m_nSP - 2);
            m_nSP--;
            Stack(m_nSP - 1) = nLeft >> nRight;
            return 0;
        }

        long CVMCpu::OpAdd()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) + Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpAddF()
        {
            long nSP = m_nSP;
            m_nSP--;
            StackF(m_nSP - 1) = StackF(nSP - 2) + StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpSub()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) - Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpSubF()
        {
            long nSP = m_nSP;
            m_nSP--;
            StackF(m_nSP - 1) = StackF(nSP - 2) - StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpMul()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) * Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpMulF()
        {
            long nSP = m_nSP;
            m_nSP--;
            StackF(m_nSP - 1) = StackF(nSP - 2) * StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpDiv()
        {
            long nSP = m_nSP;
            m_nSP--;
            Stack(m_nSP - 1) = Stack(nSP - 2) / Stack(nSP - 1);
            return 0;
        }

        long CVMCpu::OpDivF()
        {
            long nSP = m_nSP;
            m_nSP--;
            StackF(m_nSP - 1) = StackF(nSP - 2) / StackF(nSP - 1);
            return 0;
        }

        long CVMCpu::OpMod()
        {
            long nLeft = Stack(m_nSP - 2);
            long nRight = Stack(m_nSP - 1);
            m_nSP--;
            Stack(m_nSP - 1) = nLeft % nRight;
            return 0;
        }

        // String operands are pointers into the static data (OpPushStaticPtr).
        long CVMCpu::OpStrEq()
        {
            const char *pRight = reinterpret_cast<const char *>(Stack(m_nSP - 1));
            const char *pLeft = reinterpret_cast<const char *>(Stack(m_nSP - 2));
            m_nSP--;
            Stack(m_nSP - 1) = strcmp(pLeft, pRight) == 0;
            return 0;
        }

        long CVMCpu::OpStrNe()
        {
            const char *pRight = reinterpret_cast<const char *>(Stack(m_nSP - 1));
            const char *pLeft = reinterpret_cast<const char *>(Stack(m_nSP - 2));
            m_nSP--;
            Stack(m_nSP - 1) = strcmp(pLeft, pRight) != 0;
            return 0;
        }

        long CVMCpu::OpJump()
        {
            long nOffset = FetchOperand();
            m_pPC = m_pData->GetCode() + nOffset;
            return 0;
        }

        long CVMCpu::OpJumpTrue()
        {
            long nOffset = FetchOperand();
            long nValue = Stack(m_nSP - 1);
            m_nSP--;
            if (nValue != 0) {
                m_pPC = m_pData->GetCode() + nOffset;
            }
            return 0;
        }

        long CVMCpu::OpJumpFalse()
        {
            long nOffset = FetchOperand();
            long nValue = Stack(m_nSP - 1);
            m_nSP--;
            if (nValue == 0) {
                m_pPC = m_pData->GetCode() + nOffset;
            }
            return 0;
        }

        // Jumps and drops both words when the top word equals the one
        // under it; otherwise drops only the top word.
        long CVMCpu::OpCase()
        {
            long nOffset = FetchOperand();
            if (Stack(m_nSP - 1) == Stack(m_nSP - 2)) {
                m_nSP -= 2;
                m_pPC = m_pData->GetCode() + nOffset;
            } else {
                m_nSP -= 1;
            }
            return 0;
        }

        // The frame of a call: the argument count (pushed by the caller),
        // the caller's frame pointer and the return offset (-1: return to
        // the host, see SetCall).
        long CVMCpu::OpCall()
        {
            long nOffset = FetchOperand();
            Stack(m_nSP) = m_nFP;
            Stack(m_nSP + 1) = m_pPC - m_pData->GetCode();
            m_pPC = m_pData->GetCode() + nOffset;
            m_nSP += 2;
            m_nFP = m_nSP;
            return 0;
        }

        long CVMCpu::OpReturn()
        {
            m_nSP = m_nFP;
            m_nFP = Stack(m_nSP - 2);
            long nReturn = Stack(m_nSP - 1);
            m_nSP += -3 - Stack(m_nSP - 3);
            if (nReturn >= 0) {
                m_pPC = m_pData->GetCode() + nReturn;
                return 0;
            }
            m_pPC = 0;
            return 2;
        }

        long CVMCpu::OpReturnValue()
        {
            long nValue = Stack(m_nSP - 1);
            m_nSP = m_nFP;
            m_nFP = Stack(m_nSP - 2);
            long nReturn = Stack(m_nSP - 1);
            long nArg = Stack(m_nSP - 3);
            if (nReturn >= 0) {
                m_nSP += -2 - nArg;
                Stack(m_nSP - 1) = nValue;
                m_pPC = m_pData->GetCode() + nReturn;
                return 0;
            }
            m_nSP += -3 - nArg;
            m_nUnk1C = nValue;
            m_pPC = 0;
            return 2;
        }

        long CVMCpu::OpReturnBlock()
        {
            long nNum = FetchOperand();
            long nSrc = m_nSP - nNum;
            m_nSP = m_nFP;
            m_nFP = Stack(m_nSP - 2);
            long nReturn = Stack(m_nSP - 1);
            if (nReturn >= 0) {
                m_nSP += -3 - Stack(m_nSP - 3);
                for (long i = 0; i < nNum; i++) {
                    Stack(m_nSP) = Stack(nSrc);
                    m_nSP++;
                    nSrc++;
                }
                m_pPC = m_pData->GetCode() + nReturn;
                return 0;
            }
            m_nUnk1C = Stack(nSrc);
            m_pPC = 0;
            return 2;
        }

        long CVMCpu::OpSysCall()
        {
            return 3;
        }

        long CVMCpu::OpUnused2F()
        {
            return 3;
        }

        long CVMCpu::OpUnused30()
        {
            return 3;
        }

        long CVMCpu::OpUnused46()
        {
            return 3;
        }

        CVMCpu::OPFUNC CVMCpu::s_aOpFunc[] = {
            &CVMCpu::OpPushInt,              // 0x00
            &CVMCpu::OpPushStaticPtr,        // 0x01
            &CVMCpu::OpPushValue,            // 0x02
            &CVMCpu::OpPushLocal,            // 0x03
            &CVMCpu::OpLoadValueIndex,       // 0x04
            &CVMCpu::OpLoadLocalIndex,       // 0x05
            &CVMCpu::OpPushRef,              // 0x06
            &CVMCpu::OpLoadRefIndex,         // 0x07
            &CVMCpu::OpPushAddress,          // 0x08
            &CVMCpu::OpAddAddress,           // 0x09
            &CVMCpu::OpPopValue,             // 0x0A
            &CVMCpu::OpPopLocal,             // 0x0B
            &CVMCpu::OpPopValueIndex,        // 0x0C
            &CVMCpu::OpPopLocalIndex,        // 0x0D
            &CVMCpu::OpPopRef,               // 0x0E
            &CVMCpu::OpPopRefIndex,          // 0x0F
            &CVMCpu::OpFillInt,              // 0x10
            &CVMCpu::OpEnter,                // 0x11
            &CVMCpu::OpDrop,                 // 0x12
            &CVMCpu::OpNeg,                  // 0x13
            &CVMCpu::OpEq,                   // 0x14
            &CVMCpu::OpNe,                   // 0x15
            &CVMCpu::OpGt,                   // 0x16
            &CVMCpu::OpGe,                   // 0x17
            &CVMCpu::OpLt,                   // 0x18
            &CVMCpu::OpLe,                   // 0x19
            &CVMCpu::OpLogicalAnd,           // 0x1A
            &CVMCpu::OpLogicalOr,            // 0x1B
            &CVMCpu::OpAnd,                  // 0x1C
            &CVMCpu::OpOr,                   // 0x1D
            &CVMCpu::OpShl,                  // 0x1E
            &CVMCpu::OpShr,                  // 0x1F
            &CVMCpu::OpAdd,                  // 0x20
            &CVMCpu::OpSub,                  // 0x21
            &CVMCpu::OpMul,                  // 0x22
            &CVMCpu::OpDiv,                  // 0x23
            &CVMCpu::OpMod,                  // 0x24
            &CVMCpu::OpStrEq,                // 0x25
            &CVMCpu::OpStrNe,                // 0x26
            &CVMCpu::OpJump,                 // 0x27
            &CVMCpu::OpJumpTrue,             // 0x28
            &CVMCpu::OpJumpFalse,            // 0x29
            &CVMCpu::OpCase,                 // 0x2A
            &CVMCpu::OpCall,                 // 0x2B
            &CVMCpu::OpSysCall,              // 0x2C
            &CVMCpu::OpReturn,               // 0x2D
            &CVMCpu::OpReturnValue,          // 0x2E
            &CVMCpu::OpUnused2F,             // 0x2F
            &CVMCpu::OpUnused30,             // 0x30
            &CVMCpu::OpPushFloat,            // 0x31
            &CVMCpu::OpNot,                  // 0x32
            &CVMCpu::OpInc,                  // 0x33
            &CVMCpu::OpDec,                  // 0x34
            &CVMCpu::OpPostInc,              // 0x35
            &CVMCpu::OpPostDec,              // 0x36
            &CVMCpu::OpPushValueBlock,       // 0x37
            &CVMCpu::OpPushLocalBlock,       // 0x38
            &CVMCpu::OpPushValueBlockIndex,  // 0x39
            &CVMCpu::OpPushLocalBlockIndex,  // 0x3A
            &CVMCpu::OpPushRefBlock,         // 0x3B
            &CVMCpu::OpPushRefBlockIndex,    // 0x3C
            &CVMCpu::OpPopValueBlock,        // 0x3D
            &CVMCpu::OpPopLocalBlock,        // 0x3E
            &CVMCpu::OpPopValueBlockIndex,   // 0x3F
            &CVMCpu::OpPopLocalBlockIndex,   // 0x40
            &CVMCpu::OpPopRefBlock,          // 0x41
            &CVMCpu::OpPopRefBlockIndex,     // 0x42
            &CVMCpu::OpDropN,                // 0x43
            &CVMCpu::OpReturnBlock,          // 0x44
            &CVMCpu::OpFillFloat,            // 0x45
            &CVMCpu::OpUnused46,             // 0x46
            &CVMCpu::OpNegF,                 // 0x47
            &CVMCpu::OpEqF,                  // 0x48
            &CVMCpu::OpNeF,                  // 0x49
            &CVMCpu::OpGtF,                  // 0x4A
            &CVMCpu::OpGeF,                  // 0x4B
            &CVMCpu::OpLtF,                  // 0x4C
            &CVMCpu::OpLeF,                  // 0x4D
            &CVMCpu::OpAddF,                 // 0x4E
            &CVMCpu::OpSubF,                 // 0x4F
            &CVMCpu::OpMulF,                 // 0x50
            &CVMCpu::OpDivF,                 // 0x51
            &CVMCpu::OpNotF,                 // 0x52
            &CVMCpu::OpIncF,                 // 0x53
            &CVMCpu::OpDecF,                 // 0x54
            &CVMCpu::OpPostIncF,             // 0x55
            &CVMCpu::OpPostDecF,             // 0x56
            &CVMCpu::OpPushStaticBlockIndex, // 0x57
            &CVMCpu::OpMulImm,               // 0x58
            &CVMCpu::OpPushAddrBlock,        // 0x59
            &CVMCpu::OpPopAddrBlock,         // 0x5A
            &CVMCpu::OpRemoveBlock,          // 0x5B
        };

        // ---- CVMCpu ----

        void CVMCpu::Reset()
        {
            ResetBind();
            m_nSP = 0;
            m_nFP = 0;
            m_pPC = 0;
            m_nState = 4;
            m_nUnk1C = 0;
        }

        void CVMCpu::ResetBind()
        {
        }

        int CVMCpu::SetCall(const char *pName, long nArg, long nState, long nGroup)
        {
            CVMData::FUNCTION_TAG *pTag;

            m_nSP += 3;
            m_nFP = m_nSP;
            SetStack(m_nSP - 3, nArg);
            SetStack(m_nSP - 2, m_nFP);
            SetStack(m_nSP - 1, -1);
            if (nState != -1) {
                m_nState = nState;
            }
            m_nUnk1C = 0;

            pTag = m_pData->FindFunctionTag(pName, nGroup);
            if (pTag != 0 && pTag->nOffset != -1) {
                m_pPC = m_pData->GetCode() + pTag->nOffset;
                m_nArgNum = pTag->nArgNum;
                return 1;
            }
            m_pPC = 0;
            m_nArgNum = 0;
            return 0;
        }

        long CVMCpu::Run()
        {
            while (m_pPC != 0) {
                if ((m_nState = (this->*s_aOpFunc[*m_pPC++])()) != 0) {
                    break;
                }
            }
            return m_nState;
        }

        // ---- CVMScript: the script VM with bound system calls ----

        void CVMScript::SetScript(void *pImage)
        {
            ReleaseData();
            m_pOwnData = new CVMData(pImage);
            m_bOwnData = 1;
            m_pData = m_pOwnData;
            UpdateBind();
        }

        void CVMScript::SetScript(const CVMScript &script)
        {
            ReleaseData();
            m_pOwnData = script.m_pOwnData;
            m_pData = m_pOwnData;
            UpdateBind();
        }

        long CVMScript::CallEntry()
        {
            while (m_pPC != 0) {
                if ((m_nState = (this->*s_aOpFunc[*m_pPC++])()) != 0) {
                    break;
                }
            }
            return m_nState;
        }

        bcs::FVEC3 CVMScript::GetVector3(int nIndex) const
        {
            if (!IsValue(nIndex)) {
                return bcs::FVEC3(GetStackF(nIndex), GetStackF(nIndex + 1), GetStackF(nIndex + 2));
            }
            nIndex = ValueIndex(nIndex);
            return bcs::FVEC3(GetValueF(nIndex), GetValueF(nIndex + 1), GetValueF(nIndex + 2));
        }

        void CVMScript::SetVector3(int nIndex, bcs::FVEC3 v)
        {
            if (!IsValue(nIndex)) {
                SetStackF(nIndex, v.x);
                SetStackF(nIndex + 1, v.y);
                SetStackF(nIndex + 2, v.z);
            } else {
                nIndex = ValueIndex(nIndex);
                SetValueF(nIndex, v.x);
                SetValueF(nIndex + 1, v.y);
                SetValueF(nIndex + 2, v.z);
            }
        }

        long CVMScript::_OpSysCallYield()
        {
            m_nSP -= GetArgNum() + 1;
            return 1;
        }

        void CVMScript::UpdateBind()
        {
            if (m_pFunc != 0) {
                delete[] m_pFunc;
            }
            m_pFunc = 0;
            m_nFuncNum = m_pData->GetSysCallNum();
            m_pFunc = new FUNC_BUF[m_nFuncNum];
            for (long i = 0; i < m_nFuncNum; i++) {
                m_pFunc[i].Clear();
            }
            BindFunctions();
        }

        long CVMScript::OpSysCall()
        {
            // The call number follows the opcode, big-endian.
            long nCall = m_pPC[3];
            nCall |= m_pPC[2] << 8;
            nCall |= m_pPC[1] << 16;
            nCall |= m_pPC[0] << 24;
            m_pPC += 4;
            if (nCall < 0 || nCall >= m_nFuncNum) {
                return 3;
            }
            return reinterpret_cast<CFuncBase *>(&m_pFunc[nCall])->Call(this);
        }
    }
}

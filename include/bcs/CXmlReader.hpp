#ifndef BCS_CXMLREADER_HPP_
#define BCS_CXMLREADER_HPP_

namespace bcs {

    // Size 0x30 (operator new in MenuF game/menu/CRuleInfo.cpp). Lives in
    // the DOL. One element of a parsed XML text: the constructor parses
    // the text into the element and its children. Only the members used so
    // far; the names of the arguments are guesses.
    class CXmlReader {
        public:
            CXmlReader(const char *pText, const CXmlReader *pParent);
            ~CXmlReader();

            // The element's name and text.
            const char *GetKey();
            const char *GetValue();
            // The value of attribute pName; what the long selects is not
            // known (callers pass 0).
            const char *GetProperty(const char *pName, long nUnk);
            long GetChildrenNum();
            CXmlReader *GetChild(long nIndex);

        private:
            unsigned char m_Unk00[0x30];
    };
}

#endif /* !BCS_CXMLREADER_HPP_ */


#include "NexHeaders.h"
#include "GenericStreams.h"
#include "NexXml.h"

namespace nextar {
    namespace xml {

        static void FormatString(const String& in, String& out);
        static const char* ReadName(const char* parse_data, String& name_str);
        static const char* ReadText(const char* parse_data, String &text_str, const char* end_tag, bool ignore_case);
        static bool StringEqual(const char* p, const char* tag, bool ignore_case);

        class CommentNode : public Node {
            NEX_LOG_HELPER(CommentNode);
        public:

            CommentNode(Document* doc) : Node(doc) {
            }

            ~CommentNode() {
            }

            //

            virtual NodeType GetType() {
                return XML_COMMENT;
            }

            virtual void Print(std::ostream&, int32 depth);
            /*	starts: at the ! of the !-- */
            /*	ends:	Next char past '>' */
            virtual const char* ParseData(const char*);
        };

        class TextNode : public Node {
            NEX_LOG_HELPER(TextNode);
        public:

            TextNode(Document* doc) : Node(doc) {
            }

            ~TextNode() {
            }

            _NexInline bool IsBlank() const {
                return (value.length() == 0);
            }

            // text

            virtual NodeType GetType() {
                return XML_TEXT;
            }

            virtual void Print(std::ostream&, int32 depth);
            /* starts: first char of the text */
            /* end:		Next char past '>' */
            virtual const char* ParseData(const char*);
        };

        class PIXml : public Node {
            NEX_LOG_HELPER(PIXml);
        private:
        public:

            PIXml(Document* doc) : Node(doc) {
            }

            virtual NodeType GetType() {
                return XML_PI;
            }

            virtual void Print(std::ostream&, int32 depth);
            /*	starts: Next char past '<' */
            /*	ends: Next char past '>' */
            virtual const char* ParseData(const char*);
        };

        //@ this changes the source file altogether, works on native
        //@ file paths, relative to current path

        class IncludeNode : public Node {
            NEX_LOG_HELPER(IncludeNode);
            Node* root_element;
        private:
            void SwitchParents(Node*);
        public:

            IncludeNode(Document* doc) : Node(doc), root_element(0) {
            }

            virtual ~IncludeNode() {
            }
            //

            virtual NodeType GetType() {
                return XML_INCLUDE;
            }
            virtual void Print(std::ostream&, int32 depth);
            /*	starts: Next char past '<' */
            /*	ends: Next char past '>' */
            virtual const char* ParseData(const char*);
        };

        class UnknownNode : public Node {
            NEX_LOG_HELPER(UnknownNode);
        private:
        public:

            UnknownNode(Document* doc) : Node(doc) {
            }

            virtual ~UnknownNode() {
            }

            //

            virtual NodeType GetType() {
                return XML_UNKNOWN;
            }
            virtual void Print(std::ostream&, int32 depth);
            /*	starts: Next char past '<' */
            /*	ends: Next char past '>' */
            virtual const char* ParseData(const char*);
        };

        class ConditionBlock : public Node {
            NEX_LOG_HELPER(ConditionBlock);
            typedef vector<Node*>::type NodePtrArray;
            bool is_true;
            NodePtrArray added_nodes;
        public:

            ConditionBlock(Document* doc) : Node(doc), is_true(false) {
            }

            virtual ~ConditionBlock() {
            }

            virtual NodeType GetType() {
                return XML_CONDITION_BLOCK;
            }
            virtual void Print(std::ostream&, int32 depth);
            virtual const char* ParseData(const char*);

            _NexInline bool IsTrueNode() {
                return is_true;
            }

            void SetAsTrueNode();
            void SetAsFalseNode();
        };

        class ConditionNode : public Node {
            NEX_LOG_HELPER(ConditionNode);
            ConditionBlock* true_node;
        public:

            ConditionNode(Document* doc) : Node(doc), true_node(0) {
            }

            virtual ~ConditionNode() {
            }
            //

            virtual NodeType GetType() {
                return XML_CONDITION;
            }
            const char* ParseBlocks(const char*);

            _NexInline ConditionBlock* GetTrueNode() {
                return true_node;
            }
            void Evaluate(ConditionCallback*);
        protected:
            virtual void Print(std::ostream&, int32 depth);
            /*	starts: Next char past '<' */
            /*	ends: Next char past '>' */
            virtual const char* ParseData(const char*);
        };

        template <const MemoryCatagory catag>
        static char* TryLoad(InputStreamPtr& p) {
            size_t s = static_cast<size_t> (p->GetSize() - p->Tell());
            char* buffp = static_cast<char*> (NEX_ALLOC(s + 1, catag));
            try {
                s = p->Read(buffp, s);
                buffp[s] = 0;
            } catch (Exception& e) {
                NEX_FREE(buffp, catag);
                throw e;
            }
            return buffp;
        }

        // ----------- NODE -------------

        Node::~Node() {
            Iterator it = FirstChildIt();
            Node::Iterator nextit;
            while (it.IsOk()) {
                nextit = it.Next();
                NEX_DELETE it.data;
                it = nextit;
            }
        }

        const char* Node::ReadValue(const char *p, const char *endtag) {
            // Read in text and elements in any order.
            p = StringUtils::EatWhite(p);
            while (p && *p) {
                if (StringEqual(p, endtag, false))
                    return p;
                if (*p != '<') {
                    // Take what we have, make a text element.
                    TextNode* tn = NEX_NEW TextNode(document);

                    if (!tn)
                        return 0;

                    p = tn->ParseData(p);

                    if (!tn->IsBlank())
                        AddChildLast(tn);
                    else
                        NEX_DELETE tn;
                } else {
                    // We hit a '<'
                    // Have we hit a new element or an end tag?
                    Node* node = ParseNodeType(p);
                    if (node) {
                        p = node->ParseData(p);
                        AddChildLast(node);
                    }
                    else
                        return 0;
                }
                p = StringUtils::EatWhite(p);
            }
            return p;
        }

        Node* Node::ParseNodeType(const char *p) {
            Node* returnNode = 0;

            p = StringUtils::EatWhite(p);

            if (!p || !*p || *p != '<')
                return 0;

            // What is this thing?
            // - Elements Start with a letter or underscore, but xml is reserved.
            // - Comments: <!--
            // - Decleration: <?xml
            // - Everthing else is unknown to tinyxml.
            //
            const char* commentHeader = "<!--";
            const char* ifHeader = "<?if";
            const char* incHeader = "<?include";
            const char* piXml = "<?";

            if (isalpha(*(p + 1)) || *(p + 1) == '_')
                returnNode = NEX_NEW Element(document);
            else if (StringEqual(p, commentHeader, false))
                returnNode = NEX_NEW CommentNode(document);
            else if (StringEqual(p, ifHeader, false))
                returnNode = NEX_NEW ConditionNode(document);
            else if (StringEqual(p, incHeader, false))
                returnNode = NEX_NEW IncludeNode(document);
            else if (StringEqual(p, piXml, true))
                returnNode = NEX_NEW PIXml(document);
            else
                returnNode = NEX_NEW UnknownNode(document);
            if (returnNode)
                // Set the parent, so it can report errors
                returnNode->parent = this;
            return returnNode;
        }

        void Node::AddChildLast(Node* n) {
            NEX_ASSERT(n);
            n->parent = this;
            n->_prevptr = lastChild;
            if (lastChild)
                lastChild->_nextptr = n;
            lastChild = n;
            if (!firstChild)
                firstChild = lastChild;
        }

        void Node::AddChildFirst(Node* n) {
            NEX_ASSERT(n);
            n->parent = this;
            n->_nextptr = firstChild;
            if (firstChild)
                firstChild->_prevptr = n;
            firstChild = n;
            if (!lastChild)
                lastChild = firstChild;
        }

        void Node::RemoveChild(Node* n) {
            NEX_ASSERT(n);

            if (n->parent == this) {
                n->parent = 0;
                Iterator it = FirstChildIt();
                while (it.IsOk()) {
                    if (it.data == n) {
                        RemoveChild(it);
                        return;
                    }
                    ++it;
                }
            }
        }

        Node::Iterator Node::RemoveChild(Node::Iterator it) //@ @todo: check this intresting method.
        {
            Node* prev = (Node*) (*it.Prev());
            Node* next = (Node*) (*it.Next());
            if (prev)
                prev->_nextptr = next;
            if (next)
                next->_prevptr = prev;
            if (*it == lastChild)
                lastChild = prev;
            if (*it == firstChild)
                firstChild = next;
            return Node::Iterator(next);
        }

        Node* Node::FindChild(const char* sName) {
            Iterator it = FirstChildIt();
            while (it.IsOk()) {
                const char* name = (*it)->GetValue().c_str();
                if (name && !StringUtils::NoCaseCompare(name, sName))
                    return (*it);
                ++it;
            }
            return 0;
        }

        String Node::ReadTextNode() {
            Node* it = FirstChild();
            if (it && it->GetType() == XML_TEXT) {
                return it->GetValue();
            }
            return StringUtils::Null;
        }

        const String& Node::GetText(const char* forelment, const String& defaultValue) {
            Node* it = FindChild(forelment);
            if (it && it->GetType() == XML_ELEMENT) {
                xml::Element* e = static_cast<xml::Element*> (it);
                return e->GetEnclosedText();
            }
            return defaultValue;
        }

        void Node::EvalConditionNodes(ConditionCallback* cbk) {
            Iterator it = FirstChildIt();
            // nodes will be appended as the evaluation proceeds
            // but thats ok..as they will be appended at the end
            // of the list
            while (it.IsOk()) {
                if ((*it)->GetType() == XML_CONDITION) {
                    ConditionNode* n = static_cast<ConditionNode*> ((*it));
                    n->Evaluate(cbk);
                }
                ++it;
            }
        }

        // ----------- ELEMENT -------------

        const String& Element::GetEnclosedText() {
            Node* nx = FirstChild();
            if (nx && nx->GetType() == XML_TEXT)
                return nx->GetValue();
            return StringUtils::Null;
        }

        const String& Element::GetAttributeValue(const char* name, const String& defaultValue) {
            NameValueMap::iterator ret = attributes.find(String(name));
            return (ret != attributes.end()) ? (*ret).second : defaultValue;
        }

        void Element::AddAttribute(const char* name, const char* value) {
            attributes.insert(NameValuePair(String(name), String(value)));
        }

        void Element::RemoveAttribute(const char* name) {
            attributes.erase(String(name));
        }

        Element* Element::AddTextNode(const char* element, const char* text) {
            xml::Element* ret = NEX_NEW Element(document);
            xml::TextNode* t = NEX_NEW xml::TextNode(document);
            ret->SetValue(element);
            t->SetValue(text);
            ret->AddChildLast(t);
            AddChildLast(ret);
            return ret;
        }

        // ----------- CONDITION BLOCK -------------

        void ConditionBlock::SetAsTrueNode() {
            NEX_ASSERT(!(static_cast<ConditionNode*> (parent)->GetTrueNode()));
            NEX_ASSERT(!is_true);
            is_true = true;
            Iterator it = FirstChildIt();
            Node* pre_parent = parent->GetParent();
            NEX_ASSERT(pre_parent);
            while (it.IsOk()) {
                Node* n = *it;
                it = RemoveChild(it);
                pre_parent->AddChildLast(n);
                n->SetPrintable(false);
                added_nodes.push_back(n);
            }
        }

        void ConditionBlock::SetAsFalseNode() {
            NEX_ASSERT(!(static_cast<ConditionNode*> (parent)->GetTrueNode()));
            NEX_ASSERT(is_true);
            is_true = false;
            Node* pre_parent = parent->GetParent();
            NEX_ASSERT(pre_parent);
            for (size_t i = 0; i < added_nodes.size(); ++i) {
                pre_parent->RemoveChild(added_nodes[i]);
                AddChildLast(added_nodes[i]);
                added_nodes[i]->SetPrintable(true);
            }
            added_nodes.clear();
        }

        // ----------- CONDITION NODE -------------

        void ConditionNode::Evaluate(ConditionCallback* cbk) {
            Iterator it = FirstChildIt();
            while (it.IsOk()) {
                const String& cond = (*it)->GetValue();
                bool res = false;
                if (cond.length() > 0)
                    res = cbk->EvalCondition(cond);
                else
                    res = true;
                ConditionBlock* b = static_cast<ConditionBlock*> (*it);
                if (b->IsTrueNode()) {
                    if (!res)
                        b->SetAsFalseNode();
                } else {
                    if (res)
                        b->SetAsTrueNode();
                }
            }
        }

        // ----------- DOCUMENT -------------

        Document::Document() : Node(0) {
            document = this;
        }

        Document::~Document() {
        }

        Element* Document::GetFirstElement() {
            Node* n = FirstChild();
            if (n && n->GetType() == XML_ELEMENT)
                return static_cast<Element*> (n);
            return 0;
        }

        void Document::Load(InputStreamPtr& stream) {
            ScopedPtr< char, DisposerFree<char, AllocatorGeneral> > data(TryLoad< MEMCAT_GENERAL > (stream));
            if (!ParseData(data))
                NEX_THROW_GracefulError(EXCEPT_COMPILATION_FAILED);
        }

        void Document::Parse(const char* data) {
            if (!ParseData(data))
                NEX_THROW_GracefulError(EXCEPT_COMPILATION_FAILED);
        }

        void Document::Save(OutputStreamPtr& outfile) {
            OutStringStream out;
            Print(out, 0);
            outfile->Write(out.str().c_str(), out.str().length());
        }

        void Document::EvalConditionNodes(ConditionCallback* nodes) {
            Node::EvalConditionNodes(nodes);
        }

        void Document::Print(std::ostream& out, int32 depth) {
            Node::Iterator it = FirstChildIt();
            while (it.IsOk()) {
                it->Print(out, depth);
                ++it;
            }
        }

        const char* Document::ParseData(const char* data) {
            if (!data || !*data)
                return 0;

            data = StringUtils::EatWhite(data);

            if (!data || !*data)
                return 0;

            while (data && *data) {
                Node* n = ParseNodeType(data);
                if (n) {
                    data = n->ParseData(data);
                    AddChildLast(n);
                } else
                    return 0;
                data = StringUtils::EatWhite(data);
            }
            return data;
        }
        //------------ Parsing -----------------

        void PrintAttribute(std::ostream& writer, const String& name, const String& value) {
            String out_name, out_value;
            OutStringStream s;

            FormatString(name, out_name);
            FormatString(value, out_value);

            if (out_value.find_first_of('\"') == String::npos)
                writer << out_name << "=\"" << out_value << "\"";
            else
                writer << out_name << "=\'" << out_value << "\'";
        }

        const char* ParseAttributeData(const char * parse_data, String& name, String& value) {
            parse_data = StringUtils::EatWhite(parse_data);
            if (!parse_data || !*parse_data)
                return 0;
            // Read the name, the '=' and the value.
            parse_data = ReadName(parse_data, name);
            if (!parse_data || !*parse_data)
                return 0;
            parse_data = StringUtils::EatWhite(parse_data);

            if (!parse_data || !*parse_data || *parse_data != ('='))
                return 0;

            ++parse_data; // skip '='
            parse_data = StringUtils::EatWhite(parse_data);

            if (!parse_data || !*parse_data)
                return 0;

            if (*parse_data == ('\'')) {
                ++parse_data;
                parse_data = ReadText(parse_data, value, "\'", false);
            } else if (*parse_data == ('\"')) {
                ++parse_data;
                parse_data = ReadText(parse_data, value, "\"", false);
            } else {
                // All attribute values should be in single or double quotes.
                // But this is such a common error that the parser will try
                // its best, even without them.
                value = StringUtils::Null;
                while (parse_data && *parse_data // existence
                        && !isspace(*parse_data) && // whitespace
                        *parse_data != '/' && *parse_data != '>') // tag end
                {
                    value += *parse_data;
                    ++parse_data;
                }
            }

            return parse_data;
        }

        const char* Element::ParseData(const char *p) {
            p = StringUtils::EatWhite(p);

            if (!p || !*p || *p != '<')
                return 0;

            p = StringUtils::EatWhite(p + 1);
            // Read the name.
            p = ReadName(p, value);

            if (!p || !*p)
                return 0;

            String endTag(("</"));
            endTag.append(value);
            endTag.append(">");

            // Check for and read attributes. Also look for an empty
            // tag or an end tag.
            while (p && *p) {
                p = StringUtils::EatWhite(p);
                if (!p || !*p)
                    return 0;
                if (*p == ('/')) {
                    ++p;
                    // Empty tag.
                    if (*p != ('>'))
                        return 0;
                    return (p + 1);
                } else if (*p == ('>')) {
                    // Done with attributes (if there were any.)
                    // Read the value -- which can include other
                    // elements -- read the end tag, and return.
                    ++p;
                    p = ReadValue(p, "</"); // Note this is an Element method, and will set the error if one happens.
                    if (!p || !*p)
                        return 0;

                    // We should Find the end tag now
                    if (StringEqual(p, endTag.c_str(), false)) {
                        p += endTag.length();
                        return p;
                    } else
                        return 0;
                } else {
                    // Try to read an element:
                    String atr_name, atr_value;
                    p = ParseAttributeData(p, atr_name, atr_value);

                    if (!p || !*p)
                        return 0;
                    attributes.insert(NameValuePair(atr_name, atr_value));
                }
            }
            return p;
        }

        void Element::Print(std::ostream& writer, int32 depth) {
            String Indent = StringUtils::Indent(depth);
            writer << Indent
                    << "<" << value;

            for (NameValueMap::iterator it = attributes.begin(); it != attributes.end(); ++it) {
                writer << " ";
                PrintAttribute(writer, (*it).second, (*it).first);
            }
            // There are 3 different formatting approaches:
            // 1) An element without children is printed as a <foo /> node
            // 2) An element with only a text child is printed as <foo> text </foo>
            // 3) An element with children is printed on multiple lines.
            if (!firstChild) {
                writer << " />";
            } else if (firstChild == lastChild && firstChild->GetType() == XML_TEXT) {
                writer << ">";
                firstChild->Print(writer, depth + 1);
                goto endtag;
            } else {
                writer << ">";
                for (Node::Iterator it = FirstChildIt(); it.IsOk(); ++it) {
                    if (!(*it)->GetPrintable())
                        continue;
                    if ((*it)->GetType() != XML_TEXT)
                        writer << std::endl;
                    (*it)->Print(writer, depth + 1);
                }

                writer << std::endl;

endtag:
                writer << Indent << "</" << value << ">";
            }
        }

        const char* CommentNode::ParseData(const char *p) {

            p = StringUtils::EatWhite(p);
            const char* startTag = "<!--";
            const char* endTag = "-->";

            if (!StringEqual(p, startTag, false))
                return 0;
            p += 4;
            p = ReadText(p, value, endTag, false);
            return p;
        }

        void CommentNode::Print(std::ostream& writer, int32 depth) {
            writer << StringUtils::Indent(depth)
                    << "<!--" << value << "-->";
        }

        const char* TextNode::ParseData(const char *p) {
            const char* end = "<";
            p = ReadText(p, value, end, false);
            if (p)
                return p - 1; // don't truncate the '<'
            else
                return 0;
        }

        void TextNode::Print(std::ostream& writer, int32) {
            String out;
            FormatString(value, out);
            writer << out;
        }

        const char* PIXml::ParseData(const char *p) {
            p = StringUtils::EatWhite(p);
            // Find the beginning, Find the end, and look for
            // the stuff in-between.
            if (!p || !*p || !StringEqual(p, ("<?xml"), true))
                return 0;
            p += 2;
            value = "";
            while (p && *p) {
                if (*p == ('>')) {
                    ++p;
                    return p;
                }
                value += *p;
            }
            return 0;
        }

        void PIXml::Print(std::ostream& writer, int32 depth) {
            writer << StringUtils::Indent(depth) << "<?" << value << "?>";
        }

        const char* UnknownNode::ParseData(const char *p) {
            p = StringUtils::EatWhite(p);
            if (!p || !*p || *p != ('<'))
                return 0;
            ++p;
            value = "";

            while (p && *p && *p != '>') {
                value += *p;
                ++p;
            }

            if (!p)
                return 0;

            if (*p == ('>'))
                return p + 1;
            return p;
        }

        void UnknownNode::Print(std::ostream& writer, int32 depth) {
            writer << StringUtils::Indent(depth) << "<" << value << ">";
        }


        //@************************************************************************@//

        void ConditionBlock::Print(std::ostream& out, int32 depth) {
            for (size_t i = 0; i < added_nodes.size(); ++i) {
                added_nodes[i]->Print(out, depth);
            }
            for (Node::Iterator it = FirstChildIt(); it.IsOk(); ++it) {
                if ((*it)->GetType() != XML_TEXT)
                    out << std::endl;
                (*it)->Print(out, depth);
            }
        }

        const char* ConditionBlock::ParseData(const char* p) {
            // Read in text and elements in any order.
            p = StringUtils::EatWhite(p);
            while (p && *p) {
                if (StringEqual(p, "<?else", false) ||
                        StringEqual(p, "<?elif", false) ||
                        StringEqual(p, "<?endif", false))
                    break;
                if (*p != '<') {
                    // Take what we have, make a text element.
                    TextNode* tn = NEX_NEW TextNode(document);
                    if (!tn)
                        return 0;

                    p = tn->ParseData(p);

                    if (!tn->IsBlank())
                        AddChildLast(tn);
                    else
                        NEX_DELETE tn;
                } else {
                    // We hit a '<'
                    // Have we hit a new element or an end tag?
                    Node* node = ParseNodeType(p);
                    if (node) {
                        p = node->ParseData(p);
                        AddChildLast(node);
                    }
                    else
                        return 0;
                }
                p = StringUtils::EatWhite(p);
            }
            return p;
        }

        const char* ConditionNode::ParseBlocks(const char *p) {
            // Read in text and elements in any order.
            bool elseblock = false;
            while (p && *p) {
                p = StringUtils::EatWhite(p);
                if (StringEqual(p, "<?endif", false))
                    return p;
                if (StringEqual(p, "<?if", false)) {
                    p += 4;
                } else if (StringEqual(p, "<?elif", false)) {
                    p += 6;
                } else if (StringEqual(p, "<?else", false)) {
                    p += 6;
                    elseblock = true;
                    p = StringUtils::EatWhite(p);
                    if (!StringEqual(p, "?>", false))
                        return 0;
                } else
                    return 0;
                String condition = StringUtils::Null;
                if (!elseblock) {
                    p = StringUtils::EatWhite(p);
                    while (*p && !StringEqual(p, "?>", false))
                        condition += *p;
                    StringUtils::TrimTrailing(condition);
                }
                // trim condition
                ConditionBlock* b = NEX_NEW ConditionBlock(document);
                b->SetValue(condition);
                AddChildLast(b);
                p = b->ParseData(p + 2);
            }
            return p;
        }

        const char* ConditionNode::ParseData(const char* p) {
            p = StringUtils::EatWhite(p);
            // Find the beginning, Find the end, and look for
            // the stuff in-between.
            if (!p || !*p || !StringEqual(p, ("<?if"), true))
                return 0;

            p = ParseBlocks(p);

            if (!p || !*p)
                return 0;

            if (StringEqual(p, "<?endif", true)) {
                p += 7;
                while (p && *p && *p != ('>') && !isspace(*p))
                    ++p;
            } else
                return 0;
            return p;
        }

        void ConditionNode::Print(std::ostream& out, int32 depth) {
            String Indent = StringUtils::Indent(depth);

            Iterator it = FirstChildIt();
            bool first = true;
            while (it.IsOk()) {
                const String& cond = (*it)->GetValue();
                if (first)
                    out << "<?if ";
                else if (cond.length() > 0)
                    out << "<?elif ";
                else
                    out << "<?else ";
                out << cond << " ?>" << std::endl;
                (*it)->Print(out, depth);
            }
            out << std::endl << "<?endif ?>";
        }
        //@************************************************************************@//
        //@IncludeNode**********************************************************@//

        void IncludeNode::SwitchParents(Node* make_par) {
            Node* cur_par = root_element->GetParent();
            if (cur_par == make_par)
                return;
            cur_par->RemoveChild(root_element);
            make_par->AddChildLast(root_element);
        }

        const char* IncludeNode::ParseData(const char* p) {
            p = StringUtils::EatWhite(p);
            // Find the beginning, Find the end, and look for
            // the stuff in-between.
            if (!p || !*p || !StringEqual(p, ("<?include"), true))
                return 0;
            p += 9;

            value = "";

            while (*p && *p != '?')
                value += *p;

            StringUtils::TrimLeading(value);
            StringUtils::TrimTrailing(value);

            while (*p && *p != '>')
                p++;

            if (!p || !*p)
                return 0;
            p++;
            {
                std::ifstream f;
                f.open(value.c_str());
                if (!f.is_open())
                    Warn(String("XML Document referred in include node was not found: ") + value);
                else {
                    GenericInputStream strm(&f);
                    InputStreamPtr inp_ptr = Assign< InputStream >(&strm);
                    ScopedPtr< char, DisposerFree<char, AllocatorGeneral> > newp(
                    TryLoad<MEMCAT_GENERAL > (inp_ptr));
                    if (newp)
                        ReadValue(newp, "");
                    Node* n = FirstChild();
                    if (n) {
                        if (n->Next()) {
                            Error(String("XML Document in include node needs a single root: ") + value);
                            return 0;
                        }
                        root_element = n;
                        SwitchParents(parent);
                    }
                }
            }
            return p;
        }

        void IncludeNode::Print(std::ostream& writer, int32 depth) {
            Document doc;
            SwitchParents(&doc);
            writer << StringUtils::Indent(depth) << "<?include " << value << " ?>";
            std::ofstream f;
            f.open(value.c_str());
            doc.Print(f, 0);
        }

        //@************************************************************************@//

        struct BasicEntity {
            const char* str;
            int32 len;
            char c;
        };
        static BasicEntity entities[] ={
            { ("&amp;"), 5, '&'},
            { ("&lt;"), 4, '<'},
            { ("&gt;"), 4, '>'},
            { ("&quot;"), 6, '\"'},
            { ("&apos;"), 6, '\''}
        };
        const int32 NUM_ENTITY = sizeof (entities) / sizeof (entities[0]);


        //@globals ****************************************************************@//

        static void FormatString(const String& in, String& out) {
            size_t i = 0;

            while (i < in.length()) {
                char c = in[i];

                if (c == ('&') && i < (in.length() - 2) && in[i + 1] == ('#') && in[i + 2] == ('x')) {
                    // Hexadecimal character reference.
                    // Pass through unchanged.
                    // &#xA9;	-- copyright symbol, for example.
                    while (i < in.length()) {
                        out.append(in.c_str() + i, 1);
                        ++i;
                        if (in[i] == (';'))
                            break;
                    }
                } else {
                    bool found = false;
                    for (int32 j = 0; j < NUM_ENTITY; j++) {
                        if (entities[j].c == c) {
                            out.append(entities[j].str, entities[j].len);
                            ++i;
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        if (c < 32 || c > 126) {
                            // Easy pass at non-alpha/numeric/symbol
                            // 127 is the delete key. Below 32 is symbolic.
                            char buf[ 32 ];
#if defined(NEX_MSVC)
                            sprintf_s(buf, 32, "&#x%02X;", (unsigned) (c & 0xff));
#else
							sprintf(buf, "&#x%02X;", (unsigned) (c & 0xff));
#endif
                            out.append(buf, (int32) StringUtils::Length(buf));
                            ++i;
                        } else {
                            char realc = (char) c;
                            out.append(&realc, 1);
                            ++i;
                        }
                    }
                }
            }
        }

        static const char* ReadName(const char* parse_data, String& name_str) {
            name_str = "";
            NEX_ASSERT(parse_data);

            // Names Start with letters or underscores.
            // After that, they can be letters, underscores, numbers,
            // hyphens, or colons. (Colons are valid ony for namespaces,
            // but tinyxml can't tell namespaces from names.)
            if (parse_data && *parse_data && (isalpha((char) *parse_data) || *parse_data == ('_'))) {
                while (*parse_data && (isalnum((char) *parse_data)
                        || *parse_data == ('_') || *parse_data == ('-') || *parse_data == (':'))) {
                    name_str += *parse_data;
                    ++parse_data;
                }
                return parse_data;
            }
            return 0;
        }

        static bool StringEqual(const char* p, const char* tag, bool ignore_case) {
            NEX_ASSERT(p);
            if (!*p)
                return false;

            if (_tolower(*p) == _tolower(*tag)) {
                const char* q = p;

                if (ignore_case) {
                    while (*q && *tag && *q == *tag) {
                        ++q;
                        ++tag;
                    }

                    if (*tag == 0) // Have we found the end of the tag, and everything equal?
                    {
                        return true;
                    }
                } else {
                    while (*q && *tag && _tolower(*q) == _tolower(*tag)) {
                        ++q;
                        ++tag;
                    }

                    if (*tag == 0) {
                        return true;
                    }
                }
            }
            return false;
        }

        static const char* GetEntity(const char* p, char* value) {
            // Presume an entity, and pull it out.
            String ent;
            int32 i;

            // Ignore the &#x entities.
            if (strncmp("&#x", p, 3) == 0 && *(p + 3) && *(p + 4)) {
                *value = 0;

                if (isalpha(*(p + 3)))
                    *value = (char) (*value + ((_tolower(*(p + 3)) - ('a') + 10) * 16));
                else
                    *value = (char) (*value + (*(p + 3) - ('0')) * 16);

                if (isalpha(*(p + 4)))
                    *value = (char) (*value + (_tolower(*(p + 4)) - ('a') + 10));
                else
                    *value = (char) (*value + (*(p + 4) - ('0')));

                return p + 6;
            }

            // Now try to match it.
            for (i = 0; i < NUM_ENTITY; ++i) {
                if (strncmp(entities[i].str, p, entities[i].len) == 0) {
                    *value = entities[i].c;
                    return ( p + entities[i].len);
                }
            }

            // So it wasn't an entity, its unrecognized, or something like that.
            *value = *p; // Don't put back the last one, since we return it!
            return p + 1;
        }

        _NexInline static const char* GetChar(const char* p, char* value) {
            NEX_ASSERT(p);
            if (*p == ('&'))
                return GetEntity(p, value);
            else
                *value = *p;
            return p + 1;
        }

        static const char* ReadText(const char* parse_data, String &text_str, const char* end_tag, bool ignore_case) {
            text_str = "";
            while (parse_data && *parse_data && !StringEqual(parse_data, end_tag, ignore_case)) {
                char c;
                parse_data = GetChar(parse_data, &c);
                text_str += c;
            }
            return parse_data + StringUtils::Length(end_tag);
        }

    }
}


#ifndef NEXTAR_NEXXML_H
#define NEXTAR_NEXXML_H

#include <NexBase.h>

namespace nextar {

namespace xml {
class Node;
class Element;
class PIXml;
class Document;

enum NodeType {
	XML_ELEMENT,
	XML_COMMENT,
	XML_TEXT,
	XML_UNKNOWN,
	XML_DOCUMENT,
	XML_CONDITION,
	XML_INCLUDE,
	XML_CONDITION_BLOCK,
	XML_PI,
};

class _NexNoVtable ConditionCallback {
public:
	virtual bool EvalCondition(const String&) = 0;
protected:
	~ConditionCallback() {
	}
};

class _NexBaseAPI Node: public AllocGeneral {
	NEX_LOG_HELPER (Node);
public:

	enum {
		TYPE = XML_UNKNOWN
	};
public:

	/** Ctor/Dtor */
	_NexInline Node(Document* doc) :
			document(doc), _nextptr(0), _prevptr(0), firstChild(0), lastChild(
					0), parent(0), printable(true) {
	}

	virtual ~Node();

	/** Node Travarsal */
	struct Iterator {
		Node* data;

		_NexInline Iterator & operator ++() {
			if (data)
				data = data->Next();
			return *this;
		}

		_NexInline Iterator & operator --() {
			if (data)
				data = data->Prev();
			return *this;
		}

		_NexInline Node * operator *() {
			return data;
		}

		_NexInline Node * operator ->() {
			return data;
		}

		_NexInline bool operator !() {
			return data == NULL;
		}

		_NexInline Iterator Next() const {
			if (data)
				return Iterator(data->Next());
			return Iterator();
		}

		_NexInline Iterator Prev() const {
			if (data)
				return Iterator(data->Prev());
			return Iterator();
		}

		_NexInline bool IsOk() {
			return data != NULL;
		}

		_NexInline Iterator(Node * nd) :
				data(nd) {
		}

		_NexInline Iterator() :
				data(0) {
		}
	};

	_NexInline Iterator NextIt() {
		return Iterator(Next());
	}

	_NexInline Iterator PrevIt() {
		return Iterator(Prev());
	}

	_NexInline Iterator ThisIt() {
		return Iterator(this);
	}

	_NexInline Iterator FirstChildIt() {
		return Iterator(FirstChild());
	}

	_NexInline Iterator NextChildIt(Iterator it) {
		return ++it;
	}

	_NexInline Iterator PrevChildIt(Iterator it) {
		return --it;
	}

	_NexInline Iterator LastChildIt() {
		return Iterator(LastChild());
	}

	_NexInline Node* Next() {
		return _nextptr;
	}

	_NexInline Node* Prev() {
		return _prevptr;
	}

	_NexInline Node* FirstChild() {
		return firstChild;
	}

	_NexInline Node* LastChild() {
		return lastChild;
	}

	/* General functions */
	_NexInline void SetValue(const char* v) {
		value = v;
	}

	_NexInline void SetValue(const String& v) {
		value = v;
	}

	_NexInline const String& GetValue() {
		return value;
	}

	_NexInline Node* GetParent() {
		return parent;
	}

	_NexInline void SetPrintable(bool p) {
		printable = p;
	}

	_NexInline bool GetPrintable() {
		return printable;
	}
	virtual NodeType GetType() = 0;

	void AddChildLast(Node*);
	void AddChildFirst(Node*);
	void RemoveChild(Node*);
	Iterator RemoveChild(Iterator); //@ todo: check this intresting method.
	Node* FindChild(const char*);
	String ReadTextNode();

	/* Specialized functions */
	_NexInline Document* GetDocument() {
		return document;
	}

	const String& GetText(const char* forelment, const String& defaultValue =
			StringUtils::Null);

protected:
	virtual void EvalConditionNodes(ConditionCallback*);
public:

	virtual void Print(std::ostream&, int32 depth) = 0;
	virtual const char* ParseData(const char*) = 0;
	virtual const char* ReadValue(const char*, const char* endtag);
	virtual Node* ParseNodeType(const char*);

	template<typename T>
	friend _NexInline T* SafeCast(Node* node) {
		if (node && node->GetType() == T::TYPE)
			return static_cast<T*>(node);
		return 0;
	}

protected:
	bool printable;
	String value;
	Node *_nextptr, *_prevptr;
	Node *firstChild, *lastChild;
	Node *parent;
	Document *document;
};

class _NexBaseAPI Element: public Node {
	NEX_LOG_HELPER (Element);
	NameValueMap attributes;
public:

	enum {
		TYPE = XML_ELEMENT
	};
public:

	_NexInline Element(Document* doc) :
			Node(doc) {
	}
	/** @remarks For a single text element objects get the only enclosed text **/
	const String& GetEnclosedText();

	_NexInline size_t GetAttributeCount() {
		return attributes.size();
	}

	/** @remarks Returns the attribute value, else StringUtils::Null if it is not present **/
	const String& GetAttributeValue(const char*, const String& defaultValue =
			StringUtils::Null);
	void AddAttribute(const char* name, const char* value);
	void RemoveAttribute(const char* name);

	/** @remarks Retrieve the attribute list **/
	_NexInline const NameValueMap& GetAllAttributes() const {
		return attributes;
	}
	/** @remarks This will Add a new element named 'element'
	 *           and Add text to
	 **/
	Element* AddTextNode(const char* element, const char* text);

	/* Node Specific */
	virtual NodeType GetType() {
		return XML_ELEMENT;
	}

protected:

	/* Node Specific */
	virtual void Print(std::ostream&, int32 depth);
	virtual const char* ParseData(const char*);

};

class _NexBaseAPI Document: public Node {
	NEX_LOG_HELPER (Document);bool parsed;
public:

	enum {
		TYPE = XML_DOCUMENT
	};
public:
	Document();
	~Document();

	Element* GetFirstElement();
	/** @remarks Evaluate all condition nodes **/
	void EvalConditionNodes(ConditionCallback*);
	/** @remarks Throws error if failed **/
	void Load(InputStreamPtr&);
	/** @remarks Throws error if failed **/
	void Parse(const char*);
	void Save(OutputStreamPtr&);

	/* Node Specific */
	virtual NodeType GetType() {
		return XML_DOCUMENT;
	}

protected:

	friend class IncludeNode;
	/* Node Specific */
	virtual void Print(std::ostream&, int32 depth);
	virtual const char* ParseData(const char*);

};
}
}

#endif //NEXTAR_NEXXML_H

#ifndef _wRnRXML_HG_
#define _wRnRXML_HG_

// RnRXML (Good enough for Rock-n-Roll XML library)
// Version 1.5.0 (July 2008)
// http://www.badmonster.com/RnRXML/RnRXML.aspx
//
// This software is released under the lesser GPL agreement: http://www.gnu.org/licenses/lgpl.html
// Basically, you may use this as you wish, but at your own risk. 
// (It would be lovely for me to know it was being used. Postcards or e-mails are nice.)
//
// Written by Michael Feeney, London, Ontario, Canada
// Comments, bugs, etc. to: RnRXML@badmonster.com
//

#include <string>
#include <fstream>
#include <vector>

namespace RnRXML
{
	enum enumRnRNodeTypes
	{
		RnR_TYPE_ELEMENT,
		RnR_TYPE_COMMENT,
		RnR_TYPE_HEADER,
		RnR_TYPE_PROCESSING,	// NOTE: This ISN'T the HEADER node
		RnR_TYPE_UNKNOWN
	};

	enum enumTagType
	{
		RnR_START_TAG,
		RnR_END_TAG,
		RnR_STARTEND_TAG,
		RnR_COMMENT_TAG,
		RnR_UNKNOWN_TAG,
		RnR_PROCESSING_TAG
	};

#define STANDARDXMLHEADER L"<?xml version=\"1.0\" encoding=\"utf-8\"?>"

	class wRnRXMLAttribute
	{
	public:
		std::wstring Name;
		std::wstring Value;
	};

	class wRnRXMLNode
	{
	public:
		wRnRXMLNode();
		~wRnRXMLNode();
		std::wstring OuterXML;						// This is what was read
		std::wstring InnerXML;
		bool SaveXMLToStream(std::wostream &theStream, int currentIndent);
		std::wstring Name;
		std::vector<wRnRXMLNode*> m_p_ChildNodes;
		std::vector<wRnRXMLAttribute> m_ChildAttributes;
		enumRnRNodeTypes NodeType;
		enumTagType	TagType;
		// Getters and Setters for attributes
		bool SetAttribute(std::wstring Name, std::wstring Value);
		std::wstring GetAttribute(std::wstring Name);					// Returns empty string if can't find it
		
		// These are different versions of get attribute		
		double GetAttributeDouble(std::wstring Name, bool &bError);
		float  GetAttributeFloat(std::wstring Name, bool &bError);
		int	   GetAttributeInt(std::wstring Name, bool &bError);

		// Methods
		bool ProcessAttributes(std::wstring &ErrorLog);
		bool ProcessChildNodes(std::vector<wRnRXMLNode*> vecpChildNodes,
									  std::vector<wRnRXMLNode*> &vecpRemainingNodes,
									  std::wstring &ErrorLog);
		std::wstring GetLastError(void);
		void ClearNode(void);	// This sets everything back to an unknown state
		// Getters and Setters
		enumRnRNodeTypes GetNodeType(void);
		//
		void SetValue(std::wstring newValue);
		void SetValueDouble(double newValue);
		void SetValueFloat(float newValue);
		void SetValueInt(float newValue);
		std::wstring GetValue(void);
		double GetValueDouble(void);
		float GetValueFloat(void);
		int GetValueInt(void);
		
		
	private:
		//
		std::wstring m_TextValue;	// This only has something if its a start tag with a matching
							// end tag and there is only text in between.
		//
		std::wstring m_LastError;
		std::wstring m_ErrorLog;
		void m_AppendErrorText(std::wstring theText, std::wstring &theErrorLog);
		//
		std::wstring m_GenerateOuterXML(int currentIndent);	// This generates the outer XML from the innter info
		std::wstring m_GenerateAttributeString(void);
		std::wstring m_GenerateTabString(int currentIndent);
		// 
		std::wstring m_CopyFromIndexToIndex(std::wstring theText, std::wstring::size_type start, std::wstring::size_type end);
		std::wstring m_TrimBothEnds(std::wstring theString, std::wstring charToTrim);
		std::wstring m_TrimBothEnds(std::wstring theString);
		std::wstring m_ClearNonSpaceWhiteSpace(std::wstring theString);


	};

	// This is a single step through the XPath thing when it's being parsed
	class wXPathStep
	{
	public:
		wXPathStep();
		std::wstring TagName;			// This is the name of the particular tag
		bool bTagIsAttribute;
		std::wstring PredicateLHS;			// This is size[position()=1] or planet[2], etc.
		bool bPredicateLHSIsAttribute;
		std::wstring PredicateOperator;
		std::wstring PredicateRHS;
		bool bIsNodeTest;				// This is text(), etc.
	};

	class wRnRXMLDOM
	{
	public:
		wRnRXMLDOM();
		~wRnRXMLDOM();
		bool LoadXMLFile(std::wstring fileName, std::wstring &ErrorLog);
		bool SaveXMLFile(std::wstring fileName, std::wstring &ErrorLog);
		// This executes the XPath expression and returns the node (or nodes) if possible
		bool ExecuteXPath(std::wstring XPathExpression, std::vector<wRnRXMLNode*> &vec_pNodes);
		bool ExecuteXPathSingleNode(std::wstring XPathExpression, wRnRXMLNode& theNode);
		// This executes the XPath expression and returns only the value of the node,
		//	if possible. NOTE: It will also calculate the value of an attribute
		//	(in the case of "/someNode/@theAttrib/text()" which is technically invalid XPath
		//	 - in XPath, you can not get the text() value of an attribute)
		bool ExecuteXPathScalar(std::wstring XPathExpression,
										std::wstring &theValue);		
		// This is for creating a new XML file
		bool CreateDocument(std::wstring RootNodeName, std::wstring &ErrorLog);

		std::wstring GetLastError(void);
	private:
		std::wstring m_DocumentHeader;
		wRnRXMLNode*	m_p_DocumentNode;
		// Used when parsing the XML file
		bool m_GetNextNodeText(std::wstring &NodeText);
		std::wifstream m_theInputFile;
		bool m_bFileAlreadyLoaded;
		//std::vector<RnRXMLNode> m_TheNodes;
		//
		// Used when parsing the XPath
		bool m_ParseXPath(std::wstring XPathString, std::vector<wXPathStep> &theXPathSteps);
		bool m_ProcessNextXPathStep(std::vector<wXPathStep>::iterator itXPathStep, 
											 std::vector<wXPathStep> &theXPathSteps,
											 std::vector<wRnRXMLNode*> &vec_pNodes,
											 wRnRXMLNode* p_curNode,
											 std::vector<wRnRXMLNode*>::size_type NodePosition);
		// 
		bool m_bIsOnlyWhiteSpace(std::wstring theText);
		std::wstring m_TrimFirstCharacter(std::wstring theText);
		std::wstring m_TrimLastCharacter(std::wstring theText);
		std::wstring m_ExtractNameFromInnerXML(std::wstring innerXML);
		std::wstring m_CopyFromIndexToIndex(std::wstring theText, std::wstring::size_type start, std::wstring::size_type end);
		std::wstring m_TrimBothEnds(std::wstring theString, std::wstring charToTrim);
		std::wstring m_TrimBothEnds(std::wstring theString);
		std::wstring m_ClearNonSpaceWhiteSpace(std::wstring theString);
		std::wstring m_CleanQuotesFromString(std::wstring theString);
		
		bool m_IsValidXMLHeader(std::wstring theHeader);
		bool m_ExtractAttribute(std::wstring &theText, std::wstring attributeName, RnRXML::wRnRXMLAttribute &theAttrib);

		//
		std::wstring m_LastError;
		std::wstring m_ErrorLog;
		//
		void m_AppendErrorText(std::wstring theText, std::wstring &theErrorLog);
	};
};

#endif

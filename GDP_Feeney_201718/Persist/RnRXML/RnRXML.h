#ifndef _RnRXML_HG_
#define _RnRXML_HG_

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
		RnR_START_TAG = ( RnR_TYPE_UNKNOWN + 100 ),			// To be sure it's not overlapping the NodeType
		RnR_END_TAG,
		RnR_STARTEND_TAG,
		RnR_COMMENT_TAG,
		RnR_UNKNOWN_TAG,
		RnR_PROCESSING_TAG
	};

//#define STANDARDXMLHEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"

	class RnRXMLAttribute
	{
	public:
		std::string Name;
		std::string Value;
	};

	class RnRXMLNode
	{
	public:
		RnRXMLNode();
		~RnRXMLNode();
		std::string OuterXML;						// This is what was read
		std::string InnerXML;
		bool SaveXMLToStream(std::ostream &theStream, int currentIndent, bool bMakePretty);
		std::string Name;
		std::vector<RnRXMLNode*> m_p_ChildNodes;
		std::vector<RnRXMLAttribute> m_ChildAttributes;
		enumRnRNodeTypes NodeType;
		enumTagType	TagType;
		// Getters and Setters for attributes
		bool SetAttribute(std::string Name, std::string Value);	
		bool AddOrOverwriteAttribute(std::string name, std::string Value);	// Added: October 20, 2015
		// Returns empty string if can't find it
		std::string GetAttribute(std::string Name);					
		
		// These are different versions of get attribute		
		double GetAttributeDouble(std::string Name, bool &bError);
		float  GetAttributeFloat(std::string Name, bool &bError);
		int	   GetAttributeInt(std::string Name, bool &bError);

		// Methods
		bool ProcessAttributes(std::string &ErrorLog);
		bool ProcessChildNodes(std::vector<RnRXMLNode*> vecpChildNodes,
									  std::vector<RnRXMLNode*> &vecpRemainingNodes,
									  std::string &ErrorLog);
		std::string GetLastError(void);
		void ClearNode(void);	// This sets everything back to an unknown state
		// Getters and Setters
		enumRnRNodeTypes GetNodeType(void);
		//
		void SetValue(std::string newValue);
		void SetValueDouble(double newValue);
		void SetValueFloat(float newValue);
		void SetValueInt(float newValue);
		std::string GetValue(void);
		double GetValueDouble(void);
		float GetValueFloat(void);
		int GetValueInt(void);
		
		
	private:
		//
		std::string m_TextValue;	// This only has something if its a start tag with a matching
							// end tag and there is only text in between.
		//
		std::string m_LastError;
		std::string m_ErrorLog;
		void m_AppendErrorText(std::string theText, std::string &theErrorLog);
		//
		std::string m_GenerateOuterXML(int currentIndent);	// This generates the outer XML from the innter info
		std::string m_GenerateAttributeString(void);
		std::string m_GenerateTabString(int currentIndent);
		// 
		std::string m_CopyFromIndexToIndex(std::string theText, std::string::size_type start, std::string::size_type end);
		std::string m_TrimBothEnds(std::string theString, std::string charToTrim);
		std::string m_TrimBothEnds(std::string theString);
		std::string m_ClearNonSpaceWhiteSpace(std::string theString);


	};

	// This is a single step through the XPath thing when it's being parsed
	class XPathStep
	{
	public:
		XPathStep();
		std::string TagName;			// This is the name of the particular tag
		bool bTagIsAttribute;
		std::string PredicateLHS;			// This is size[position()=1] or planet[2], etc.
		bool bPredicateLHSIsAttribute;
		std::string PredicateOperator;
		std::string PredicateRHS;
		bool bIsNodeTest;				// This is text(), etc.
	};

	class RnRXMLDOM
	{
	public:
		RnRXMLDOM();
		~RnRXMLDOM();
		// TODO: LoadXMLFileFromXMLText( ... );
		bool LoadXMLFile(std::string fileName, std::string &ErrorLog);			// New, reads binary file, then passes to LoadFromString
		bool LoadXMLFile_OLD(std::string fileName, std::string &ErrorLog);		// Original, file based parser (read directly from file)
		//bool LoadXMLFile(std::string fileName, std::string &ErrorLog);
		// bIgnoreHeaderNode allows us to load an invalid XML file with just the 
		//	internal node structure, for just building XML nodes, for instance
		bool LoadXMLFromXMLString( const std::string &xmlString, std::string &ErrorLog, bool bIgnoreHeaderNode = false );		
		bool SaveXMLFile(std::string fileName, std::string &ErrorLog);
		// This executes the XPath expression and returns the node (or nodes) if possible
		bool ExecuteXPath(std::string XPathExpression, std::vector<RnRXMLNode*> &vec_pNodes);
		bool ExecuteXPathSingleNode(std::string XPathExpression, RnRXMLNode& theNode);
		// This executes the XPath expression and returns only the value of the node,
		//	if possible. NOTE: It will also calculate the value of an attribute
		//	(in the case of "/someNode/@theAttrib/text()" which is technically invalid XPath
		//	 - in XPath, you can not get the text() value of an attribute)
		bool ExecuteXPathScalar(std::string XPathExpression,
										std::string &theValue);		
		// This is for creating a new XML file
		bool CreateDocument(std::string RootNodeName, std::string &ErrorLog);

		// These are used for getting the document node and doing something else with it.
		// For example, if you are manipulating nodes without 'official' XML headers, or 
		//	aren't treating the XML as a full file
		// WARNING: the destructor will check if a document node is present by comparing with
		//          zero (0); if you aren't careful, it can delete the node you extracted and-or
		//          attempt to delete an invalid (deleted) node
		// Use if you simply want access...
		RnRXMLNode*	pGetDocumentNodePointer(void);
		// Use if you want to "extract" the node (i.e. "remove" it from this document and
		//	manage it yourself - you'll have to call delete)
		RnRXMLNode*	pGetDocumentNodePointerClearDOM(void);		


		bool ClearAll(void);	// Blows everything away
		bool bHasData(void);	// True if anything is inside

		std::string GetLastError(void);
	private:
		static const std::string STANDARDXMLHEADER;// = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
		std::string m_DocumentHeader;
		bool m_bHasValidXMLDocumentHeader;

		static const unsigned int MAXBUFFERSIZE;	// 4096

		RnRXMLNode*	m_p_DocumentNode;
		// Used when parsing the XML file
		bool m_GetNextNodeTextFromFile(std::ifstream &theInputFile, std::string &NodeText);
		bool m_GetNextNodeTextFromStream(std::stringstream &theInputStream, std::string &NodeText);
		//std::ifstream m_theInputFile;
		
		//bool m_bFileAlreadyLoaded;
		bool m_bContainsData;

		//std::vector<RnRXMLNode> m_TheNodes;
		//
		// Used when parsing the XPath
		bool m_ParseXPath(std::string XPathString, std::vector<XPathStep> &theXPathSteps);
		bool m_ProcessNextXPathStep(std::vector<XPathStep>::iterator itXPathStep, 
											 std::vector<XPathStep> &theXPathSteps,
											 std::vector<RnRXMLNode*> &vec_pNodes,
											 RnRXMLNode* p_curNode,
											 std::vector<RnRXMLNode*>::size_type NodePosition);
		// 
		bool m_bIsOnlyWhiteSpace(std::string theText);
		std::string m_TrimFirstCharacter(std::string theText);
		std::string m_TrimLastCharacter(std::string theText);
		std::string m_ExtractNameFromInnerXML(std::string innerXML);
		std::string m_CopyFromIndexToIndex(std::string theText, std::string::size_type start, std::string::size_type end);
		std::string m_TrimBothEnds(std::string theString, std::string charToTrim);
		std::string m_TrimBothEnds(std::string theString);
		std::string m_ClearNonSpaceWhiteSpace(std::string theString);
		std::string m_CleanQuotesFromString(std::string theString);
		
		bool m_IsValidXMLHeader(std::string theHeader);
		bool m_ExtractAttribute(std::string &theText, std::string attributeName, RnRXML::RnRXMLAttribute &theAttrib);

		//
		std::string m_LastError;
		std::string m_ErrorLog;
		//
		void m_AppendErrorText(std::string theText, std::string &theErrorLog);
	};
};

#endif

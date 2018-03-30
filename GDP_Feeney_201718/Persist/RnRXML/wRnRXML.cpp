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

#include "wRnRXML.h"
#include <algorithm>
#include <fstream>
#include <sstream>	// Used to extract the position infomrmation form the predicate
#include <map>			// Used to keep track of the positions of the child nodes

#define MAXBUFFERSIZE 4096

RnRXML::wRnRXMLNode::wRnRXMLNode()
{
	this->ClearNode();
}

RnRXML::wRnRXMLNode::~wRnRXMLNode()
{
}

RnRXML::enumRnRNodeTypes RnRXML::wRnRXMLNode::GetNodeType(void)
{
	return this->NodeType;
}

bool RnRXML::wRnRXMLNode::SetAttribute(std::wstring Name, std::wstring Value)
{
	// See if it's there
	std::vector<wRnRXMLAttribute>::iterator itAttrib; 
	for (itAttrib = this->m_ChildAttributes.begin(); itAttrib != this->m_ChildAttributes.end(); itAttrib++)
	{	// Is this the attribute we are looking for? i.e. is the NAME a match?
		if ( (*itAttrib).Name == Name )
		{	// Yup. Change value
			(*itAttrib).Value = Value;
			return true;
		}
	}
	return false;
}

// Returns empty string if can't find it
std::wstring RnRXML::wRnRXMLNode::GetAttribute(std::wstring Name)
{
	// See if it's there
	std::vector<wRnRXMLAttribute>::iterator itAttrib; 
	for (itAttrib = this->m_ChildAttributes.begin(); itAttrib != this->m_ChildAttributes.end(); itAttrib++)
	{	// Is this the attribute we are looking for? i.e. is the NAME a match?
		if ( (*itAttrib).Name == Name )
		{	// Yup. Change value
			return (*itAttrib).Value;
		}
	}
	return L"";
}


bool RnRXML::wRnRXMLNode::ProcessChildNodes(std::vector<wRnRXMLNode*> vecpChildNodes,
														 std::vector<wRnRXMLNode*> &vecpRemainingNodes,
														 std::wstring &ErrorLog)
{
	// We are inside a parent node and these are the child nodes.
	// We need to process the nodes in order, from top to bottom. 
	// 
	//	For each node, there are a few possible scenerios:
	//	1. There are none left --> return
	//	2. It's a processing, comment, or start-end node --> Add it and remove from vector
	//	3. It's a start node. In this case, there must be a matching end node. 
	//		Moving from the bottom, find the matching end node. 
	//			--> If there is none, return with an error
	//			--> When we find it, pass all the nodes in between to ProcessChildNodes 
	//					of this start node, but delete these nodes first. When the call returns,
	//					continue processing the node list, if there is anything left. 
	while (!vecpChildNodes.empty())
	{
		// There are more nodes. 
		// Get the first node, and remove it
		wRnRXMLNode* p_tempNode = vecpChildNodes.at(0);
		vecpChildNodes.erase(vecpChildNodes.begin());
		// What kind of a node is it? 
		if ((p_tempNode->NodeType == RnR_TYPE_COMMENT) ||
			 (p_tempNode->NodeType == RnR_TYPE_PROCESSING) || 
			 ((p_tempNode->NodeType == RnR_TYPE_ELEMENT) && (p_tempNode->TagType == RnR_STARTEND_TAG)))
		{	// It's only an isolated node, so add it
			this->m_p_ChildNodes.push_back(p_tempNode);
		}
		// Is it a start tag?
		else if ((p_tempNode->NodeType == RnR_TYPE_ELEMENT) &&
				   (p_tempNode->TagType == RnR_START_TAG))
		{	
			// It's a start tag...
			// Pass all the remaming nodes to this child for processing
			vecpRemainingNodes.clear();
			if (!p_tempNode->ProcessChildNodes(vecpChildNodes, vecpRemainingNodes, ErrorLog))
			{	// Something is wrong
				this->m_AppendErrorText(L"RnRXMLNode ERROR: Node: ", ErrorLog);
				this->m_AppendErrorText(this->Name, ErrorLog);
				this->m_AppendErrorText(L" ProcessChildNode() call on node ", ErrorLog);
				this->m_AppendErrorText(p_tempNode->Name, ErrorLog);
				this->m_AppendErrorText(L" returned error.\n", ErrorLog);
				return false;
			}
			// Now the returning (remaining nodes) are the child nodes\
			// (NOTE: The matching end tag will be removed or an error will occur.
			vecpChildNodes = vecpRemainingNodes;

			// Add this node to the child nodes (it's processed)
			this->m_p_ChildNodes.push_back(p_tempNode);
		}
		// Is it an end node?
		else if ((p_tempNode->NodeType == RnR_TYPE_ELEMENT) &&
					(p_tempNode->TagType == RnR_END_TAG))
		{	// It's an end tag
			// Now, the only kind of END tag we should see is our matching end tag
			//		That's because any start tag we see processes all internal tags.
			//		If this isn't OUR end tag, something is wrong - not well formed
			if (p_tempNode->Name != this->Name)
			{	// It's NOT our end tag
				this->m_AppendErrorText(L"RnRXMLNode ERROR: ", ErrorLog);
				this->m_AppendErrorText(this->Name, ErrorLog);
				this->m_AppendErrorText(L" node. Found non-matching end tag: Node name = ", ErrorLog);
				this->m_AppendErrorText(p_tempNode->Name, ErrorLog);
				this->m_AppendErrorText(L"\n", ErrorLog);
				return false;
			}
			//// It's our end tag, so remove it and return remaining tags
			// This current node has already been erased from the child list
			//ChildNodes.erase(ChildNodes.begin());
			// Load the returning vector of nodes
			vecpRemainingNodes.clear();
			vecpRemainingNodes = vecpChildNodes;
			return true;	// All done processing the set of nodes
		}
		else
		{	// Something is wrong: Not a single node nor a start node
			this->m_AppendErrorText(L"RnRXMLNode ERROR: ", ErrorLog);
			this->m_AppendErrorText(this->Name, ErrorLog);
			this->m_AppendErrorText(L" node: Found a node that is not a single node (start-end, comment, or processing) or a start node.\n", ErrorLog);
			return false;
		}//if ((tempNode.NodeType == RnR_TYPE_COMMENT) ||
	}
	// If we are here, all the nodes have been processed
	vecpRemainingNodes.clear();

	return true;
}


bool RnRXML::wRnRXMLNode::ProcessAttributes(std::wstring &ErrorLog)
{
	// Replace any funky whitespace characters
	this->InnerXML = this->m_ClearNonSpaceWhiteSpace(this->InnerXML);

	// Go through the Raw XML and process any attributes we find
	// First, strip the name off of the raw XML
	std::wstring RawAttribString;
	for (std::wstring::size_type charPos = this->Name.length();
		  charPos != this->InnerXML.length(); charPos++)
	{
		RawAttribString = RawAttribString + this->InnerXML.at(charPos);
	}
	// Trim the ends
	RawAttribString = this->m_TrimBothEnds(RawAttribString);
	// Is there any text left? 
	while (RawAttribString.length() > 0)
	{	// Yup, so process it
		// Each attrib name ends with an "=" character, so find that
		wRnRXMLAttribute tempAttrib;
		std::wstring::size_type itEquals = RawAttribString.find_first_of('=');
		// Is it at the end? 
		if (itEquals == std::string::npos)
		{	// Something is screwy...
			this->m_AppendErrorText(L"RnRXMLNode ERROR (ProcessAttributes) : ", ErrorLog);
			this->m_AppendErrorText(this->Name, ErrorLog);
			this->m_AppendErrorText(L" node. Can not find = in attribute, so can't find attribute name.\n", ErrorLog);
			return false;
		}
		// Start to one before itEquals is name
		tempAttrib.Name = this->m_CopyFromIndexToIndex(RawAttribString, 0, itEquals);
		RawAttribString = this->m_CopyFromIndexToIndex(RawAttribString, itEquals + 1, RawAttribString.size());
		// At this point, RawAttribString should have the attrib value, which is 
		//	at least 2 characters long - even an empty attrib would be "".
		RawAttribString = this->m_TrimBothEnds(RawAttribString);
		if (RawAttribString.length() < 2)
		{	// Attribute too short
			this->m_AppendErrorText(L"RnRXMLNode ERROR (ProcessAttributes) : ", ErrorLog);
			this->m_AppendErrorText(this->Name, ErrorLog);
			this->m_AppendErrorText(L" node. Attribute value for attribute ", ErrorLog);
			this->m_AppendErrorText(tempAttrib.Name, ErrorLog);
			this->m_AppendErrorText(L" is too short (less than two characters long).\n", ErrorLog);
			return false;
		}
		// Now get the first quote
		std::wstring::size_type QuotePos = RawAttribString.find_first_of('"');
		// Copy this, except this quote
		RawAttribString = this->m_CopyFromIndexToIndex(RawAttribString, QuotePos + 1, RawAttribString.size());
		// Now find the ending quote.
		QuotePos = RawAttribString.find_first_of('"');
		tempAttrib.Value = this->m_CopyFromIndexToIndex(RawAttribString, 0, QuotePos);
		// Push this new attribute onto the Node's attribute vector
		this->m_ChildAttributes.push_back(tempAttrib);
		// Now clear this value + " character
		RawAttribString = this->m_CopyFromIndexToIndex(RawAttribString, QuotePos + 1, RawAttribString.size());
		RawAttribString = this->m_TrimBothEnds(RawAttribString);
		// And we are ready to process the next attribute
	}
	return true;
}

bool RnRXML::wRnRXMLNode::SaveXMLToStream(std::wostream &theStream, int currentIndent)
{
	// Print out this tag
	// Is it a comment tag?
	if ((this->NodeType == RnR_TYPE_COMMENT) && (this->TagType == RnR_COMMENT_TAG))
	{	// Print comment tag
		theStream << this->m_GenerateTabString(currentIndent)
			<< L"<!--" << this->InnerXML << L"-->" << std::endl;
		return true;
	}
	else if ((this->NodeType == RnR_TYPE_PROCESSING) && (this->TagType == RnR_PROCESSING_TAG))
	{	// Print processing tag
		theStream << this->m_GenerateTabString(currentIndent)
			<< L"<?" << this->InnerXML << L"?>" << std::endl;
		return true;
	}
	else if ((this->NodeType == RnR_TYPE_ELEMENT) && (this->TagType == RnR_STARTEND_TAG))
	{	// It's a start-end tag
		theStream << this->m_GenerateTabString(currentIndent)
			<< L"<" << this->Name
			<< this->m_GenerateAttributeString() << L"/>" << std::endl;
		return true;
	}	
	else if ((this->NodeType == RnR_TYPE_ELEMENT) && (this->TagType == RnR_START_TAG))
	{	// It's a start tag (with an end tag)
		// Print the start tag
		theStream << this->m_GenerateTabString(currentIndent) 
			<< L"<" << this->Name
			<< this->m_GenerateAttributeString() << L">"
			<< this->GetValue();
		if (!this->m_p_ChildNodes.empty())
		{
			// Print a new line
			theStream << std::endl;
			// Print the contents of this tag
			std::vector<wRnRXMLNode*>::iterator itChildNode;
			for (itChildNode = this->m_p_ChildNodes.begin(); itChildNode != this->m_p_ChildNodes.end(); itChildNode++)
			{
				(*itChildNode)->SaveXMLToStream(theStream, (currentIndent + 1) );
			}
			// Print the end tag - on the next line (with an indent)
			theStream << this->m_GenerateTabString(currentIndent) 
				<< L"</" << this->Name << L">" << std::endl;
		}
		else
		{	// There are no child elements, so print this on the same line
			theStream << L"</" << this->Name << L">" << std::endl;
		}
	}


	return true;
}

std::wstring RnRXML::wRnRXMLNode::m_GenerateTabString(int currentIndent)
{
	std::wstring indentString;
	for (int x = 0; x != currentIndent; x++)
	{
		indentString = indentString + L"  ";
	}
	return indentString;
}


std::wstring RnRXML::wRnRXMLNode::m_GenerateAttributeString(void)
{
	std::wstring theAttribString;
	std::vector<wRnRXMLAttribute>::iterator itAttrib;
	for (itAttrib = this->m_ChildAttributes.begin(); itAttrib != this->m_ChildAttributes.end(); itAttrib++)
	{
		theAttribString = theAttribString + L" " +
			(*itAttrib).Name + L"=\"" + (*itAttrib).Value + L"\"";
	}
	return theAttribString;
}



void RnRXML::wRnRXMLNode::m_AppendErrorText(std::wstring theText, std::wstring &theErrorLog)
{
	this->m_LastError = theText;
	theErrorLog = theErrorLog + theText;
}

void RnRXML::wRnRXMLNode::ClearNode(void)
{
	try
	{
		this->m_TextValue = L"";
		this->OuterXML = L"";
		this->InnerXML = L"";
		this->Name = L"";
		this->m_ChildAttributes.clear();

		std::vector<wRnRXMLNode*>::iterator itCN;
		for (itCN = this->m_p_ChildNodes.begin(); itCN != this->m_p_ChildNodes.end(); itCN++)
		{
			if ( (*itCN) != 0 )
			{
				(*itCN)->ClearNode();
				delete (*itCN);
			}
		}	
		// All the child nodes are deleted, so delete the entries from the vector
		this->m_p_ChildNodes.clear();
		this->NodeType = RnRXML::RnR_TYPE_UNKNOWN;
		this->TagType = RnRXML::RnR_UNKNOWN_TAG;
	}
	catch (...)
	{
	}
}

std::wstring RnRXML::wRnRXMLNode::m_CopyFromIndexToIndex(std::wstring theText, std::wstring::size_type start, std::wstring::size_type end)
{
	if ( (start > theText.size() ) ||
		  (end > theText.size() ) )
	{
		return theText;
	}
	if ( start > end )
	{
		return theText;
	}
	// Ranges are OK
	std::wstring returnString;
	for (std::wstring::size_type itPos = start; itPos != end; itPos++)
	{
		returnString = returnString + theText.at(itPos);
	}
	return returnString;
}

// This is a rock-n-roll string trimming thing
// Taken from: http://www.codeproject.com/vcpp/stl/stdstringtrim.asp
// (Needed to be cleaned up a fair bit - what is this? Java? Sheesh.
//  also used string, not wstring.)
std::wstring RnRXML::wRnRXMLNode::m_TrimBothEnds(std::wstring theString, std::wstring charToTrim)
{
	if ( theString.empty() )
	{
		return theString;
	}
	if ( charToTrim.length() > 1)
	{	// Invalid trim character
		return theString;
	}
	// 
	using namespace std;
	wstring::size_type pos = theString.find_last_not_of(charToTrim[0]);
	if(pos != wstring::npos) 
	{
		theString.erase(pos + 1);
		pos = theString.find_first_not_of(charToTrim[0]);
		if(pos != wstring::npos) 
		{
			theString.erase(0, pos);
		}
	}
	else 
	{
		theString.erase(theString.begin(), theString.end());
	}
	return theString;
}

std::wstring RnRXML::wRnRXMLNode::m_TrimBothEnds(std::wstring theString)
{
	if ( theString.empty() )
	{
		return theString;
	}
	// no spaces
	theString = this->m_TrimBothEnds(theString, L" ");
	theString = this->m_TrimBothEnds(theString, L"\t");	// Horizontal tab
	
	return theString;
}

std::wstring RnRXML::wRnRXMLNode::m_ClearNonSpaceWhiteSpace(std::wstring theString)
{
	std::wstring returnString;
	std::wstring::iterator itPos;
	for (itPos = theString.begin(); itPos != theString.end(); itPos++)
	{
		wchar_t theChar = *itPos;
		if ((theChar == '\n') ||	
			 (theChar == '\r') ||	// Carriage return 
			 (theChar == '\b') ||	// Backspace
			 (theChar == '\a') ||	// Bell (alert)
			 (theChar == '\f') ||	// Formfeed 
			 (theChar == '\v') ||	// Vertical tab 
			 (theChar == '\t'))		// Horizontal tab
		{	// Replace with a space
			 theChar = ' ';
		}
		returnString = returnString + theChar;
	}
	return returnString;
}

// Getters and Setters
std::wstring RnRXML::wRnRXMLNode::GetValue(void)
{
	return this->m_TextValue;
}

void RnRXML::wRnRXMLNode::SetValue(std::wstring newValue)
{
	this->m_TextValue = this->m_TrimBothEnds(newValue);
}

void RnRXML::wRnRXMLNode::SetValueDouble(double newValue)
{
	std::wstringstream ss;
	ss << newValue;
	ss >> this->m_TextValue;
}

void RnRXML::wRnRXMLNode::SetValueFloat(float newValue)
{
	std::wstringstream ss;
	ss << newValue;
	ss >> this->m_TextValue;
}

void RnRXML::wRnRXMLNode::SetValueInt(float newValue)
{
	std::wstringstream ss;
	ss << newValue;
	ss >> this->m_TextValue;
}

double RnRXML::wRnRXMLNode::GetValueDouble(void)
{
	std::wstringstream ss;
	ss << this->m_TextValue;
	double temp;
	ss >> temp;
	return temp;
}

float RnRXML::wRnRXMLNode::GetValueFloat(void)
{
	std::wstringstream ss;
	ss << this->m_TextValue;
	float temp;
	ss >> temp;
	return temp;
}

int RnRXML::wRnRXMLNode::GetValueInt(void)
{
	std::wstringstream ss;
	ss << this->m_TextValue;
	int temp;
	ss >> temp;
	return temp;
}

double RnRXML::wRnRXMLNode::GetAttributeDouble(std::wstring Name, bool &bError)
{
	std::wstring textValue = this->GetAttribute(Name);
	if (textValue == L"")
	{
		bError = true;
		return 0;
	}
	else
	{
		bError = false;
		std::wstringstream ss;
		ss << textValue;
		double temp;
		ss >> temp;
		return temp;
	}
}

float RnRXML::wRnRXMLNode::GetAttributeFloat(std::wstring Name, bool &bError)
{
	std::wstring textValue = this->GetAttribute(Name);
	if (textValue == L"")
	{
		bError = true;
		return 0;
	}
	else
	{
		bError = false;
		std::wstringstream ss;
		ss << textValue;
		float temp;
		ss >> temp;
		return temp;
	}
}

int RnRXML::wRnRXMLNode::GetAttributeInt(std::wstring Name, bool &bError)
{
	std::wstring textValue = this->GetAttribute(Name);
	if (textValue == L"")
	{
		bError = true;
		return 0;
	}
	else
	{
		bError = false;
		std::wstringstream ss;
		ss << textValue;
		int temp;
		ss >> temp;
		return temp;
	}
}


// *************************************************************************
// RnRXMLDOM
// *************************************************************************

RnRXML::wRnRXMLDOM::wRnRXMLDOM()
{
	this->m_DocumentHeader = STANDARDXMLHEADER;
	this->m_p_DocumentNode = 0;
	this->m_bFileAlreadyLoaded = false;
}

RnRXML::wRnRXMLDOM::~wRnRXMLDOM()
{
	// Now we clear (and delete) any existing nodes.
	try
	{
		if (this->m_p_DocumentNode)
		{
			this->m_p_DocumentNode->ClearNode();
			delete this->m_p_DocumentNode;
			this->m_p_DocumentNode = 0;
		}
	}
	catch (...)
	{
		// Do nothing
		this->m_AppendErrorText(L"ERROR: Exception when trying to delete document nodes for XML load.", this->m_ErrorLog);
	}
}

// Recursive
//		returns false if there is an XPath error
//		results are in the vecNodes vector
bool RnRXML::wRnRXMLDOM::m_ProcessNextXPathStep(
			std::vector<wXPathStep>::iterator itXPathStep,
			std::vector<wXPathStep> &theXPathSteps,
			std::vector<wRnRXMLNode*> &vec_pNodes,
			wRnRXMLNode* p_curNode,
			std::vector<wRnRXMLNode*>::size_type NodePosition)
{
	// The p_curNode is the node that is passed in (i.e. the 'parent' node)
	// 1) First, check that this node matches the current XPath locatoin
	//			If NOT, return false (can't find or can't continue)
	//			If TRUE, move to the next XPath
	// 2) At next XPath:
	//			if it's an attribute, scan attribs and return
	//			if it's an element, go through the child nodes, processing each matching node
	//				recursively.

	// NOTE: This is NOT to process attribs
	if ( (*itXPathStep).bTagIsAttribute )	return true;

	// Check that this parent node matches the XPath step
	if ( (*itXPathStep).TagName != p_curNode->Name ) return true;

	// Yes, the parent node NAME matches, but does the predicate match?
	if ( (*itXPathStep).PredicateLHS != L"" )
	{	// Predicates: The parent node NAME maches, but is there a match for the predicate?
		// Could be either an position thing (like /someTag[2]
		//	or could be an attribute match
		if ( (*itXPathStep).bPredicateLHSIsAttribute )
		{	// It's an attribute match. Does this tag have a matching attribute? 
			std::vector<wRnRXMLAttribute>::iterator itAttrib; 
			for (itAttrib = p_curNode->m_ChildAttributes.begin(); itAttrib != p_curNode->m_ChildAttributes.end(); itAttrib++)
			{	// Is this the attribute we are looking for?
				// i.e. is the NAME a match, the VALUE a match and the operator is "="
				if (( (*itAttrib).Name == (*itXPathStep).PredicateLHS ) &&
					 ( (*itAttrib).Value == (*itXPathStep).PredicateRHS ) &&
					 ( (*itXPathStep).PredicateOperator == L"=" ) ) 
				{	//Yup.
					break;
				}
			}
			// If we are at the end, the we DIDN'T find a match
			if (itAttrib == p_curNode->m_ChildAttributes.end()) return true;
		}
		else
		{	// It's a position thing (or an error)
			// Get the position infomation
			std::wstringstream ss;
			ss << (*itXPathStep).PredicateLHS;
			int positionInfo = -INT_MAX;		
			ss >> positionInfo;
			// If it's still -INT_MAX (less than zero), then the position stuff is screwed up
			if (positionInfo < 0) return true;		// May not be an error
			// Position is OK (i.e. it's a positive number)
			// Is this the node we are looking for (i.e. is NodePosition == positionInfo?)
			if (positionInfo != NodePosition) return true;	// NOPE
			// Yup, this is the node we want.
		}
	}//if ( (*itXPathStep).PredicateLHS == L"" )

	// If we are here, then these are the droids we are looking for....
	// ...I mean, this is the node we want

	// Move to next step
	itXPathStep++;
	// If we are at the end of the XPath, we've found what we're looking for
	//		and it's "this" node, so push in on the return vector and return
	if (itXPathStep == theXPathSteps.end())
	{
		vec_pNodes.push_back( p_curNode );
		return true;
	}
	// There is still (at least) one more XPath step. 
	//	 Is this an attribute? 
	if ( !(*itXPathStep).bTagIsAttribute )
	{	// It's an element. Go through all the child elements of this node
		// This part is a little funky as the child nodes can repeat and may not be in any
		//		particular order. However, we need to determine which absolute position each
		//		particular node is. For example, is it the 3rd "Sound" node, even if there 
		//		are other nodes scattered between the "Sound" nodes. 
		// So, we have a map that has a list of all the child node names and a count. Initially,
		//		the count of all the nodes is 0 (we haven't used them). When we come accross a node
		//		we increment the count of that node, keeping track of the position of that node.
		//		(NOTE: In XPath, the FIRST node is 1, not zero)
		std::map<std::wstring, int> mapNodeNameCount;
		std::vector<wRnRXMLNode*>::iterator itCN;
		for (itCN = p_curNode->m_p_ChildNodes.begin(); itCN != p_curNode->m_p_ChildNodes.end(); itCN++)
		{
			mapNodeNameCount[ (*itCN)->Name ] = 0;
		}

		for (itCN = p_curNode->m_p_ChildNodes.begin(); itCN != p_curNode->m_p_ChildNodes.end(); itCN++)
		{	// Calculate the count of this node name (how many times we've see it)
			std::map<std::wstring, int>::iterator itNodeCount
										= mapNodeNameCount.find( (*itCN)->Name );
			mapNodeNameCount[ (*itCN)->Name ] = itNodeCount->second + 1;		// Increment the count
			itNodeCount = mapNodeNameCount.find( (*itCN)->Name );		// Find it again (to pass count)
			// Pass this child node to the function recursively

			if (!this->m_ProcessNextXPathStep(itXPathStep, theXPathSteps, vec_pNodes, 
														 (*itCN),
														 itNodeCount->second ) )
				return false;	// Error
		}		
		// All done, so return.
		return true;
	}
	else
	{	// Child node is an attribute, which we are not dealing with 
		return true;
	}
}

// NOTE: If we are passing something like a node text XPath
// (like /blah/blah/@whatever/text() ), this will return the 
//	value of the node test in the InnerXML of the first returned node.
//	Yeah, yeah, it's not 'correct.' So sue me.
bool RnRXML::wRnRXMLDOM::ExecuteXPath(
		std::wstring XPathExpression, std::vector<wRnRXMLNode*> &vec_pNodes)
{
	std::vector<wXPathStep> theXPathSteps;
	
	if (!this->m_ParseXPath(XPathExpression, theXPathSteps) )
	{	// Can't parse it for some reason
		return false;
	}
	// XPath was pased without a problem...
	// Go through all the nodes, along with the steps.
	// Are there any steps? 
	if (theXPathSteps.empty()) return false;
	std::vector<wXPathStep>::iterator itXPathStep = theXPathSteps.begin();
	//std::vector<RnRXMLNode>::iterator itNodes = this->m_TheNodes.begin();
	//RnRXMLNode* p_curNode = &(this->m_p_DocumentNode);
	wRnRXMLNode* p_curNode = this->m_p_DocumentNode;
	// No document node, no point in continuing...
	if (!p_curNode) return false;

	// Pass this off to the processor
	bool result 
		= this->m_ProcessNextXPathStep(itXPathStep, theXPathSteps, vec_pNodes, p_curNode, 0);

	return result;
}

bool RnRXML::wRnRXMLDOM::ExecuteXPathSingleNode(
		std::wstring XPathExpression, wRnRXMLNode &theNode)
{
	// TODO

	return true;
}

bool RnRXML::wRnRXMLDOM::ExecuteXPathScalar(std::wstring XPathExpression,
											std::wstring &theValue)
{
	theValue = L"";

	// TODO

	return true;
}

RnRXML::wXPathStep::wXPathStep()
{
	this->bIsNodeTest = false;
	this->bPredicateLHSIsAttribute = false;
	this->bTagIsAttribute = false;
}


//	Parse through the XPath doing this:
// 1. Stripping out the node names as we go
// 2. Adding any predicates things to the nodes that we are finding
// 3. If we get to a function, we likely need to stop(??)
//class XPathStep
//{
//	std::wstring TagName;		// This is the name of the particular tag
//	std::wstring Predicate;		// This is size[position()=1] or planet[2], etc.
//	std::wstring NodeTest;		// This is text(), etc.
bool RnRXML::wRnRXMLDOM::m_ParseXPath(std::wstring XPathString, std::vector<wXPathStep> &theSteps)
{
	std::string::size_type startSlash = XPathString.find_first_of(L"//");
	if (startSlash == std::string::npos)
	{
		return false;
	}
	std::wstring::size_type endSlash = std::wstring::npos;

	while (true)
	{
		wXPathStep curStep;

		endSlash = XPathString.find_first_of(L"//", startSlash + 1);
		// If there is no end slash, then we are done
		if (endSlash != std::wstring::npos)
		{
			curStep.TagName = XPathString.substr(startSlash + 1, (endSlash - startSlash - 1) );
			theSteps.push_back(curStep);
			startSlash = endSlash;
		}
		else
		{
			curStep.TagName = XPathString.substr(startSlash + 1, endSlash);
			theSteps.push_back(curStep);
			break;
		}
	}
	// Now process the nodes, stripping out any node tests (i.e. text() )
	std::vector<wXPathStep>::iterator itSteps;
	for (itSteps = theSteps.begin(); itSteps != theSteps.end(); itSteps++)
	{
		// Are there openning and closing round brackets? 
		std::wstring::size_type secondBracket = std::wstring::npos;
		std::wstring::size_type firstBracket = (*itSteps).TagName.find_first_of(L"(");
		if (firstBracket != std::wstring::npos)
		{	// Found a first bracket			
			secondBracket = (*itSteps).TagName.find_first_of(L")", firstBracket + 1);
			if (secondBracket == std::wstring::npos)
			{	// Something is wrong - there is no end bracket
				return false;
			}
			(*itSteps).bIsNodeTest = true;
		}
		else
		{	// DIDN'T return a round bracket, so look for square ones
			firstBracket = (*itSteps).TagName.find_first_of(L"[");
			if (firstBracket != std::string::npos)
			{	// There is a square bracket - may be a predicate
				secondBracket = (*itSteps).TagName.find_first_of(L"]", firstBracket + 1);
				if (secondBracket == std::wstring::npos)
				{	// Something is wrong - start bracket, but not end bracket
					return false;
				}
				// Strip the predicate from the rest of the tag
				(*itSteps).PredicateLHS = (*itSteps).TagName.substr(firstBracket + 1, (secondBracket - firstBracket - 1) );
				(*itSteps).TagName = (*itSteps).TagName.substr(0, firstBracket);
				(*itSteps).TagName = this->m_TrimBothEnds( (*itSteps).TagName );

				// Is there an operator in there. 
				std::wstring::size_type opPos = (*itSteps).PredicateLHS.find_first_of(L"=");
				if ((opPos != std::string::npos) &&
					 (opPos < secondBracket))
				{	// It's in there 
					(*itSteps).PredicateOperator = (*itSteps).PredicateLHS.substr(opPos, 1);
					(*itSteps).PredicateOperator = this->m_TrimBothEnds( (*itSteps).PredicateOperator );
					// Get the RHS
					(*itSteps).PredicateRHS = (*itSteps).PredicateLHS.substr(opPos + 1, std::wstring::npos);
					(*itSteps).PredicateRHS = this->m_TrimBothEnds( (*itSteps).PredicateRHS );
					(*itSteps).PredicateRHS = this->m_CleanQuotesFromString( (*itSteps).PredicateRHS );
					// Get the LHS
					(*itSteps).PredicateLHS = (*itSteps).PredicateLHS.substr(0, opPos);
					(*itSteps).PredicateLHS = this->m_TrimBothEnds( (*itSteps).PredicateLHS );
					// Check to see if the first item is an attribute - it should be
					std::wstring::size_type atSymbol = (*itSteps).PredicateLHS.find_first_of(L"@");
					if (atSymbol != std::wstring::npos)
					{	// There is a symbol. Is it the first char? 
						if (atSymbol != 0)
						{	// No, so something is wrong
							return false;
						}
						(*itSteps).bPredicateLHSIsAttribute = true;
						// Strip off the first character
						(*itSteps).PredicateLHS = (*itSteps).PredicateLHS.substr(atSymbol + 1, std::wstring::npos);
					}
				}//if ((opPos != std::wstring::npos) && (opPos < secondBracket))
			}//if (firstBracket != std::wstring::npos) --- Square brackets (i.e. predicate)
			
			// At this point, the predicates and/or node tests are all parsed out
			// See if the node is an attribute
			// Check to see if the first item is an attribute - it should be
			std::wstring::size_type atSymbol = (*itSteps).TagName.find_first_of(L"@");
			if (atSymbol != std::wstring::npos)
			{	// There is a symbol. Is it the first char? 
				if (atSymbol != 0)
				{	// No, so something is wrong
					return false;
				}
				(*itSteps).bTagIsAttribute = true;
				// Strip off the first character
				(*itSteps).TagName = (*itSteps).TagName.substr(atSymbol + 1, std::wstring::npos);
			}



		}//if (firstBracket != std::wstring::npos) --- Round brackets (i.e. node test)


	}//for (itSteps = theSteps.begin(); itSteps != theSteps.end(); itSteps++)
	
	// Now process the nodes, stripping out any predicate information


	return true;
}


bool RnRXML::wRnRXMLDOM::m_GetNextNodeText(std::wstring &NodeText)
{
	if (!this->m_theInputFile.is_open())
	{
		return false;
	}
	// File is open, so get next tag
	// Is there a "next character/tag"
	if (this->m_theInputFile.eof())
	{
		return false;
	}
	// Not EOF
	wchar_t rawChars[MAXBUFFERSIZE];
	this->m_theInputFile.getline(rawChars, MAXBUFFERSIZE, '>');
	// Did it work?
	//if (this->m_theInputFile.failbit)
	//{
	//	return false;
	//}
	NodeText.append(rawChars);
	NodeText = NodeText + L">";	// Append the > delimeter that was stripped out by istream
	return true;
}

bool RnRXML::wRnRXMLDOM::LoadXMLFile(std::wstring fileName, std::wstring &ErrorLog)
{
	// If we have already loaded a file, we can't load another
	if (this->m_bFileAlreadyLoaded)
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: File is already loaded.\n", ErrorLog);
		return false;
	}

	this->m_theInputFile.open(fileName.c_str());
	if (!this->m_theInputFile.is_open())
	{		
		return false;
	}

	enum enumReadState
	{
		WAITINGFORTAGSTART,
		INSIDE_TAG,					// We are reading the tag and name, etc.

		OUTSIDE_TAG,				// This is the case where we have some text that is 
										// just floating around (this shouldn't be here for this
										//	parser to handle - all text should be between tags)
										//	The only way this is happening is if there is
										//	whitespace between tags (like tabs, etc.)

		INSIDE_TAG_TEXT,			// This is the case when we have read a start tag
										// but it is not an "open-closed" tag - i.e. a tag
										//	that ends in />, so the text we are reading is
										//	the 'value' of that tag. The next tags name MUST
										//	be the same value as the previous tag, or we bail

		READING_FREE_TEXT			
	} curReadState = WAITINGFORTAGSTART;
	// Read the header first. If it doesn't match, it ain't an XML file
	std::vector<wRnRXMLNode*> vecpRawNodes;
	wRnRXMLNode* p_theCurNode = new wRnRXMLNode();
	wRnRXMLNode* p_TheLastNode;
	p_theCurNode->NodeType = RnR_TYPE_UNKNOWN;
	p_theCurNode->TagType = RnR_UNKNOWN_TAG;
	std::wstring curText;

	while (true)
	{
		// Get next character
		wchar_t theChar;
		this->m_theInputFile.get(theChar);
		// State machine
		switch (curReadState)
		{
		case WAITINGFORTAGSTART:
			// Is the the first tag?
			if (theChar == '<')
			{
				curReadState = INSIDE_TAG;
			}
			break;
		case OUTSIDE_TAG:
			// Is this a start tag?
			if (theChar == '<')
			{	// Found a start tag, so start reading the inside of the tag
				curReadState = INSIDE_TAG;
				// PROCESS current text - if the last tag we had is a start tag
				//	then this text goes with it...
				if (p_TheLastNode->NodeType == RnR_START_TAG)
				{	
					if (!this->m_bIsOnlyWhiteSpace(curText))
					{
						p_TheLastNode->SetValue(curText);
					}
					//else
					//{
					//	p_TheLastNode->TextValue = L"";
					//}
					//theCurNode.TextValue = curText;
				}
				curText.clear();		// Clear for next time through loop
			}
			else
			{	// If it's not a backspace, verticle tab, form feed, or carriage return, 
				//		add it to the text. If it's a Horizontal tab (\t), leave it as it might be used.
				if ((theChar != '\n') &&	// New line
					 (theChar != '\r') &&	// Carriage return 
					 (theChar != '\b') &&	// Backspace
					 (theChar != '\a') &&	// Bell (alert)
					 (theChar != '\f') &&	// Formfeed 
					 (theChar != '\v'))		// Vertical tab 
				{
					curText = curText + theChar;
				}
			}

			break;
		case INSIDE_TAG:
			// Is this the end tag thing?
			if (theChar == '>')
			{
				curReadState = OUTSIDE_TAG;
				// PROCESS THIS TAG...
				wchar_t firstChar = ' ';
				wchar_t lastChar = ' ';
				// Does this have a '/' at the start? (end tag)
				if (p_theCurNode->OuterXML.size() != 0)
				{
					if (p_theCurNode->OuterXML.size() >= 2 )
					{	
						firstChar = p_theCurNode->OuterXML.at(0);
						lastChar = p_theCurNode->OuterXML.at(p_theCurNode->OuterXML.size() - 1);
					}
				}		
				// Assume it's and open tag - has < and >
				p_theCurNode->NodeType = RnR_TYPE_ELEMENT;
				p_theCurNode->TagType = RnR_START_TAG;

				// Now figure out what kind of a tag it is...
				if ((firstChar == '?') && (lastChar == '?'))			// PROCESSING tag
				{
					p_theCurNode->NodeType = RnR_TYPE_PROCESSING;
					p_theCurNode->TagType = RnR_PROCESSING_TAG;
					// Get rid of the '?' at both ends.
					p_theCurNode->InnerXML = p_theCurNode->OuterXML;
					// Get rid of the first '?'
					p_theCurNode->InnerXML = this->m_TrimFirstCharacter(p_theCurNode->InnerXML);
					// Get rid of the last '?'
					p_theCurNode->InnerXML = this->m_TrimLastCharacter(p_theCurNode->InnerXML);					
				}
				else if (firstChar == '/')								// END tag
				{
					p_theCurNode->NodeType = RnR_TYPE_ELEMENT;
					p_theCurNode->TagType = RnR_END_TAG;
					// Get rid of first '/' 
					p_theCurNode->InnerXML = this->m_TrimFirstCharacter(p_theCurNode->OuterXML);
				}
				else if (lastChar == '/')								// START END tag
				{	
					p_theCurNode->NodeType = RnR_TYPE_ELEMENT;
					p_theCurNode->TagType = RnR_STARTEND_TAG;
					// Get rid of last '/'
					p_theCurNode->InnerXML = this->m_TrimLastCharacter(p_theCurNode->OuterXML);
				}
				else if ((firstChar == '!') && (lastChar == '-'))		// COMMENT tag
				{	// Could be a comment tag <!---->
					if (p_theCurNode->OuterXML.size() >= 5)
					{
						if (( p_theCurNode->OuterXML.at(1) == '-') && ( p_theCurNode->OuterXML.at(2) == '-') &&
							 ( p_theCurNode->OuterXML.at(p_theCurNode->OuterXML.size() - 2) == '-'))
						{	// Yup, it's a comment
							p_theCurNode->NodeType = RnR_TYPE_COMMENT;
							p_theCurNode->TagType = RnR_COMMENT_TAG;
							// Trim the '!--' at the front...
							p_theCurNode->InnerXML = this->m_TrimFirstCharacter(p_theCurNode->OuterXML);	// The '!'
							p_theCurNode->InnerXML = this->m_TrimFirstCharacter(p_theCurNode->InnerXML);	// first '-'
							p_theCurNode->InnerXML = this->m_TrimFirstCharacter(p_theCurNode->InnerXML);	// second '-'
							// Trim the ending '--'
							p_theCurNode->InnerXML = this->m_TrimLastCharacter(p_theCurNode->InnerXML);	// The second last '-'
							p_theCurNode->InnerXML = this->m_TrimLastCharacter(p_theCurNode->InnerXML);	// The last '-'
						}
					}
				}
				else			// It's just a starting element
				{
					p_theCurNode->InnerXML = p_theCurNode->OuterXML;					
				}
				// Add this tag to the vector of tags
				// Add the < and > if the node type is appropriate
				switch (p_theCurNode->TagType)		// Instead of a massive if statement
				{
				case RnR_START_TAG:
				case RnR_END_TAG:
				case RnR_STARTEND_TAG:
				case RnR_COMMENT_TAG:
				case RnR_PROCESSING_TAG:
					p_theCurNode->OuterXML = L"<" + p_theCurNode->OuterXML + L">";
					break;
				}
				// Get the name of the tag...if it's a start, end, or startend tag
				switch (p_theCurNode->TagType)
				{
				case RnR_START_TAG:
				case RnR_END_TAG:
				case RnR_STARTEND_TAG:
					p_theCurNode->Name = this->m_ExtractNameFromInnerXML(p_theCurNode->InnerXML);
					break;
				}

				vecpRawNodes.push_back(p_theCurNode);
				// Get the last node in the vector
				//p_TheLastNode = &(*(vecpRawNodes.rbegin() ));
				p_TheLastNode = *(vecpRawNodes.rbegin() );
				//theCurNode.ClearNode();
				p_theCurNode = new wRnRXMLNode();
			}
			else
			{
				p_theCurNode->OuterXML = p_theCurNode->OuterXML + theChar;
			}
		}

		//std::wcout << theChar;
		if (this->m_theInputFile.eof())
			break;
	}
	// At this point, all the nodes are loaded into the vector
	// Did we actually load anything?
	if (vecpRawNodes.empty())
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: Didn't read any nodes from XML file.\n", ErrorLog);
		this->m_theInputFile.close();
		return false;
	}

	// Check to see if the first node is a document header...
	//if (	(*vecpRawNodes.begin())->OuterXML != STANDARDXMLHEADER)
	if (   ! this->m_IsValidXMLHeader((*vecpRawNodes.begin())->OuterXML))
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: The first node is not a standard XML header. \n", ErrorLog);
		this->m_theInputFile.close();
		return false;
	}
	// The first node is a standard document header, so remove it from the vector
	delete *(vecpRawNodes.begin());
	vecpRawNodes.erase(vecpRawNodes.begin());
	
	// Copy the nodes from the temp vector to the final vector
	// The first and last nodes should be document nodes (unless there is only one node)
	// Only one node?
	if (vecpRawNodes.size() == 1)
	{	// Is this an START_END type tag?
		if ((vecpRawNodes.at(0)->TagType != RnR_STARTEND_TAG) || 
			 (vecpRawNodes.at(0)->NodeType != RnR_TYPE_ELEMENT))
		{	// It's screwed - the one and only node is not an element or a START_END node
			this->m_AppendErrorText(L"RnRXMLDOM ERROR: There is only one node and it's not a START-END node.\n", ErrorLog);
			this->m_theInputFile.close();
			return false;
		}
		else
		{	// This is a special case where there is only one node - the doucment
			//		node - that is both start and end
			this->m_p_DocumentNode = vecpRawNodes.at(0);
			// All done
			this->m_theInputFile.close();
			return true;
		}
	}
	else
	{	// There are at least two nodes. The first and last should be matching 
		//		start and end tags.
		if (vecpRawNodes.at(0)->Name != vecpRawNodes.at( vecpRawNodes.size() - 1)->Name)
		{
			this->m_AppendErrorText(L"RnRXMLDOM ERROR: The first and last nodes (Document node) have different names.", ErrorLog);
			this->m_AppendErrorText(L"\tFirst node name = ", ErrorLog);
			this->m_AppendErrorText(vecpRawNodes.at(0)->Name, ErrorLog);
			this->m_AppendErrorText(L"\n\t, last node name = ", ErrorLog);
			this->m_AppendErrorText(vecpRawNodes.at( vecpRawNodes.size() - 1)->Name, ErrorLog);
			this->m_AppendErrorText(L"\n", ErrorLog);
			this->m_theInputFile.close();
			return false;
		}
		// The names are the same. Is the first one a start and the last an end?
		if ((vecpRawNodes.at(0)->TagType != RnR_START_TAG) ||
			 (vecpRawNodes.at( vecpRawNodes.size() - 1)->TagType != RnR_END_TAG))
		{
			this->m_AppendErrorText(L"RnRXMLDOM ERROR: The first and last node (document node) have same name but are not matching start and end nodes.\n", ErrorLog);
			this->m_theInputFile.close();
			return false;
		}
	}
	// Go through all the nodes and process the attribute list 
	//	into the attribute vector. 
	std::vector<wRnRXMLNode*>::iterator itNodes;
	for (itNodes = vecpRawNodes.begin(); itNodes != vecpRawNodes.end(); itNodes++)
	{	// Only start and startend nodes can have attributes
		if ( (*itNodes)->NodeType == RnR_TYPE_ELEMENT )
		{	
			if (( (*itNodes)->TagType == RnR_START_TAG) ||
				 ( (*itNodes)->TagType == RnR_STARTEND_TAG))
			{	// Process the attributes
				if (! (*itNodes)->ProcessAttributes(ErrorLog) )
				{	// Something is wrong with the attribute processing
					this->m_AppendErrorText(L"RnRXMLDOM ERROR: ProcessAttributes() failed when calling " + (*itNodes)->Name + L" node.\n", ErrorLog);
					this->m_theInputFile.close();
					return false;
				}
			}
		}//if ( (*itNodes).NodeType == RnR_TYPE_ELEMENT )
	}
	// ************************************************
	// At this point, all the attributes are processed.
	// ************************************************

	// There is a root document node.
	this->m_p_DocumentNode = vecpRawNodes.at(0);
	// Delete the first node (document start node) from the vector
	vecpRawNodes.erase(vecpRawNodes.begin());


	// Now, we process the nodes, matching up the heiarchy of the nodes.
	// There are two possible conditions at this point:
	//	1. There are no child nodes, in the case where there was a startend node
	//		(in this case, we are done)
	// 2. There is one or more child nodes, which is an END tag, matching the starting document
	//		tag. If there is anything else, we have an error.

	// Are there any child nodes?
	if (vecpRawNodes.empty())
	{	// there was only one node. Was it a start end node?
		if ((this->m_p_DocumentNode->NodeType == RnR_TYPE_ELEMENT) &&
			 (this->m_p_DocumentNode->TagType == RnR_STARTEND_TAG))
		{	// It's all good - though the document has only one node
			this->m_theInputFile.close();
			return true;
		}	
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: There is a starting document node, but it is not a start-end node.\t", ErrorLog);
		this->m_theInputFile.close();
		return false;	// There is only one node, but it's NOT a start end node
	}
	// At this point, there is at least one node in the child nodes.
	// The last node should be a matching end tag to the document node.
	// Is the document node a start tag
	if ((this->m_p_DocumentNode->NodeType != RnR_TYPE_ELEMENT) ||
		 (this->m_p_DocumentNode->TagType != RnR_START_TAG))
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: First child node is not an element or start tag. Node name = " + this->m_p_DocumentNode->Name + L"\n", ErrorLog);
		this->m_theInputFile.close();
		return false;		// The start node is not an element or a start tag
	}
	// Is the last node a matching end node to the document node?
	std::vector<wRnRXMLNode*>::iterator itLast = vecpRawNodes.end();
	itLast--;	
	if (((*itLast)->Name != this->m_p_DocumentNode->Name) ||
		 ((*itLast)->NodeType != RnR_TYPE_ELEMENT) ||
		 ((*itLast)->TagType != RnR_END_TAG))
	{	// The end tag doesn't match or isn't an end tag
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: Last node is not an end tag. Node name = " + (*itLast)->Name + L"\n", ErrorLog);
		this->m_theInputFile.close();
		return false;
	}
	// At this point, the document has at least one document node that 
	//	is made up of matching start and end tags. 

	// Now copy all the remaining nodes into the document node and 
	//	recursively process the heirarchy by calling ProcessChildNodes() on the root
	std::vector<wRnRXMLNode*> remainingNodes;
	if (!this->m_p_DocumentNode->ProcessChildNodes(vecpRawNodes, remainingNodes, ErrorLog))
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: ProcessChildNodes() on document node return error.\t", ErrorLog);
		this->m_theInputFile.close();
		return false;	// There was some problem somewhere along the chain
	}
	// If there are any nodes left, we have a problem
	if (!remainingNodes.empty())
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: Child nodes of document node processed, but some nodes remain unprocessed.\n", ErrorLog);
		this->m_theInputFile.close();
		return false;
	}

	this->m_theInputFile.close();
	this->m_bFileAlreadyLoaded = true;
	return true;
}


// This tests for all sorts of valid XML headers.
// <?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?> 
// <?xml version="1.0" encoding="utf-8"?>"
// <?xml version="1.0" encoding="UTF-8"?>
// <?xml?>  
// <?xml encoding="ISO-8859-1"?>
// <?xml version="1.0" encoding="http://www.w3.org/2003/05/soap-envelope" standalone="no"?>
//
// <?xml version="version_number" encoding="encoding_declaration" standalone="standalone_status" ?>
//
// Attribute Name:       Possible values
//  version                1.0 
//  encoding               UTF-8, UTF-16, ISO-10646-UCS-2, ISO-10646-UCS-4, 
//                         ISO-8859-1 to ISO-8859-9, ISO-2022-JP, Shift_JIS, EUC-JP  
//  standalone             yes, no  

//
// So:
//	* Requires that it be a processing directive
//	* MUST have XML at the start (?xml)
//	* Optional: encoding, standalone, and version
//
// This method takes a complete XML header (including open and close tags)
bool RnRXML::wRnRXMLDOM::m_IsValidXMLHeader(std::wstring theHeader)
{
	// First, check that the first five letters are "<?xml"
	if (theHeader.size() < 5)
	{
		return false;
	}
	std::wstring::iterator itText = theHeader.end();
	if ( theHeader.find(L"<?xml",0) != 0)
	{
		return false;
	}
	// Trim the first characters off of the header...
	theHeader.erase(0, 5);

	// Does it end with the "?>" characters?
	if ( theHeader.find(L"?>", 0) != (theHeader.size() - 2))
	{
		return false;
	}
	// Trim off the end characters...
	theHeader.erase( theHeader.size() - 2, 2);

	// Now see only the three possible attributes are there...
	RnRXML::wRnRXMLAttribute theAttrib;
	this->m_ExtractAttribute(theHeader, L"version", theAttrib);
	this->m_ExtractAttribute(theHeader, L"encoding", theAttrib);
	this->m_ExtractAttribute(theHeader, L"standalone", theAttrib);
	// Trim what's left...
	this->m_TrimBothEnds(theHeader);
	// If there are any characters left, it's not a valid header
	if (this->m_bIsOnlyWhiteSpace(theHeader))
	{
		return true;
	}
	// There is something left in the header, so it's not valid (we hope)
	return false;
}

bool RnRXML::wRnRXMLDOM::m_ExtractAttribute(std::wstring &theText, std::wstring attributeName, RnRXML::wRnRXMLAttribute &theAttrib)
{
	theAttrib.Name = L"";
	theAttrib.Value = L"";

	std::wstring::size_type nameLoc = 0;
	std::wstring::size_type equalLoc = 0;
	std::wstring::size_type quoteStart = 0;
	std::wstring::size_type quoteEnd = 0;

	nameLoc = theText.find(attributeName, 0);
	if (nameLoc == std::wstring::npos)
	{	// Didn't find the attribute name
		return false;
	}
	// Find the equal sign after the attribute name
	equalLoc = theText.find(L"=", (nameLoc + attributeName.size() ));
	if (equalLoc == std::wstring::npos)
	{	// Didn't find an equal sign
		return false;
	}
	// Now look for the matching quotes...
	quoteStart = theText.find(L"\"", equalLoc + 1);
	if (quoteStart == std::wstring::npos)
	{	// Either there are no quotes (an error) or we are using single quotes...
		quoteStart = theText.find(L"\'", equalLoc + 1);
		if (quoteStart == std::wstring::npos)
		{	// There is no quote...
			return false;
		}
		quoteEnd = theText.find(L"\'", quoteStart + 1);
		if (quoteEnd == std::wstring::npos)
		{	// No closing quote
			return false;
		}
	}
	else
	{	// Find matching double quote
		quoteEnd = theText.find(L"\"", quoteStart + 1);
		if (quoteEnd == std::wstring::npos)
		{	// Didn't find matching end quote
			return false;
		}
	}
	
	// Load up the attribute information
	theAttrib.Name = attributeName;
	theAttrib.Value = theText.substr(quoteStart + 1, quoteEnd - quoteStart - 1);

	// Clear text from start of name to ending quote...	
	theText.erase(nameLoc, quoteEnd - nameLoc + 1);

	return true;
}

bool RnRXML::wRnRXMLDOM::SaveXMLFile(std::wstring fileName, std::wstring &ErrorLog)
{
	std::wofstream theFile;
	theFile.open(fileName.c_str());
	if (!theFile.is_open())
	{
		this->m_AppendErrorText(L"RnRXMLDOM ERROR: SaveXMLFile() file isn't open.\n", ErrorLog);
		return false;
	}
	// We're open
	// Print out the header...
	theFile << this->m_DocumentHeader << std::endl;

	this->m_p_DocumentNode->SaveXMLToStream(theFile, 0);


	// Closer er up
	theFile.close();
	return true;
}

bool RnRXML::wRnRXMLDOM::CreateDocument(std::wstring RootNodeName, std::wstring &ErrorLog)
{
	// if there are no nodes, then allow this to be created
	if (this->m_bFileAlreadyLoaded)
	{	
		ErrorLog = L"RnRXML::RnRXMLDOM: CreateDocument() XML document is already loaded.";
		return false;
	}
	RnRXML::wRnRXMLNode* pRootNode = new RnRXML::wRnRXMLNode();
	pRootNode->InnerXML = RootNodeName;
	pRootNode->NodeType = RnRXML::RnR_TYPE_ELEMENT;
	this->m_p_DocumentNode = pRootNode;

	return true;
}


// Returns true if the string consists of only white space
bool RnRXML::wRnRXMLDOM::m_bIsOnlyWhiteSpace(std::wstring theText)
{
	std::wstring::iterator itChar;
	for (itChar = theText.begin(); itChar != theText.end(); itChar++)
	{
		wchar_t theChar = *itChar;
		if ((theChar != '\n') &&	// New line
			 (theChar != '\r') &&	// Carriage return 
			 (theChar != '\b') &&	// Backspace
			 (theChar != '\a') &&	// Bell (alert)
			 (theChar != '\f') &&	// Formfeed 
			 (theChar != '\v') &&	// Vertical tab 
			 (theChar != '\t') &&	// Horizontal tab
			 (theChar != ' '))
		{
			return false;
		}
	}
	// If we are here, we have only seen whitespace
	return true;
}

std::wstring RnRXML::wRnRXMLDOM::m_TrimFirstCharacter(std::wstring theText)
{
	std::wstring returnText = theText;
	if (theText.size() >= 1)
	{
		theText.erase(theText.begin());
	}
	else
	{
		theText.clear();
	}
	return theText;
}

std::wstring RnRXML::wRnRXMLDOM::m_TrimLastCharacter(std::wstring theText)
{
	std::wstring returnText;
	if (theText.size() >= 1)
	{
		for (std::wstring::size_type theChar = 0; theChar != (theText.size() - 1); theChar++)
		{
			returnText += theText.at(theChar);
		}
	}
	return returnText;
}

// This reads along a string until it gets to the first space or the end.
// The "InnerXML" has no start or end < > symbols, ? symbols, or / symbols 
//		at the start or the end. 
std::wstring RnRXML::wRnRXMLDOM::m_ExtractNameFromInnerXML(std::wstring innerXML)
{
	std::wstring theName;
	std::wstring::iterator itChar;
	for (itChar = innerXML.begin(); itChar != innerXML.end(); itChar++)
	{
		// Is it a space? 
		if ( (*itChar) != ' ' )
		{
			theName = theName + (*itChar);
		}
		else
		{
			break;			// We've hit the first space
		}
	}
	return theName;
}

void RnRXML::wRnRXMLDOM::m_AppendErrorText(std::wstring theText, std::wstring &theErrorLog)
{
	this->m_LastError = theText;
	theErrorLog = theErrorLog + theText;
}

std::wstring RnRXML::wRnRXMLDOM::m_CopyFromIndexToIndex(std::wstring theText, std::wstring::size_type start, std::wstring::size_type end)
{
	if ( (start > theText.size() ) ||
		  (end > theText.size() ) )
	{
		return theText;
	}
	if ( start > end )
	{
		return theText;
	}
	// Ranges are OK
	std::wstring returnString;
	for (std::string::size_type itPos = start; itPos != end; itPos++)
	{
		returnString = returnString + theText.at(itPos);
	}
	return returnString;
}

// This is a rock-n-roll string trimming thing
// Taken from: http://www.codeproject.com/vcpp/stl/stdstringtrim.asp
// (Needed to be cleaned up a fair bit - what is this? Java? Sheesh.
//  also used string, not wstring.)
std::wstring RnRXML::wRnRXMLDOM::m_TrimBothEnds(std::wstring theString, std::wstring charToTrim)
{
	if ( theString.empty() )
	{
		return theString;
	}
	if ( charToTrim.length() > 1)
	{	// Invalid trim character
		return theString;
	}
	// 
	using namespace std;
	wstring::size_type pos = theString.find_last_not_of(charToTrim[0]);
	if(pos != wstring::npos) 
	{
		theString.erase(pos + 1);
		pos = theString.find_first_not_of(charToTrim[0]);
		if(pos != wstring::npos) 
		{
			theString.erase(0, pos);
		}
	}
	else 
	{
		theString.erase(theString.begin(), theString.end());
	}
	return theString;
}

std::wstring RnRXML::wRnRXMLDOM::m_TrimBothEnds(std::wstring theString)
{
	if ( theString.empty() )
	{
		return theString;
	}
	// no spaces
	theString = this->m_TrimBothEnds(theString, L" ");
	theString = this->m_TrimBothEnds(theString, L"\t");	// Horizontal tab
	
	return theString;
}

std::wstring RnRXML::wRnRXMLDOM::m_ClearNonSpaceWhiteSpace(std::wstring theString)
{
	std::wstring returnString;
	std::wstring::iterator itPos;
	for (itPos = theString.begin(); itPos != theString.end(); itPos++)
	{
		wchar_t theChar = *itPos;
		if ((theChar == '\n') ||	
			 (theChar == '\r') ||	// Carriage return 
			 (theChar == '\b') ||	// Backspace
			 (theChar == '\a') ||	// Bell (alert)
			 (theChar == '\f') ||	// Formfeed 
			 (theChar == '\v') ||	// Vertical tab 
			 (theChar == '\t'))		// Horizontal tab
		{	// Replace with a space
			 theChar = ' ';
		}
		returnString = returnString + theChar;
	}
	return returnString;
}

std::wstring RnRXML::wRnRXMLDOM::m_CleanQuotesFromString(std::wstring theString)
{
	std::wstring returnString;
	std::wstring::iterator itChar;
	for (itChar = theString.begin(); itChar != theString.end(); itChar++)
	{
		if ( (*itChar) != '\"')
		{
			returnString = returnString + (*itChar);
		}
	}
	return returnString;
}

// Getters and Setters


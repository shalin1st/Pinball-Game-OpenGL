#include "CNVPair.h"
#include <sstream>		// for making the XML text
#include "../Utilities/CStringUtils.h"
#include <sstream>

void CNVPair::Clear(void)
{
	this->fvec4Value.x = this->fvec4Value.y = this->fvec4Value.z = this->fvec4Value.w = 0.0f;
	this->sValue = "";
	this->iValue = 0;
	// Get rid of all the child nodes, too
	for ( std::vector<CNVPair>::iterator itChild = this->vecChildNVPairs.begin(); itChild != this->vecChildNVPairs.end(); itChild++ )
	{
		itChild->Clear();
	}
	this->vecChildNVPairs.clear();

	return;
}

CNVPair::CNVPair()
{
	this->Clear();
	return;
}

CNVPair::CNVPair( const std::string &name )
{
	this->Clear();
	this->name = name;
	return;
}

CNVPair::CNVPair( const std::string &name, fvec4 fvec4Value )
{
	this->Clear();
	this->name = name;
	this->fvec4Value = fvec4Value;
	return;
}

CNVPair::CNVPair( const std::string &name, float x )
{
	this->Clear();
	this->name = name;
	this->fvec4Value.x = x;
	return;
}

CNVPair::CNVPair( const std::string &name, float x, float y )
{
	this->Clear();
	this->fvec4Value.x = x;
	this->fvec4Value.y = y;
	return;
}

CNVPair::CNVPair( const std::string &name, float x, float y, float z )
{
	this->Clear();
	this->fvec4Value.x = x;
	this->fvec4Value.y = y;
	this->fvec4Value.z = z;
	return;
}

CNVPair::CNVPair( const std::string &name, float x, float y, float z, float w )
{
	this->Clear();
	this->fvec4Value.x = x;
	this->fvec4Value.y = y;
	this->fvec4Value.z = z;
	this->fvec4Value.w = w;
	return;
}

CNVPair::CNVPair( const std::string &name, int iValue )
{
	this->Clear();
	this->name = name;
	this->iValue = iValue;
	return;
}

CNVPair::CNVPair( const std::string &name, std::string sValue )
{
	this->Clear();
	this->name = name;
	this->sValue = sValue;
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
                  const CNVPair childNVP3 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
                  const CNVPair &childNVP3, const CNVPair &childNVP4 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
                  const CNVPair childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
                  const CNVPair &childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5, const CNVPair &childNVP6 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	this->vecChildNVPairs.push_back( childNVP6 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
                  const CNVPair &childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5, const CNVPair &childNVP6, 
				  const CNVPair &childNVP7 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	this->vecChildNVPairs.push_back( childNVP6 );
	this->vecChildNVPairs.push_back( childNVP7 );
	return;
}

CNVPair::CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
                  const CNVPair &childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5, const CNVPair &childNVP6, 
				  const CNVPair &childNVP7, const CNVPair &childNVP8 )
{
	this->Clear();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	this->vecChildNVPairs.push_back( childNVP6 );
	this->vecChildNVPairs.push_back( childNVP8 );
	return;
}

void CNVPair::AddChild( const CNVPair &childNVP )
{
	this->vecChildNVPairs.push_back( childNVP );
	return;
}

bool CNVPair::bHadChildren(void)
{
	return ( ! this->vecChildNVPairs.empty() );
}


bool CNVPair::ScanChildrenForName( std::string nameToFind, CNVPair &childNVP, bool bScanRecursively /*= false*/ )
{
	for ( std::vector<CNVPair>::iterator itChildNVPair = this->vecChildNVPairs.begin();
		  itChildNVPair != this->vecChildNVPairs.end(); itChildNVPair++ )
	{
		if ( itChildNVPair->name == nameToFind )
		{
			childNVP = (*itChildNVPair);
			return true;
		}
		// Scan children's children?
		if ( bScanRecursively )
		{
			if ( itChildNVPair->ScanChildrenForName( nameToFind, childNVP, bScanRecursively ) )
			{
				return true;
			}
		}// if ( bScanRecursively )
	}// for ( std::vector<CNameValuePair>::iterator itChildNVPair 

	// Sorry it didn't work out...
	return false;
}

//  <NVPair>
//    <name></name>
//    <fvec4 x="0.0f" y="0.0f" z="0.0f" w="0.0f"></fvec4>
//    <int></int>
//    <string></string>
//    <children></children>
//  </NVPair>

// Mainly used for updating child nodes from parent
// NOTE: will REPLACE (call "new" on) the pXMLNode that is passed (i.e. DON'T create before calling)
bool CNVPair::GetXMLNodeFromState( RnRXML::RnRXMLNode* &pXMLNode )
{
	pXMLNode = new RnRXML::RnRXMLNode();
	pXMLNode->Name = "NVPair";
	pXMLNode->NodeType = RnRXML::enumRnRNodeTypes::RnR_TYPE_ELEMENT;
	pXMLNode->TagType = RnRXML::enumTagType::RnR_START_TAG;		// No children
		
	// Name
	RnRXML::RnRXMLNode* pNameNode = new RnRXML::RnRXMLNode();
	pNameNode->Name = "name";	
	pNameNode->SetValue( this->name );
	pNameNode->NodeType = RnRXML::enumRnRNodeTypes::RnR_TYPE_ELEMENT;
	pNameNode->TagType = RnRXML::enumTagType::RnR_START_TAG;
	pXMLNode->m_p_ChildNodes.push_back( pNameNode );

	// Actual values...
	RnRXML::RnRXMLNode* pfvec4ValueNode = new RnRXML::RnRXMLNode();
	pfvec4ValueNode->Name = "fvec4";
	pfvec4ValueNode->NodeType = RnRXML::enumRnRNodeTypes::RnR_TYPE_ELEMENT;
	pfvec4ValueNode->TagType = RnRXML::enumTagType::RnR_STARTEND_TAG;
	std::stringstream ssX; ssX << this->fvec4Value.x;
	std::stringstream ssY; ssY << this->fvec4Value.y;
	std::stringstream ssZ; ssZ << this->fvec4Value.z;
	std::stringstream ssW; ssW << this->fvec4Value.w;
	pfvec4ValueNode->AddOrOverwriteAttribute( "x", ssX.str() );
	pfvec4ValueNode->AddOrOverwriteAttribute( "y", ssY.str() );
	pfvec4ValueNode->AddOrOverwriteAttribute( "z", ssZ.str() );
	pfvec4ValueNode->AddOrOverwriteAttribute( "w", ssW.str() );
	pXMLNode->m_p_ChildNodes.push_back( pfvec4ValueNode );

	RnRXML::RnRXMLNode* pIntValNode = new RnRXML::RnRXMLNode();
	pIntValNode->Name = "int";
	std::stringstream ssIntValue;	ssIntValue << this->iValue;
	pIntValNode->SetValue( ssIntValue.str() );
	pIntValNode->NodeType = RnRXML::enumRnRNodeTypes::RnR_TYPE_ELEMENT;
	pIntValNode->TagType = RnRXML::enumTagType::RnR_START_TAG;
	pXMLNode->m_p_ChildNodes.push_back( pIntValNode );

	RnRXML::RnRXMLNode* pStringValNode = new RnRXML::RnRXMLNode();
	pStringValNode->Name = "string";
	pStringValNode->SetValue( this->sValue );
	pStringValNode->NodeType = RnRXML::enumRnRNodeTypes::RnR_TYPE_ELEMENT;
	pStringValNode->TagType = RnRXML::enumTagType::RnR_START_TAG;
	pXMLNode->m_p_ChildNodes.push_back( pStringValNode );

	// Now go through all the children and do the same thing
	for ( std::vector<CNVPair>::iterator itChildNVP = this->vecChildNVPairs.begin();
			itChildNVP != this->vecChildNVPairs.end(); itChildNVP++ )
	{
		RnRXML::RnRXMLNode* pChildNode = 0;
		itChildNVP->GetXMLNodeFromState( pChildNode );
		pXMLNode->m_p_ChildNodes.push_back( pChildNode );
	}// for ( std::vector<CNameValuePair>::iterator itChildNVP...
		
	return true;
}


std::string CNVPair::get_fvec4ValueAsString(void)
{
	std::stringstream ss;
	ss << "xyzw: " 
		<< this->fvec4Value.x << ", " 
		<< this->fvec4Value.y << ", "
		<< this->fvec4Value.z << ", " 
		<< this->fvec4Value.w ;
	return ss.str();
}


bool CNVPair::GetXMLTextFromState( std::string &XMLText, bool bMakeItPretty )
{
	std::stringstream ssXML;
	RnRXML::RnRXMLNode* pXMLNode = 0;
	this->GetXMLNodeFromState( pXMLNode );
	pXMLNode->SaveXMLToStream( ssXML, 0, bMakeItPretty );
	XMLText = ssXML.str();
	return true;
}

// WARNING: These will REPLACE any state currently there
// Mainly used for updating child nodes from parent
bool CNVPair::CreateStateFromXMLNode( RnRXML::RnRXMLNode* &pXMLNode )
{

	return true;
}

bool CNVPair::CreateStateFromXMLText( std::string xmlText, std::string &error )
{
	//RnRXML::RnRXMLNode* pXMLNode = new RnRXML::RnRXMLNode();
	RnRXML::RnRXMLDOM* pXMLDom = new RnRXML::RnRXMLDOM();
	// Load XML text, but it won't have header information
	if ( !pXMLDom->LoadXMLFromXMLString( xmlText, error, true ) )
	{
		return false;
	}
	// It's loaded. Take the document node and start parsing
	RnRXML::RnRXMLNode* pNode = pXMLDom->pGetDocumentNodePointer();
	if ( pNode == 0 )
	{
		error = "Could not get the internal XML document node.";
		return false;
	}
	
	return true;
}

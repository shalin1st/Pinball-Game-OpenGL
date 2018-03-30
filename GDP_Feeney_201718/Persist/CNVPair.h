#ifndef _CNVPair_HG_
#define _CNVPair_HG_

#include <string>
#include <vector>

#include "RnRXML/RnRXML.h"

class CNVPair
{
public:
	class fvec4
	{
	public:
		fvec4() : x(0.0f), y(0.0f), z(0.0), w(1.0f) {};
		fvec4( float initX, float initY, float initZ, float initW ) : x(initX), y(initY), z(initZ), w(initW) {};
		float x, y, z, w;
	};	
	CNVPair();
	CNVPair( const std::string &name );
	CNVPair( const std::string &name, fvec4 fVec4Value );
	CNVPair( const std::string &name, float x );
	CNVPair( const std::string &name, float x, float y );
	CNVPair( const std::string &name, float x, float y, float z );
	CNVPair( const std::string &name, float x, float y, float z, float w );
	CNVPair( const std::string &name, int iValue );
	CNVPair( const std::string &name, std::string sValue );
	CNVPair( const std::string &name, const CNVPair &childNVP0 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
		            const CNVPair childNVP3 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
		            const CNVPair &childNVP3, const CNVPair &childNVP4 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
		            const CNVPair childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
		            const CNVPair &childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5, 
					const CNVPair &childNVP6 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
		            const CNVPair &childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5, 
					const CNVPair &childNVP6, const CNVPair &childNVP7 );
	CNVPair( const std::string &name, const CNVPair &childNVP0, const CNVPair &childNVP1, const CNVPair &childNVP2, 
		            const CNVPair &childNVP3, const CNVPair &childNVP4, const CNVPair &childNVP5, 
					const CNVPair &childNVP6, const CNVPair &childNVP7, const CNVPair &childNVP8 );

	// These uses the RnRXML class to create and read XML persist information
	// NOTE: will REPLACE (call "new" on) the pXMLNode that is passed (i.e. DON'T create before calling)	
	bool GetXMLNodeFromState( RnRXML::RnRXMLNode* &pXMLNode );		// Mainly used for updating child nodes from parent
	bool GetXMLTextFromState( std::string &XMLText, bool bMakeItPretty = false );
	// WARNING: These will REPLACE any state currently there
	bool CreateStateFromXMLNode( RnRXML::RnRXMLNode* &pXMLNode );	// Mainly used for updating child nodes from parent
	bool CreateStateFromXMLText( std::string xmlText, std::string &errors );	

	void AddChild( const CNVPair &childNVP );
	bool bHadChildren(void);
	// ****************
	std::string name;
	// ** The values **
	fvec4 fvec4Value;		// Can also be used for one float
	std::string get_fvec4ValueAsString(void);
	int iValue;
	std::string sValue;
	// ****************
	void Clear(void);
	std::vector<CNVPair> vecChildNVPairs;
	// Helper function to scan through the children, looking for certain commands
	// Will only scan one level deep unless you tell it to (but is that a Good Idea?)
	// Will return after finding the first one. If you want to "beef this up", you could 
	//	emulate the STL find() and find_next() type methods which can take a starting iterator, which you can 
	//  pass the iterator in as a starting point
	bool ScanChildrenForName( std::string nameToFind, CNVPair &childNVP, bool bScanRecursively = false );
};




#endif 











//typedef struct fvec4
//{
//	fvec4() : x(0.0f), y(0.0f), z(0.0), w(1.0f) {};
//	float x, y, z, w;
//} fvec4;
//
//union TheDataTypes
//{
//	// VS work-around from: http://stackoverflow.com/questions/21968635/invalid-union-member
//	struct {
//		fvec4 fVec4;
//	};
//	float fValue;
//	bool bValue;
//	int iValue;
//	unsigned int uiValue;
//};
//
//class CNameValuePair
//{
//public:
//	enum enumTypeInUse
//	{
//		IS_A_STRING,
//		IS_A_VEC4,
//		IS_A_FLOAT,
//		IS_AN_INT,
//		IS_AN_UINT,
//		UNKNOWN_OR_NOT_SET
//	};

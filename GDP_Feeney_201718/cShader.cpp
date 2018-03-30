#include "cShaderManager.h"
#include <string.h>

cShaderManager::cShader::cShader()
{
	this->ID = 0;
	this->shaderType = cShader::UNKNOWN;
	return;
}

cShaderManager::cShader::~cShader() 
{
	return;
}

std::string cShaderManager::cShader::getShaderTypeString(void)
{
	switch ( this->shaderType )
	{
	case cShader::VERTEX_SHADER:
		return "VERTEX_SHADER";
		break;
	case cShader::FRAGMENT_SHADER:
		return "FRAGMENT_SHADER";
		break;
	case cShader::GEOMETRY_SHADER:
		return "GEOMETRY_SHADER";
		break;
	case cShader::UNKNOWN:
	default:
		return "UNKNOWN_SHADER_TYPE";
		break;
	}
	// Should never reach here...
	return "UNKNOWN_SHADER_TYPE";
}


void cShaderManager::cShader::parseStringIntoMultiLine(std::string singleLineSouce)
{
#ifdef _WIN32
	#pragma warning(disable : 4996)				// bitching about strcpy_s
#endif
//#ifdef _WIN64
//	#pragma warning(disable : 4996)				// bitching about strcpy_s
//#endif
	this->vecSource.clear();

	unsigned int stringsizeX4 = (unsigned int)singleLineSouce.size() * 4;
	char* charSourceString = new char[stringsizeX4];
	strcpy(charSourceString, singleLineSouce.c_str());

	char * pCharLine;
	pCharLine = strtok(charSourceString,"\n");
	//char *next_token = NULL;  
	//pCharLine = strtok_s(charSourceString,"\n",&next_token);

	while (pCharLine != NULL)
	{	// Push back this line
		this->vecSource.push_back( std::string(pCharLine) );
		pCharLine = strtok (NULL, "\n");
		//pCharLine = strtok_s(NULL,"\n",&next_token);
	}
	delete [] charSourceString;
	return;
//#undef _CRT_SECURE_NO_WARNINGS
}

// Vertex attributes
cShaderManager::cShader::cAttributeDesc::cAttributeDesc()
{
	this->Clear();
}

void cShaderManager::cShader::cAttributeDesc::Clear(void)
{
	this->programName = "";
	this->programID = 0;
	this->name = "";
	this->index = 0;
	this->size = 0;
	this->type = GL_ID_UNKNOWN;
	this->typeAsString = "";
	return;
}

cShaderManager::cShader::cUniformDesc::cUniformDesc()
{
	this->Clear();
	return;
}

void cShaderManager::cShader::cUniformDesc::Clear(void)
{
	this->programName = "";
	this->programID = 0;
	this->name = "";
	this->index = 0;
	this->location = 0;
	this->size = 0;
	this->type = GL_ID_UNKNOWN;
	this->typeAsString = "";
	// Added
	this->NUB_Index = -1;	// -1 if in default block
	this->blockOffsetInBytes = -1;
	this->blockOffsetInFloats = -1;
	this->bIsInDefaultBlock = true;
	this->nameLength = 0;
	this->arrayStride = -1;
	this->bIsAnArray = false;
	this->matrixStride = -1;
	this->bIsRowMajor = false;
	this->bIsMatrix = false;
	return;
}

// This is used to compare different uniform blocks to see if they match. 
// Basically, you call this on each uniform in a block, in the index order, and compare them.
// (this is only done when you are loading interface blocks so it's only done when created 
//  or assinged, so performance isn't a big deal)
std::string cShaderManager::cShader::cUniformDesc::GenerateInterfaceDefinitionHash(void)
{
	// Note we aren't including the name as it could be different in different blocks
	// This only really gives us information about the particular uniform within the block:
	//  where it is (offset) and what kind of thing it is (type, matrix, array, strides, etc.)
	// For any given uniform, these should match. 
	std::stringstream ss;
	ss << typeAsString << "," << size << ","
		<< blockOffsetInBytes << "," << arrayStride << "," << matrixStride << ","
		<< ( bIsAnArray ? "Is_Array" : "Not_Array" ) << ","
		<< ( bIsRowMajor ? "Row_Major" : "Col_Major" ) << ","
		<< ( bIsMatrix ? "Is_Matrix" : "Not_Matrix" );
	return ss.str();
}

cShaderManager::cShaderProgram::CNamedUniformBlockDesc::CNamedUniformBlockDesc()
{
	this->Clear();
	return;
}

void cShaderManager::cShaderProgram::CNamedUniformBlockDesc::Clear(void)
{
	this->programID = -1;
	this->nameLength = 0;				// GL_UNIFORM_BLOCK_NAME_LENGTH?
	this->bindingPoint = -1;			// GL_UNIFORM_BLOCK_BINDING
	this->uniformBlockIndex = -1;
	this->dataSizeInBytes = 0;			// GL_UNIFORM_BLOCK_DATA_SIZE

	this->bReferencedFromVertexShader = false;
	this->bReferencedFromTessControlShader = false;
	this->bReferencedFromTessEvaluationShader = false;
	this->bReferencedFromGeometryShader = false;
	this->bReferencedFromFragmentShader = false;
	this->bReferencedFromComputeShader = false;

	return;
}
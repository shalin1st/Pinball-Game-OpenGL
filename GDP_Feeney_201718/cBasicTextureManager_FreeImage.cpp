#include "cBasicTextureManager.h"

#include <FreeImage/FreeImage.h>

#include <sstream>

void cBasicTextureManager::SetBasePath(std::string basepath)
{
	this->m_basePath = basepath;
	return;
}

bool cBasicTextureManager::Create2DTextureFromImageFile( std::string textureFileName, bool bGenerateMIPMap )
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;

	FreeImage_Initialise( TRUE );
	 
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType( fileToLoadFullPath.c_str(), 0 );
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
	{
		fif = FreeImage_GetFIFFromFilename( fileToLoadFullPath.c_str() );
	}
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
	{
		return false;
	}

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load( fif, fileToLoadFullPath.c_str() );
	}
	//if the image failed to load, return failure
	if(!dib)
	{
		return false;
	}

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
	{
		return false;
	}
	
	//if this texture ID is in use, unload the current texture
//	if( m_texID.find(texID) != m_texID.end() )
//	{
//		glDeleteTextures(1, &(m_texID[texID]));
//	}

	//OpenGL's image ID to map to
	GLuint gl_texID = 0;
	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
//	m_texID[texID] = gl_texID;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use
//	glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height,
//		border, image_format, GL_UNSIGNED_BYTE, bits);

	glTexStorage2D( GL_TEXTURE_2D, 
		        1, 
				GL_RGBA8, 
				width, 
				height );

	if ( bGenerateMIPMap )
	{
		glGenerateMipmap( GL_TEXTURE_2D );		// OpenGL 4.0
	}

	//if ( this->bWasThereAnOpenGLError() )	{ return false;	}


	//glBindTexture(GL_TEXTURE_2D, m_textureNumber);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, /*GL_CLAMP*/ GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, /*GL_CLAMP*/ GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, /*GL_NEAREST*/ GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	FreeImage_DeInitialise();

	return true;
}

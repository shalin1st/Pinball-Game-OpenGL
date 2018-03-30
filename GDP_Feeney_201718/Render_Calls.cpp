#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <iostream>

// cSimpleAssimpSkinnedMesh class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include "cAnimationState.h"

// HACK
#include "cFBO.h"
// Here, the scene is rendered in 3 passes:
// 1. Render geometry to G buffer
// 2. Perform deferred pass, rendering to Deferred buffer
// 3. Then post-pass ("2nd pass" to the scree)
//    Copying from the Pass2_Deferred buffer to the final screen
extern cFBO g_FBO_Pass1_G_Buffer;
extern cFBO g_FBO_Pass2_Deferred;

// Draw a single object
// If pParentGO == NULL, then IT'S the parent
void DrawObject( cGameObject* pTheGO, cGameObject* pParentGO );

// Draws one mesh in the game object. 
// The game object is passed to get the orientation.
void DrawMesh( sMeshDrawInfo &theMesh, cGameObject* pTheGO );



//    ___  _    _                      _  __  __           _     
//   / __|| |__(_) _ _   _ _   ___  __| ||  \/  | ___  ___| |_   
//   \__ \| / /| || ' \ | ' \ / -_)/ _` || |\/| |/ -_)(_-<| ' \  
//   |___/|_\_\|_||_||_||_||_|\___|\__,_||_|  |_|\___|/__/|_||_| 
//                                                               
void CalculateSkinnedMeshBonesAndLoad( sMeshDrawInfo &theMesh, cGameObject* pTheGO,
                                       unsigned int UniformLoc_numBonesUsed,
									   unsigned int UniformLoc_bonesArray );



// This a simple cached texture binding system
void setTextureBindings(GLint shaderID, sMeshDrawInfo &theMesh);

// See: http://www.glfw.org/docs/latest/window_guide.html#window_size
void window_size_callback(GLFWwindow* window, int width, int height)
{
	if ( ( ::g_FBO_Pass1_G_Buffer.width != width ) || ( ::g_FBO_Pass1_G_Buffer.height != height ) )
	{
		// Window size has changed, so resize the offscreen frame buffer object
		std::string error;
		if ( ! ::g_FBO_Pass1_G_Buffer.reset( width, height, error ) )
		{
			std::cout << "In window_size_callback(), the g_FBO_Pass1_G_Buffer.reset() call returned an error:" << std::endl;
			std::cout << "\t" << error << std::endl;
		}
		else
		{
			std::cout << "Offscreen g_FBO_Pass1_G_Buffer now: " << width << " x " << height << std::endl;
		}
	}//if ( ( ::g_FBO_Pass1_G_Buffer.width....

	if ( ( ::g_FBO_Pass2_Deferred.width != width ) || ( ::g_FBO_Pass2_Deferred.height != height ) )
	{
		// Window size has changed, so resize the offscreen frame buffer object
		std::string error;
		if ( ! ::g_FBO_Pass2_Deferred.reset( width, height, error ) )
		{
			std::cout << "In window_size_callback(), the g_FBO_Pass2_Deferred.reset() call returned an error:" << std::endl;
			std::cout << "\t" << error << std::endl;
		}
		else
		{
			std::cout << "Offscreen g_FBO_Pass2_Deferred now: " << width << " x " << height << std::endl;
		}
	}//if ( ( ::g_FBO_Pass1_G_Buffer.width....

	return;
}


void RenderScene( std::vector< cGameObject* > &vec_pGOs, GLFWwindow* pGLFWWindow, double deltaTime )
{

		float ratio;
		int width, height;
		glm::mat4x4 matProjection;			// was "p"
	
		glfwGetFramebufferSize(pGLFWWindow, &width, &height);
		ratio = width / (float) height;
		glViewport(0, 0, width, height);
	
//		// Clear colour AND depth buffer
//		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//glEnable(GL_DEPTH_TEST);
	
		//        glUseProgram(program);
		::g_pShaderManager->useShaderProgram( "mySexyShader" );
		GLint curShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
	
		// Update all the light uniforms...
		// (for the whole scene)
		::g_pLightManager->CopyLightInformationToCurrentShader();
	
		// Projection and view don't change per scene (maybe)
		matProjection = glm::perspective( 0.6f,			// FOV
											ratio,		// Aspect ratio
											1.0f,		// Near (as big as possible)
											10000.0f );	// Far (as small as possible)
	
	
		// View or "camera" matrix
		glm::mat4 matView = glm::mat4(1.0f);	// was "v"
	
		// Now the veiw matrix is taken right from the camera class
		matView = ::g_pTheCamera->getViewMatrix();
		//matView = glm::lookAt( g_cameraXYZ,						// "eye" or "camera" position
		//					   g_cameraTarget_XYZ,		// "At" or "target" 
		//					   glm::vec3( 0.0f, 1.0f, 0.0f ) );	// "up" vector

		GLint uniLoc_mView = glGetUniformLocation( curShaderID, "mView" );
		GLint uniLoc_mProjection = glGetUniformLocation( curShaderID, "mProjection" );

		glUniformMatrix4fv( uniLoc_mView, 1, GL_FALSE, 
							(const GLfloat*) glm::value_ptr(matView) );
		glUniformMatrix4fv( uniLoc_mProjection, 1, GL_FALSE, 
							(const GLfloat*) glm::value_ptr(matProjection) );
	
		// Set ALL texture units and binding for ENTIRE SCENE (is faster)
		//{
		//	// 0 
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D,
		//		::g_pTextureManager->getTextureIDFromName("Utah_Teapot_xyz_n_uv_Enterprise.bmp"));
		//		//::g_pTextureManager->getTextureIDFromName(pTheGO->textureNames[0]));
		//	// 1
		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_2D,
		//		::g_pTextureManager->getTextureIDFromName("GuysOnSharkUnicorn.bmp"));
		//		//::g_pTextureManager->getTextureIDFromName(pTheGO->textureNames[1]));
		//	// 2..  and so on... 
	
		//	// Set sampler in the shader
		//	// NOTE: You shouldn't be doing this during the draw call...
		//	GLint curShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");
		//	GLint textSampler00_ID = glGetUniformLocation(curShaderID, "texSamp2D00");
		//	GLint textSampler01_ID = glGetUniformLocation(curShaderID, "texSamp2D01");
		//	// And so on (up to 10, or whatever number of textures)... 
	
		//	GLint textBlend00_ID = glGetUniformLocation(curShaderID, "texBlend00");
		//	GLint textBlend01_ID = glGetUniformLocation(curShaderID, "texBlend01");
	
		//	// This connects the texture sampler to the texture units... 
		//	glUniform1i(textSampler00_ID, 0);		// Enterprise
		//	glUniform1i(textSampler01_ID, 1);		// GuysOnSharkUnicorn
		//}
	
	
		// Enable blend ("alpha") transparency for the scene
		// NOTE: You "should" turn this OFF, then draw all NON-Transparent objects
		//       Then turn ON, sort objects from far to near ACCORDING TO THE CAMERA
		//       and draw them
		glEnable(GL_BLEND);		// Enables "blending"
								//glDisable( GL_BLEND );
								// Source == already on framebuffer
								// Dest == what you're about to draw
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
		// Draw the scene
		//unsigned int sizeOfVector = (unsigned int)::g_vecGameObjects.size();	//*****//
		unsigned int sizeOfVector = (unsigned int)vec_pGOs.size();	//*****//
		for ( int index = 0; index != sizeOfVector; index++ )
		{
			//cGameObject* pTheGO = ::g_vecGameObjects[index];
			cGameObject* pTheGO = vec_pGOs[index];

			// HACK!
			if ( pTheGO->friendlyName == "bugs" )
			{	// SUPER HACK: Slightly adjust the mesh inside the game object each frame
				pTheGO->vecMeshes[0].adjMeshOrientationEulerAngles(glm::vec3(0.35f, 0.5f, 0.15f), true );
			}
	
			// This is the top level vector, so they are all "parents" 
			DrawObject( pTheGO, NULL );
	
		}//for ( int index = 0...
	
	
	
		
		::g_pDebugRenderer->RenderDebugObjects(matView, matProjection, deltaTime);


	
		//



	return;
}



// Draw a single object
// If pParentGO == NULL, then IT'S the parent
void DrawObject( cGameObject* pTheGO, cGameObject* pParentGO )
{
	if (!pTheGO)
	{	// Shouldn't happen, but if GO pointer is invlaid, return
		return;
	}
	// Is there a game object? 
	if ( ! pTheGO->bIsVisible )	//if ( ::g_GameObjects[index] == 0 )
	{	// Nothing to draw

		// Draw any children
		unsigned int numChildGOs = (unsigned int)pTheGO->vec_pChildObjects.size();
		for ( unsigned int index = 0; index != numChildGOs; index++ ) 
		{
			DrawObject( pTheGO->vec_pChildObjects[index], pTheGO );
		}
	}

	if ( pTheGO->friendlyName == "ReallyBigShip" )
	{
		std::cout << "it's the really big ship!" << std::endl;
	}

	// Go through all meshes, drawing them
	unsigned int numMeshes = (unsigned int)pTheGO->vecMeshes.size();
	for ( unsigned int meshIndex = 0; meshIndex != numMeshes; meshIndex++ )
	{
		DrawMesh( pTheGO->vecMeshes[meshIndex], pTheGO );
	}


	return;
}

// Very quick and dirty way of connecting the samplers
namespace QnDTexureSamplerUtility
{
	void LoadUniformLocationsIfNeeded(unsigned int shaderID);
	void setAllSamplerUnitsToInvalidNumber( GLint invalidTextureUnit );
	void clearAllBlendValuesToZero(void);
	void SetSamplersForMeshTextures( sMeshDrawInfo &meshDrawInfo, 
									 std::map<std::string /*textureName*/, CTexUnitInfoBrief> &mapTexAndUnitInfo );
//	void set2DSamplerAndBlenderByIndex(GLint samplerIndex, float blendRatio, GLint textureUnitID );
//	void setCubeSamplerAndBlenderByIndex( GLint samplerIndex, float blendRatio, GLint textureUnitID );
};



// Draws one mesh in the game object. 
// The game object is passed to get the orientation.
void DrawMesh( sMeshDrawInfo &theMesh, cGameObject* pTheGO )
{
	if ( ! theMesh.bIsVisible )
	{
		return;
	}

	if ( pTheGO->friendlyName == LEFTTEAPOTNAME )
	{
		int sexyBreakpoint = 0;
	}


	sVAOInfo VAODrawInfo;
	if ( ::g_pVAOManager->lookupVAOFromName( theMesh.name, VAODrawInfo ) == false )
	{	// Didn't find mesh
		//std::cout << "WARNING: Didn't find mesh " << meshToDraw << " in VAO Manager" << std::endl;
		return;
	}
	
	// 'model' or 'world' matrix
	glm::mat4x4 mModel = glm::mat4x4(1.0f);	//		mat4x4_identity(m);
//	glm::mat4x4 mModel = pTheGO->getFinalMeshQOrientation();

	glm::mat4 trans = glm::mat4x4(1.0f);
	trans = glm::translate( trans, 
							theMesh.offset + pTheGO->getPosition() );
	mModel = mModel * trans; 

	//// Euler orientation stuff, which is so "Fall 2017"
	//// ***************************
	//// POST-Rotation
	//glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
	//matPostRotZ = glm::rotate( matPostRotZ, pTheGO->orientation2.z, 
	//							glm::vec3(0.0f, 0.0f, 1.0f) );
	//mModel = mModel * matPostRotZ;
//
	//glm::mat4 matPostRotY = glm::mat4x4(1.0f);
	//matPostRotY = glm::rotate( matPostRotY, pTheGO->orientation2.y, 
	//							glm::vec3(0.0f, 1.0f, 0.0f) );
	//mModel = mModel * matPostRotY;
//
	//glm::mat4 matPostRotX = glm::mat4x4(1.0f);
	//matPostRotX = glm::rotate( matPostRotX, pTheGO->orientation2.x, 
	//							glm::vec3(1.0f, 0.0f, 0.0f) );
	//mModel = mModel * matPostRotX;
	//// ***************************
	
	// Now with quaternion rotation
	// This is combined with the original game object
	// (i.e. if the game object is rotated, then the mesh is rotated relative to the game object)
	glm::mat4 postRotQuat = glm::mat4( pTheGO->getFinalMeshQOrientation( theMesh.getQOrientation() ) );
	mModel = mModel * postRotQuat;

//	mModel = mModel * trans * postRotQuat;


	// The scale is relative to the original model
	glm::mat4 matScale = glm::mat4x4(1.0f);
	matScale = glm::scale( matScale, 
							glm::vec3( theMesh.scale,
								       theMesh.scale,
								       theMesh.scale ) );
	mModel = mModel * matScale;


//	//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
////			p = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f );
//	p = glm::perspective( 0.6f,			// FOV
//							ratio,		// Aspect ratio
//							0.1f,			// Near (as big as possible)
//							1000.0f );	// Far (as small as possible)
//
//	// View or "camera" matrix
//	glm::mat4 v = glm::mat4(1.0f);	// identity
//
//	//glm::vec3 cameraXYZ = glm::vec3( 0.0f, 0.0f, 5.0f );	// 5 units "down" z
//	v = glm::lookAt( g_cameraXYZ,						// "eye" or "camera" position
//						g_cameraTarget_XYZ,		// "At" or "target" 
//						glm::vec3( 0.0f, 1.0f, 0.0f ) );	// "up" vector
//
//
////        glUseProgram(program);
//	::g_pShaderManager->useShaderProgram( "mySexyShader" );
//	GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

//			mvp = p * v * m;			// This way (sort of backwards)
//			glUniformMatrix4fv(mvp_location, 1, GL_FALSE, 
//							   (const GLfloat*) glm::value_ptr(mvp) );

	// All of these shouldn't be in the render loop, as they 
	//	aren't changing if we are using the same shader.
	//GLint mModel_locID = glGetUniformLocation( shaderID, "mModel" );
	//GLint mView_locID = glGetUniformLocation( shaderID, "mView" );
	//GLint mProjection = glGetUniformLocation( shaderID, "mProjection" );

	GLint curShaderProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	GLint uniLoc_mModel = glGetUniformLocation( curShaderProgID, "mModel" );
	glUniformMatrix4fv( uniLoc_mModel, 1, GL_FALSE, 
						(const GLfloat*) glm::value_ptr(mModel) );

	glm::mat4 mWorldInTranpose = glm::inverse(glm::transpose(mModel));
	GLint uniLoc_mWorldInvTrans = glGetUniformLocation( curShaderProgID, "mWorldInvTranspose");
	glUniformMatrix4fv( uniLoc_mWorldInvTrans, 1, GL_FALSE, 
						(const GLfloat*) glm::value_ptr(mWorldInTranpose) );
	

	//GLint myLight_00_diffuse = glGetUniformLocation( shaderID, "myLight[0].diffuse" );
	//GLint myLight_01_diffuse = glGetUniformLocation( shaderID, "myLight[1].diffuse" );
	//GLint myLight_02_diffuse = glGetUniformLocation( shaderID, "myLight[2].diffuse" );
	// 
	//glUniform4f( myLight_00_diffuse, 1.0f, 1.0f, 1.0f, 1.0f );

//			GLint diffuseColour_loc = glGetUniformLocation( shaderID, "diffuseColour" );

	// Other uniforms:
	GLint uniLoc_eyePosition = glGetUniformLocation(curShaderProgID, "eyePosition");
	glm::vec3 eye = ::g_pTheCamera->getEyePosition();
	glUniform3f(uniLoc_eyePosition, eye.x, eye.y, eye.z );


	// Diffuse is often 0.2-0.3 the value of the diffuse
	GLint uniLoc_ambientToDiffuseRatio = glGetUniformLocation(curShaderProgID, "ambientToDiffuseRatio");
	glUniform1f( uniLoc_ambientToDiffuseRatio, 0.2f );

	// Specular: For now, set this colour to white, and the shininess to something high 
	//	it's an exponent so 64 is pretty shinny (1.0 is "flat", 128 is excessively shiny)
	GLint uniLoc_materialSpecular = glGetUniformLocation(curShaderProgID, "materialSpecular");
	glUniform4f( uniLoc_materialSpecular, 1.0f, 1.0f, 1.0f, 64.0f );

	GLint uniLoc_materialDiffuse = glGetUniformLocation(curShaderProgID, "materialDiffuse");	
	glUniform4f( uniLoc_materialDiffuse, 
	             theMesh.debugDiffuseColour.r, 
	             theMesh.debugDiffuseColour.g, 
	             theMesh.debugDiffuseColour.b, 
	             theMesh.debugDiffuseColour.a );



//	cShaderManager::cShaderProgram* pShaderProgram = ::g_pShaderManager->getShaderProgPointerFromID( curShaderProgID );
//	if ( pShaderProgram )
//	{
//		pShaderProgram->set2DTextureBindings( theMesh.vecMehs2DTextures, ::g_pTextureManager );
//	}

	// Set ALL the samplers to something (so they don't point to GL_TEXTURE0)
	QnDTexureSamplerUtility::LoadUniformLocationsIfNeeded(curShaderProgID);
	// There's no way we have 999 texture units...
	QnDTexureSamplerUtility::setAllSamplerUnitsToInvalidNumber(999);
	QnDTexureSamplerUtility::clearAllBlendValuesToZero();

	// Now set our samplers, and blend function to something
	// Basic texture binding setup (assign all loaded textures to samplers)
	g_pTextureManager->UpdateTextureBindingsByTextureNameSimple();
	std::map<std::string, CTexUnitInfoBrief> mapTexNameToTexUnit;
	g_pTextureManager->GetBoundTextureUnitsByTextureNames( mapTexNameToTexUnit );

	// Now look up what textures our object is using and set the samplers
	QnDTexureSamplerUtility::SetSamplersForMeshTextures( theMesh, mapTexNameToTexUnit );


	// change one of the samplers to be the FBO texture texture
	// uniform sampler2D texSamp2D00;		// Represents a 2D image


	// ***************************************
	// Texture based height map example code
	//	uniform sampler2D texHeightMap;
	//	uniform bool bIsHeightMap;
	GLint texHeightMap_UniLoc = glGetUniformLocation(curShaderProgID, "texHeightMap");
	GLint bIsHeightMap_UniLoc = glGetUniformLocation(curShaderProgID, "bIsHeightMap");

//	// HACK: Check for the mesh name
//	if ( theMesh.name == "MeshLabTerrain_xyz_n_uv" )
//	{	// YES! It's the height map!
//		glUniform1f( bIsHeightMap_UniLoc, GL_TRUE );
//		// Also set the texture in the VERTEX shader...
//		// What texture Unit was the height map assigned to?
////		std::map<std::string, CTexUnitInfoBrief>::iterator itTexHM 
////					= mapTexNameToTexUnit.find("height_map_norway-height-map-aster-30m.bmp");
////		// DANGER Will Robinson! 
////		int texureUnit = itTexHM->second.texUnitNumOffsetBy_GL_TEXTURE0;
////		glUniform1f(texHeightMap_UniLoc, texureUnit );
//
//		// This is the TEXTURE ID, NOT the "texture UNIT" ID
//		GLint textureNameID 
//			= ::g_pTextureManager->getTextureIDFromTextureName("A_height_map_norway-height-map-aster-30m.bmp");
//
//		// Pick a texture unit we are not likely using elswhere
//		glActiveTexture(GL_TEXTURE31);
//		// Connect the TEXTURE to the TEXTURE UNIT
////		glBindTexture(GL_TEXTURE_2D, textureNameID);
//		glBindTexture(GL_TEXTURE_2D, textureNameID);
//		// Now assign the sampler to the texture Unit
//		glUniform1f(texHeightMap_UniLoc, 31 );		// Because GL_TEXTURE31
//
//
//		// At this point, the black and white height map is connected to the texture unit
//		//	that the texture manager assigned it to in the UpdateTextureBindingsByTextureNameSimple()
//		//	call. 
//	}
//	else
	{
		glUniform1f( bIsHeightMap_UniLoc, GL_FALSE );
	}


	// ***************************************

	//// Load the mesh textures, if they are invalid
	//GLint texSampCube00_LocID = glGetUniformLocation(curShaderProgID, "texSampCube00");
	//// At last run, "space" texture was set to TextureUnit #4
	//// (We aren't loading any more textures RIGHT NOW, so we'll hard code it to 4)
	//glUniform1i(texSampCube00_LocID, 4);
	//// Our cube map (of space) is on this sampler

	GLint uniLoc_bIsSkyBoxObject = glGetUniformLocation(curShaderProgID, "isASkyBox");
	if (theMesh.bIsSkyBoxObject)
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_TRUE);
	}
	else
	{
		glUniform1f(uniLoc_bIsSkyBoxObject, GL_FALSE);
	}


	GLint uniLoc_bIsDebugWireFrameObject = glGetUniformLocation(curShaderProgID, "bIsDebugWireFrameObject");

	// Wireframe?
	if ( theMesh.bDrawAsWireFrame )
	{
		glUniform1f( uniLoc_bIsDebugWireFrameObject, 1.0f );	// TRUE
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );	// Default
	}
	else
	{
		glUniform1f( uniLoc_bIsDebugWireFrameObject, 0.0f );	// FALSE
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Default
	}
	
	// GL_CULL_FACE
	if ( theMesh.bDisableBackFaceCulling )
	{
		glDisable( GL_CULL_FACE );
	}
	else
	{
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
	}// GL_CULL_FACE

	// GL_DEPTH_TEST
	if ( theMesh.bDisableDepthTest )
	{
		glDisable(GL_DEPTH_TEST);		// Test for z and store in z buffer
	}
	else
	{
		glEnable(GL_DEPTH_TEST);		// Test for z and store in z buffer
	}// GL_DEPTH_TEST


	// STARTOF: Reflection and refraction shader uniforms
	GLint isReflectRefract_UniLoc = glGetUniformLocation(curShaderProgID, "isReflectRefract" );
	GLint reflectBlendRatio_UniLoc = glGetUniformLocation(curShaderProgID, "reflectBlendRatio");		// How much reflection (0-1)
	GLint refractBlendRatio_UniLoc = glGetUniformLocation(curShaderProgID, "refractBlendRatio");		// How much refraction (0-1)
	GLint coefficientRefract_UniLoc = glGetUniformLocation(curShaderProgID, "coefficientRefract");		// coefficient of refraction 

	// Is this the reflective sphere
	if ( theMesh.bIsEnvirMapped )
	{
		glUniform1f( isReflectRefract_UniLoc, GL_TRUE );
	}
	else
	{
		glUniform1f( isReflectRefract_UniLoc, GL_FALSE );
	}
	// 
	glUniform1f( reflectBlendRatio_UniLoc, theMesh.reflectBlendRatio );
	glUniform1f(refractBlendRatio_UniLoc, theMesh.refractBlendRatio );
	glUniform1f(coefficientRefract_UniLoc, theMesh.coefficientRefract );
	// And more environment things

	GLint bUseTextureAsDiffuse_UniLoc = glGetUniformLocation( curShaderProgID, "bUseTextureAsDiffuse" );

	if ( theMesh.bUseDebugColour )
	{
		GLint materialDiffuse_UniLoc = glGetUniformLocation( curShaderProgID, "materialDiffuse" );
		glUniform4f( materialDiffuse_UniLoc, 
					 theMesh.debugDiffuseColour.x, 
					 theMesh.debugDiffuseColour.y, 
					 theMesh.debugDiffuseColour.z, 
					 theMesh.debugDiffuseColour.w );
		glUniform1f( bUseTextureAsDiffuse_UniLoc, GL_FALSE );
	}
	else
	{
		glUniform1f( bUseTextureAsDiffuse_UniLoc, GL_TRUE );
	}

					 


	// ***************************************************
	//    ___  _    _                      _  __  __           _     
	//   / __|| |__(_) _ _   _ _   ___  __| ||  \/  | ___  ___| |_   
	//   \__ \| / /| || ' \ | ' \ / -_)/ _` || |\/| |/ -_)(_-<| ' \  
	//   |___/|_\_\|_||_||_||_||_|\___|\__,_||_|  |_|\___|/__/|_||_| 
	//                                                               
	GLint UniLoc_IsSkinnedMesh = glGetUniformLocation( curShaderProgID, "bIsASkinnedMesh");

	if ( pTheGO->pSimpleSkinnedMesh )
	{
		// Calculate the pose and load the skinned mesh stuff into the shader, too
		GLint UniLoc_NumBonesUsed = glGetUniformLocation( curShaderProgID, "numBonesUsed");
		GLint UniLoc_BoneIDArray = glGetUniformLocation( curShaderProgID, "bones");
		CalculateSkinnedMeshBonesAndLoad( theMesh, pTheGO, UniLoc_NumBonesUsed, UniLoc_BoneIDArray );

		glUniform1f( UniLoc_IsSkinnedMesh, GL_TRUE );
	}
	else 
	{
		glUniform1f( UniLoc_IsSkinnedMesh, GL_FALSE );
	}

	// ***************************************************


	// EDNOF: Reflection and refraction shader uniforms

	glBindVertexArray( VAODrawInfo.VAO_ID );

	glDrawElements( GL_TRIANGLES, 
					VAODrawInfo.numberOfIndices,		// testMesh.numberOfTriangles * 3,	// How many vertex indices
					GL_UNSIGNED_INT,					// 32 bit int 
					0 );
	// Unbind that VAO
	glBindVertexArray( 0 );

	return;
}



// Very quick and dirty way of connecting the samplers
namespace QnDTexureSamplerUtility
{
	bool bIsLoaded = false;

	// 2D textures:
	GLint texSamp2D00_LocID = -1;
	GLint texSamp2D01_LocID = -1;
	GLint texSamp2D02_LocID = -1;
	GLint texSamp2D03_LocID = -1;
	GLint texSamp2D04_LocID = -1;
	GLint texSamp2D05_LocID = -1;
	GLint texSamp2D06_LocID = -1;
	GLint texSamp2D07_LocID = -1;

	GLint texBlend00_LocID = -1;
	GLint texBlend01_LocID = -1;
	GLint texBlend02_LocID = -1;
	GLint texBlend03_LocID = -1;
	GLint texBlend04_LocID = -1;
	GLint texBlend05_LocID = -1;
	GLint texBlend06_LocID = -1;
	GLint texBlend07_LocID = -1;

	// Cube map textures
	GLint texSampCube00_LocID = -1;
	GLint texSampCube01_LocID = -1;
	GLint texSampCube02_LocID = -1;
	GLint texSampCube03_LocID = -1;

	GLint texCubeBlend00_LocID = -1;
	GLint texCubeBlend01_LocID = -1;
	GLint texCubeBlend02_LocID = -1;
	GLint texCubeBlend03_LocID = -1;

	void LoadUniformLocationsIfNeeded(unsigned int shaderID)
	{
		if ( ! bIsLoaded )	
		{	// Load the uniform locations
			texSamp2D00_LocID = glGetUniformLocation( shaderID, "texSamp2D00" );
			texSamp2D01_LocID = glGetUniformLocation( shaderID, "texSamp2D01" );
			texSamp2D02_LocID = glGetUniformLocation( shaderID, "texSamp2D02" );
			texSamp2D03_LocID = glGetUniformLocation( shaderID, "texSamp2D03" );
			texSamp2D04_LocID = glGetUniformLocation( shaderID, "texSamp2D04" );
			texSamp2D05_LocID = glGetUniformLocation( shaderID, "texSamp2D05" );
			texSamp2D06_LocID = glGetUniformLocation( shaderID, "texSamp2D06" );
			texSamp2D07_LocID = glGetUniformLocation( shaderID, "texSamp2D07" );

			texBlend00_LocID = glGetUniformLocation( shaderID, "texBlend00" );
			texBlend01_LocID = glGetUniformLocation( shaderID, "texBlend01" );
			texBlend02_LocID = glGetUniformLocation( shaderID, "texBlend02" );
			texBlend03_LocID = glGetUniformLocation( shaderID, "texBlend03" );
			texBlend04_LocID = glGetUniformLocation( shaderID, "texBlend04" );
			texBlend05_LocID = glGetUniformLocation( shaderID, "texBlend05" );
			texBlend06_LocID = glGetUniformLocation( shaderID, "texBlend06" );
			texBlend07_LocID = glGetUniformLocation( shaderID, "texBlend07" );

			texSampCube00_LocID = glGetUniformLocation( shaderID, "texSampCube00" );
			texSampCube01_LocID = glGetUniformLocation( shaderID, "texSampCube00" );
			texSampCube02_LocID = glGetUniformLocation( shaderID, "texSampCube00" );
			texSampCube03_LocID = glGetUniformLocation( shaderID, "texSampCube00" );

			texCubeBlend00_LocID = glGetUniformLocation( shaderID, "texCubeBlend00" );
			texCubeBlend01_LocID = glGetUniformLocation( shaderID, "texCubeBlend01" );
			texCubeBlend02_LocID = glGetUniformLocation( shaderID, "texCubeBlend02" );
			texCubeBlend03_LocID = glGetUniformLocation( shaderID, "texCubeBlend03" );

			bIsLoaded = true;	// So we won't call these again
		}

		return;
	}// void LoadUniformLocationsIfNeeded()


	void setAllSamplerUnitsToInvalidNumber( GLint invalidTextureUnit )
	{
		glUniform1i( texSamp2D00_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D01_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D02_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D03_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D04_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D05_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D06_LocID, invalidTextureUnit );
		glUniform1i( texSamp2D07_LocID, invalidTextureUnit );

		// Cube map textures
		glUniform1i( texSampCube00_LocID, invalidTextureUnit );
		glUniform1i( texSampCube01_LocID, invalidTextureUnit );
		glUniform1i( texSampCube02_LocID, invalidTextureUnit );
		glUniform1i( texSampCube03_LocID, invalidTextureUnit );

		return;
	}//void setAllSamplerUnitsToInvalidNumber()

	void clearAllBlendValuesToZero(void)
	{
		glUniform1f( texBlend00_LocID, 0.0f );
		glUniform1f( texBlend01_LocID, 0.0f );
		glUniform1f( texBlend02_LocID, 0.0f );
		glUniform1f( texBlend03_LocID, 0.0f );
		glUniform1f( texBlend04_LocID, 0.0f );
		glUniform1f( texBlend05_LocID, 0.0f );
		glUniform1f( texBlend06_LocID, 0.0f );
		glUniform1f( texBlend07_LocID, 0.0f );

		glUniform1f( texCubeBlend00_LocID, 0.0f );
		glUniform1f( texCubeBlend01_LocID, 0.0f );
		glUniform1f( texCubeBlend02_LocID, 0.0f );
		glUniform1f( texCubeBlend03_LocID, 0.0f );

		return;
	}//void clearAllBlendValuesToZero()

	// Handy function to set sampelrs
	void set2DSamplerAndBlenderByIndex(GLint samplerIndex, float blendRatio, GLint textureUnitID )
	{
		switch (samplerIndex)			
		{
		case 0:	
			glUniform1i( texSamp2D00_LocID, textureUnitID );	
			glUniform1f( texBlend00_LocID, blendRatio );
			break;
		case 1:	
			glUniform1i( texSamp2D01_LocID, textureUnitID );	
			glUniform1f( texBlend01_LocID, blendRatio );
			break;
		case 2:	
			glUniform1i( texSamp2D02_LocID, textureUnitID );	
			glUniform1f( texBlend02_LocID, blendRatio );
			break;
		case 3:	
			glUniform1i( texSamp2D03_LocID, textureUnitID );	
			glUniform1f( texBlend03_LocID, blendRatio );
			break;
		case 4:	
			glUniform1i( texSamp2D04_LocID, textureUnitID );	
			glUniform1f( texBlend04_LocID, blendRatio );
			break;
		case 5:	
			glUniform1i( texSamp2D05_LocID, textureUnitID );	
			glUniform1f( texBlend05_LocID, blendRatio );
			break;
		case 6:	
			glUniform1i( texSamp2D06_LocID, textureUnitID );	
			glUniform1f( texBlend06_LocID, blendRatio );
			break;
		case 7:	
			glUniform1i( texSamp2D07_LocID, textureUnitID );	
			glUniform1f( texBlend07_LocID, blendRatio );
			break;
		default:
			// Invalid samplerIndex
			break;
		}//switch (samplerIndex)
		return;
	}//void set2DSamplerAndBlenderByIndex()

	void setCubeSamplerAndBlenderByIndex( GLint samplerIndex, float blendRatio, GLint textureUnitID )
	{
		switch (samplerIndex)
		{
		case 0: 
			glUniform1i( texSampCube00_LocID, textureUnitID );	
			glUniform1f( texCubeBlend00_LocID, blendRatio );
			break;
		case 1: 
			glUniform1i( texSampCube01_LocID, textureUnitID );	
			glUniform1f( texCubeBlend01_LocID, blendRatio );
			break;
		case 2: 
			glUniform1i( texSampCube02_LocID, textureUnitID );	
			glUniform1f( texCubeBlend02_LocID, blendRatio );
			break;
		case 3: 
			glUniform1i( texSampCube03_LocID, textureUnitID );	
			glUniform1f( texCubeBlend03_LocID, blendRatio );
			break;
		default:
			// Invalid samplerIndex;
			break;
		}//switch (samplerIndex)
		return;
	}//void setCubeSamplerAndBlenderByIndex()


	void SetSamplersForMeshTextures( sMeshDrawInfo &meshDrawInfo, 
									 std::map<std::string /*textureName*/, CTexUnitInfoBrief> &mapTexAndUnitInfo )
	{
		// 2D textures first
		int numTextures = (int)meshDrawInfo.vecMehs2DTextures.size();
		for ( int samplerIndex = 0; samplerIndex != numTextures; samplerIndex++ )
		{
			// What texture unit is this texture set to?
			std::map<std::string , CTexUnitInfoBrief>::iterator itTextUnitInfo 
				= mapTexAndUnitInfo.find( meshDrawInfo.vecMehs2DTextures[samplerIndex].name );
			// Have we mapped that one?
			if ( itTextUnitInfo != mapTexAndUnitInfo.end() )
			{	// Yes, so assign it
				set2DSamplerAndBlenderByIndex( samplerIndex, 
									 meshDrawInfo.vecMehs2DTextures[samplerIndex].blendRatio,
									 itTextUnitInfo->second.texUnitNumOffsetBy_GL_TEXTURE0 );
				// Set blend function, too
			}
		}// 2D textures

		// Now cube maps
		numTextures = (int)meshDrawInfo.vecMeshCubeMaps.size();
		for ( int samplerIndex = 0; samplerIndex != numTextures; samplerIndex++ )
		{
			// What texture unit is this texture set to?
			std::map<std::string , CTexUnitInfoBrief>::iterator itTextUnitInfo 
				= mapTexAndUnitInfo.find( meshDrawInfo.vecMeshCubeMaps[samplerIndex].name );
			// Have we mapped that one?
			if ( itTextUnitInfo != mapTexAndUnitInfo.end() )
			{	// Yes, so assign it
				setCubeSamplerAndBlenderByIndex( samplerIndex, 
									   meshDrawInfo.vecMeshCubeMaps[samplerIndex].blendRatio,
									   itTextUnitInfo->second.texUnitNumOffsetBy_GL_TEXTURE0 );
			}
		}// cube maps



	}//void SetSamplersForMeshTextures()

};//namespace QnDTexureSamplerUtility

// float g_frameTime = 0.0f;
// g_frameTimeIncrement = +0.0025f;

// Good enough for Rock-n-Roll example
//std::string g_AnimationToPlay = "assets/modelsFBX/RPG-Character(FBX2013).FBX";
//std::string g_AnimationToPlay = "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX";
//std::string g_AnimationToPlay = "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX";

	//****************************************************************************************
//    ___  _    _                      _  __  __           _     
//   / __|| |__(_) _ _   _ _   ___  __| ||  \/  | ___  ___| |_   
//   \__ \| / /| || ' \ | ' \ / -_)/ _` || |\/| |/ -_)(_-<| ' \  
//   |___/|_\_\|_||_||_||_||_|\___|\__,_||_|  |_|\___|/__/|_||_| 
//                                                               
void CalculateSkinnedMeshBonesAndLoad( sMeshDrawInfo &theMesh, cGameObject* pTheGO,
									   unsigned int UniformLoc_numBonesUsed, 
									   unsigned int UniformLoc_bonesArray )
{

	std::string animationToPlay = "";
	float curFrameTime = 0.0;


	// See what animation should be playing... 
	cAnimationState* pAniState = pTheGO->pAniState;

	// Are there any animations in the queue of animations
	if ( ! pAniState->vecAnimationQueue.empty() )
	{
		// Play the "1st" animation in the queue 
		animationToPlay = pAniState->vecAnimationQueue[0].name;
		curFrameTime = pAniState->vecAnimationQueue[0].currentTime;

		// Increment the top animation in the queue
		if ( pAniState->vecAnimationQueue[0].IncrementTime() )
		{
			// The animation reset to zero on increment...
			// ...meaning that the 1st animation is done
			// (WHAT!? Should you use a vector for this???)
			pAniState->vecAnimationQueue.erase( pAniState->vecAnimationQueue.begin() );

		}//vecAnimationQueue[0].IncrementTime()
	}
	else
	{	// Use the default animation.
		pAniState->defaultAnimation.IncrementTime();

		animationToPlay = pAniState->defaultAnimation.name;
		curFrameTime = pAniState->defaultAnimation.currentTime;

	}//if ( pAniState->vecAnimationQueue.empty()




//	::g_frameTime += ( ::g_frameTimeIncrement );
//	
//	// Is frame time LT zero? 
//	if ( ::g_frameTime <= 0.0f )
//	{
//		::g_frameTime = pTheGO->pSimpleSkinnedMesh->GetDuration();
//	}
//	// Is frame time GT the animation frame time?
//	if ( ::g_frameTime > pTheGO->pSimpleSkinnedMesh->GetDuration() )
//	{
//		::g_frameTime = 0.0f; 
//	}
//
	if ( pTheGO->friendlyName == "Sophie" )
	{
//		std::cout << pAniState->defaultAnimation.currentTime << std::endl;
	}

	// Set up the animation pose:
	std::vector< glm::mat4x4 > vecFinalTransformation;	// Replaced by	theMesh.vecFinalTransformation
//	std::vector< glm::mat4x4 > vecObjectBoneTransformation;
	std::vector< glm::mat4x4 > vecOffsets;
	// Final transformation is the bone transformation + boneOffsetPerVertex
	// ObjectBoneTransformation (or "Global") is the final location of the bones
	// vecOffsets is the relative offsets of the bones from each other


	pTheGO->pSimpleSkinnedMesh->BoneTransform( 
	                                curFrameTime,
									animationToPlay,		//**NEW**
		                            vecFinalTransformation,		// Final bone transforms for mesh
									theMesh.vecObjectBoneTransformation,  // final location of bones
									vecOffsets );                 // local offset for each bone

	unsigned int numberOfBonesUsed = static_cast< unsigned int >( vecFinalTransformation.size() );
	glUniform1i( UniformLoc_numBonesUsed, numberOfBonesUsed );

	glm::mat4x4* pBoneMatrixArray = &(vecFinalTransformation[0]);

	// UniformLoc_bonesArray is the getUniformLoc of "bones[0]" from
	//	uniform mat4 bones[MAXNUMBEROFBONES] 
	// in the shader
	glUniformMatrix4fv( UniformLoc_bonesArray, numberOfBonesUsed, GL_FALSE, 
	                    (const GLfloat*) glm::value_ptr( *pBoneMatrixArray ) );


	// Update the extents of the skinned mesh from the bones...
	//	sMeshDrawInfo.minXYZ_from_SM_Bones(glm::vec3(0.0f)), 
	//  sMeshDrawInfo.maxXYZ_from_SM_Bones(glm::vec3(0.0f))
	for ( unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++ )
	{
		glm::mat4 boneLocal = theMesh.vecObjectBoneTransformation[boneIndex];

		float scale = theMesh.scale;
		boneLocal = glm::scale( boneLocal, glm::vec3(scale, scale, scale) );

		//cPhysicalProperties phyProps;
		//pTheGO->GetPhysState( phyProps );
		//glm::vec4 GameObjectLocalOriginLocation = glm::vec4( phyProps.position, 1.0f );

		//glm::vec4 boneBallLocation = boneLocal * GameObjectLocalOriginLocation;
		glm::vec4 boneBallLocation = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f );
		boneBallLocation *= scale;

		// Update the extents of the mesh
		if ( boneIndex == 0 )
		{	
			// For the 0th bone, just assume this is the extent
			theMesh.minXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
			theMesh.maxXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
		}
		else
		{	// It's NOT the 0th bone, so compare with current max and min
			if ( theMesh.minXYZ_from_SM_Bones.x < boneBallLocation.x ) { theMesh.minXYZ_from_SM_Bones.x = boneBallLocation.x; }
			if ( theMesh.minXYZ_from_SM_Bones.y < boneBallLocation.y ) { theMesh.minXYZ_from_SM_Bones.y = boneBallLocation.y; }
			if ( theMesh.minXYZ_from_SM_Bones.z < boneBallLocation.z ) { theMesh.minXYZ_from_SM_Bones.z = boneBallLocation.z; }

			if ( theMesh.maxXYZ_from_SM_Bones.x > boneBallLocation.x ) { theMesh.maxXYZ_from_SM_Bones.x = boneBallLocation.x; }
			if ( theMesh.maxXYZ_from_SM_Bones.y > boneBallLocation.y ) 
			{ 
				theMesh.maxXYZ_from_SM_Bones.y = boneBallLocation.y;
			}
			if ( theMesh.maxXYZ_from_SM_Bones.z > boneBallLocation.z ) 
			{ 
				theMesh.maxXYZ_from_SM_Bones.z = boneBallLocation.z;
			}
		}//if ( boneIndex == 0 )


		//boneBallLocation += glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			
		//DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f );

//		if ( boneIndex == 35 )
//		{
//			DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f );
//		}
	}


	//****************************************************************************************
	return;
}
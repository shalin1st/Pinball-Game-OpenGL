#ifndef _cLightManager_HG_
#define _cLightManager_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>



class cLight
{
public:
	cLight();

	// These were all vec3, but now with NUB, they are all vec4
	glm::vec3 position;
	glm::vec3 diffuse;		// rgb
	glm::vec3 ambient;
	glm::vec4 specular;		// Colour (xyz), intensity (w)
	glm::vec3 attenuation;	// x = constant, y = linear, z = quadratic
	glm::vec3 direction;
	glm::vec4 typeParams;	// x = type, y = distance cut-off
							// z angle1, w = angle2


	// This represents a uniform variable inside a specific shader
	// (We are only using one shader)	int ShaderlocID_position;
	int shaderlocID_position;
	int shaderlocID_diffuse;
	int shaderlocID_ambient;
	int shaderlocID_specular;
	int shaderlocID_attenuation;
	int shaderlocID_direction;
	int shaderlocID_typeParams;

	// Taken from Day 12 cLightDescription class...
	static const unsigned int DEFAULTMAXITERATIONS = 50;
	static const float DEFAULTINFINITEDISTANCE;	// = 10,000.0f;	
	static const float DEFAULDIFFUSEACCURACYTHRESHOLD; // = 0.001f;
	float calcApproxDistFromAtten( float targetLightLevel );	// Uses the defaults
	float calcApproxDistFromAtten( float targetLightLevel, float accuracy );	// Uses the defaults
	float calcApproxDistFromAtten( float targetLightLevel, float accuracy, float infiniteDistance, unsigned int maxIterations = DEFAULTMAXITERATIONS);
	static const float DEFAULTZEROTHRESHOLD;	// = 0.0001f;
	// If value gets below the "zeroThreshold", will be treated as zero
	//float calcAttenFromDist(float distance, float zeroThreshold = DEFAULTZEROTHRESHOLD);
	float calcDiffuseFromAttenByDistance(float distance, float zeroThreshold = DEFAULTZEROTHRESHOLD);
	// *****************************************************************************
	// Helper functions
//	glm::vec3 attenuation;	// x = constant, y = linear, z = quadratic
//	glm::vec4 typeParams;	// x = type, y = distance cut-off
							// z angle1, w = angle2

	void setLightAttenConst( float constAttenuation );		//	glm::vec3 attenuation.x = constant
	void setLightAttenLinear(  float linearAttenuation );	//	glm::vec3 attenuation.y = linear
	void setLightAttenQuad( float quadraticAttenuation );	//	glm::vec3 attenuation.z = quadratic
	void setLightAtten( float constAtten, float linAtten, float quadAtten );
	float getLightAttenConst(void);
	float getLightAttenLinear(void);
	float getLightAttenQuad(void);
	enum eLightType
	{
		DIRECTIONAL,	// Not implemented
		POINT,
		SPOT,			// Not implemented
		UNKNOWN
	};
	void setLightParamType( eLightType lightType );						//	glm::vec4 typeParams.x = type,
	void setLightParamDistCutOff( float distanceCutoff );				//	glm::vec4 typeParams.y = distance cut-off
	void setLightParamSpotPrenumAngleInner( float innerPrenumAngle );	//	glm::vec4 typeParams.z = angle1
	void setLightParamSpotPrenumAngleOuter( float outerPrenumAngle );	//	glm::vec4 typeParams.w = angle2
	float getLightParamSpotPrenumAngleInner(void);	//	glm::vec4 typeParams.z = angle1
	float getLightParamSpotPrenumAngleOuter(void);	//	glm::vec4 typeParams.w = angle2
	// 
};




class cLightManager
{
public:
	cLightManager();
	~cLightManager();

	void CreateLights( int numberOfLights, bool bKeepOldValues = true );

	void LoadShaderUniformLocations(int shaderID);
	void CopyLightInformationToCurrentShader(void);

	// The C++ side class for the lights
	std::vector<cLight> vecLights;

	// CopyLightInfoToNUBArray()
	// GetNUBlocation()
	// CopyLightInfoToNUB()

	struct sLightDescNUB
	{
		glm::vec4 position;
		glm::vec4 diffuse;
		glm::vec4 ambient;
		glm::vec4 specular;		// Colour (xyz), intensity (w)
		glm::vec4 attenuation;	// x = constant, y = linear, z = quadratic
		glm::vec4 direction;
		glm::vec4 typeParams;	// x = type
	};
	//layout(std140) uniform NUB_lighting
	//{
	//	sLightDesc myLight[NUMBEROFLIGHTS];
	//} lightingNUB;
	static const int NUMBEROFLIGHTS = 100;
	struct sNUB_lighting
	{
		sLightDescNUB myLight[NUMBEROFLIGHTS];
	};
};

#endif

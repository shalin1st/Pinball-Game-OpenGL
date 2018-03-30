// Vertex shader
#version 420

//uniform mat4 MVP;
//uniform mat4 mModelRotationOnly;
uniform mat4 mView;
uniform mat4 mProjection;
uniform mat4 mModel;
uniform mat4 mWorldInvTranspose;

uniform bool isASkyBox;		// Same as the one in the fragments

uniform sampler2D texHeightMap;			// Is Texture Unit 0 no matter WHAT! WHY???
uniform bool bIsHeightMap;


// "Vertex" attribute (now use 'in')
in vec4 vCol;		// attribute
in vec3 vPos;		// was: vec2 vPos
in vec3 vNorm;		// Vertex normal
in vec4 uvX2;		// Added: UV 1 and 2
// Added for bump mapping:
in vec3 vTangent;		// For bump (or normal) mapping
in vec3 vBitangent;		// For bump (or normal) mapping
// For skinned mesh
in vec4 vBoneIDs_x4;		// IS OK. Note these are only used in the vertex shader
in vec4 vBoneWeights_x4;	// IS OK. Note these are only used in the vertex shader  

// For skinned mesh
const int MAXNUMBEROFBONES = 100;
uniform mat4 bones[MAXNUMBEROFBONES];
// Pass the acutal number you are using to control how often this loops
uniform int numBonesUsed;			
uniform bool bIsASkinnedMesh;	// True to do the skinned mesh


out vec4 fColor;				// was: vec4
out vec3 fVertNormal;		// Also in "world" (no view or projection)
out vec3 fVecWorldPosition;	// 
out vec4 fUV_X2;			// Added: UV 1 and 2 to fragment
out vec3 fTangent;		// For bump (or normal) mapping
out vec3 fBitangent;	// For bump (or normal) mapping


layout(std140) uniform NUB_perFrame
{
	vec3 eyePosition;	// Camera position
} perFramNUB;

void main()
{
    //gl_Position = MVP * vec4(vPos, 0.0, 1.0);	
	vec4 vertPosition = vec4(vPos, 1.0f);
	
	mat4 matModel = mModel;
	
//	if ( isASkyBox )
//	{	// DON'T Move the skybox.
//		// Set model matrix to identity
//		// ("moves" the skybox to the camera)
//		// And set the scale value, too
//		matModel = mat4(1.0f);	
//	}
	
	if ( bIsHeightMap )
	{
		// Alter the height of the mesh using a texture...
		vec4 heightChangeXYZ = texture( texHeightMap, uvX2.xy );
		// The RGB is from black to white, so 000 to 111, so I'll ignore 
		//	the the GB and just use the R
		
		vertPosition.y += heightChangeXYZ.r * 1000.0f;
	}
	

	// NOT a skinned mesh
	if ( ! bIsASkinnedMesh )
	{
		// Calculate the model view projection matrix here
		mat4 MVP = mProjection * mView * matModel;
		//gl_Position = MVP * vec4(vertPosition, 1.0f);
		gl_Position = MVP * vertPosition;
	
		// Calculate vertex and normal based on ONLY world 
		//fVecWorldPosition = vec3( matModel * vec4(vertPosition, 1.0f) ).xyz;
		fVecWorldPosition = vec3( matModel * vertPosition ).xyz;
	
		// Inv Tran - strips translation and scale from model transform
		// Alternative is you pass a "rotation only" model mat4
		//	mat4 mWorldInTranspose = inverse(transpose(matModel));		/*Now passed in*/
	
		// Was: MVP * vNorm;
		// This normal is in "world space" but only has rotation
		fVertNormal = vec3( mWorldInvTranspose * vec4(vNorm, 1.0f) ).xyz;	

		//	mat4 mWorldInverseTranspose = inverse(transpose(matModel));
		//	vertNormal = vec3( mWorldInverseTranspose * vec4(vNorm, 1.0f) ).xyz;	
			
		// Pass the tangent and bi-tangent out to the fragment shader
		fTangent = vTangent;
		fBitangent = vBitangent;

	}//if ( ! bIsASkinnedMesh )

	if ( bIsASkinnedMesh )
	{
		// If a single bone... 
//		mat4 BoneTransform = mat4(1.0f);
//		BoneTransform = bones[ int(vBoneIDs_x4[0]) ] * vBoneWeights_x4[0];		
//		BoneTransform += bones[ int(vBoneIDs_x4[1]) ] * vBoneWeights_x4[1];
//		BoneTransform += bones[ int(vBoneIDs_x4[2]) ] * vBoneWeights_x4[2];
//		BoneTransform += bones[ int(vBoneIDs_x4[3]) ] * vBoneWeights_x4[3];

		mat4 BoneTransform = bones[ int(vBoneIDs_x4[0]) ] * vBoneWeights_x4[0];
		BoneTransform += bones[ int(vBoneIDs_x4[1]) ] * vBoneWeights_x4[1];
		BoneTransform += bones[ int(vBoneIDs_x4[2]) ] * vBoneWeights_x4[2];
		BoneTransform += bones[ int(vBoneIDs_x4[3]) ] * vBoneWeights_x4[3];
				
	//	matrixWorld = BoneTransform * matrixWorld;
	
		vertPosition = BoneTransform * vertPosition;
		
		mat4 matMVP = mProjection * mView * mModel;		// m = p * v * m;
		
		// Final screen space position	
		gl_Position = matMVP * vertPosition;	
		
//		gl_Position = mProjection * mView * mModel * BoneTransform * vertPosition;	
		
		// Additional transformations so lighthing (normal) will work
		//mat4 matMV = matrixView * matrixWorld;	// model-view matrix
		//fVecWorldView = vec3(matMV * vertPosition).xyz;	
		
		// Inverse transform to keep ONLY rotation...
		mat4 matNormal = inverse( transpose(BoneTransform * mModel) );
		//
		fVertNormal = mat3(matNormal) * normalize(vNorm.xyz);
		fTangent = 	mat3(matNormal) * normalize(vTangent.xyz);
		fBitangent = 	mat3(matNormal) * normalize(vBitangent.xyz);
		
		fVecWorldPosition = (mModel * vertPosition).xyz;

	}//if ( bIsASkinnedMesh )
	
	
    fColor = vCol;
	fUV_X2 = uvX2;			// Sent to fragment shader

}


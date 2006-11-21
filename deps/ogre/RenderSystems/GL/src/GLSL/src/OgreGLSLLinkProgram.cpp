/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/

#include "OgreGLSLExtSupport.h"
#include "OgreGLSLLinkProgram.h"

namespace Ogre {

	//-----------------------------------------------------------------------
#define NO_ATTRIB 0xFFFF
	GLSLLinkProgram::GLSLLinkProgram(void)
        : mUniformRefsBuilt(false)
        , mLinked(false)
		, mTangentAttrib(NO_ATTRIB)
		, mBinormalAttrib(NO_ATTRIB)
		, mBlendIndicesAttrib(NO_ATTRIB)
		, mBlendWeightsAttrib(NO_ATTRIB)

	{
			checkForGLSLError( "GLSLLinkProgram::GLSLLinkProgram", "Error prior to Creating GLSL Program Object", 0 );
		    mGLHandle = glCreateProgramObjectARB();
			checkForGLSLError( "GLSLLinkProgram::GLSLLinkProgram", "Error Creating GLSL Program Object", 0 );

	}

	//-----------------------------------------------------------------------
	GLSLLinkProgram::~GLSLLinkProgram(void)
	{
		glDeleteObjectARB(mGLHandle);

	}

	//-----------------------------------------------------------------------
	void GLSLLinkProgram::activate(void)
	{
		if (!mLinked)
		{
			glLinkProgramARB( mGLHandle );
			glGetObjectParameterivARB( mGLHandle, GL_OBJECT_LINK_STATUS_ARB, &mLinked );
			// force logging and raise exception if not linked
			checkForGLSLError( "GLSLLinkProgram::Activate",
				"Error linking GLSL Program Object", mGLHandle, !mLinked, !mLinked );
			if(mLinked)
			{
				logObjectInfo( String("GLSL link result : "), mGLHandle );
				buildUniformReferences();
				extractAttributes();
			}

		}

		if (mLinked)
		{
		    glUseProgramObjectARB( mGLHandle );
		}
	}

	//-----------------------------------------------------------------------
	void GLSLLinkProgram::extractAttributes(void)
	{
		GLint attrib = glGetAttribLocationARB(mGLHandle, "tangent");
		mTangentAttrib = (attrib == -1)? NO_ATTRIB : (GLuint)attrib;
		
		attrib = glGetAttribLocationARB(mGLHandle, "binormal");
		mBinormalAttrib = (attrib == -1)? NO_ATTRIB : (GLuint)attrib;

		attrib = glGetAttribLocationARB(mGLHandle, "blendIndices");
		mBlendIndicesAttrib = (attrib == -1)? NO_ATTRIB : (GLuint)attrib;

		attrib = glGetAttribLocationARB(mGLHandle, "blendWeights");
		mBlendWeightsAttrib = (attrib == -1)? NO_ATTRIB : (GLuint)attrib;

	}
	//-----------------------------------------------------------------------
	GLuint GLSLLinkProgram::getAttributeIndex(VertexElementSemantic semantic)
	{
		switch(semantic)
		{
		case VES_TANGENT:
			return mTangentAttrib;
		case VES_BINORMAL:
			return mBinormalAttrib;
		case VES_BLEND_WEIGHTS:
			return mBlendWeightsAttrib;
		case VES_BLEND_INDICES:
			return mBlendIndicesAttrib;
		default:
			assert(false && "Shouldn't be calling this with standard attribs!");
			return 0;
		};
	}
	//-----------------------------------------------------------------------
	bool GLSLLinkProgram::isAttributeValid(VertexElementSemantic semantic)
	{
		switch(semantic)
		{
		case VES_TANGENT:
			return mTangentAttrib != NO_ATTRIB;
		case VES_BINORMAL:
			return mBinormalAttrib != NO_ATTRIB;
		case VES_BLEND_WEIGHTS:
			return mBlendWeightsAttrib != NO_ATTRIB;
		case VES_BLEND_INDICES:
			return mBlendIndicesAttrib != NO_ATTRIB;
		default:
			return false;
		};
	}
	//-----------------------------------------------------------------------
	void GLSLLinkProgram::buildUniformReferences(void)
	{
		if (!mUniformRefsBuilt)
		{
			// scan through the active uniforms and add them to the reference list
			GLint uniformCount;

			#define BUFFERSIZE 100
			char   uniformName[BUFFERSIZE];
			//GLint location;
			UniformReference newUniformReference;

			// get the number of active uniforms
			glGetObjectParameterivARB(mGLHandle, GL_OBJECT_ACTIVE_UNIFORMS_ARB,
					&uniformCount);

			// Loop over each of the active uniforms, and add them to the reference container
			// only do this for user defined uniforms, ignore built in gl state uniforms
			for (int index = 0; index < uniformCount; index++)
			{
				glGetActiveUniformARB(mGLHandle, index, BUFFERSIZE, NULL, &newUniformReference.mArraySize, &newUniformReference.mType, uniformName);
				// don't add built in uniforms
				newUniformReference.mLocation = glGetUniformLocationARB(mGLHandle, uniformName);
				if (newUniformReference.mLocation >= 0)
				{
					// user defined uniform found, add it to the reference list
					newUniformReference.mName = String( uniformName );
					// default to real since most uniforms are real
					newUniformReference.isReal = true;
                    newUniformReference.isSampler = false;
					// decode uniform size and type
					switch (newUniformReference.mType)
					{
					case GL_FLOAT:
						newUniformReference.mElementCount = 1;
						break;

					case GL_FLOAT_VEC2:
						newUniformReference.mElementCount = 2;
						break;

					case GL_FLOAT_VEC3:
						newUniformReference.mElementCount = 3;
						break;

					case GL_FLOAT_VEC4:
						newUniformReference.mElementCount = 4;
						break;

					case GL_SAMPLER_1D:
					case GL_SAMPLER_2D:
					case GL_SAMPLER_3D:
					case GL_SAMPLER_CUBE:
					case GL_SAMPLER_1D_SHADOW:
					case GL_SAMPLER_2D_SHADOW:
                    case GL_SAMPLER_2D_RECT_ARB:
                    case GL_SAMPLER_2D_RECT_SHADOW_ARB:
                        newUniformReference.isSampler = true;
					case GL_INT:
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						break;

					case GL_INT_VEC2:
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 2;
						break;

					case GL_INT_VEC3:
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 3;
						break;

					case GL_INT_VEC4:
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 4;
						break;

                    case GL_FLOAT_MAT2:
						newUniformReference.mElementCount = 4;
						break;

                    case GL_FLOAT_MAT3:
						newUniformReference.mElementCount = 9;
						break;

                    case GL_FLOAT_MAT4:
						newUniformReference.mElementCount = 16;
						break;

                    default:
                        // Ignore silently for unknown/unsupported types
                        continue;

					}// end switch

					mUniformReferences.push_back(newUniformReference);

				} // end if
			} // end for

			mUniformRefsBuilt = true;
		}
	}

	//-----------------------------------------------------------------------
	void GLSLLinkProgram::updateUniforms(GpuProgramParametersSharedPtr params)
	{
        // float array buffer used to pass arrays to GL
        static float floatBuffer[256];

		// iterate through uniform reference list and update uniform values
		UniformReferenceIterator currentUniform = mUniformReferences.begin();
		UniformReferenceIterator endUniform = mUniformReferences.end();

		GpuProgramParameters::RealConstantEntry* currentRealConstant;
		GpuProgramParameters::IntConstantEntry* currentIntConstant;

		while (currentUniform != endUniform)
		{
			// get the index in the parameter real list

			if (currentUniform->isReal)
			{
				currentRealConstant = params->getNamedRealConstantEntry( currentUniform->mName );
				if (currentRealConstant != NULL)
				{
					if (currentRealConstant->isSet)
					{
						switch (currentUniform->mElementCount)
						{
						case 1:
							glUniform1fvARB( currentUniform->mLocation, 1, currentRealConstant->val );
							break;

						case 2:
							glUniform2fvARB( currentUniform->mLocation, 1, currentRealConstant->val );
							break;

						case 3:
							glUniform3fvARB( currentUniform->mLocation, 1, currentRealConstant->val );
							break;

						case 4:
                            {
                                if (currentUniform->mType == GL_FLOAT_MAT2)
                                {
                                    glUniformMatrix2fvARB( currentUniform->mLocation, 1, GL_TRUE, currentRealConstant->val);
                                }
                                else
                                {
									// Support arrays of vec4, as supported by Cg and HLSL
									if (currentUniform->mArraySize > 1)
									{
										// Build a combined buffer
										size_t arr = currentUniform->mArraySize;
										float* pBuffer = floatBuffer;
										while (arr--)
										{
											memcpy(pBuffer, currentRealConstant++->val, sizeof(float) * 4);
											pBuffer += 4;
										}
										glUniform4fvARB(currentUniform->mLocation, currentUniform->mArraySize, floatBuffer);

									}
									else
									{
										glUniform4fvARB(currentUniform->mLocation, 1, currentRealConstant->val);
									}
                                }
                            }
							break;

                        case 9:
                            {
                                //float mat[9];
                                // assume that the 3x3 matrix is packed
                                memcpy(floatBuffer, currentRealConstant++->val, sizeof(float) * 4);
                                memcpy(floatBuffer + 4, currentRealConstant++->val, sizeof(float) * 4);
                                memcpy(floatBuffer + 4, currentRealConstant->val, sizeof(float) );

                                glUniformMatrix3fvARB( currentUniform->mLocation, 1, GL_TRUE, floatBuffer);
                                break;
                            }

                        case 16:
                            {
                                //float mat[16];
                                memcpy(floatBuffer, currentRealConstant++->val, sizeof(float) * 4);
                                memcpy(floatBuffer + 4, currentRealConstant++->val, sizeof(float) * 4);
                                memcpy(floatBuffer + 8, currentRealConstant++->val, sizeof(float) * 4);
                                memcpy(floatBuffer + 12, currentRealConstant++->val, sizeof(float) * 4);

                                glUniformMatrix4fvARB( currentUniform->mLocation, 1, GL_TRUE, floatBuffer);
                                break;
                            }


						} // end switch
					}
				}
			}
			else
			{
				currentIntConstant = params->getNamedIntConstantEntry( currentUniform->mName );
				if (currentIntConstant != NULL)
				{
					if (currentIntConstant->isSet)
					{
						switch (currentUniform->mElementCount)
						{
						case 1:
							glUniform1ivARB( currentUniform->mLocation, 1, (const GLint*)currentIntConstant->val );
							break;

						case 2:
							glUniform2ivARB( currentUniform->mLocation, 1, (const GLint*)currentIntConstant->val );
							break;

						case 3:
							glUniform3ivARB( currentUniform->mLocation, 1, (const GLint*)currentIntConstant->val );
							break;

						case 4:
							glUniform4ivARB( currentUniform->mLocation, 1, (const GLint*)currentIntConstant->val );
							break;
						} // end switch
					}
				}

			}
			// get the next uniform
			++currentUniform;

		} // end while
	}

	//-----------------------------------------------------------------------
	void GLSLLinkProgram::updatePassIterationUniforms(GpuProgramParametersSharedPtr params)
	{
		// iterate through uniform reference list and update pass iteration uniform values
		UniformReferenceIterator currentUniform = mUniformReferences.begin();
		UniformReferenceIterator endUniform = mUniformReferences.end();

		GpuProgramParameters::RealConstantEntry* currentRealConstant;
		//GpuProgramParameters::IntConstantEntry* currentIntConstant;

        currentRealConstant = params->getPassIterationEntry();
        if (currentRealConstant)
        {
            // need to find the uniform that matches the multi pass entry
		    while (currentUniform != endUniform)
		    {
			    // get the index in the parameter real list

			    if (currentUniform->isReal)
			    {

				    if (currentRealConstant == params->getNamedRealConstantEntry( currentUniform->mName ))
				    {
                        glUniform1fvARB( currentUniform->mLocation, 1, currentRealConstant->val );
                        // there will only be one multipass entry
                        return;
                    }
                }
			    // get the next uniform
			    ++currentUniform;
            }
        }

    }
} // namespace Ogre

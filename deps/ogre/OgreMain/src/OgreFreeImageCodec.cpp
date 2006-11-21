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

#include "OgreStableHeaders.h"

#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreFreeImageCodec.h"
#include "OgreImage.h"
#include "OgreException.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include <FreeImage.h>

namespace Ogre {

	FreeImageCodec::RegisteredCodecList FreeImageCodec::msCodecList;
	//---------------------------------------------------------------------
	void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) 
	{
		// Callback method as required by FreeImage to report problems
		StringUtil::StrStreamType str;
		str << "FreeImage error: '" << message << "' when loading format "
			<< FreeImage_GetFormatFromFIF(fif);

		LogManager::getSingleton().logMessage(str.str());

	}
	//---------------------------------------------------------------------
	void FreeImageCodec::startup(void)
	{
		// Note: FreeImage needs no explicit initialisation / deinitialisation 
		// when using the dynamic library version. If a user decides to use the static
		// library version (not recommended at this time since it is much larger to 
		// distribute for us), the library must be initialised and shut down manually

		//For linux & OSX, FreeImage needs an explicit startup even when using shared lib.. There is
		//no dllEntry point used under linux (how FreeImage loads up on win32)
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE || defined(FREEIMAGE_LIB)
		FreeImage_Initialise(false);
#endif
		LogManager::getSingleton().logMessage(
			LML_NORMAL,
			"FreeImage version: " + String(FreeImage_GetVersion()));
		LogManager::getSingleton().logMessage(
			LML_NORMAL,
			FreeImage_GetCopyrightMessage());

		// Register codecs
		StringUtil::StrStreamType strExt;
		strExt << "Supported formats: ";
		bool first = true;
		for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
		{

			// Skip DDS codec since FreeImage does not have the option 
			// to keep DXT data compressed, we'll use our own codec
			if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
				continue;
			
			String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
			if (!first)
			{
				strExt << ",";
			}
			first = false;
			strExt << exts;
			
			// Pull off individual formats (separated by comma by FI)
			StringVector extsVector = StringUtil::split(exts, ",");
			for (StringVector::iterator v = extsVector.begin(); v != extsVector.end(); ++v)
			{
				ImageCodec* codec = new FreeImageCodec(*v, i);
				msCodecList.push_back(codec);
				Codec::registerCodec(codec);
			}
		}
		LogManager::getSingleton().logMessage(
			LML_NORMAL,
			strExt.str());

		// Set error handler
		FreeImage_SetOutputMessage(FreeImageErrorHandler);




	}
	//---------------------------------------------------------------------
	void FreeImageCodec::shutdown(void)
	{
		//For linux & OSX, FreeImage needs an explicit destruction even when using shared lib.. There is
		//no dllEntry point used under linux (how FreeImage shuts down win32)
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE || defined(FREEIMAGE_LIB)
		FreeImage_DeInitialise();
#endif

		for (RegisteredCodecList::iterator i = msCodecList.begin();
			i != msCodecList.end(); ++i)
		{
			Codec::unRegisterCodec(*i);
			delete *i;
		}
		msCodecList.clear();

	}
	//---------------------------------------------------------------------
    FreeImageCodec::FreeImageCodec(const String &type, unsigned int fiType):
        mType(type),
        mFreeImageType(fiType)
    { 
    }
	//---------------------------------------------------------------------
	FIBITMAP* FreeImageCodec::encode(MemoryDataStreamPtr& input, CodecDataPtr& pData) const
	{
		FIBITMAP* ret = 0;

		ImageData* pImgData = static_cast< ImageData * >( pData.getPointer() );
		PixelBox src(pImgData->width, pImgData->height, pImgData->depth, pImgData->format, input->getPtr());

		// determine the settings
		FREE_IMAGE_TYPE imageType;
		switch(pImgData->format)
		{
		case PF_L8:
		case PF_A8:
		case PF_R5G6B5:
		case PF_B5G6R5:
		case PF_R8G8B8:
		case PF_B8G8R8:
		case PF_A8R8G8B8:
		case PF_X8R8G8B8:
		case PF_A8B8G8R8:
		case PF_X8B8G8R8:
		case PF_B8G8R8A8:
		case PF_R8G8B8A8:
			imageType = FIT_BITMAP;
			break;
		case PF_L16:
			imageType = FIT_UINT16;
			break;
		case PF_FLOAT16_RGB:
			imageType = FIT_RGB16;
			break;
		case PF_FLOAT16_RGBA:
			imageType = FIT_RGBA16;
			break;
		case PF_FLOAT32_R:
			imageType = FIT_FLOAT;
			break;
		case PF_FLOAT32_RGB:
			imageType = FIT_RGBF;
			break;
		case PF_FLOAT32_RGBA:
			imageType = FIT_RGBAF;
			break;
		default:
			OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Invalid image format", "FreeImageCodec::encode");
		};

		unsigned bpp = static_cast<unsigned>(PixelUtil::getNumElemBits(pImgData->format));

		bool conversionRequired = false;
		PixelFormat requiredFormat = pImgData->format;
		// I'd like to be able to use r/g/b masks to get FreeImage to load the data
		// in it's existing format, but that doesn't work, FreeImage needs to have
		// data in RGB[A] (big endian) and BGR[A] (little endian), always.
		if (bpp == 24)
		{
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
			requiredFormat = PF_BYTE_RGB;
#else
			requiredFormat = PF_BYTE_BGR;
#endif
		}
		else if (bpp == 32)
		{
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
			requiredFormat = PF_BYTE_RGBA;
#else
			requiredFormat = PF_BYTE_BGRA;
#endif
		}

		unsigned char* srcData = input->getPtr();
		PixelBox convBox(pImgData->width, pImgData->height, 1, requiredFormat);
		if (requiredFormat != pImgData->format)
		{
			conversionRequired = true;
			// Allocate memory
			convBox.data = new uchar[convBox.getConsecutiveSize()];
			// perform conversion and reassign source
			PixelBox src(pImgData->width, pImgData->height, 1, pImgData->format, input->getPtr());
			PixelUtil::bulkPixelConversion(src, convBox);
			srcData = static_cast<unsigned char*>(convBox.data);

		}

		ret = FreeImage_AllocateT(imageType, pImgData->width, pImgData->height, bpp);

		unsigned dstPitch = FreeImage_GetPitch(ret);
		unsigned srcPitch = pImgData->width * PixelUtil::getNumElemBytes(pImgData->format);


		// Copy data, invert scanlines and respect FreeImage pitch
		uchar* pSrc;
		uchar* pDst = FreeImage_GetBits(ret);
		for (size_t y = 0; y < pImgData->height; ++y)
		{
			pSrc = srcData + (pImgData->height - y - 1) * srcPitch;
			memcpy(pDst, pSrc, srcPitch);
			pDst += dstPitch;
		}

		if (conversionRequired)
		{
			// delete temporary conversion area
			delete [] static_cast<uchar*>(convBox.data);
		}

		return ret;
	}
    //---------------------------------------------------------------------
    DataStreamPtr FreeImageCodec::code(MemoryDataStreamPtr& input, Codec::CodecDataPtr& pData) const
    {        
		FIBITMAP* fiBitmap = encode(input, pData);

		// open memory chunk allocated by FreeImage
		FIMEMORY* mem = FreeImage_OpenMemory();
		// write data into memory
		FreeImage_SaveToMemory((FREE_IMAGE_FORMAT)mFreeImageType, fiBitmap, mem);
		// Grab data information
		BYTE* data;
		DWORD size;
		FreeImage_AcquireMemory(mem, &data, &size);
		// Copy data into our own buffer
		BYTE* ourData = new BYTE[size];
		memcpy(ourData, data, size);
		// Wrap data in stream, tell it to free on close 
		DataStreamPtr outstream(new MemoryDataStream(ourData, size, true));
		// Now free FreeImage memory buffers
		FreeImage_CloseMemory(mem);
		// Unload bitmap
		FreeImage_Unload(fiBitmap);

		return outstream;


    }
    //---------------------------------------------------------------------
    void FreeImageCodec::codeToFile(MemoryDataStreamPtr& input, 
        const String& outFileName, Codec::CodecDataPtr& pData) const
    {
		FIBITMAP* fiBitmap = encode(input, pData);

		FreeImage_Save((FREE_IMAGE_FORMAT)mFreeImageType, fiBitmap, outFileName.c_str());
		FreeImage_Unload(fiBitmap);


    }
    //---------------------------------------------------------------------
    Codec::DecodeResult FreeImageCodec::decode(DataStreamPtr& input) const
    {
		// Buffer stream into memory (TODO: override IO functions instead?)
		MemoryDataStream memStream(input, true);

		FIMEMORY* fiMem = 
			FreeImage_OpenMemory(memStream.getPtr(), memStream.size());

		FIBITMAP* fiBitmap = FreeImage_LoadFromMemory(
			(FREE_IMAGE_FORMAT)mFreeImageType, fiMem);


		ImageData* imgData = new ImageData();
		MemoryDataStreamPtr output;

		imgData->depth = 1; // only 2D formats handled by this codec
		imgData->width = FreeImage_GetWidth(fiBitmap);
		imgData->height = FreeImage_GetHeight(fiBitmap);
        imgData->num_mipmaps = 0; // no mipmaps in non-DDS 
        imgData->flags = 0;

		// Must derive format first, this may perform conversions
		
		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
		FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(fiBitmap);
		unsigned bpp = FreeImage_GetBPP(fiBitmap);

		switch(imageType)
		{
		case FIT_UNKNOWN:
		case FIT_COMPLEX:
		case FIT_UINT32:
		case FIT_INT32:
		case FIT_DOUBLE:
			OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
				"Unknown or unsupported image format", 
				"FreeImageCodec::decode");
				
			break;
		case FIT_BITMAP:
			// Standard image type
			// Perform any colour conversions for greyscale
			if (colourType == FIC_MINISWHITE || colourType == FIC_MINISBLACK)
			{
				FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fiBitmap);
				// free old bitmap and replace
				FreeImage_Unload(fiBitmap);
				fiBitmap = newBitmap;
				// get new formats
				bpp = FreeImage_GetBPP(fiBitmap);
				colourType = FreeImage_GetColorType(fiBitmap);
			}
			// Perform any colour conversions for RGB
			else if (bpp < 8 || colourType == FIC_PALETTE || colourType == FIC_CMYK)
			{
				FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(fiBitmap);
				// free old bitmap and replace
				FreeImage_Unload(fiBitmap);
				fiBitmap = newBitmap;
				// get new formats
				bpp = FreeImage_GetBPP(fiBitmap);
				colourType = FreeImage_GetColorType(fiBitmap);
			}

			// by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
			switch(bpp)
			{
			case 8:
				imgData->format = PF_L8;
				break;
			case 16:
				// Determine 555 or 565 from green mask
				// cannot be 16-bit greyscale since that's FIT_UINT16
				if(FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
				{
					imgData->format = PF_R5G6B5;
				}
				else
				{
					// FreeImage doesn't support 4444 format so must be 1555
					imgData->format = PF_A1R5G5B5;
				}
				break;
			case 24:
				// FreeImage differs per platform
				//     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
				//     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
				imgData->format = PF_BYTE_RGB;
#else
				imgData->format = PF_BYTE_BGR;
#endif
				break;
			case 32:
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
				imgData->format = PF_BYTE_RGBA;
#else
				imgData->format = PF_BYTE_BGRA;
#endif
				break;
				
				
			};
			break;
		case FIT_UINT16:
		case FIT_INT16:
			// 16-bit greyscale
			imgData->format = PF_L16;
			break;
		case FIT_FLOAT:
			// Single-component floating point data
			imgData->format = PF_FLOAT32_R;
			break;
		case FIT_RGB16:
			// TODO: is this valid?
			imgData->format = PF_FLOAT16_RGB;
			break;
		case FIT_RGBA16:
			// TODO: is this valid?
			imgData->format = PF_FLOAT16_RGBA;
			break;
		case FIT_RGBF:
			imgData->format = PF_FLOAT32_RGB;
			break;
		case FIT_RGBAF:
			imgData->format = PF_FLOAT32_RGBA;
			break;
			
			
		};

		unsigned char* srcData = FreeImage_GetBits(fiBitmap);
		unsigned srcPitch = FreeImage_GetPitch(fiBitmap);

		// Final data - invert image and trim pitch at the same time
		unsigned dstPitch = imgData->width * PixelUtil::getNumElemBytes(imgData->format);
		imgData->size = dstPitch * imgData->height;
        // Bind output buffer
        output.bind(new MemoryDataStream(imgData->size));

		uchar* pSrc;
		uchar* pDst = output->getPtr();
		for (size_t y = 0; y < imgData->height; ++y)
		{
			pSrc = srcData + (imgData->height - y - 1) * srcPitch;
			memcpy(pDst, pSrc, dstPitch);
			pDst += dstPitch;
		}

		
		FreeImage_Unload(fiBitmap);
		FreeImage_CloseMemory(fiMem);


        DecodeResult ret;
        ret.first = output;
        ret.second = CodecDataPtr(imgData);
		return ret;

    }
    //---------------------------------------------------------------------    
    String FreeImageCodec::getType() const 
    {
        return mType;
    }
}

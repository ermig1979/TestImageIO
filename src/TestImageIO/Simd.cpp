/*
* Test Image IO Project (http://github.com/ermig1979/TestImageIO).
*
* Copyright (c) 2021-2022 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "TestImageIO/Common.h"

namespace Test
{
	class FrameworkSimd : public Framework 
	{
	public:
		virtual String Name() const
		{
			return "Simd";
		}

		virtual bool Save(const Image& image, int quality, Compressed& compressed)
		{
			compressed.data = SimdImageSaveToMemory(image.data, image.stride, image.width, image.height, 
				(SimdPixelFormatType)image.format, SimdImageFileJpeg, quality, &compressed.size);

			return compressed.data == NULL;
		}

		virtual bool Load(const Compressed& compressed, Image& image)
		{
			return image.Load(compressed.data, compressed.size, Image::Rgb24);
		};

		virtual bool Load(const Compressed& compressed)
		{
			Image image;

			return Load(compressed, image);
		};

		virtual void Free(Compressed& compressed)
		{
			if (compressed.data)
			{
				SimdFree(compressed.data);
				compressed.data = NULL;
				compressed.size = 0;
			}
		}
	};

	//------------------------------------------------------------------------------------------------

	Framework* InitSimd()
	{
		return new FrameworkSimd();
	}
}

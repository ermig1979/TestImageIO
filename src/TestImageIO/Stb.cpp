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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Test
{
	class FrameworkStb : public Framework 
	{
		struct WriteContext
		{
			size_t capacity;
			Compressed &compressed;

			WriteContext(Compressed& compressed_)
				: compressed(compressed_)
				, capacity(compressed_.size)
			{
			}
		};

		static void WriteFunc(void* context, void* data, int size)
		{
			WriteContext* wc = (WriteContext*)context;
			if (wc->compressed.size + size > wc->capacity)
			{
				wc->capacity = wc->compressed.size + std::max<size_t>(size, wc->compressed.size);
				wc->compressed.data = (uint8_t*)realloc(wc->compressed.data, wc->capacity);
			}
			const uint8_t* src = (uint8_t*)data;
			uint8_t* dst = wc->compressed.data;
			for (size_t i = 0; i < size; ++i)
				dst[i] = src[i];
			wc->compressed.size += size;
		}

	public:
		virtual String Name() const
		{
			return "Stb";
		}

		virtual bool Save(const Image& image, int quality, Compressed& compressed)
		{
			WriteContext context(compressed);

			return stbi_write_jpg_to_func(WriteFunc, &context, (int)image.width, (int)image.height, 3, image.data, quality) != 0;
		}

		virtual bool Load(const Compressed& compressed, Image& image)
		{
			int x, y, c;
			stbi_uc* data = stbi_load_from_memory(compressed.data, compressed.size, &x, &y, &c, STBI_rgb);
			if (data)
			{
				image.Recreate(x, y, Image::Rgb24);
				Simd::Copy(Image(x, y, x * c, Image::Rgb24, data), image);
				stbi_image_free(data);
				return true;
			}
			else
				return false;
		};

		virtual bool Load(const Compressed& compressed)
		{
			int x, y, c;
			stbi_uc* data = stbi_load_from_memory(compressed.data, compressed.size, &x, &y, &c, STBI_rgb);
			if (data)
			{
				stbi_image_free(data);
				return true;
			}
			else
				return false;
		};

		virtual void Free(Compressed& compressed)
		{
			if (compressed.data)
			{
				free(compressed.data);
				compressed.data = NULL;
				compressed.size = 0;
			}
		}
	};

	//------------------------------------------------------------------------------------------------

	Framework* InitStb()
	{
		return new FrameworkStb();
	}
}

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
#pragma once

#include "Cpl/Performance.h"

#include "Simd/SimdLib.hpp"

namespace Test
{
	typedef Cpl::Ints Ints;

	typedef Cpl::String String;
	typedef Cpl::Strings Strings;

	typedef Simd::View<Simd::Allocator> Image;

	//------------------------------------------------------------------------------------------------

	struct Compressed
	{
		uint8_t* data;
		size_t size;

		Compressed()
			: data(0)
			, size(0)
		{
		}
	};

	//------------------------------------------------------------------------------------------------

	class Framework
	{
	public:
		virtual ~Framework() {};

		virtual String Name() const = 0;

		virtual bool Save(const Image& image, int quality, Compressed & compressed) = 0;

		virtual bool Save(const Image& image, int quality)
		{
			Compressed compressed;
			bool result = Save(image, quality, compressed);
			Free(compressed);
			return result;
		};

		virtual bool Load(const Compressed& compressed, Image& image) = 0;

		virtual bool Load(const Compressed& compressed) = 0;

		virtual void Free(Compressed& compressed) = 0;
	};

	//------------------------------------------------------------------------------------------------

	typedef std::shared_ptr<Framework> FrameworkPtr;
	typedef std::vector<FrameworkPtr> FrameworkPtrs;

	//------------------------------------------------------------------------------------------------

	Framework* InitSimd();

	Framework* InitStb();

	Framework* InitTurboJpeg();
}

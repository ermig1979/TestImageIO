/*
* Test Image IO Project (http://github.com/ermig1979/TestImageIO).
*
* Copyright (c) 2021-2021 Yermalayeu Ihar.
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

#include <turbojpeg.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Test
{
	void TestSimd(const Image& src, int quality)
	{
		uint8_t* data = NULL;
		size_t size = 0;

		data = SimdImageSaveToMemory(src.data, src.stride, src.width, src.height, (SimdPixelFormatType)src.format, SimdImageFileJpeg, quality, &size);

		if (data)
			SimdFree(data);
		else
			std::cout << "Simd can't save image to memory!" << std::endl;
	}

	void TestTurboJpeg(tjhandle _jpegCompressor, const Image& src, int quality, const String& name)
	{
		long unsigned int _jpegSize = 0;
		uint8_t* _compressedImage = NULL;

		tjCompress2(_jpegCompressor, src.data, src.width, src.stride, src.height, TJPF_RGB,
			&_compressedImage, &_jpegSize, quality <= 90 ? TJSAMP_420 : TJSAMP_444, quality, TJFLAG_FASTDCT);

		if (!name.empty())
		{
			FILE* file = fopen(name.c_str(), "wb");
			if (!file)
			{
				std::cout << "Could not open JPEG file: " << std::endl;
				return;
			}
			if (fwrite(_compressedImage, _jpegSize, 1, file) < 1)
			{
				std::cout << "Could not write JPEG file: " << std::endl;
				return;
			}
			fclose(file);
		}

		if (*_compressedImage)
			tjFree(_compressedImage);
		else
			std::cout << "TurboJpeg can't save image to memory!" << std::endl;
	}

	bool Test(const String & name, const int count, int quality, tjhandle jpegCompressor)
	{
		Image src;
		if (!src.Load(String("../data/image/") + name, Image::Rgb24))
		{
			std::cout << "Can't load test image!" << std::endl;
			return false;
		}

		std::cout << "Test (name = " << name << "\t, quality = " << quality << ") :";

		int64_t startSimd = Cpl::TimeCounter();
		for (int i = 0; i < count; ++i)
			TestSimd(src, quality);
		int64_t endSimd = Cpl::TimeCounter();
		double timeSimd = Cpl::Miliseconds(endSimd - startSimd) / count;

		int64_t startTj = Cpl::TimeCounter();
		for (int i = 0; i < count; ++i)
			TestTurboJpeg(jpegCompressor, src, quality, "");
		int64_t endTj = Cpl::TimeCounter();
		double timeTj = Cpl::Miliseconds(endTj - startTj) / count;

		std::cout << std::setprecision(3) << std::fixed << "TurboJpeg : " << timeTj << ", Simd : " << timeSimd << " ms, TurboJpeg / Simd : " << timeTj / timeSimd << std::endl;

		src.Save(std::string("simd_") + std::to_string(quality) + "_" + name, SimdImageFileJpeg, quality);
		TestTurboJpeg(jpegCompressor, src, quality, String("tj_") + std::to_string(quality) + "_" + name);

		return true;
	}
}

int main()
{
	const int N = 100, qulities[5] = {10, 35, 65, 85, 95};
	tjhandle jpegCompressor = tjInitCompress();

	for (int i = 0; i < 5; ++i)
	{
		Test::Test("city.jpg", N, qulities[i], jpegCompressor);
		Test::Test("face.jpg", N, qulities[i], jpegCompressor);
		Test::Test("forest.jpg", N, qulities[i], jpegCompressor);
		Test::Test("text.png", N, qulities[i], jpegCompressor);
	}

	tjDestroy(jpegCompressor);
	
	return 0;
}
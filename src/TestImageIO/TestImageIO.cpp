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
	bool EncodeTest(const String& name, double minTime, int quality, FrameworkPtrs & frameworks)
	{
		Image src;
		if (!src.Load(String("../data/image/") + name, Image::Rgb24))
		{
			std::cout << "Can't load test image!" << std::endl;
			return false;
		}

		std::cout << "Encode test (name = " << name << "\t, quality = " << quality << ") : " << std::flush;

		for (size_t f = 0; f < frameworks.size(); ++f)
		{
			std::cout << frameworks[f]->Name() << " : " << std::flush;
			int64_t start = Cpl::TimeCounter(), total = 0;
			int64_t enough = int64_t(Cpl::TimeFrequency() * minTime);
			int count = 0;
			for (; total < enough; count++)
			{
				frameworks[f]->Save(src, quality);
				total = Cpl::TimeCounter() - start;
			}
			std::cout << Cpl::ToStr(Cpl::Miliseconds(total) / count, 3) << " ms,   " << std::flush;
		}
		std::cout << std::endl << std::flush;

		return true;
	}
}

int main()
{
	Test::FrameworkPtrs frameworks;
	frameworks.push_back(Test::FrameworkPtr(Test::InitSimd()));
	frameworks.push_back(Test::FrameworkPtr(Test::InitTurboJpeg()));
	frameworks.push_back(Test::FrameworkPtr(Test::InitStb()));

	Test::Ints qulities = {10, 35, 65, 85, 95};

	Test::Strings images = { "city.jpg", "face.jpg", "forest.jpg", "text.png" };

	double minTime = 1.000;

	for (size_t q = 0; q < qulities.size(); ++q)
		for (size_t i = 0; i < images.size(); ++i)
			Test::EncodeTest(images[i], minTime, qulities[q], frameworks);
	
	return 0;
}
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
	bool EncodeTest(const Options & options, const String & name, int quality, FrameworkPtrs & frameworks)
	{
		const String& path = name;// Cpl::MakePath(options.imageDirectory, name);
		Image src;
		if (!src.Load(path, Image::Rgb24))
		{
			std::cout << "Can't load test image: " << path << " !" << std::endl;
			return false;
		}

		std::cout << "Encode test (name = " << name << "\t, quality = " << quality << ") : " << std::flush;

		for (size_t f = 0; f < frameworks.size(); ++f)
		{
			std::cout << frameworks[f]->Name() << " : " << std::flush;
			int64_t start = Cpl::TimeCounter(), total = 0;
			int64_t enough = int64_t(Cpl::TimeFrequency() * options.timeMin * 0.001);
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

	inline bool IsWanted(const Options& options, const String& path)
	{
		if (options.imageFilter.empty())
			return true;
		return path.find(options.imageFilter) != String::npos;
	}

	Strings FindImages(const Options& options)
	{
		Strings images;
		fs::file_status status = fs::status(options.imageDirectory);
		if (fs::is_directory(status))
		{
			for (fs::directory_iterator it(options.imageDirectory); it != fs::directory_iterator(); ++it)
			{
				if (it->is_regular_file() && IsWanted(options, it->path()))
					images.push_back(it->path());
			}
		}
		else
		{
			CPL_LOG_SS(Error, "Directory '" << options.imageDirectory << "' is not exist!");
		}
		return images;
	}
}

int main(int argc, char* argv[])
{
	Test::Options options(argc, argv);

	Cpl::Log::Global().AddStdWriter(options.logLevel);
	Cpl::Log::Global().SetFlags(Cpl::Log::BashFlags);

	Test::FrameworkPtrs frameworks;
	frameworks.push_back(Test::FrameworkPtr(Test::InitSimd()));
	frameworks.push_back(Test::FrameworkPtr(Test::InitTurboJpeg()));
	frameworks.push_back(Test::FrameworkPtr(Test::InitStb()));

	Test::Ints qualities = {10, 35, 65, 85, 95};

	Test::Strings images = Test::FindImages(options);// { "city.jpg", "face.jpg", "forest.jpg", "text.png" };

	double minTime = 1.000;

	for (size_t q = 0; q < qualities.size(); ++q)
		for (size_t i = 0; i < images.size(); ++i)
			Test::EncodeTest(options, images[i], qualities[q], frameworks);
	
	return 0;
}
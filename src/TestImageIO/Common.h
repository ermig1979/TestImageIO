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

#include "Cpl/Args.h"
#include "Cpl/Log.h"
#include "Cpl/Performance.h"
#include "Cpl/File.h"

#include "Simd/SimdLib.hpp"

namespace Test
{
	typedef Cpl::Ints Ints;

	typedef Cpl::String String;
	typedef Cpl::Strings Strings;

	typedef Simd::View<Simd::Allocator> Image;

	//------------------------------------------------------------------------------------------------

    struct Options : public Cpl::ArgsParser
    {
        String mode;
        String imageDirectory;
        String imageFilter;
        String outputDirectory;
		int timeMin;
		Cpl::Log::Level logLevel;

        Options(int argc, char* argv[])
            : ArgsParser(argc, argv, true)
        {
            if (HasArg("-h", "--help"))
            {
                PrintHelp();
                exit(0);
            }
            mode = GetArg2("-m", "--mode", "encode");
            imageDirectory = GetArg2("-id", "--imageDirectory", "../data/image");
            imageFilter = GetArg2("-if", "--imageFilter", "");
            outputDirectory = GetArg2("-od", "--outputDirectory", "./out");
			timeMin = Cpl::ToVal<int>(GetArg2("-tm", "--timeMin", "1000"));
			logLevel = (Cpl::Log::Level)Cpl::ToVal<int>(GetArg2("-ll", "--logLevel", "3", false));
        }

        ~Options()
        {
        }


    private:
        void PrintHelp()
        {
            std::cout << "TestImageIO is a tool to test performance of image IO libraries." << std::endl << std::endl;
            std::cout << "Using example:" << std::endl << std::endl;
            std::cout << "  ./TestImageIO -id=../data/image -m=encode" << std::endl << std::endl;
            std::cout << "Where following parameters are used:" << std::endl << std::endl;
            std::cout << "  -id=../data/image or --imageDirectories=../data/image - a path to directory with images." << std::endl << std::endl;
            std::cout << "  -m=encode or --mode=encode                - a test mode. It can be: encode." << std::endl << std::endl;
            std::cout << "Also you can use parameters: " << std::endl << std::endl;
            std::cout << "  --help or -h                       - to print this help message." << std::endl << std::endl;
            std::cout << "  --outputDirectory or -od           - to set output directory." << std::endl << std::endl;
            std::cout << "  --imageFilter=face or -if=face     - an image filter." << std::endl << std::endl;
			std::cout << "  --timeMin=1000 or -tm=1000         - a minimal time to test (in miliseconds)." << std::endl << std::endl;
			std::cout << "  --logLevel=3 or -ll=3              - a log level. It can be: 0 - None, 1 - Error, 2 - Warning, 3 - Info, 4 - Verbose, 5 - Debug. By default it is 3 (Info)" << std::endl << std::endl;
		}
    };

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

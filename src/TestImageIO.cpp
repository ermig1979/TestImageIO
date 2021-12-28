#include <iostream>
#include <iomanip>

#include <sys/time.h>

#include "Simd/SimdView.hpp"

#include <turbojpeg.h>

typedef Simd::View<Simd::Allocator> View;

inline int64_t TimeUsec()
{
	timeval t;
	gettimeofday(&t, NULL);
	return int64_t(t.tv_sec) * 1000000 + t.tv_usec;
}

void TestSimd(const View& src, int quality)
{
	uint8_t* data = NULL;
	size_t size = 0;

	data = SimdImageSaveToMemory(src.data, src.stride, src.width, src.height, (SimdPixelFormatType)src.format, SimdImageFileJpeg, quality, &size);

	if (data)
		SimdFree(data);
	else
		std::cout << "Simd can't save image to memory!" << std::endl;
}

void TestTurboJpeg(tjhandle _jpegCompressor, const View& src, int quality, const std::string & name)
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

bool Test(const std::string& name, const int count, int quality, tjhandle jpegCompressor)
{
	View src;
	if (!src.Load(std::string("../image/") + name, View::Rgb24))
	{
		std::cout << "Can't load test image!" << std::endl;
		return false;;
	}

	std::cout << "Test (name = " << name << "\t, quality = " << quality << ") :";


	int64_t startSimd = TimeUsec();
	for (int i = 0; i < count; ++i)
		TestSimd(src, quality);
	int64_t endSimd = TimeUsec();
	double timeSimd = double(endSimd - startSimd) * 0.001 / count;

	int64_t startTj = TimeUsec();
	for (int i = 0; i < count; ++i)
		TestTurboJpeg(jpegCompressor, src, quality, "");
	int64_t endTj = TimeUsec();
	double timeTj = double(endTj - startTj) * 0.001 / count;

	std::cout << std::setprecision(3) << std::fixed << "TurboJpeg : " << timeTj << ", Simd : " << timeSimd << " ms, TurboJpeg / Simd : " << timeTj / timeSimd << std::endl;

	src.Save(std::string("simd_") + std::to_string(quality) + "_" + name, SimdImageFileJpeg, quality);
	TestTurboJpeg(jpegCompressor, src, quality, std::string("tj_") + std::to_string(quality) + "_" + name);

	return true;
}

int main()
{
	const int N = 100, qulities[5] = {10, 35, 65, 85, 95};
	tjhandle jpegCompressor = tjInitCompress();

	for (int i = 0; i < 5; ++i)
	{
		Test("city.jpg", N, qulities[i], jpegCompressor);
		Test("face.jpg", N, qulities[i], jpegCompressor);
		Test("forest.jpg", N, qulities[i], jpegCompressor);
		Test("text.png", N, qulities[i], jpegCompressor);
	}

	tjDestroy(jpegCompressor);
	
	return 0;
}
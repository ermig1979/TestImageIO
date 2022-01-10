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

namespace Test
{
	class FrameworkTurboJpeg : public Framework 
	{
		tjhandle _jpegEncoder, _jpegDecoder;

	public:
		FrameworkTurboJpeg()
			: _jpegEncoder(NULL)
			, _jpegDecoder(NULL)
		{
			_jpegEncoder = tjInitCompress();

			_jpegDecoder = tjInitDecompress();
		}

		virtual ~FrameworkTurboJpeg()
		{
			if(_jpegEncoder)
			{ 
				int error = tjDestroy(_jpegEncoder);
				_jpegEncoder = NULL;
			}
			if (_jpegDecoder)
			{
				int error = tjDestroy(_jpegDecoder);
				_jpegDecoder = NULL;
			}
		}

		virtual String Name() const
		{
			return "TurboJpeg";
		}

		virtual bool Save(const Image& image, int quality, Compressed& compressed)
		{
			long unsigned int size = 0;

			int error = tjCompress2(_jpegEncoder, image.data, (int)image.width, (int)image.stride, (int)image.height, 
				TJPF_RGB, &compressed.data, &size, quality <= 90 ? TJSAMP_420 : TJSAMP_444, quality, TJFLAG_FASTDCT);

			if (error)
			{
				return false;
			}
			else
			{
				compressed.size = (size_t)size;
				return true;
			}
		}

		virtual bool Load(const Compressed& compressed, Image& image)
		{
			int width, height, subSample, colorSpace;
			int error = tjDecompressHeader3(_jpegDecoder, compressed.data, compressed.size, 
				&width, &height, &subSample, &colorSpace);
			if (error)
			{
				return false;
			}

			image.Recreate(width, height, Image::Rgb24);

			error = tjDecompress2(_jpegDecoder, compressed.data, compressed.size, 
				image.data, image.width, image.stride, image.height, TJPF_RGB, TJFLAG_FASTDCT);
			if (error)
			{
				return false;
			}

			return true;
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
				tjFree(compressed.data);
				compressed.data = NULL;
				compressed.size = 0;
			}
		}
	};

	//------------------------------------------------------------------------------------------------

	Framework* InitTurboJpeg()
	{
		return new FrameworkTurboJpeg();
	}
}

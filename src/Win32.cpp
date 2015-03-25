#include <atlimage.h>
#include "ObjectiveCInterface.h"

int DecodeImageFile(const char* filename, void** data, long* width, long* height, long* bitsPerPixel){
	wchar_t fn[256];
	MultiByteToWideChar(CP_ACP, 0, filename, -1, fn, 256);

	LPCTSTR str;
	CImage image;
	if (image.Load(fn) != S_OK){
		return 0;
	}

	int bufferSize = image.GetHeight() * image.GetWidth() * image.GetBPP() / 8;
	void* buffer = malloc(bufferSize);
	if (image.GetPitch()<0){
		char* src = reinterpret_cast<char*>(image.GetBits());
		char* dst = reinterpret_cast<char*>(buffer);

		int bytesPerLine = image.GetWidth() * image.GetBPP() / 8;
		dst += bytesPerLine * (image.GetHeight() - 1);

		for (int i = 0; i < image.GetHeight(); i++){
			memcpy(dst, src, bytesPerLine);
			src -= bytesPerLine;
			dst -= bytesPerLine;
		}
	} else
		memcpy(buffer, image.GetBits(), bufferSize);

	*width = image.GetWidth();
	*height = image.GetHeight();
	*bitsPerPixel = image.GetBPP();
	*data = buffer;

	return 1;
}

void FreeDecodedImage(void** data){
	free(*data);
	*data = 0;
}
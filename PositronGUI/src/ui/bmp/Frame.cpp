#include "ui/bmp/Frame.hpp"

#include "ui/bmp/MetadataReader.hpp"
#include "factories/WICFactory.hpp"
#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI::Bmp
{
	Frame::Frame(BitmapDecoder image, UINT frameIndex) noexcept
	{
		HRESULT hr = image->GetFrame(frameIndex, 
			ComPtrHolder<IWICBitmapFrameDecode>::GetHeldComPtrAddress()); HR_L(hr);
		ComPtrHolder<IWICBitmapFrameDecode>::GetHeldComPtr().As(ComPtrHolder<IWICBitmapSource>::GetHeldComPtrAddress());
	}
	MetadataReader Frame::GetMetadata() const noexcept
	{
		return MetadataReader{ *this };
	}
}

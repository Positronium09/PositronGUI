#include "ui/bmp/Frame.hpp"

#include "ui/bmp/MetadataReader.hpp"
#include "factories/WICFactory.hpp"
#include "helpers/HelperFunctions.hpp"


namespace PGUI::UI::Bmp
{
	Frame::Frame(const BitmapDecoder& image, UINT frameIndex) noexcept
	{
		HRESULT hr = image->GetFrame(frameIndex, 
			ComPtrHolder<IWICBitmapFrameDecode>::GetHeldComPtrAddress()); HR_L(hr);
		ComPtrHolder<IWICBitmapFrameDecode>::GetHeldComPtr().As(ComPtrHolder<IWICBitmapSource>::GetHeldComPtrAddress());
	}
	auto Frame::GetMetadata() const noexcept -> MetadataReader
	{
		return MetadataReader{ *this };
	}
}

#include "ui/bmp/Palette.hpp"

#include "ui/bmp/BitmapDecoder.hpp"
#include "ui/bmp/BitmapSource.hpp"
#include "ui/bmp/Frame.hpp"
#include "helpers/HelperFunctions.hpp"
#include "factories/WICFactory.hpp"


namespace PGUI::UI::Bmp
{
	Palette::Palette(BitmapDecoder img)
	{
		auto wicFactory = WICFactory::GetFactory();

		HRESULT hr = wicFactory->CreatePalette(GetHeldPtrAddress()); HR_T(hr);

		hr = img->CopyPalette(GetHeldComPtr().Get()); HR_T(hr);
	}
	Palette::Palette(BitmapSource img)
	{
		auto wicFactory = WICFactory::GetFactory();

		HRESULT hr = wicFactory->CreatePalette(GetHeldPtrAddress()); HR_T(hr);

		hr = img->CopyPalette(GetHeldComPtr().Get()); HR_T(hr);
	}

	Palette::Palette(std::span<WICColor> colors)
	{
		auto wicFactory = WICFactory::GetFactory();

		HRESULT hr = wicFactory->CreatePalette(GetHeldPtrAddress()); HR_T(hr);

		hr = GetHeldComPtr()->InitializeCustom(colors.data(), static_cast<UINT>(colors.size())); HR_T(hr);
	}

	Palette::Palette(ComPtr<IWICBitmapDecoder> bitmapDecoder)
	{
		auto wicFactory = WICFactory::GetFactory();

		HRESULT hr = wicFactory->CreatePalette(GetHeldPtrAddress()); HR_T(hr);

		hr = bitmapDecoder->CopyPalette(GetHeldComPtr().Get()); HR_T(hr);
	}
	Palette::Palette(ComPtr<IWICBitmapSource> bitmapSource)
	{
		auto wicFactory = WICFactory::GetFactory();

		HRESULT hr = wicFactory->CreatePalette(GetHeldPtrAddress()); HR_T(hr);

		hr = bitmapSource->CopyPalette(GetHeldComPtr().Get()); HR_T(hr);
	}

	std::vector<WICColor> Palette::GetColors() const noexcept
	{
		auto colorCount = GetColorCount();

		auto ptr = GetHeldComPtr();

		UINT readColorCount{ };
		std::vector<WICColor> colors(colorCount);

		HRESULT hr = ptr->GetColors(colorCount, colors.data(), &readColorCount); HR_L(hr);

		return colors;
	}

	UINT Palette::GetColorCount() const noexcept
	{
		UINT colorCount = 0;

		HRESULT hr = GetHeldComPtr()->GetColorCount(&colorCount); HR_L(hr);

		return colorCount;
	}

	WICBitmapPaletteType Palette::GetPaletteType() const noexcept
	{
		WICBitmapPaletteType paletteType = WICBitmapPaletteTypeCustom;

		HRESULT hr = GetHeldComPtr()->GetType(&paletteType); HR_L(hr);

		return paletteType;
	}
	bool Palette::IsBlackWhite() const noexcept
	{
		BOOL isBlackWhite = FALSE;
		HRESULT hr = GetHeldComPtr()->IsBlackWhite(&isBlackWhite); HR_L(hr);

		return isBlackWhite;
	}
	bool Palette::IsGrayScale() const noexcept
	{
		BOOL isGrayScale = FALSE;
		HRESULT hr = GetHeldComPtr()->IsGrayscale(&isGrayScale); HR_L(hr);

		return isGrayScale;
	}
	bool Palette::HasAlpha() const noexcept
	{
		BOOL hasAlpha = FALSE;
		HRESULT hr = GetHeldComPtr()->HasAlpha(&hasAlpha); HR_L(hr);

		return hasAlpha;
	}
}
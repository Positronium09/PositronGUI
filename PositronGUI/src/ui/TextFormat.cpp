#include "ui/TextFormat.hpp"

#include "core/Logger.hpp"
#include "factories/DWriteFactory.hpp"
#include "ui/font/FontSet.hpp"


namespace PGUI::UI
{
	TextFormat TextFormat::GetDefTextFormat(FLOAT fontSize)
	{
		auto textFormat = TextFormat{ L"Segoe UI", fontSize, GetUserLocaleName() };

		textFormat.SetParagraphAlignment(Font::ParagraphAlignments::Center);
		textFormat.SetTextAlignment(Font::TextAlignments::Center);

		return textFormat;
	}
	TextFormat::TextFormat(std::wstring_view fontFamilyName,
		FLOAT fontSize, std::wstring_view localeName,
		std::optional<Font::FontCollection> _fontCollection,
		Font::FontWeight fontWeight, Font::FontStyle fontStyle, Font::FontStretch fontStretch) noexcept
	{
		auto factory = DWriteFactory::GetFactory();

		auto fontCollection = _fontCollection.value_or(Font::FontCollection::GetSystemFontCollection());
		if (!fontCollection)
		{
			fontCollection = Font::FontCollection::GetSystemFontCollection();
		}

		auto fontSet = fontCollection.GetFontSet();
		auto axisValues = fontSet.ConvertWeightStretchStyleToAxisValues(fontWeight, fontStretch, fontStyle, fontSize);

		HRESULT hr = factory->CreateTextFormat(fontFamilyName.data(), fontCollection,
			axisValues.data(), static_cast<UINT32>(axisValues.size()),
			fontSize, localeName.data(), GetHeldPtrAddress()); HR_L(hr);
	}

	Font::FlowDirection TextFormat::GetFlowDirection() const noexcept
	{
		return GetHeldComPtr()->GetFlowDirection();
	}
	void TextFormat::SetFlowDirection(Font::FlowDirection flowDirection) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFlowDirection(flowDirection); HR_L(hr);
	}

	Font::FontCollection TextFormat::GetFontCollection() const noexcept
	{
		ComPtr<IDWriteFontCollection> fontCollection;
		ComPtr<IDWriteFontCollection3> fontCollection3;

		HRESULT hr = GetHeldComPtr()->GetFontCollection(fontCollection.GetAddressOf()); HR_L(hr);

		if (fontCollection)
		{
			hr = fontCollection.As(&fontCollection3); HR_L(hr);
		}

		return Font::FontCollection{ fontCollection3 };
	}
	
	std::wstring TextFormat::GetFontFamilyName() const noexcept
	{
		auto tf = GetHeldComPtr();

		UINT32 length = tf->GetFontFamilyNameLength();

		if (length == 0)
		{
			return L"";
		}

		std::wstring fontFamilyName(length, L'\0');
		HRESULT hr = tf->GetFontFamilyName(fontFamilyName.data(), length + 1); HR_L(hr);

		if (FAILED(hr))
		{
			return L"";
		}

		return fontFamilyName;
	}
	
	float TextFormat::GetFontSize() const noexcept
	{
		return GetHeldComPtr()->GetFontSize();
	}
	
	DWRITE_LINE_SPACING TextFormat::GetLineSpacing() const noexcept
	{
		DWRITE_LINE_SPACING lineSpacing{ };
		
		GetHeldComPtr()->GetLineSpacing(&lineSpacing);

		return lineSpacing;
	}
	void TextFormat::SetLineSpacing(const DWRITE_LINE_SPACING& lineSpacing) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetLineSpacing(&lineSpacing); HR_L(hr);
	}

	Font::FontStretch TextFormat::GetFontStretch() const noexcept
	{
		return GetHeldComPtr()->GetFontStretch();
	}
	Font::FontStyle TextFormat::GetFontStyle() const noexcept
	{
		return GetHeldComPtr()->GetFontStyle();
	}
	Font::FontWeight TextFormat::GetFontWeight() const noexcept
	{
		return GetHeldComPtr()->GetFontWeight();
	}
	
	float TextFormat::GetIncrementalTabStop() const noexcept
	{
		return GetHeldComPtr()->GetIncrementalTabStop();
	}
	
	void TextFormat::SetIncrementalTabStop(float incrementalTabStop) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetIncrementalTabStop(incrementalTabStop); HR_L(hr);
	}

	std::wstring TextFormat::GetLocaleName() const noexcept
	{
		auto tf = GetHeldComPtr();

		UINT32 length = tf->GetLocaleNameLength();

		if (length == 0)
		{
			return L"";
		}

		std::wstring localeName(length, L'\0');
		
		HRESULT hr = tf->GetLocaleName(localeName.data(), length + 1); HR_L(hr);
		
		if (FAILED(hr))
		{
			return L"";
		}

		return localeName;
	}
	
	Font::ParagraphAlignment TextFormat::GetParagraphAlignment() const noexcept
	{
		return GetHeldComPtr()->GetParagraphAlignment();
	}
	void TextFormat::SetParagraphAlignment(Font::ParagraphAlignment paragraphAlignment) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetParagraphAlignment(paragraphAlignment); HR_L(hr);
	}

	Font::ReadingDirection TextFormat::GetReadingDirection() const noexcept
	{
		return GetHeldComPtr()->GetReadingDirection();
	}
	void TextFormat::SetReadingDirection(Font::ReadingDirection readingDirection) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetReadingDirection(readingDirection); HR_L(hr);
	}
	
	Font::TextAlignment TextFormat::GetTextAlignment() const noexcept
	{
		return Font::TextAlignment();
	}
	void TextFormat::SetTextAlignment(Font::TextAlignment textAlignment) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetTextAlignment(textAlignment); HR_L(hr);
	}

	std::pair<DWRITE_TRIMMING, ComPtr<IDWriteInlineObject>> TextFormat::GetTrimming() const noexcept
	{
		DWRITE_TRIMMING trimming{ };
		ComPtr<IDWriteInlineObject> inlineObject;

		HRESULT hr = GetHeldComPtr()->GetTrimming(&trimming, inlineObject.GetAddressOf()); HR_L(hr);

		return { trimming, inlineObject };
	}
	void TextFormat::SetTrimming(const DWRITE_TRIMMING& trimming, ComPtr<IDWriteInlineObject> inlineObject) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetTrimming(&trimming, inlineObject.Get()); HR_L(hr);
	}

	Font::WordWrapping TextFormat::GetWordWrapping() const noexcept
	{
		return GetHeldComPtr()->GetWordWrapping();
	}
	void TextFormat::SetWordWrapping(Font::WordWrapping wordWrapping) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetWordWrapping(wordWrapping); HR_L(hr);
	}
}

#include "ui/TextFormat.hpp"

#include "helpers/HelperFunctions.hpp"
#include "factories/DWriteFactory.hpp"
#include "ui/font/FontSet.hpp"


namespace PGUI::UI
{
	auto TextFormat::GetDefTextFormat(FLOAT fontSize) -> TextFormat
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

	auto TextFormat::AdjustFontSizeToDPI(float fontSize) const noexcept -> TextFormat
	{
		TextFormat newTf{ GetFontFamilyName(), fontSize, GetLocaleName(),
			GetFontCollection(), GetFontWeight(), GetFontStyle(), GetFontStretch() };

		newTf.SetFlowDirection(GetFlowDirection());
		newTf.SetIncrementalTabStop(GetIncrementalTabStop());
		newTf.SetLineSpacing(GetLineSpacing());
		newTf.SetParagraphAlignment(GetParagraphAlignment());
		newTf.SetReadingDirection(GetReadingDirection());
		newTf.SetTextAlignment(GetTextAlignment());
		auto [trimming, inlineObj] = GetTrimming();
		newTf.SetTrimming(trimming, inlineObj);
		newTf.SetWordWrapping(GetWordWrapping());

		return newTf;
	}

	auto TextFormat::GetFlowDirection() const noexcept -> Font::FlowDirection
	{
		return GetHeldComPtr()->GetFlowDirection();
	}
	void TextFormat::SetFlowDirection(Font::FlowDirection flowDirection) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFlowDirection(flowDirection); HR_L(hr);
	}

	auto TextFormat::GetFontCollection() const noexcept -> Font::FontCollection
	{
		ComPtr<IDWriteFontCollection> fontCollection;
		ComPtr<IDWriteFontCollection3> fontCollection3;

		HRESULT hr = GetHeldComPtr()->GetFontCollection(&fontCollection); HR_L(hr);

		if (fontCollection)
		{
			hr = fontCollection.As(&fontCollection3); HR_L(hr);
		}

		return Font::FontCollection{ fontCollection3 };
	}
	
	auto TextFormat::GetFontFamilyName() const noexcept -> std::wstring
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
	
	auto TextFormat::GetFontSize() const noexcept -> float
	{
		return GetHeldComPtr()->GetFontSize();
	}
	
	auto TextFormat::GetLineSpacing() const noexcept -> DWRITE_LINE_SPACING
	{
		DWRITE_LINE_SPACING lineSpacing{ };
		
		GetHeldComPtr()->GetLineSpacing(&lineSpacing);

		return lineSpacing;
	}
	void TextFormat::SetLineSpacing(const DWRITE_LINE_SPACING& lineSpacing) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetLineSpacing(&lineSpacing); HR_L(hr);
	}

	auto TextFormat::GetFontStretch() const noexcept -> Font::FontStretch
	{
		return GetHeldComPtr()->GetFontStretch();
	}
	auto TextFormat::GetFontStyle() const noexcept -> Font::FontStyle
	{
		return GetHeldComPtr()->GetFontStyle();
	}
	auto TextFormat::GetFontWeight() const noexcept -> Font::FontWeight
	{
		return GetHeldComPtr()->GetFontWeight();
	}
	
	auto TextFormat::GetIncrementalTabStop() const noexcept -> float
	{
		return GetHeldComPtr()->GetIncrementalTabStop();
	}
	
	void TextFormat::SetIncrementalTabStop(float incrementalTabStop) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetIncrementalTabStop(incrementalTabStop); HR_L(hr);
	}

	auto TextFormat::GetLocaleName() const noexcept -> std::wstring
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
	
	auto TextFormat::GetParagraphAlignment() const noexcept -> Font::ParagraphAlignment
	{
		return GetHeldComPtr()->GetParagraphAlignment();
	}
	void TextFormat::SetParagraphAlignment(Font::ParagraphAlignment paragraphAlignment) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetParagraphAlignment(paragraphAlignment); HR_L(hr);
	}

	auto TextFormat::GetReadingDirection() const noexcept -> Font::ReadingDirection
	{
		return GetHeldComPtr()->GetReadingDirection();
	}
	void TextFormat::SetReadingDirection(Font::ReadingDirection readingDirection) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetReadingDirection(readingDirection); HR_L(hr);
	}
	
	auto TextFormat::GetTextAlignment() const noexcept -> Font::TextAlignment
	{
		return {};
	}
	void TextFormat::SetTextAlignment(Font::TextAlignment textAlignment) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetTextAlignment(textAlignment); HR_L(hr);
	}

	auto TextFormat::GetTrimming() const noexcept -> std::pair<DWRITE_TRIMMING, ComPtr<IDWriteInlineObject>>
	{
		DWRITE_TRIMMING trimming{ };
		ComPtr<IDWriteInlineObject> inlineObject;

		HRESULT hr = GetHeldComPtr()->GetTrimming(&trimming, &inlineObject); HR_L(hr);

		return { trimming, inlineObject };
	}
	void TextFormat::SetTrimming(const DWRITE_TRIMMING& trimming, ComPtr<IDWriteInlineObject> inlineObject) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetTrimming(&trimming, inlineObject.Get()); HR_L(hr);
	}

	auto TextFormat::GetWordWrapping() const noexcept -> Font::WordWrapping
	{
		return GetHeldComPtr()->GetWordWrapping();
	}
	void TextFormat::SetWordWrapping(Font::WordWrapping wordWrapping) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetWordWrapping(wordWrapping); HR_L(hr);
	}
}

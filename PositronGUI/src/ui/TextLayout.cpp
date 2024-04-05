#include "ui/TextLayout.hpp"

#include "ui/TextFormat.hpp"
#include "factories/DWriteFactory.hpp"


namespace PGUI::UI
{
	TextLayout::TextLayout(std::wstring_view text, const TextFormat& textFormat, SizeF maxSize) noexcept
	{
		ComPtr<IDWriteTextLayout> textLayout;

		auto factory = DWriteFactory::GetFactory();

		HRESULT hr = factory->CreateTextLayout(text.data(), static_cast<UINT32>(text.length()), 
			textFormat, maxSize.cx, maxSize.cy, textLayout.GetAddressOf()); HR_L(hr);

		if (textLayout)
		{
			hr = textLayout.As(GetHeldComPtrAddress()); HR_L(hr);
		}
	}

	RectF TextLayout::GetBoundingRect() const noexcept
	{
		DWRITE_TEXT_METRICS metrics{ };
		GetHeldComPtr()->GetMetrics(&metrics);

		return RectF{ metrics.left, metrics.top, metrics.left + metrics.width, metrics.top + metrics.height };
	}

	float TextLayout::CalculateMinWidth() const noexcept
	{
		float minWidth = 0.0f;
		
		HRESULT hr = GetHeldComPtr()->DetermineMinWidth(&minWidth); HR_L(hr);

		return minWidth;
	}

	float TextLayout::GetMaxHeight() const noexcept
	{
		return GetHeldComPtr()->GetMaxHeight();
	}

	void TextLayout::SetMaxHeight(float maxHeight) const noexcept
	{
		GetHeldComPtr()->SetMaxHeight(maxHeight);
	}

	float TextLayout::GetMaxWidth() const noexcept
	{
		return GetHeldComPtr()->GetMaxWidth();
	}

	void TextLayout::SetMaxWidth(float maxHeight) const noexcept
	{
		GetHeldComPtr()->SetMaxWidth(maxHeight);
	}

	std::vector<DWRITE_CLUSTER_METRICS> TextLayout::GetClusterMetrics() const noexcept
	{
		auto tl = GetHeldComPtr();

		UINT32 requiredSize = 0;
		HRESULT hr = tl->GetClusterMetrics(nullptr, 0, &requiredSize); HR_L(hr);

		std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics(requiredSize);

		tl->GetClusterMetrics(clusterMetrics.data(), requiredSize, &requiredSize); HR_L(hr);

		return clusterMetrics;
	}
	std::vector<DWRITE_LINE_METRICS1> TextLayout::GetLineMetrics() const noexcept
	{
		auto tl = GetHeldComPtr();

		UINT32 requiredSize = 0;
		HRESULT hr = tl->GetLineMetrics((DWRITE_LINE_METRICS1*)nullptr, 0, &requiredSize); HR_L(hr);

		std::vector<DWRITE_LINE_METRICS1> lineMetrics(requiredSize);

		tl->GetLineMetrics(lineMetrics.data(), requiredSize, &requiredSize); HR_L(hr);

		return lineMetrics;
	}

	Font::FontCollection TextLayout::GetFontCollection(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		ComPtr<IDWriteFontCollection> fontCollectionPtr;
		ComPtr<IDWriteFontCollection3> fontCollection3Ptr;

		HRESULT hr = GetHeldComPtr()->GetFontCollection(position, 
			fontCollectionPtr.GetAddressOf(), GetTextRangePtr(textRange)); HR_L(hr);

		fontCollectionPtr.As(&fontCollection3Ptr);

		return Font::FontCollection{ fontCollection3Ptr };
	}
	void TextLayout::SetFontCollection(Font::FontCollection fontCollection, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontCollection(fontCollection, textRange); HR_L(hr);
	}

	std::wstring TextLayout::GetFontFamilyName(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		auto tl = GetHeldComPtr();

		UINT32 length = 0;
		HRESULT hr = tl->GetFontFamilyNameLength(position, &length, GetTextRangePtr(textRange)); HR_L(hr);

		if (length == 0)
		{
			return L"";
		}

		std::wstring fontFamilyName(length, L'\0');
		tl->GetFontFamilyName(fontFamilyName.data(), length + 1);

		return fontFamilyName;
	}

	void TextLayout::SetFontFamilyName(std::wstring_view familyName, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontFamilyName(familyName.data(), textRange); HR_L(hr);
	}

	std::wstring TextLayout::GetLocaleName(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		auto tl = GetHeldComPtr();

		UINT32 length = 0;
		HRESULT hr = tl->GetLocaleNameLength(position, &length, GetTextRangePtr(textRange)); HR_L(hr);

		if (length == 0)
		{
			return L"";
		}

		std::wstring localeName(length, L'\0');
		tl->GetLocaleName(localeName.data(), length + 1);

		return localeName;
	}

	void TextLayout::SetLocaleName(std::wstring_view localeName, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetLocaleName(localeName.data(), textRange); HR_L(hr);
	}
	
	float TextLayout::GetFontSize(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		float fontSize = 0.0f;
		
		HRESULT hr = GetHeldComPtr()->GetFontSize(position, &fontSize, GetTextRangePtr(textRange)); HR_L(hr);

		return fontSize;
	}

	void TextLayout::SetFontSize(float fontSize, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontSize(fontSize, textRange); HR_L(hr);
	}

	Font::FontStretch TextLayout::GetFontStretch(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL;
		
		HRESULT hr = GetHeldComPtr()->GetFontStretch(position, &fontStretch, GetTextRangePtr(textRange)); HR_L(hr);

		return fontStretch;
	}

	void TextLayout::SetFontStretch(Font::FontStretch fontStretch, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontStretch(fontStretch, textRange); HR_L(hr);
	}

	Font::FontStyle TextLayout::GetFontStyle(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;

		HRESULT hr = GetHeldComPtr()->GetFontStyle(position, &fontStyle, GetTextRangePtr(textRange)); HR_L(hr);

		return fontStyle;
	}

	void TextLayout::SetFontStyle(Font::FontStyle fontStyle, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontStyle(fontStyle, textRange); HR_L(hr);
	}

	Font::FontWeight TextLayout::GetFontWeight(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL;

		HRESULT hr = GetHeldComPtr()->GetFontWeight(position, &fontWeight, GetTextRangePtr(textRange)); HR_L(hr);

		return fontWeight;
	}

	void TextLayout::SetFontWeight(Font::FontWeight fontWeight, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontWeight(fontWeight, textRange); HR_L(hr);
	}

	ComPtr<IDWriteInlineObject> TextLayout::GetInlineObject(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		ComPtr<IDWriteInlineObject> inlineObject;

		HRESULT hr = GetHeldComPtr()->GetInlineObject(position, inlineObject.GetAddressOf(), GetTextRangePtr(textRange)); HR_L(hr);

		return inlineObject;
	}

	void TextLayout::SetInlineObject(ComPtr<IDWriteInlineObject> inlineObject, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetInlineObject(inlineObject.Get(), textRange); HR_L(hr);
	}

	bool TextLayout::GetStrikethrough(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		BOOL hasStrikethrough = FALSE;

		HRESULT hr = GetHeldComPtr()->GetStrikethrough(position, &hasStrikethrough, GetTextRangePtr(textRange)); HR_L(hr);

		return hasStrikethrough;
	}
	void TextLayout::SetStrikethrough(bool strikethrough, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetStrikethrough(strikethrough, textRange); HR_L(hr);
	}

	bool TextLayout::GetUnderline(UINT32 position, OptionalTextRangeRet textRange) const noexcept
	{
		BOOL hasUnderline = FALSE;

		HRESULT hr = GetHeldComPtr()->GetUnderline(position, &hasUnderline, GetTextRangePtr(textRange)); HR_L(hr);

		return hasUnderline;
	}

	void TextLayout::SetUnderline(bool underline, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetUnderline(underline, textRange); HR_L(hr);
	}
	
	DWRITE_TEXT_RANGE* TextLayout::GetTextRangePtr(OptionalTextRangeRet textRange) const noexcept
	{
		DWRITE_TEXT_RANGE* trp = nullptr;
		if (trp)
		{
			trp = &(textRange.value().get());
		}
		return trp;
	}
}

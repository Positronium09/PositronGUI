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
			textFormat, maxSize.cx, maxSize.cy, &textLayout); HR_T(hr);
		
		if (textLayout)
		{
			hr = textLayout.As(GetHeldComPtrAddress()); HR_T(hr);
		}
	}

	auto TextLayout::GetBoundingRect() const noexcept -> RectF
	{
		DWRITE_TEXT_METRICS metrics{ };
		GetHeldComPtr()->GetMetrics(&metrics);

		return RectF{ metrics.left, metrics.top, metrics.left + metrics.width, metrics.top + metrics.height };
	}

	auto TextLayout::CalculateMinWidth() const noexcept -> float
	{
		float minWidth = 0.0F;
		
		HRESULT hr = GetHeldComPtr()->DetermineMinWidth(&minWidth); HR_L(hr);

		return minWidth;
	}

	auto TextLayout::GetMaxHeight() const noexcept -> float
	{
		return GetHeldComPtr()->GetMaxHeight();
	}

	void TextLayout::SetMaxHeight(float maxHeight) const noexcept
	{
		GetHeldComPtr()->SetMaxHeight(maxHeight);
	}

	auto TextLayout::GetMaxWidth() const noexcept -> float
	{
		return GetHeldComPtr()->GetMaxWidth();
	}

	void TextLayout::SetMaxWidth(float maxHeight) const noexcept
	{
		GetHeldComPtr()->SetMaxWidth(maxHeight);
	}

	auto TextLayout::GetClusterMetrics() const noexcept -> std::vector<DWRITE_CLUSTER_METRICS>
	{
		auto tl = GetHeldComPtr();

		UINT32 requiredSize = 0;
		HRESULT hr = tl->GetClusterMetrics(nullptr, 0, &requiredSize); HR_L(hr);

		std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics(requiredSize);

		tl->GetClusterMetrics(clusterMetrics.data(), requiredSize, &requiredSize); HR_L(hr);

		return clusterMetrics;
	}
	auto TextLayout::GetLineMetrics() const noexcept -> std::vector<DWRITE_LINE_METRICS1>
	{
		auto tl = GetHeldComPtr();

		UINT32 requiredSize = 0;
		HRESULT hr = tl->GetLineMetrics((DWRITE_LINE_METRICS1*)nullptr, 0, &requiredSize); HR_L(hr);

		std::vector<DWRITE_LINE_METRICS1> lineMetrics(requiredSize);

		tl->GetLineMetrics(lineMetrics.data(), requiredSize, &requiredSize); HR_L(hr);

		return lineMetrics;
	}

	auto TextLayout::GetMetrics() const noexcept -> DWRITE_TEXT_METRICS1
	{
		DWRITE_TEXT_METRICS1 textMetrics{ };
		HRESULT hr = GetHeldComPtr()->GetMetrics(&textMetrics); HR_L(hr);

		return textMetrics;
	}

	auto TextLayout::GetFontCollection(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> Font::FontCollection
	{
		ComPtr<IDWriteFontCollection> fontCollectionPtr;
		ComPtr<IDWriteFontCollection3> fontCollection3Ptr;

		HRESULT hr = GetHeldComPtr()->GetFontCollection(position, 
			&fontCollectionPtr, GetTextRangePtr(textRange)); HR_L(hr);

		fontCollectionPtr.As(&fontCollection3Ptr);

		return Font::FontCollection{ fontCollection3Ptr };
	}
	void TextLayout::SetFontCollection(const Font::FontCollection& fontCollection, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontCollection(fontCollection, textRange); HR_L(hr);
	}

	auto TextLayout::GetFontFamilyName(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> std::wstring
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

	auto TextLayout::GetLocaleName(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> std::wstring
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
	
	auto TextLayout::GetFontSize(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> float
	{
		float fontSize = 0.0F;
		
		HRESULT hr = GetHeldComPtr()->GetFontSize(position, &fontSize, GetTextRangePtr(textRange)); HR_L(hr);

		return fontSize;
	}

	void TextLayout::SetFontSize(float fontSize, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontSize(fontSize, textRange); HR_L(hr);
	}

	auto TextLayout::GetFontStretch(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> Font::FontStretch
	{
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL;
		
		HRESULT hr = GetHeldComPtr()->GetFontStretch(position, &fontStretch, GetTextRangePtr(textRange)); HR_L(hr);

		return fontStretch;
	}

	void TextLayout::SetFontStretch(Font::FontStretch fontStretch, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontStretch(fontStretch, textRange); HR_L(hr);
	}

	auto TextLayout::GetFontStyle(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> Font::FontStyle
	{
		DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL;

		HRESULT hr = GetHeldComPtr()->GetFontStyle(position, &fontStyle, GetTextRangePtr(textRange)); HR_L(hr);

		return fontStyle;
	}

	void TextLayout::SetFontStyle(Font::FontStyle fontStyle, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontStyle(fontStyle, textRange); HR_L(hr);
	}

	auto TextLayout::GetFontWeight(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> Font::FontWeight
	{
		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL;

		HRESULT hr = GetHeldComPtr()->GetFontWeight(position, &fontWeight, GetTextRangePtr(textRange)); HR_L(hr);

		return fontWeight;
	}

	void TextLayout::SetFontWeight(Font::FontWeight fontWeight, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetFontWeight(fontWeight, textRange); HR_L(hr);
	}

	auto TextLayout::GetInlineObject(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> ComPtr<IDWriteInlineObject>
	{
		ComPtr<IDWriteInlineObject> inlineObject;

		HRESULT hr = GetHeldComPtr()->GetInlineObject(position, &inlineObject, GetTextRangePtr(textRange)); HR_L(hr);

		return inlineObject;
	}

	void TextLayout::SetInlineObject(const ComPtr<IDWriteInlineObject>& inlineObject, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetInlineObject(inlineObject.Get(), textRange); HR_L(hr);
	}

	auto TextLayout::GetStrikethrough(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> bool
	{
		BOOL hasStrikethrough = FALSE;

		HRESULT hr = GetHeldComPtr()->GetStrikethrough(position, &hasStrikethrough, GetTextRangePtr(textRange)); HR_L(hr);

		return hasStrikethrough;
	}
	void TextLayout::SetStrikethrough(bool strikethrough, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetStrikethrough(strikethrough, textRange); HR_L(hr);
	}

	auto TextLayout::GetUnderline(UINT32 position, OptionalTextRangeRet textRange) const noexcept -> bool
	{
		BOOL hasUnderline = FALSE;

		HRESULT hr = GetHeldComPtr()->GetUnderline(position, &hasUnderline, GetTextRangePtr(textRange)); HR_L(hr);

		return hasUnderline;
	}

	void TextLayout::SetUnderline(bool underline, TextRange textRange) const noexcept
	{
		HRESULT hr = GetHeldComPtr()->SetUnderline(underline, textRange); HR_L(hr);
	}
	
	auto TextLayout::GetTextRangePtr(OptionalTextRangeRet textRange) noexcept -> DWRITE_TEXT_RANGE*
	{
		DWRITE_TEXT_RANGE* trp = nullptr;
		if (trp)
		{
			trp = &(textRange.value().get());
		}
		return trp;
	}
}

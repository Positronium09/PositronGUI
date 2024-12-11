#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Size.hpp"
#include "core/Rect.hpp"
#include "helpers/HelperFunctions.hpp"
#include "ui/font/FontCollection.hpp"
#include "ui/font/FontEnums.hpp"

#include <optional>
#include <string>
#include <vector>
#include <dwrite_3.h>


namespace PGUI::UI
{
	class TextFormat;

	struct TextRange : public DWRITE_TEXT_RANGE
	{
		public:
		TextRange() noexcept = default;
		explicit(false) TextRange(DWRITE_TEXT_RANGE textRange) noexcept : DWRITE_TEXT_RANGE()
		{
			startPosition = textRange.startPosition;
			length = textRange.length;
		}
		TextRange(UINT32 _startPosition, UINT32 _length) noexcept : DWRITE_TEXT_RANGE()
		{
			startPosition = _startPosition;
			length = _length;
		}

		[[nodiscard]] constexpr auto operator==(const TextRange& other) const noexcept -> bool
		{
			return startPosition == other.startPosition && length == other.length;
		}
	};


	class TextLayout : public ComPtrHolder<IDWriteTextLayout4>
	{
		using OptionalTextRangeRet = std::optional<std::reference_wrapper<TextRange>>;

		public:
		TextLayout() noexcept = default;
		TextLayout(std::wstring_view text, const TextFormat& textFormat, SizeF maxSize) noexcept;

		[[nodiscard]] auto GetBoundingRect() const noexcept -> RectF;

		[[nodiscard]] auto CalculateMinWidth() const noexcept -> float;

		[[nodiscard]] auto GetMaxHeight() const noexcept -> float;
		void SetMaxHeight(float maxHeight) const noexcept;

		[[nodiscard]] auto GetMaxWidth() const noexcept -> float;
		void SetMaxWidth(float maxHeight) const noexcept;

		[[nodiscard]] auto GetClusterMetrics() const noexcept -> std::vector<DWRITE_CLUSTER_METRICS>;
		[[nodiscard]] auto GetLineMetrics() const noexcept -> std::vector<DWRITE_LINE_METRICS1>;

		[[nodiscard]] auto GetMetrics() const noexcept -> DWRITE_TEXT_METRICS1;

		template <typename T>
		[[nodiscard]] auto GetDrawingEffect(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) -> ComPtr<T>
		{
			ComPtr<IUnknown> unknown;
			ComPtr<T> ret;

			HRESULT hr = GetHeldComPtr()->GetDrawingEffect(position, &unknown, GetTextRangePtr(textRange)); HR_L(hr);

			if (unknown)
			{
				unknown.As(&ret);
			}

			return ret;
		}
		template <typename T>
		void SetDrawingEffect(ComPtr<T> drawingEffect, TextRange textRange) const noexcept
		{
			HRESULT hr = GetHeldComPtr()->SetDrawingEffect(drawingEffect.Get(), textRange); HR_L(hr);
		}

		[[nodiscard]] auto GetFontCollection(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> Font::FontCollection;
		void SetFontCollection(const Font::FontCollection& fontCollection, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetFontFamilyName(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> std::wstring;
		void SetFontFamilyName(std::wstring_view familyName, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetLocaleName(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> std::wstring;
		void SetLocaleName(std::wstring_view localeName, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetFontSize(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> float;
		void SetFontSize(float fontSize, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetFontStretch(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> Font::FontStretch;
		void SetFontStretch(Font::FontStretch fontStretch, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetFontStyle(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> Font::FontStyle;
		void SetFontStyle(Font::FontStyle fontStyle, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetFontWeight(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> Font::FontWeight;
		void SetFontWeight(Font::FontWeight fontWeight, TextRange textRange) const noexcept;
		
		[[nodiscard]] auto GetInlineObject(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> ComPtr<IDWriteInlineObject>;
		void SetInlineObject(const ComPtr<IDWriteInlineObject>& inlineObject, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetStrikethrough(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> bool;
		void SetStrikethrough(bool strikethrough, TextRange textRange) const noexcept;

		[[nodiscard]] auto GetUnderline(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept -> bool;
		void SetUnderline(bool underline, TextRange textRange) const noexcept;

		private:
		[[nodiscard]] static auto GetTextRangePtr(OptionalTextRangeRet textRange) noexcept -> DWRITE_TEXT_RANGE*;
	};
}

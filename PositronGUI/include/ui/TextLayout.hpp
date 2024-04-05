#pragma once

#include "helpers/ComPtrHolder.hpp"
#include "core/Size.hpp"
#include "core/Rect.hpp"
#include "core/Logger.hpp"
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
		explicit(false) TextRange(DWRITE_TEXT_RANGE textRange) noexcept
		{
			startPosition = textRange.startPosition;
			length = textRange.length;
		}
		TextRange(UINT32 _startPosition, UINT32 _length) noexcept
		{
			startPosition = _startPosition;
			length = _length;
		}

		explicit(false) operator DWRITE_TEXT_RANGE() const noexcept { return DWRITE_TEXT_RANGE{ startPosition, length }; }

		[[nodiscard]] constexpr bool operator==(const TextRange& other) const noexcept = default;
	};


	class TextLayout : public ComPtrHolder<IDWriteTextLayout4>
	{
		using OptionalTextRangeRet = std::optional<std::reference_wrapper<TextRange>>;

		public:
		TextLayout() noexcept = default;
		TextLayout(std::wstring_view text, const TextFormat& textFormat, SizeF maxSize) noexcept;

		[[nodiscard]] RectF GetBoundingRect() const noexcept;

		[[nodiscard]] float CalculateMinWidth() const noexcept;

		[[nodiscard]] float GetMaxHeight() const noexcept;
		void SetMaxHeight(float maxHeight) const noexcept;

		[[nodiscard]] float GetMaxWidth() const noexcept;
		void SetMaxWidth(float maxHeight) const noexcept;

		[[nodiscard]] std::vector<DWRITE_CLUSTER_METRICS> GetClusterMetrics() const noexcept;
		[[nodiscard]] std::vector<DWRITE_LINE_METRICS1> GetLineMetrics() const noexcept;

		template <typename T>
		[[nodiscard]] ComPtr<T> GetDrawingEffect(UINT32 position, OptionalTextRangeRet textRange = std::nullopt)
		{
			ComPtr<IUnknown> unknown;
			ComPtr<T> ret;

			HRESULT hr = GetHeldComPtr()->GetDrawingEffect(position, unknown.GetAddressOf(), GetTextRangePtr(textRange)); HR_L(hr);

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

		[[nodiscard]] Font::FontCollection GetFontCollection(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetFontCollection(Font::FontCollection fontCollection, TextRange textRange) const noexcept;

		[[nodiscard]] std::wstring GetFontFamilyName(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetFontFamilyName(std::wstring_view familyName, TextRange textRange) const noexcept;

		[[nodiscard]] std::wstring GetLocaleName(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetLocaleName(std::wstring_view localeName, TextRange textRange) const noexcept;

		[[nodiscard]] float GetFontSize(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetFontSize(float fontSize, TextRange textRange) const noexcept;

		[[nodiscard]] Font::FontStretch GetFontStretch(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetFontStretch(Font::FontStretch fontStretch, TextRange textRange) const noexcept;

		[[nodiscard]] Font::FontStyle GetFontStyle(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetFontStyle(Font::FontStyle fontStyle, TextRange textRange) const noexcept;

		[[nodiscard]] Font::FontWeight GetFontWeight(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetFontWeight(Font::FontWeight fontWeight, TextRange textRange) const noexcept;
		
		[[nodiscard]] ComPtr<IDWriteInlineObject> GetInlineObject(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetInlineObject(ComPtr<IDWriteInlineObject> inlineObject, TextRange textRange) const noexcept;

		[[nodiscard]] bool GetStrikethrough(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetStrikethrough(bool strikethrough, TextRange textRange) const noexcept;

		[[nodiscard]] bool GetUnderline(UINT32 position, OptionalTextRangeRet textRange = std::nullopt) const noexcept;
		void SetUnderline(bool underline, TextRange textRange) const noexcept;

		private:
		[[nodiscard]] DWRITE_TEXT_RANGE* GetTextRangePtr(OptionalTextRangeRet textRange) const noexcept;
	};
}

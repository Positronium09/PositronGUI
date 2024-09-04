#pragma once

#include "helpers/HelperFunctions.hpp"
#include "ui/font/FontCollection.hpp"
#include "ui/font/FontEnums.hpp"

#include <string>
#include <optional>
#include <utility>
#include <dwrite_3.h>


namespace PGUI::UI
{
	class TextFormat : public ComPtrHolder<IDWriteTextFormat3>
	{
		public:
		[[nodiscard]] static TextFormat GetDefTextFormat(FLOAT fontSize = 16);

		TextFormat() noexcept = default;
		TextFormat(std::wstring_view fontFamilyName, 
			FLOAT fontSize, std::wstring_view localeName,
			std::optional<Font::FontCollection> fontCollection = std::nullopt,
			Font::FontWeight fontWeight = Font::FontWeights::Medium, 
			Font::FontStyle fontStyle = Font::FontStyles::Normal,
			Font::FontStretch fontStretch = Font::FontStretches::Normal) noexcept;

		[[nodiscard]] TextFormat AdjustFontSizeToDPI(float fontSize) const noexcept;

		[[nodiscard]] Font::FlowDirection GetFlowDirection() const noexcept;
		void SetFlowDirection(Font::FlowDirection flowDirection) const noexcept;
		
		[[nodiscard]] Font::FontCollection GetFontCollection() const noexcept;
		[[nodiscard]] std::wstring GetFontFamilyName() const noexcept;
		
		[[nodiscard]] float GetFontSize() const noexcept;

		[[nodiscard]] DWRITE_LINE_SPACING GetLineSpacing() const noexcept;
		void SetLineSpacing(const DWRITE_LINE_SPACING& lineSpacing) const noexcept;
		
		[[nodiscard]] Font::FontStretch GetFontStretch() const noexcept;
		[[nodiscard]] Font::FontStyle GetFontStyle() const noexcept;
		[[nodiscard]] Font::FontWeight GetFontWeight() const noexcept;

		[[nodiscard]] float GetIncrementalTabStop() const noexcept;
		void SetIncrementalTabStop(float incrementalTabStop) const noexcept;

		[[nodiscard]] std::wstring GetLocaleName() const noexcept;

		[[nodiscard]] Font::ParagraphAlignment GetParagraphAlignment() const noexcept;
		void SetParagraphAlignment(Font::ParagraphAlignment paragraphAlignment) const noexcept;

		[[nodiscard]] Font::ReadingDirection GetReadingDirection() const noexcept;
		void SetReadingDirection(Font::ReadingDirection readingDirection) const noexcept;

		[[nodiscard]] Font::TextAlignment GetTextAlignment() const noexcept;
		void SetTextAlignment(Font::TextAlignment textAlignment) const noexcept;

		[[nodiscard]] std::pair<DWRITE_TRIMMING, ComPtr<IDWriteInlineObject>>
			GetTrimming() const noexcept;
		void SetTrimming(const DWRITE_TRIMMING& trimming, ComPtr<IDWriteInlineObject> inlineObject) const noexcept;

		[[nodiscard]] Font::WordWrapping GetWordWrapping() const noexcept;
		void SetWordWrapping(Font::WordWrapping wordWrapping) const noexcept;
	};
}

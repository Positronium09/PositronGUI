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
		[[nodiscard]] static auto GetDefTextFormat(FLOAT fontSize = 16) -> TextFormat;

		TextFormat() noexcept = default;
		TextFormat(std::wstring_view fontFamilyName, 
			FLOAT fontSize, std::wstring_view localeName,
			std::optional<Font::FontCollection> fontCollection = std::nullopt,
			Font::FontWeight fontWeight = Font::FontWeights::Medium, 
			Font::FontStyle fontStyle = Font::FontStyles::Normal,
			Font::FontStretch fontStretch = Font::FontStretches::Normal) noexcept;

		[[nodiscard]] auto AdjustFontSizeToDPI(float fontSize) const noexcept -> TextFormat;

		[[nodiscard]] auto GetFlowDirection() const noexcept -> Font::FlowDirection;
		void SetFlowDirection(Font::FlowDirection flowDirection) const noexcept;
		
		[[nodiscard]] auto GetFontCollection() const noexcept -> Font::FontCollection;
		[[nodiscard]] auto GetFontFamilyName() const noexcept -> std::wstring;
		
		[[nodiscard]] auto GetFontSize() const noexcept -> float;

		[[nodiscard]] auto GetLineSpacing() const noexcept -> DWRITE_LINE_SPACING;
		void SetLineSpacing(const DWRITE_LINE_SPACING& lineSpacing) const noexcept;
		
		[[nodiscard]] auto GetFontStretch() const noexcept -> Font::FontStretch;
		[[nodiscard]] auto GetFontStyle() const noexcept -> Font::FontStyle;
		[[nodiscard]] auto GetFontWeight() const noexcept -> Font::FontWeight;

		[[nodiscard]] auto GetIncrementalTabStop() const noexcept -> float;
		void SetIncrementalTabStop(float incrementalTabStop) const noexcept;

		[[nodiscard]] auto GetLocaleName() const noexcept -> std::wstring;

		[[nodiscard]] auto GetParagraphAlignment() const noexcept -> Font::ParagraphAlignment;
		void SetParagraphAlignment(Font::ParagraphAlignment paragraphAlignment) const noexcept;

		[[nodiscard]] auto GetReadingDirection() const noexcept -> Font::ReadingDirection;
		void SetReadingDirection(Font::ReadingDirection readingDirection) const noexcept;

		[[nodiscard]] auto GetTextAlignment() const noexcept -> Font::TextAlignment;
		void SetTextAlignment(Font::TextAlignment textAlignment) const noexcept;

		[[nodiscard]] auto
			GetTrimming() const noexcept -> std::pair<DWRITE_TRIMMING, ComPtr<IDWriteInlineObject>>;
		void SetTrimming(const DWRITE_TRIMMING& trimming, ComPtr<IDWriteInlineObject> inlineObject) const noexcept;

		[[nodiscard]] auto GetWordWrapping() const noexcept -> Font::WordWrapping;
		void SetWordWrapping(Font::WordWrapping wordWrapping) const noexcept;
	};
}

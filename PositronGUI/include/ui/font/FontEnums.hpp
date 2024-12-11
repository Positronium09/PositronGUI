#pragma once


#include <dwrite.h>


namespace PGUI::UI::Font
{
	class FontWeight
	{
		public:
		FontWeight() noexcept :
			fontWeight(DWRITE_FONT_WEIGHT_NORMAL)
		{ }
		explicit(false) FontWeight(DWRITE_FONT_WEIGHT _fontWeight) noexcept :
			fontWeight(_fontWeight)
		{ }

		explicit(false) operator DWRITE_FONT_WEIGHT() const noexcept { return fontWeight; }
		auto operator=(const DWRITE_FONT_WEIGHT& _fontWeight) noexcept -> FontWeight& { fontWeight = _fontWeight; return *this; }

		private:
		DWRITE_FONT_WEIGHT fontWeight;

	};
	namespace FontWeights
	{
		static const FontWeight Thin = DWRITE_FONT_WEIGHT_THIN;
		static const FontWeight ExtraLight = DWRITE_FONT_WEIGHT_EXTRA_LIGHT;
		static const FontWeight UltraLight = DWRITE_FONT_WEIGHT_ULTRA_LIGHT;
		static const FontWeight Light = DWRITE_FONT_WEIGHT_LIGHT;
		static const FontWeight SemiLight = DWRITE_FONT_WEIGHT_SEMI_LIGHT;
		static const FontWeight Normal = DWRITE_FONT_WEIGHT_NORMAL;
		static const FontWeight Regular = DWRITE_FONT_WEIGHT_REGULAR;
		static const FontWeight Medium = DWRITE_FONT_WEIGHT_MEDIUM;
		static const FontWeight DemiBold = DWRITE_FONT_WEIGHT_DEMI_BOLD;
		static const FontWeight SemiBold = DWRITE_FONT_WEIGHT_SEMI_BOLD;
		static const FontWeight Bold = DWRITE_FONT_WEIGHT_BOLD;
		static const FontWeight ExtraBold = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
		static const FontWeight UltraBold = DWRITE_FONT_WEIGHT_ULTRA_BOLD;
		static const FontWeight Black = DWRITE_FONT_WEIGHT_BLACK;
		static const FontWeight Heavy = DWRITE_FONT_WEIGHT_HEAVY;
		static const FontWeight ExtraBlack = DWRITE_FONT_WEIGHT_EXTRA_BLACK;
		static const FontWeight UltraBlack = DWRITE_FONT_WEIGHT_ULTRA_BLACK;
	}

	class FontStyle
	{
		public:
		FontStyle() noexcept :
			fontStyle(DWRITE_FONT_STYLE_NORMAL)
		{ }
		explicit(false) FontStyle(DWRITE_FONT_STYLE _fontStyle) noexcept :
			fontStyle(_fontStyle)
		{ }

		explicit(false) operator DWRITE_FONT_STYLE() const noexcept { return fontStyle; }

		private:
		DWRITE_FONT_STYLE fontStyle;
	};

	namespace FontStyles
	{
		static const FontStyle Normal = DWRITE_FONT_STYLE_NORMAL;
		static const FontStyle Oblique = DWRITE_FONT_STYLE_OBLIQUE;
		static const FontStyle Italic = DWRITE_FONT_STYLE_ITALIC;
	}

	class FontStretch
	{
		public:
		FontStretch() noexcept :
			fontStretch(DWRITE_FONT_STRETCH_NORMAL)
		{ }
		explicit(false) FontStretch(DWRITE_FONT_STRETCH _fontStretch) noexcept : 
			fontStretch(_fontStretch)
		{ }

		explicit(false) operator DWRITE_FONT_STRETCH() const noexcept { return fontStretch; }

		private:
		DWRITE_FONT_STRETCH fontStretch;
	};

	namespace FontStretches
	{
		static const FontStretch Undefined = DWRITE_FONT_STRETCH_UNDEFINED;
		static const FontStretch UltraCondensed = DWRITE_FONT_STRETCH_ULTRA_CONDENSED;
		static const FontStretch ExtraCondensed = DWRITE_FONT_STRETCH_EXTRA_CONDENSED;
		static const FontStretch Condensed = DWRITE_FONT_STRETCH_CONDENSED;
		static const FontStretch SemiCondensed = DWRITE_FONT_STRETCH_SEMI_CONDENSED;
		static const FontStretch Normal = DWRITE_FONT_STRETCH_NORMAL;
		static const FontStretch Medium = DWRITE_FONT_STRETCH_MEDIUM;
		static const FontStretch SemiExpanded = DWRITE_FONT_STRETCH_SEMI_EXPANDED;
		static const FontStretch Expanded = DWRITE_FONT_STRETCH_EXPANDED;
		static const FontStretch ExtraExpanded = DWRITE_FONT_STRETCH_EXTRA_EXPANDED;
		static const FontStretch UltraExpanded = DWRITE_FONT_STRETCH_ULTRA_EXPANDED;
	}

	class FlowDirection
	{
		public:
		FlowDirection() noexcept :
			flowDirection(DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT)
		{
		}
		explicit(false) FlowDirection(DWRITE_FLOW_DIRECTION _flowDirection) noexcept :
			flowDirection(_flowDirection)
		{
		}

		explicit(false) operator DWRITE_FLOW_DIRECTION() const noexcept { return flowDirection; }

		private:
		DWRITE_FLOW_DIRECTION flowDirection;
	};

	namespace FlowDirections
	{
		static const FlowDirection LeftToRight = DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT;
		static const FlowDirection RightToLeft = DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT;
		static const FlowDirection TopToBottom = DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM;
		static const FlowDirection BottomToTop = DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP;
	}

	class ParagraphAlignment
	{
		public:
		ParagraphAlignment() noexcept :
			paragraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)
		{
		}
		explicit(false) ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT _paragraphAlignment) noexcept :
			paragraphAlignment(_paragraphAlignment)
		{
		}

		explicit(false) operator DWRITE_PARAGRAPH_ALIGNMENT() const noexcept { return paragraphAlignment; }

		private:
		DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment;
	};

	namespace ParagraphAlignments
	{
		static const ParagraphAlignment Near = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		static const ParagraphAlignment Center = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		static const ParagraphAlignment Far = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	}

	class ReadingDirection
	{
		public:
		ReadingDirection() noexcept :
			readingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT)
		{
		}
		explicit(false) ReadingDirection(DWRITE_READING_DIRECTION _readingDirection) noexcept :
			readingDirection(_readingDirection)
		{
		}

		explicit(false) operator DWRITE_READING_DIRECTION() const noexcept { return readingDirection; }

		private:
		DWRITE_READING_DIRECTION readingDirection;
	};

	namespace ReadingDirections
	{
		static const ReadingDirection LeftToRight = DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
		static const ReadingDirection RightToLeft = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
		static const ReadingDirection TopToBottom = DWRITE_READING_DIRECTION_TOP_TO_BOTTOM;
		static const ReadingDirection BottomToTop = DWRITE_READING_DIRECTION_BOTTOM_TO_TOP;
	}

	class TextAlignment
	{
		public:
		TextAlignment() noexcept :
			textAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)
		{
		}
		explicit(false) TextAlignment(DWRITE_TEXT_ALIGNMENT _textAlignment) noexcept :
			textAlignment(_textAlignment)
		{
		}

		explicit(false) operator DWRITE_TEXT_ALIGNMENT() const noexcept { return textAlignment; }

		private:
		DWRITE_TEXT_ALIGNMENT textAlignment;
	};

	namespace TextAlignments
	{
		static const TextAlignment Center = DWRITE_TEXT_ALIGNMENT_CENTER;
		static const TextAlignment Justified = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
		static const TextAlignment Leading = DWRITE_TEXT_ALIGNMENT_LEADING;
		static const TextAlignment Trailing = DWRITE_TEXT_ALIGNMENT_TRAILING;
	}

	class WordWrapping
	{
		public:
		WordWrapping() noexcept :
			wordWrapping(DWRITE_WORD_WRAPPING_EMERGENCY_BREAK)
		{
		}
		explicit(false) WordWrapping(DWRITE_WORD_WRAPPING _wordWrapping) noexcept :
			wordWrapping(_wordWrapping)
		{
		}

		explicit(false) operator DWRITE_WORD_WRAPPING() const noexcept { return wordWrapping; }

		private:
		DWRITE_WORD_WRAPPING wordWrapping;
	};

	namespace WordWrappings
	{
		static const WordWrapping Character = DWRITE_WORD_WRAPPING_CHARACTER;
		static const WordWrapping EmergencyBreak = DWRITE_WORD_WRAPPING_EMERGENCY_BREAK;
		static const WordWrapping NoWrap = DWRITE_WORD_WRAPPING_NO_WRAP;
		static const WordWrapping WholeWord = DWRITE_WORD_WRAPPING_WHOLE_WORD;
		static const WordWrapping Wrap = DWRITE_WORD_WRAPPING_WRAP;
	}
}

#pragma once


namespace PGUI::Core
{
	class MessageLoopBase
	{
		public:
		virtual ~MessageLoopBase() noexcept = default;

		virtual auto Run() -> int = 0;
	};

	class GetMessageLoop : public MessageLoopBase
	{
		public:
		[[nodiscard]] auto Run() noexcept -> int override;
	};

	class PeekMessageLoop : public MessageLoopBase
	{
		public:
		[[nodiscard]] auto Run() -> int override;
	};
}

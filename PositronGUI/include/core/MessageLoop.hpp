#pragma once


namespace PGUI::Core
{
	class MessageLoopBase
	{
		public:
		virtual ~MessageLoopBase() noexcept = default;

		virtual int Run() = 0;
	};

	class GetMessageLoop : public MessageLoopBase
	{
		public:
		[[nodiscard]] int Run() noexcept override;
	};

	class PeekMessageLoop : public MessageLoopBase
	{
		public:
		[[nodiscard]] int Run() override;
	};
}

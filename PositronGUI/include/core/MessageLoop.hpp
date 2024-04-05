#pragma once


namespace PGUI::Core
{
	class IMessageLoop
	{
		public:
		virtual ~IMessageLoop() noexcept = default;

		virtual int Run() = 0;
	};

	class GetMessageLoop : public IMessageLoop
	{
		public:
		[[nodiscard]] int Run() noexcept override;
	};

	class PeekMessageLoop : public IMessageLoop
	{
		public:
		[[nodiscard]] int Run() override;
	};
}

#include "ui/dialogs/MessageBoxDialog.hpp"
#include "ui/Colors.hpp"

#include "factories/DWriteFactory.hpp"


namespace PGUI::UI::Dialogs
{
	using namespace PGUI::UI::Controls;

	MessageBoxDialog::MessageBoxDialog(std::wstring_view _text, 
		MessageBoxButtonSet _buttonSet, MessageBoxIcon _icon) noexcept : 
		ModalDialog{ Core::WindowClass::Create(L"MessageBoxDialog_Dialog") },
		text(_text), buttonSet(_buttonSet)
	{
		RegisterMessageHandler(WM_CREATE, &MessageBoxDialog::OnCreate);
		RegisterMessageHandler(WM_PAINT, &MessageBoxDialog::OnPaint);

		switch (_icon)
		{
			using enum MessageBoxIcon;

			#pragma warning (push)
			#pragma warning (disable : 4302)

			case Error: // Stop, Hand
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, MAKEINTRESOURCEW(IDI_ERROR), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			case Question:
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, MAKEINTRESOURCEW(IDI_QUESTION), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			case Warning: // Exclamation
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, MAKEINTRESOURCEW(IDI_WARNING), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			case Information: // Asterisk
			{ 
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, 
						MAKEINTRESOURCEW(IDI_INFORMATION), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			default:
				break;
		}

		#pragma warning (pop)
	}

	void MessageBoxDialog::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!buttonHighlightBrush)
		{
			g.CreateBrush(buttonHighlightBrush);
		}
	}

	void MessageBoxDialog::DiscardDeviceResources()
	{
		buttonHighlightBrush.ReleaseBrush();
	}

	MessageBoxChoice MessageBoxDialog::Display() noexcept
	{
		RunModal();
		return choice;
	}

	void MessageBoxDialog::CreateButtons()
	{
		auto size = GetClientSize();

		auto clipParams = RoundedRectangeClipParameters{ RoundedRect{ RectF{ }, 5.0f, 5.0f } };

		const auto createButton = [this, &size, &clipParams](std::size_t id, 
			std::wstring_view buttonText, MessageBoxChoice buttonChoice)
		{
			auto button = AddChildWindow<TextButton>(
				Core::WindowCreateParams{ buttonText,
				{ size.cx - static_cast<int>(id) * (buttonSize.cx + 20), 
				size.cy - buttonSize.cy - 20 },
				buttonSize,
				NULL
				});
			button->ClickedEvent().Subscribe(
				std::bind_front(BindMemberFunc(&MessageBoxDialog::ButtonHandler, this), buttonChoice)
			);
			button->Show();
			button->SetClip(clipParams);
			buttons.push_back(button);
		};

		switch (buttonSet)
		{
			case PGUI::UI::Dialogs::MessageBoxButtonSet::Ok:
			{
				createButton(1, L"Ok", MessageBoxChoice::Ok);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::OkCancel:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"Ok", MessageBoxChoice::Ok);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::RetryCancel:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"Retry", MessageBoxChoice::Retry);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::YesNo:
			{
				createButton(1, L"No", MessageBoxChoice::No);
				createButton(2, L"Yes", MessageBoxChoice::Yes);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::YesNoCancel:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"No", MessageBoxChoice::No);
				createButton(3, L"Yes", MessageBoxChoice::Yes);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::CancelTryContinue:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"Try Again", MessageBoxChoice::TryAgain);
				createButton(3, L"Continue", MessageBoxChoice::Continue);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::AbortRetryIgnore:
			{
				createButton(1, L"Abort", MessageBoxChoice::Abort);
				createButton(2, L"Retry", MessageBoxChoice::Retry);
				createButton(3, L"Ignore", MessageBoxChoice::Ignore);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::Help:
			{
				createButton(buttons.size() + 1, L"Help", MessageBoxChoice::Continue);
				break;
			}
			default:
			{
				std::unreachable();
			}
		}
	}

	void MessageBoxDialog::SetToRequiredSize() const noexcept
	{
		AdjustForClientSize(CalculateSize());
		CenterAroundParent();
	}

	void MessageBoxDialog::ButtonHandler(MessageBoxChoice _choice) noexcept
	{
		choice = _choice;
		SetRunning(false);
	}

	SizeI MessageBoxDialog::CalculateSize() const noexcept
	{
		auto metrics = staticText->GetTextLayout().GetMetrics();
		
		SizeI size{ 
			static_cast<int>(std::ceilf(metrics.width)), 
			static_cast<int>(std::ceilf(metrics.height)) };

		SizeI monitorSize_5_8{  };

		if (HMONITOR monitor = MonitorFromWindow(ParentHwnd(), MONITOR_DEFAULTTONEAREST);
			monitor != nullptr)
		{
			MONITORINFOEXW monitorInfo{ };
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (BOOL ret = GetMonitorInfoW(monitor, &monitorInfo);
				ret != 0)
			{
				SizeI monitorSize = RectI{ monitorInfo.rcMonitor }.Size();
				monitorSize_5_8 = 5 * monitorSize / 8;
			}
		}

		SizeI totalButtonSize{ };
		totalButtonSize = { 20, buttonSize.cy + 40 };
		switch (buttonSet)
		{
			using enum PGUI::UI::Dialogs::MessageBoxButtonSet;
			case Ok:
				totalButtonSize.cx += (buttonSize.cx + 20);
				break;
			case OkCancel:
			case RetryCancel:
			case YesNo:
				totalButtonSize.cx += 2 * (buttonSize.cx + 20);
				break;
			case YesNoCancel:
			case CancelTryContinue:
			case AbortRetryIgnore:
				totalButtonSize.cx += 3 * (buttonSize.cx + 20);
				break;
			default:
				break;
		}

		if (size.cx > monitorSize_5_8.cx)
		{
			size.cx = monitorSize_5_8.cx;
		}
		if (size.cy > monitorSize_5_8.cy)
		{
			size.cy = monitorSize_5_8.cy;
		}

		if (size.cx < totalButtonSize.cx)
		{
			size.cx = totalButtonSize.cx;
		}
		size.cy += totalButtonSize.cy;

		if (iconBmp)
		{
			size.cx += icon.GetSize().cx + margin.left * 2;
		}

		return size;
	}

	Core::HandlerResult MessageBoxDialog::OnCreate(UINT, WPARAM, LPARAM)
	{
		if (UIColors::IsDarkMode())
		{
			EnableDarkTitleBar(Hwnd());
		}

		if (icon.ComPtrHolder<IWICBitmap>::IsInitialized())
		{
			iconBmp = GetGraphics().CreateBitmap(icon);
		}

		TextFormat tf = TextFormat::GetDefTextFormat();
		tf.SetParagraphAlignment(Font::ParagraphAlignments::Center);
		tf.SetTextAlignment(Font::TextAlignments::Leading);
		staticText = AddChildWindow<Controls::StaticText>(
			Core::WindowCreateParams{ text, PointL{ 0, 0 }, SizeL{ 10, 10 }, NULL },
			tf
		);
		staticText->Show();

		staticText->SetBackgroundBrush(Brush{ RGBA{ 0, 0 } });
		SetToRequiredSize();
		CreateButtons();

		auto size = GetClientSize();

		size.cy -= 4 * margin.bottom + buttonSize.cy;

		PointL position{
			margin.left,
			margin.top
		};

		if (iconBmp)
		{
			auto iconSize = icon.GetSize();

			position.x = 2 * margin.left + iconSize.cx;
		}

		size.cx -= position.x + margin.right;

		staticText->MoveAndResize(position, size);

		return 0;
	}

	Core::HandlerResult MessageBoxDialog::OnPaint(UINT, WPARAM, LPARAM)
	{
		BeginDraw();

		SizeF size = GetClientSize();

		auto g = GetGraphics();
		g.Clear(RGBA{ 0x111111 });

		auto prevTransform = g.GetTransform();
		g.SetTransform(GetDpiScaleTransform());

		g.FillRect(RectF{ 0, size.cy - buttonSize.cy - 40, size.cx, size.cy }, 
			buttonHighlightBrush);

		SizeI iconSize{ };
		if (iconBmp)
		{
			float middleY = (size.cy - buttonSize.cy - 40) / 2.f;

			iconSize = icon.GetSize();
			g.DrawBitmap(iconBmp, RectF{
				static_cast<float>(margin.left), 
				middleY - static_cast<float>(iconSize.cy) / 2.f,
				static_cast<float>(margin.left + iconSize.cx), 
				middleY + 
				static_cast<float>(iconSize.cy) / 2.f
				});
		}

		g.SetTransform(prevTransform);

		EndDraw();

		return 0;
	}

	MessageBoxChoice ShowMessageBox(HWND parentHwnd,
		std::wstring_view caption, std::wstring_view text,
		MessageBoxButtonSet buttonSet, MessageBoxIcon icon)
	{
		auto msgBox = Dialog::Create<MessageBoxDialog>(
			DialogCreateParams{ parentHwnd, DS_MODALFRAME },
			Core::WindowCreateParams{ caption, PointI{ }, SizeI{ 200, 200 }, NULL },
			text, buttonSet, icon
		);

		return msgBox->Display();
	}
}

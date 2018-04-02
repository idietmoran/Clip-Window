// clipWindow.cpp : Defines the exported functions for the DLL application.
//

#include "clipWindow.h"
#include <thread>
#include <chrono>

namespace clipWindow
{

	clipWindow::clipWindow(HWND win)
	{
		_window = win;
		_isLive = false;
	}

	clipWindow::clipWindow(HANDLE win)
	{
		_window = static_cast<HWND>(win);
		_isLive = false;
	}


	void clipWindow::cursorLock()
	{

		RECT size;

		while (_isLive)
		{
			//DWORD waiting = WaitForSingleObject(hMessageStop, INFINITE);

			//WaitForSingleObject(&args->mutex, INFINITE);    // wait for mutex
			_mu.lock();

															//GetWindowRect(menu->selectedWindow.hWnd, &menu->selectedWindow.size);
			GetClientRect(_window, &size);
			ClientToScreen(_window, reinterpret_cast<LPPOINT>(&size.right));
			ClientToScreen(_window, reinterpret_cast<LPPOINT>(&size.left));

			HWND active = GetForegroundWindow();

			if (_window == active)
			{
				ClipCursor(&size);
			}
			_mu.unlock();
			std::this_thread::sleep_for(std::chrono::nanoseconds(2));
		}
		ClipCursor(nullptr);
	}


	void clipWindow::start()
	{
		_isLive = true;

		std::thread t1([=]()
		{
			cursorLock();
		});
		t1.detach();
	}

	void clipWindow::stop()
	{
		_mu.lock();
		_isLive = false;
		_mu.unlock();
	}

}

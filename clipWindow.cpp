// clipWindow.cpp : Defines the exported functions for the DLL application.
//

#include "clipWindow.h"
#include <thread>
#include <chrono>

namespace clipWindow
{

	clipWindow::clipWindow()
	{
		_window = nullptr;
		_isLive = false;
	}

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
			_mu.lock();		// lock the thread

			// get rectangle stuff
			GetClientRect(_window, &size);
			ClientToScreen(_window, reinterpret_cast<LPPOINT>(&size.right));
			ClientToScreen(_window, reinterpret_cast<LPPOINT>(&size.left));

			HWND active = GetForegroundWindow();	// get window in focus

			// if window active lock the cursor to the window
			if (_window == active)
				ClipCursor(&size);


			_mu.unlock();	// unlock the thread

			// sleep the thread, this reduces CPU usage greatly
			std::this_thread::sleep_for(std::chrono::nanoseconds(2));
		}
		ClipCursor(nullptr);
	}


	void clipWindow::start()
	{
		_isLive = true;
		// create thread to watch the lock
		// non-blocking
		std::thread([=]()
		{
			cursorLock();
		}).detach();
	}

	void clipWindow::stop()
	{
		_mu.lock();
		_isLive = false;
		_mu.unlock();
	}
	
	void clipWindow::updateWindow(HWND win)
	{
		_mu.lock();		// lock the thread
		_window = win;
		_mu.unlock();	// unlock the thread
	}

}

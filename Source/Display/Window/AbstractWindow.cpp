#include "AbstractWindow.h"
#include "AbstractManager.h"

#include "Parallelism/Fiber/Scheduler.h"


/* Constructor. */
AbstractWindow::AbstractWindow(WindowType inWin, const std::string& inTitle, uint x, uint y, uint iwidth, uint iheight, void* monitorIn, void* sharedContext)
{
	windowType = inWin;
	xPos = x;
	yPos = y;
	width = iwidth;
	height = iheight;
	title = inTitle;
	windowHandle = nullptr;
}
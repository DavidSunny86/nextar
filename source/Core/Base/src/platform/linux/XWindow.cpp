#include <BaseHeaders.h>
#include <RenderWindow.h>

#if defined(NEX_LINUX)

#include <XWindow.h>

namespace nextar {

void XWindow::ProcessEvent(XWindow* wind, XEvent& event) {

	switch (event.type) {
	case ClientMessage:
		if (wind->GetXDestroyMsg() != event.xclient.data.l[0])
			break;
		/* no break */
	case DestroyNotify: {
		bool postQuitMsg = false;
		if (wind->IsExitOnCloseEnabled()) {
			// save flag as window might cease to exist
			postQuitMsg = true;
		}
		if (!wind->IsClosed()) {
			wind->Destroy();
		}
		if (postQuitMsg) {
			ApplicationContext::Instance().SetQuitting(true);
		}
		break;
	}
	case ConfigureNotify: {
		wind->WindowConfigChanged();
		break;
	}
	case FocusIn: // Gained keyboard focus
	case FocusOut: // Lost keyboard focus
	case MapNotify: //Restored
		wind->WindowFocusChanged();
		break;
	case UnmapNotify: //Minimised
		wind->SetActive(false);
		wind->SetVisible(false);
		wind->WindowFocusChanged();
		break;
	case VisibilityNotify:
		switch (event.xvisibility.state) {
		case VisibilityUnobscured:
		case VisibilityPartiallyObscured:
			wind->SetActive(true);
			wind->SetVisible(true);
			break;
		case VisibilityFullyObscured:
			wind->SetActive(false);
			wind->SetVisible(false);
			break;
		}
		wind->WindowFocusChanged();
		break;
	default:
		break;
	} //End switch event.type

}

}

#endif

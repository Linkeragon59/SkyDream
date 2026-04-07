#include "Core_GLFW.h"

#if LINUX_BUILD

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include <unordered_map>
#include <iostream>

struct TouchPoint {
	int id;		// XInput2 tracking ID
	double x, y;   // Window-relative coordinates
};

std::unordered_map<int, TouchPoint> touches;

void setupXInput2(Display* dpy, Window win) {
	int xi_opcode, event, error;
	if (!XQueryExtension(dpy, "XInputExtension",
						 &xi_opcode, &event, &error)) {
		std::cerr << "X Input extension not available\n";
		return;
	}

	int major = 2, minor = 2;
	int rc = XIQueryVersion(dpy, &major, &minor);
	if (rc != Success) {
		std::cerr << "XInput2 not supported (need 2.2)\n";
		return;
	}

	{
		int numMask = 0;
		XIEventMask* prevMask = XIGetSelectedEvents(dpy, win, &numMask);
		XFree(prevMask);
	}

	XIEventMask mask;
	unsigned char mask_data[XIMaskLen(XI_LASTEVENT)] = {};

	mask.deviceid = XIAllMasterDevices;
	mask.mask_len = sizeof(mask_data);
	mask.mask = mask_data;

	XISetMask(mask.mask, XI_TouchBegin);
	XISetMask(mask.mask, XI_TouchUpdate);
	XISetMask(mask.mask, XI_TouchEnd);

	XISelectEvents(dpy, win, &mask, 1);
	XFlush(dpy);

	{
		int numMask = 0;
		XIEventMask* prevMask = XIGetSelectedEvents(dpy, win, &numMask);
		XFree(prevMask);
	}

	std::cout << "XInput2 multitouch enabled\n";
}

void handleXEvent(XEvent* ev) {
	if (ev->type != GenericEvent || ev->xcookie.extension == 0)
		return;

	if (!XGetEventData(ev->xcookie.display, &ev->xcookie))
		return;

	XIDeviceEvent* xiev = static_cast<XIDeviceEvent*>(ev->xcookie.data);
	int id = xiev->detail; // tracking ID
	double x = xiev->event_x;
	double y = xiev->event_y;
	if (ev->xcookie.evtype == XI_TouchBegin) {
		touches[id] = { id, x, y };
		std::cout << "Touch BEGIN id=" << id
				  << " x=" << x << " y=" << y << "\n";
	}
	else if (ev->xcookie.evtype == XI_TouchUpdate) {
		auto& t = touches[id];
		t.x = x;
		t.y = y;
		std::cout << "Touch MOVE  id=" << id
				  << " x=" << x << " y=" << y << "\n";
	}
	else if (ev->xcookie.evtype == XI_TouchEnd) {
		touches.erase(id);
		std::cout << "Touch END   id=" << id << "\n";
	}

	XFreeEventData(ev->xcookie.display, &ev->xcookie);
}

#endif

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

#if LINUX_BUILD
	const char* session = getenv("XDG_SESSION_TYPE");
	bool isWayland = session && std::string(session) == "wayland";
	if (isWayland)
		return EXIT_FAILURE;

	if (!glfwInit())
		return EXIT_FAILURE;

	GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW + XInput2 Touch", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	// ---- Get native X11 handles from GLFW ----
	Display* dpy = glfwGetX11Display();
	Window win = glfwGetX11Window(window);

	setupXInput2(dpy, win);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// ---- Pump X11 events manually ----
		while (XPending(dpy)) {
			XEvent ev;
			XNextEvent(dpy, &ev);
			handleXEvent(&ev);
		}
	}

	glfwTerminate();
#endif

	return EXIT_SUCCESS;
}

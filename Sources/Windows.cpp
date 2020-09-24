// Martijn Smit <martijn@lostdomain.org / @smitmartijn>
#include "ZoomStreamDeckPlugin.h"

#include <StreamDeckSDK/ESDLogger.h>
#include <windows.h>
#include <tchar.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

std::string exec(const char *cmd)
{
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
  if (!pipe)
  {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
  {
    result += buffer.data();
  }
  return result;
}

std::string osGetZoomStatus()
{
  std::string output = exec("getZoomStatus\\getZoomStatus.exe");
  //ESDDebug(output.c_str());
  return output;
}

void osToggleZoomMute()
{
  //ESDDebug("ZoomPlugin: Sending mute shortcut");
  std::string output = exec("getZoomStatus\\getZoomStatus.exe --toggle_mute");
  //ESDDebug(output.c_str());
}
void osToggleZoomShare()
{
  //ESDDebug("ZoomPlugin: Sending share shortcut");
  std::string output = exec("getZoomStatus\\getZoomStatus.exe --toggle_share");
  //ESDDebug(output.c_str());
}
void osToggleZoomView()
{
  //ESDDebug("ZoomPlugin: Sending speaker view shortcut");
  std::string output = exec("getZoomStatus\\getZoomStatus.exe --toggle_view");
  //ESDDebug(output.c_str());
}
void osToggleZoomVideo()
{
  //ESDDebug("ZoomPlugin: Sending video shortcut");
  std::string output = exec("getZoomStatus\\getZoomStatus.exe --toggle_video");
  //ESDDebug(output.c_str());
}
void osLeaveZoomMeeting()
{
  //ESDDebug("ZoomPlugin: Ending meeting");
  std::string output = exec("getZoomStatus\\getZoomStatus.exe --end_meeting");
  //ESDDebug(output.c_str());
}

void SetForegroundWindowInternal(HWND hWnd)
{
  if (!::IsWindow(hWnd))
    return;

  BYTE keyState[256] = {0};
  //to unlock SetForegroundWindow we need to imitate Alt pressing
  if (::GetKeyboardState((LPBYTE)&keyState))
  {
    if (!(keyState[VK_MENU] & 0x80))
    {
      ::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
    }
  }

  ::SetForegroundWindow(hWnd);

  if (::GetKeyboardState((LPBYTE)&keyState))
  {
    if (!(keyState[VK_MENU] & 0x80))
    {
      ::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
  }
}

void osFocusZoomWindow()
{
  // Get the handle of the Zoom window.
  HWND zoomWnd = ::FindWindow(NULL, "Zoom");
  if (zoomWnd == NULL)
  {
    //ESDDebug("ZoomPlugin: Unable to find Window for Zoom: it's closed!");
    return;
  }

  SetForegroundWindowInternal(zoomWnd);
}

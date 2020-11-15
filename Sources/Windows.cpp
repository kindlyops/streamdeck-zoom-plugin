// Martijn Smit <martijn@lostdomain.org / @smitmartijn>
#include "ZoomStreamDeckPlugin.h"

#include <StreamDeckSDK/ESDLogger.h>
#include <windows.h>
#include <uiautomationclient.h>
#include <string>
#include <sstream>
#include <regex>

IUIAutomationElement *GetTopLevelWindowByName(LPWSTR windowName);
void rawListDescendants(IUIAutomationElement *pParent, std::string windowType);
void altPlusKey(UINT key);

std::string g_statusZoom = "stopped";
std::string g_statusMute = "unknown";
std::string g_statusVideo = "unknown";
std::string g_statusShare = "unknown";
std::string g_statusRecord = "unknown";
std::string g_windowName = "unknown";

IUIAutomation *g_pAutomation = nullptr;

void init()
{
  CoInitializeEx(NULL, COINIT_MULTITHREADED);
  auto hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void **)&g_pAutomation);
  if (FAILED(hr) || g_pAutomation == nullptr)
  {
    ESDDebug("failed to init automation");
    return;
  }
}

std::string formatStatus()
{
  std::stringstream ss;
  ss << "zoomStatus:" << g_statusZoom << ",";
  ss << "zoomMute:" << g_statusMute << ",";
  ss << "zoomVideo:" << g_statusVideo << ",";
  ss << "zoomRecord:" << g_statusRecord << ",";
  ss << "zoomShare:" << g_statusShare;
  return ss.str();
}

std::string osGetZoomStatus()
{
  init();

  // Get the handle of the Zoom window.
  HWND zoomWnd = ::FindWindow(NULL, "Zoom");
  if (zoomWnd != NULL)
  {
    g_statusZoom = "open";
    IUIAutomationElement *zoom = GetTopLevelWindowByName(L"Zoom");
    rawListDescendants(zoom, "Zoom");
    g_windowName = "Zoom";
  }

  // Get the handle of the Zoom Meetings window.
  zoomWnd = ::FindWindow(NULL, "Zoom Meeting");
  if (zoomWnd != NULL)
  {
    g_statusZoom = "call";
    IUIAutomationElement *zoom = GetTopLevelWindowByName(L"Zoom Meeting");
    rawListDescendants(zoom, "Meeting");
    g_windowName = "Zoom Meeting";
  }

  // Get the handle of the Meetings Controls window.
  zoomWnd = ::FindWindow(NULL, "Meeting Controls");
  if (zoomWnd != NULL)
  {
    g_statusZoom = "call";
    IUIAutomationElement *zoom = GetTopLevelWindowByName(L"Meeting Controls");
    rawListDescendants(zoom, "Controls");
    g_windowName = "Meeting Controls";
  }

  if (g_pAutomation != NULL)
    g_pAutomation->Release();

  CoUninitialize();

  return formatStatus();
}

IUIAutomationElement *GetTopLevelWindowByName(LPWSTR windowName)
{
  if (windowName == NULL)
  {
    return NULL;
  }

  VARIANT varProp;
  varProp.vt = VT_BSTR;
  varProp.bstrVal = SysAllocString(windowName);
  if (varProp.bstrVal == NULL)
  {
    return NULL;
  }

  IUIAutomationElement *pRoot = NULL;
  IUIAutomationElement *pFound = NULL;

  // Get the desktop element.
  HRESULT hr = g_pAutomation->GetRootElement(&pRoot);
  if (FAILED(hr) || pRoot == NULL)
  {
    goto cleanup;
  }

  // Get a top-level element by name, such as "Program Manager"
  IUIAutomationCondition *pCondition = NULL;
  hr = g_pAutomation->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pCondition);
  if (FAILED(hr))
  {
    goto cleanup;
  }

  pRoot->FindFirst(TreeScope_Children, pCondition, &pFound);

cleanup:
  if (pRoot != NULL)
    pRoot->Release();

  if (pCondition != NULL)
    pCondition->Release();

  VariantClear(&varProp);

  return pFound;
}

void rawListDescendants(IUIAutomationElement *pParent, std::string windowType)
{
  if (pParent == NULL)
    return;

  IUIAutomationTreeWalker *pControlWalker = NULL;
  IUIAutomationElement *pNode = NULL;

  g_pAutomation->get_RawViewWalker(&pControlWalker);
  if (pControlWalker == NULL)
  {
    goto cleanup;
  }

  pControlWalker->GetFirstChildElement(pParent, &pNode);
  if (pNode == NULL)
  {
    goto cleanup;
  }

  while (pNode)
  {
    BSTR desc;
    BSTR sName;

    pNode->get_CurrentLocalizedControlType(&desc);
    pNode->get_CurrentName(&sName);

    /*if (DEBUG)
    {
      std::cout << "Element type = ";
      if (NULL == desc)
        std::cout << "Empty";
      else
        std::wcout << desc;

      std::cout << "  Name  = ";
      if (NULL == sName)
        std::cout << "Empty";
      else
        std::wcout << sName;

      std::cout << "\n";
    }*/

    // we have a name of the element - match it against what we're looking for
    if (sName != NULL)
    {
      std::wstring strName(sName, SysStringLen(sName));

      // we're looking for different buttons per window. This is the Main Zoom window
      if (windowType == "Meeting")
      {
        if (strName.find(L"currently unmuted") != std::string::npos)
        {
          g_statusMute = "unmuted";
        }
        if (strName.find(L"currently muted") != std::string::npos)
        {
          g_statusMute = "muted";
        }

        if (strName.find(L"start my video") != std::string::npos)
        {
          g_statusVideo = "stopped";
        }
        if (strName.find(L"stop my video") != std::string::npos)
        {
          g_statusVideo = "started";
        }

        if (strName.find(L"Share Screen") != std::string::npos)
        {
          g_statusShare = "stopped";
        }

        if (strName.find(L"Record") != std::string::npos && g_statusRecord != "started")
        {
          g_statusRecord = "stopped";
        }
        if (strName.find(L"Stop Recording") != std::string::npos)
        {
          g_statusRecord = "started";
        }
      }
      // we're looking for different buttons per window. This is the minimized Zoom window
      else if (windowType == "Zoom")
      {
        if (strName.find(L"Mute My Audio") != std::string::npos)
        {
          g_statusMute = "unmuted";
        }
        if (strName.find(L"Unmute My Audio") != std::string::npos)
        {
          g_statusMute = "muted";
        }

        if (strName.find(L"Start Video") != std::string::npos)
        {
          g_statusVideo = "stopped";
        }
        if (strName.find(L"Stop Video") != std::string::npos)
        {
          g_statusVideo = "started";
        }

        // you cannot minimize the window while recording
        g_statusRecord = "stopped";
        g_statusShare = "stopped";
      }

      // we're looking for different buttons per window. This is the Zoom window when you're sharing
      else if (windowType == "Controls")
      {
        if (strName.find(L"currently unmuted") != std::string::npos)
        {
          g_statusMute = "unmuted";
        }
        if (strName.find(L"currently muted") != std::string::npos)
        {
          g_statusMute = "muted";
        }

        if (strName.find(L"Start Video") != std::string::npos)
        {
          g_statusVideo = "stopped";
        }
        if (strName.find(L"Stop Video") != std::string::npos)
        {
          g_statusVideo = "started";
        }

        // toolbar open = sharing
        g_statusShare = "started";
        // can't read record button in this view
        g_statusRecord = "disabled";
      }
    }

    // only go into windows and panes
    if (desc != NULL)
      if (0 == wcscmp(desc, L"window") || 0 == wcscmp(desc, L"pane"))
        rawListDescendants(pNode, windowType);

    if (desc != NULL)
      SysFreeString(desc);
    if (sName != NULL)
      SysFreeString(sName);

    // get the next element
    IUIAutomationElement *pNext;
    pControlWalker->GetNextSiblingElement(pNode, &pNext);
    pNode->Release();
    pNode = pNext;
  }

cleanup:
  if (pControlWalker != NULL)
    pControlWalker->Release();

  if (pNode != NULL)
    pNode->Release();

  return;
}

void osFocusZoomWindow()
{
  // Get the handle of the Zoom window.
  HWND zoomWnd = ::FindWindow(NULL, g_windowName.c_str());
  if (zoomWnd == NULL)
  {
    return;
  }
  SetForegroundWindow(zoomWnd);
}

void osToggleZoomMute()
{
  //ESDDebug("ZoomPlugin: Sending mute shortcut");
  osFocusZoomWindow();
  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x41, 0, 0, 0); // virtual key code for 'A'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x41, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'A'
}
void osToggleZoomShare()
{
  //ESDDebug("ZoomPlugin: Sending share shortcut");
  osFocusZoomWindow();
  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x53, 0, 0, 0); // virtual key code for 'S'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x53, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'S'
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
  osFocusZoomWindow();
  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x56, 0, 0, 0); // virtual key code for 'V'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x56, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'V'
}
void osLeaveZoomMeeting()
{
  //ESDDebug("ZoomPlugin: Ending meeting");
  osFocusZoomWindow();
  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x51, 0, 0, 0); // virtual key code for 'Q'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x51, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'Q'
}

void osToggleZoomRecordCloud()
{
  //ESDDebug("ZoomPlugin: Toggling Cloud Recording");
  osFocusZoomWindow();
  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x43, 0, 0, 0); // virtual key code for 'C'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x43, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'C'
}

void osToggleZoomRecordLocal()
{
  //ESDDebug("ZoomPlugin: Toggling Local Recording");
  osFocusZoomWindow();

  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x52, 0, 0, 0); // virtual key code for 'R'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x52, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'R'
}
void osMuteAll()
{
  osFocusZoomWindow();

  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x4D, 0, 0, 0); // virtual key code for 'M'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x4D, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'M'

  Sleep(200);
  // wait 200ms and press enter to negate the "are you sure?" window
  int key_count = 2;

  INPUT *input = new INPUT[key_count];
  for (int i = 0; i < key_count; i++)
  {
    input[i].ki.dwFlags = 0;
    input[i].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_RETURN;
    input[0].ki.wScan = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
  }

  // key up
  input[1].ki.dwFlags = KEYEVENTF_KEYUP;

  SendInput(key_count, (LPINPUT)input, sizeof(INPUT));
}

void osUnmuteAll()
{
  osFocusZoomWindow();

  ::keybd_event(VK_MENU, 0, 0, 0);
  ::keybd_event(0x4D, 0, 0, 0); // virtual key code for 'M'
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
  keybd_event(0x4D, 0, KEYEVENTF_KEYUP, 0); // virtual key code for 'M'
}

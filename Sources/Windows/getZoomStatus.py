# Credit for initial script: https://github.com/oldjohngalt
#
# Martijn Smit <martijn@lostdomain.org / @smitmartijn>
import os
import sys
import time
import argparse

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import uiautomation as automation

debug = False
zoomMeetingOpen = False
zoomToolbarOpen = False
toggleMute = False
toggleVideo = False
toggleShare = False

def DetectZoomMeeting():
  global zoomToolbarOpen
  global zoomMeetingOpen

  if debug == True:
    global start
    start = time.time()
  zoomMeetingInProgress = False
  def GetFirstChild(control):
    return control.GetFirstChildControl()
  def GetNextSibling(control):
    return control.GetNextSiblingControl()
  desktop = automation.GetRootControl()

  zoomMeetingOpen = False
  zoomToolbarOpen = False
  for control, depth in automation.WalkTree(desktop, getFirstChild=GetFirstChild, getNextSibling=GetNextSibling, includeTop=False, maxDepth=2):
    if debug == True:
      print(str(depth) + ' ' * depth * 4 + str(control))
    if str(control).find("ZPContentViewWndClass") > 0:
      zoomMeetingInProgress = True
      zoomMeetingOpen = True
    if str(control).find("ZPFloatToolbarClass") > 0:
      zoomMeetingInProgress = True
      zoomToolbarOpen = True
  return zoomMeetingInProgress

def GetFirstChild(control):
  return control.GetFirstChildControl()
def GetNextSibling(control):
  return control.GetNextSiblingControl()

def GetZoomStatus():
  global debug
  global zoomToolbarOpen
  global zoomMeetingOpen
  global toggleMute
  global toggleVideo
  global toggleView
  global toggleShare
  statusMute   = "unknown"
  statusVideo  = "unknown"
  statusShare  = "unknown"
  statusView   = "unknown"
  statusRecord = "unknown"

  desktop   = automation.GetRootControl()
  if zoomToolbarOpen == True:
    window    = automation.WindowControl(searchDepth=1, ClassName='ZPFloatToolbarClass')
    toolsMenu = window
  else:
    window    = automation.WindowControl(searchDepth=1, ClassName='ZPContentViewWndClass')
    toolsMenu = window.WindowControl(searchDepth=3, ClassName='ZPControlPanelClass')
  if debug == True:
    print ("desktop="+str(desktop))
    print ("window="+str(window))
    print ("toolsMenu="+str(toolsMenu))

  if window != toolsMenu:
    for control, depth in automation.WalkTree(window, getFirstChild=GetFirstChild, getNextSibling=GetNextSibling, includeTop=False, maxDepth=3):
      if str(control).find("Gallery View (Alt+F2)") > 0:
        statusView = "speaker"
        print(str(control))
        if toggleView == True:
          print("sending keys f2")
          automation.SendKeys('{Alt}{F2}')
          statusView = "gallery"
          break
      elif str(control).find("Speaker View") > 0:
        statusView = "gallery"
        if toggleView == True:
          automation.SendKeys('{Alt}{F1}')
          statusView = "speaker"
          break

  for control, depth in automation.WalkTree(toolsMenu, getFirstChild=GetFirstChild, getNextSibling=GetNextSibling, includeTop=False, maxDepth=2):
    if debug == True:
      print(str(depth) + ' ' * depth * 4 + str(control))
    if str(control).find("currently muted") > 0:
      statusMute = "muted"
      if toggleMute == True:
        automation.SendKeys('{Alt}a')
        statusMute = "unmuted"
    elif str(control).find("currently unmuted") > 0:
      statusMute = "unmuted"
      if toggleMute == True:
        automation.SendKeys('{Alt}a')
        statusMute = "muted"
    elif str(control).find("start my video") > 0:
      statusVideo = "stopped"
      if toggleVideo == True:
        automation.SendKeys('{Alt}v')
        statusVideo = "started"
    elif str(control).find("stop my video") > 0:
      statusVideo = "started"
      if toggleVideo == True:
        automation.SendKeys('{Alt}v')
        statusVideo = "stopped"
    elif str(control).find("Share Screen") > 0:
      statusShare = "stopped"
      if toggleShare == True:
        automation.SendKeys('{Alt}s')
        statusShare = "started"
    elif str(control).find("Stop Share") > 0:
      statusShare = "started"
      if toggleShare == True:
        automation.SendKeys('{Alt}s')
        statusShare = "stopped"
    elif str(control).find("Resume Share") > 0:
      statusShare = "disabled"
    # Toolbar
    elif str(control).find("Start Video") > 0:
      statusVideo = "stopped"
      if toggleVideo == True:
        automation.SendKeys('{Alt}v')
        statusVideo = "started"
    elif str(control).find("Stop Video") > 0:
      statusVideo = "started"
      if toggleVideo == True:
        automation.SendKeys('{Alt}v')
        statusVideo = "stopped"

 # The maxDepth needs to be increased if you want to add recording status.  However, it doubles the processing time.
 #       elif  str(control).find("Name: Record") > 0:
 #           statusRecord = "stopped"
 #       elif  str(control).find("Pause/Stop Recording") > 0:
 #           statusRecord = "started"
 #       elif  str(control).find("Resume Share") > 0:
 #           statusRecord = "paused"


  if debug == True:
    stop = time.time()
    print("Elapsed Time:", stop-start)

  statusZoom = "call"
  status = "zoomStatus:"+statusZoom+",zoomMute:"+statusMute+",zoomVideo:"+statusVideo+",zoomShare:"+statusShare+",zoomView:"+statusView
  print (status)

def end_meeting():
  automation.SendKeys('{Alt}q')
  time.sleep(1)

  window = automation.WindowControl(searchDepth=1, ClassName='zLeaveWndClass')
  for control, depth in automation.WalkTree(window, getFirstChild=GetFirstChild, getNextSibling=GetNextSibling, includeTop=False, maxDepth=2):
    if str(control).find("End Meeting for All") > 0:
      control.Click(simulateMove = False)
      exit(0)


def main():
  if DetectZoomMeeting() == True:
    # Zoom Meeting in Progress
    GetZoomStatus()
  else:
    # No Zoom Meeting Detected
    status = "zoomStatus:closed,zoomMute:disabled,zoomVideo:disabled,zoomShare:disabled,zoomView:disabled"
    print (status)

if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  parser.add_argument("-d", "--debug", help = "Debug", action='store_true')
  parser.add_argument("-m", "--toggle_mute", help = "Toggle Mute", action='store_true')
  parser.add_argument("-v", "--toggle_video", help = "Toggle Video", action='store_true')
  parser.add_argument("-s", "--toggle_share", help = "Toggle Share", action='store_true')
  parser.add_argument("-t", "--toggle_view", help = "Toggle Speaker View", action='store_true')
  parser.add_argument("-e", "--end_meeting", help = "End Meeting", action='store_true')
  args = parser.parse_args()

  if args.end_meeting:
    end_meeting()
    exit(0)

  if args.debug:
    debug = True
  if args.toggle_mute:
    toggleMute = True
  if args.toggle_video:
    toggleVideo = True
  if args.toggle_share:
    toggleShare = True
  if args.toggle_view:
    toggleView = True

  main()
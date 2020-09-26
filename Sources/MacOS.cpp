// Martijn Smit <martijn@lostdomain.org / @smitmartijn>
#include "ZoomStreamDeckPlugin.h"
#include <StreamDeckSDK/ESDLogger.h>

char *execAndReturn(const char *command)
{
  FILE *fp;
  char *line = NULL;
  // Following initialization is equivalent to char* result = ""; and just
  // initializes result to an empty string, only it works with
  // -Werror=write-strings and is so much less clear.
  char *result = (char *)calloc(1, 1);
  size_t len = 0;

  fflush(NULL);
  fp = popen(command, "r");
  if (fp == NULL)
  {
    printf("Cannot execute command:\n%s\n", command);
    ESDDebug("Cannot execute command:\n%s\n", command);
    return NULL;
  }

  while (getline(&line, &len, fp) != -1)
  {
    // +1 below to allow room for null terminator.
    result = (char *)realloc(result, strlen(result) + strlen(line) + 1);
    // +1 below so we copy the final null terminator.
    strncpy(result + strlen(result), line, strlen(line) + 1);
    free(line);
    line = NULL;
  }

  fflush(fp);
  if (pclose(fp) != 0)
  {
    perror("Cannot close stream.\n");
  }
  return result;
}

std::string osGetZoomStatus()
{
  // ESDDebug("APPLESCRIPT_GET_STATUS: %s", APPLESCRIPT_GET_STATUS);

  char *zoomStatus = execAndReturn(
      "osascript -e 'set zoomStatus to \"closed\"\nset muteStatus to \"disabled\"\n"
      "set viewStatus to \"disabled\"\n"
      "set videoStatus to \"disabled\"\nset shareStatus to \"disabled\"\ntell application "
      "\"System Events\"\nif exists (window 1 of process \"zoom.us\") then\nset zoomStatus "
      "to \"open\"\ntell application process \"zoom.us\"\nif exists (menu bar item "
      "\"Meeting\" of menu bar 1) then\nset zoomStatus to \"call\"\n"
      "if exists (menu item \"Speaker View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
      "set viewStatus to \"gallery\"\n"
      "else if exists (menu item \"Gallery View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
      "set viewStatus to \"speaker\"\n"
      "end if\n"
      "if exists (menu item "
      "\"Mute audio\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\nif "
      "enabled of menu item \"Mute audio\" of menu 1 of menu bar item \"Meeting\" of menu "
      "bar 1 then\nset muteStatus to \"unmuted\"\nend if\nelse if exists (menu item "
      "\"Unmute audio\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\nif "
      "enabled of menu item \"Unmute audio\" of menu 1 of menu bar item \"Meeting\" of "
      "menu bar 1 then\nset muteStatus to \"muted\"\nend if\nend if\nif exists (menu item "
      "\"Start Video\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\nif "
      "enabled of menu item \"Start Video\" of menu 1 of menu bar item \"Meeting\" of menu "
      "bar 1 then\nset videoStatus to \"stopped\"\nend if\nelse if exists (menu item "
      "\"Stop Video\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\nif "
      "enabled of menu item \"Stop Video\" of menu 1 of menu bar item \"Meeting\" of menu "
      "bar 1 then\nset videoStatus to \"started\"\nend if\nend if\nif exists (menu item "
      "\"Start Share\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\nset "
      "shareStatus to \"stopped\"\nelse\nif exists (menu item \"Stop Share\" of menu 1 of "
      "menu bar item \"Meeting\" of menu bar 1) then\nset shareStatus to \"started\"\n"
      "end if\nend if\nend if\nend tell\nend if\nend tell\ndo shell script \"echo zoomMute:\" & "
      "(muteStatus as text) & \",zoomVideo:\" & (videoStatus as text) & \",zoomStatus:\" & "
      "(zoomStatus as text) & \"zoomView:\" & (viewStatus as text) & "
      "\",zoomShare:\" & (shareStatus as text)'");

  return std::string(zoomStatus);
}

void osToggleZoomMute()
{
  const char *script = "osascript -e 'tell application \"zoom.us\"\ntell application \"System "
                       "Events\" to tell application process \"zoom.us\"\nif exists (menu bar "
                       "item \"Meeting\" of menu bar 1) then\nkeystroke \"a\" using {shift down"
                       ", command down}\nend if\nend tell\nend tell'";
  system(script);
}

void osToggleZoomShare()
{
  const char *script = "osascript -e 'tell application \"zoom.us\"\ntell application \"System "
                       "Events\" to tell application process \"zoom.us\"\nif exists (menu bar "
                       "item \"Meeting\" of menu bar 1) then\nkeystroke \"s\" using {shift down"
                       ", command down}\nend if\nend tell\nend tell'";
  system(script);
}

void osToggleZoomView()
{
  const char *script = "osascript -e 'tell application \"zoom.us\"\ntell application \"System "
                       "Events\" to tell application process \"zoom.us\"\n"
                       "keystroke \"w\" using {shift down, command down}\nend tell\nend tell'";
  system(script);
}

void osToggleZoomVideo()
{
  const char *script = "osascript -e 'tell application \"zoom.us\"\ntell application \"System "
                       "Events\" to tell application process \"zoom.us\"\nif exists (menu bar "
                       "item \"Meeting\" of menu bar 1) then\nkeystroke \"v\" using {shift down"
                       ", command down}\nend if\nend tell\nend tell'";
  system(script);
}

void osLeaveZoomMeeting()
{
  const char *script = "osascript -e 'tell application \"System Events\"\nif exists (menu "
                       "bar item \"Meeting\" of menu bar 1 of application process \"zoom.us\") "
                       "then\ntell application \"zoom.us\" to activate\nkeystroke \"w\" using "
                       "{command down}\ntell front window of (first application process whose "
                       "frontmost is true)\nclick button 1\nend tell\nend if\nend tell'";
  system(script);
}

void osFocusZoomWindow()
{
  const char *script = "osascript -e 'tell application \"zoom.us\"\nactivate\nend tell'";
  system(script);
}
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
  /*
  Original AS:
  set zoomStatus to "closed"
set muteStatus to "disabled"
set videoStatus to "disabled"
set shareStatus to "disabled"
set recordStatus to "disabled"
tell application "System Events"
	if exists (window 1 of process "zoom.us") then
		set zoomStatus to "open"
		tell application process "zoom.us"
			if exists (menu bar item "Meeting" of menu bar 1) then
				set zoomStatus to "call"
				if exists (menu item "Mute audio" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set muteStatus to "unmuted"
				else
					set muteStatus to "muted"
				end if
				if exists (menu item "Start Video" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set videoStatus to "stopped"
				else
					set videoStatus to "started"
				end if
				if exists (menu item "Start Share" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set shareStatus to "stopped"
				else
					set shareStatus to "started"
				end if
				if exists (menu item "Record to the Cloud" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set recordStatus to "stopped"
				else if exists (menu item "Record" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set recordStatus to "stopped"
				else
					set recordStatus to "started"
				end if
			end if
		end tell
	end if
end tell
do shell script "echo zoomMute:" & (muteStatus as text) & ",zoomVideo:" & (videoStatus as text) & ",zoomStatus:" & (zoomStatus as text) & ",zoomShare:" & (shareStatus as text) & ",zoomRecord:" & (recordStatus as text)
  */
  // ESDDebug("APPLESCRIPT_GET_STATUS: %s", APPLESCRIPT_GET_STATUS);
  const char *appleScript = "set zoomStatus to \"closed\"\n"
                            "set muteStatus to \"disabled\"\n"
                            "set videoStatus to \"disabled\"\n"
                            "set viewStatus to \"disabled\"\n"
                            "set shareStatus to \"disabled\"\n"
                            "set recordStatus to \"disabled\"\n"
                            "set speakerViewStatus to \"disabled\"\n"
                            "set minimalView to \"disabled\"\n"
                            "tell application \"System Events\"\n"
                            "	if exists (window 1 of process \"zoom.us\") then\n"
                            "		set zoomStatus to \"open\"\n"
                            "		tell application process \"zoom.us\"\n"
                            "			if exists (menu bar item \"Meeting\" of menu bar 1) then\n"
                            "				set zoomStatus to \"call\"\n"
                            "				if exists (menu item \"Mute audio\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set muteStatus to \"unmuted\"\n"
                            "				else\n"
                            "					set muteStatus to \"muted\"\n"
                            "				end if\n"
                            "				if exists (menu item \"Start Video\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set videoStatus to \"stopped\"\n"
                            "				else\n"
                            "					set videoStatus to \"started\"\n"
                            "				end if\n"
                            "				if exists (menu item \"Speaker View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set viewStatus to \"gallery\"\n"
                            "				else if exists (menu item \"Gallery View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set viewStatus to \"speaker\"\n"
                            "				end if\n"
                            "				if exists (menu item \"Start Share\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set shareStatus to \"stopped\"\n"
                            "				else\n"
                            "					set shareStatus to \"started\"\n"
                            "				end if\n"
                            "				if exists (menu item \"Record to the Cloud\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set recordStatus to \"stopped\"\n"
                            "				else if exists (menu item \"Record\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                            "					set recordStatus to \"stopped\"\n"
                            "				else\n"
                            "					set recordStatus to \"started\"\n"
                            "				end if\n"
                            "			end if\n"
                            "		end tell\n"
                            "	end if\n"
                            "end tell\n"
                            "do shell script \"echo zoomMute:\" & (muteStatus as text) & \",zoomView:\" & (viewStatus as text) & \",zoomVideo:\" & (videoStatus as text) & \",zoomStatus:\" & (zoomStatus as text) & \",zoomShare:\" & (shareStatus as text) & \",zoomRecord:\" & (recordStatus as text)";

  std::string cmd = "osascript -e '";
  cmd.append(appleScript);
  cmd.append("'");
  char *zoomStatus = execAndReturn(cmd.c_str());

  return std::string(zoomStatus);
}

void osToggleZoomMute()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Unmute Audio\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Unmute Audio\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else\n"
                       "      click (menu item \"Mute Audio\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}

void osToggleZoomShare()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Start Share\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Start Share\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else\n"
                       "      click (menu item \"Stop Share\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}

void osToggleZoomView()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Speaker View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Speaker View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else if exists (menu item \"Gallery View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Gallery View\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell\n";
  system(script);
}

void osToggleZoomVideo()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Start Video\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Start Video\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else\n"
                       "      click (menu item \"Stop Video\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}

void osLeaveZoomMeeting()
{
  const char *script = "osascript -e '"
                       "tell application \"System Events\"\n"
                       " if exists (menu bar item \"Meeting\" of menu bar 1 of application process \"zoom.us\") then\n"
                       "   tell application \"zoom.us\" to activate\n"
                       "   keystroke \"w\" using {command down}\n"
                       "   tell front window of (first application process whose frontmost is true)\n"
                       "     click button 1\n"
                       "   end tell\n"
                       " end if\n"
                       "end tell'";
  system(script);
}

void osFocusZoomWindow()
{
  const char *script = "osascript -e 'tell application \"zoom.us\"\nactivate\nend tell'";
  system(script);
}

void osToggleZoomRecordCloud()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Record to the Cloud\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Record to the Cloud\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else if exists (menu item \"Record\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Record\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else\n"
                       "      click (menu item \"Stop Recording\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}

void osToggleZoomRecordLocal()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Record on this Computer\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Record on this Computer\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else if exists (menu item \"Record\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Record\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    else\n"
                       "      click (menu item \"Stop Recording\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}

void osMuteAll()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Mute All\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Mute All\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "      activate\n"
                       "      set frontmost to true\n"
                       "      delay 0.5\n"
                       "      keystroke return\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}
void osUnmuteAll()
{
  const char *script = "osascript -e '"
                       "tell application \"zoom.us\"\n"
                       "  tell application \"System Events\" to tell application process \"zoom.us\"\n"
                       "    if exists (menu item \"Ask All To Unmute\" of menu 1 of menu bar item \"Meeting\" of menu bar 1) then\n"
                       "      click (menu item \"Ask All To Unmute\" of menu 1 of menu bar item \"Meeting\" of menu bar 1)\n"
                       "    end if\n"
                       "  end tell\n"
                       "end tell'\n";
  system(script);
}
{
  "Actions": [
    {
      "States": [
        {
          "Image": "streamdeck-zoom-muted"
        },
        {
          "Image": "streamdeck-zoom-unmuted"
        },
        {
          "Image": "streamdeck-zoom-muted-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-muted-actionicon",
      "Name": "Mute Toggle",
      "Tooltip": "Toggle Zoom Mute Status",
      "UUID": "com.lostdomain.zoom.mutetoggle"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-video-stopped"
        },
        {
          "Image": "streamdeck-zoom-video-started"
        },
        {
          "Image": "streamdeck-zoom-video-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-video-actionicon",
      "Name": "Video Toggle",
      "Tooltip": "Toggle Zoom Video",
      "UUID": "com.lostdomain.zoom.videotoggle"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-view-speaker"
        },
        {
          "Image": "streamdeck-zoom-view-gallery"
        },
        {
          "Image": "streamdeck-zoom-view-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-view-gallery",
      "Name": "View Toggle",
      "Tooltip": "Toggle Zoom Gallery/Speaker View",
      "UUID": "com.lostdomain.zoom.viewtoggle"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-share-start"
        },
        {
          "Image": "streamdeck-zoom-share-stop"
        },
        {
          "Image": "streamdeck-zoom-share-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-share-actionicon",
      "Name": "Share Toggle",
      "Tooltip": "Bring up the share screen window, or stop sharing",
      "UUID": "com.lostdomain.zoom.sharetoggle"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-focus"
        },
        {
          "Image": "streamdeck-zoom-focus-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-focus-actionicon",
      "Name": "Focus",
      "Tooltip": "Bring the Zoom window to the front",
      "UUID": "com.lostdomain.zoom.focus"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-leave"
        },
        {
          "Image": "streamdeck-zoom-leave-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-leave-actionicon",
      "Name": "Leave Meeting",
      "Tooltip": "Leave an active meeting. If you're the host, this ends the meeting.",
      "UUID": "com.lostdomain.zoom.leave"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-record-cloud-stopped"
        },
        {
          "Image": "streamdeck-zoom-record-cloud-started"
        },
        {
          "Image": "streamdeck-zoom-record-cloud-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-record-cloud-actionicon",
      "Name": "Cloud Record Toggle",
      "Tooltip": "Toggle Zoom Recording to the Cloud",
      "UUID": "com.lostdomain.zoom.recordcloudtoggle"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-record-local-stopped"
        },
        {
          "Image": "streamdeck-zoom-record-local-started"
        },
        {
          "Image": "streamdeck-zoom-record-local-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-record-local-actionicon",
      "Name": "Local Record Toggle",
      "Tooltip": "Toggle Zoom Recording to your local computer",
      "UUID": "com.lostdomain.zoom.recordlocaltoggle"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-unmuteall"
        },
        {
          "Image": "streamdeck-zoom-unmuteall-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-unmuteall-actionicon",
      "Name": "Ask All to Unmute",
      "Tooltip": "Ask all participants to unmute",
      "UUID": "com.lostdomain.zoom.unmuteall"
    },
    {
      "States": [
        {
          "Image": "streamdeck-zoom-muteall"
        },
        {
          "Image": "streamdeck-zoom-muteall-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-zoom-muteall-actionicon",
      "Name": "Mute All Participants",
      "Tooltip": "Mute All Participants",
      "UUID": "com.lostdomain.zoom.muteall"
    }
  ],
  "CodePath": "sdzoomplugin.exe",
  "CodePathMac": "sdzoomplugin",
  "Author": "Martijn Smit",
  "Description": "Control your Zoom meetings; easily mute yourself, start your video, share, record, quickly leave the meeting, and more.",
  "URL": "https://lostdomain.org/stream-deck-plugin-for-zoom/",
  "Name": "Zoom Plugin",
  "Category": "Zoom",
  "CategoryIcon": "video-camera",
  "PropertyInspectorPath": "propertyinspector/index.html",
  "Icon": "video-camera-plugin",
  "Version": "${CMAKE_PROJECT_VERSION}",
  "OS": [
    {
      "Platform": "mac",
      "MinimumVersion": "10.13"
    },
    {
      "Platform": "windows",
      "MinimumVersion" : "10"
    }
  ],
  "SDKVersion": 2,
  "Software": {
    "MinimumVersion": "4.1"
  }
}

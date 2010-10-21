using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AVRStudioSDK
{
    public static class EVENT
    {
        public enum AvrEventEnum
        {
            AVREVENT_DEBUGMESSAGE = 1,
            AVREVENT_TARGETRUN = 2,
            AVREVENT_TARGETSTOP = 3,
            AVREVENT_STARTPROGRESS = 4,
            AVREVENT_ENDPROGRESS = 5,
            AVREVENT_PARTCHANGED = 6,
            AVREVENT_NEW_TARGET = 7,
            AVREVENT_TARGET_REMOVED = 8,
            AVREVENT_GUIPLUGIN_STATUS_CHANGED = 9,
            AVREVENT_SETPROGRESS = 11,
            AVREVENT_SHUTDOWN = 12,
            AVREVENT_UPDATEVIEWS = 13,
            AVREVENT_ROT_OBJECT_DELETED = 16,
            AVREVENT_ROT_NEW_OBJECT = 17,
            AVREVENT_SETSTATUSMESSAGE = 25,
            AVREVENT_AUTOSTEPON = 27,
            AVREVENT_AUTOSTEPOFF = 28,
            AVREVENT_OBJECTFILELOADED = 39,
            AVREVENT_PROGMEMORYCHANGED = 40,
            AVREVENT_TARGETINITOK = 41,
            AVREVENT_TARGETINITFAILED = 42,
            AVREVENT_TARGETDISCONNECT = 47,
            AVREVENT_OBJECTFILE_READY = 51,
            AVREVENT_BUILDWINDOWCLICKED = 57,
            AVREVENT_REBUILDNEEDED = 58,
            AVREVENT_F1_HELP = 59,
            AVREVENT_SAVEFILE = 60,
            AVREVENT_APP_GETS_FOCUS = 61,       // ?
            AVREVENT_PROJECTOPENED = 101,
            AVREVENT_GUIPLUGIN_CLOSEPROJECT = 102,
            AVREVENT_DEBUGSTARTING = 103,
            AVREVENT_DEBUGSTOPPING = 104,
            AVREVENT_PROJECTSAVED = 105
        }
    }
}

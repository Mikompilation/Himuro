#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mc_disp.h"

#include "ingame/menu/item.h"
#include "mc/mc_main.h"

void mcDispMessage()
{
    int msg_no;

    msg_no = mc_ctrl.msg_no;

#ifdef BUILD_EU_VERSION
    if (mc_ctrl.msg_no > MC_MSG_END_OVERWRITE)
#else
    if (mc_ctrl.msg_no > MC_MSG_ADD_NOEMPTY)
#endif
    {
        return;
    }

    if (mc_ctrl.mode == 7)
    {
        PutStringYW(8, msg_no, 48, 136, 0x808080, 0x80, 0xf000, 0);
    }
    else
    {
        PutStringYW(8, msg_no, 48, 356, 0x808080, 0x80, 0xf000, 0);
    }
}

void mcSetMessage(int msg_no)
{
    mc_ctrl.msg_no = msg_no;
}

char mcCheckMsgType(int msg_no)
{
    switch(msg_no)
    {
    case MC_MSG_SEL_SLOT:
    case MC_MSG_SEL_SAVEFILE:
    case MC_MSG_SEL_MAKE_DIR:
    case MC_MSG_SEL_SAVE:
    case MC_MSG_END_SAVE:
#ifdef BUILD_EU_VERSION
#else
    case MC_MSG_ERR_SAVE:
#endif
    case MC_MSG_SEL_LOAD:
    case MC_MSG_END_LOAD:
#ifdef BUILD_EU_VERSION
#else
    case MC_MSG_ERR_LOAD:
    case MC_MSG_ERR_SUM:
#endif
    case MC_MSG_SEL_ALBUM_TYPE:
    case MC_MSG_SEL_ALBUM_CHECK:
    case MC_MSG_SEL_OVERWRITE:
    case MC_MSG_SEL_NEW_OR_LOAD:
    case MC_MSG_SEL_LOADFILE:
#ifdef BUILD_EU_VERSION
    case MC_MSG_END_OVERWRITE:
#endif
        return 1;
    break;
    case MC_MSG_SEL_FORMAT:
    case MC_MSG_SEL_REMAKE_DIR:
#ifdef BUILD_EU_VERSION
    case MC_MSG_ERR_SAVE:
    case MC_MSG_ERR_LOAD:
    case MC_MSG_ERR_SUM:
#endif
    case MC_MSG_SEL_NEW_OR_LOAD2:
    case MC_MSG_SEL_GOTITLE:
        return 2;
    break;
    default:
        return 0;
    break;
    }
}

char mcCheckDrawButton(int msg_no)
{
    switch (msg_no)
    {
    case MC_MSG_ERR_ACCESS:
    case MC_MSG_ERR_NOEMPTY_GAME:
    case MC_MSG_ERR_NOEMPTY_ALBUM:
    case MC_MSG_ERR_NOCARD:
    case MC_MSG_ERR_NODIR:
    case MC_MSG_ERR_NOGAMEFILE:
    case MC_MSG_ERR_NOALBUMFILE:
    case MC_MSG_ERR_UNFORMAT:
    case MC_MSG_ERR_FORMAT:
    case MC_MSG_END_SAVE:
    case MC_MSG_ERR_SAVE:
    case MC_MSG_END_LOAD:
    case MC_MSG_ERR_LOAD:
    case MC_MSG_ERR_SUM:
    case MC_MSG_ERR_ILLEGUL_LOAD:
    case MC_MSG_CAP_ALBUM_LOAD:
#ifdef BUILD_EU_VERSION
    case MC_MSG_END_OVERWRITE:
#endif
        return 1;
    break;
    default:
        return 0;
    break;
    }
}

#ifndef UPNET_SWITCHGUI_GLOBAL_HU
#define UPNET_SWITCHGUI_GLOBAL_HU

#include <QtCore/qglobal.h>

#if defined(UPNET_SWITCHGUI_LIBRARY)
#  define UPNET_SWITCHGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define UPNET_SWITCHGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UPNET_SWITCHGUI_GLOBAL_HU



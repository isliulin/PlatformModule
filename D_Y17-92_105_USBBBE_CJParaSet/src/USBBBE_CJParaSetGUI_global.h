#ifndef USBBBE_CJPARASETGUI_GLOBAL_H
#define USBBBE_CJPARASETGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(USBBBE_CJPARASETGUI_LIBRARY)
#  define USBBBE_CJPARASETGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define USBBBE_CJPARASETGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // USBBBE_CJPARASETGUI_GLOBAL_H



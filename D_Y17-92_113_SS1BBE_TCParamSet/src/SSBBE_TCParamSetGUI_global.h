#ifndef SSBBE_TCPARASETGUI_GLOBAL_H
#define SSBBE_TCPARASETGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SSBBE_TCPARASETGUI_LIBRARY)
#  define SSBBE_TCPARASETGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define SSBBE_TCPARASETGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SSBBE_TCPARASETGUI_GLOBAL_H



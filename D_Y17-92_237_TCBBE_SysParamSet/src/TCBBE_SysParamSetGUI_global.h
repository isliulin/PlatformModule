#ifndef SS2BBE_SYSPARAMSETGUI_GLOBAL_H
#define SS2BBE_SYSPARAMSETGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SS2BBE_SYSPARAMSETGUI_LIBRARY)
#  define SS2BBE_SYSPARAMSETGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define SS2BBE_SYSPARAMSETGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SSBBE_SYSPARAMSETGUI_GLOBAL_H



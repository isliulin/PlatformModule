#ifndef RCLOCKDCUGUI_GLOBAL_H
#define RCLOCKDCUGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RCLOCKDCUGUI_LIBRARY)
#  define RCLOCKDCUGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define RCLOCKDCUGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RCLOCKDCUGUI_GLOBAL_H



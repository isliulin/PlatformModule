#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24 上午11:12
#
#-------------------------------------------------

QT       += network xml

QT       += gui
QT       += widgets


TARGET = USBBBE_ZKset
TEMPLATE = lib

DEFINES += USBBBE_ZKsetGUISHARED_LIBRARY

SOURCES += \
    USBBBE_ZKsetGUI.cpp \
    USBBBE_ZKsetGUIDlg.cpp \
    USBBBE_ZKsetGUIExport.cpp \
    ../../Common/DeviceMap.cpp \
    ../../Common/Log.cpp \
    checkparam.cpp

HEADERS += \
    USBBBE_ZKsetGUI.h \
    USBBBE_ZKsetGUI_global.h \
    USBBBE_ZKsetGUIDlg.h \
    ../../Common/DataStruct2311.h \
    ../../Common/Dcu_common_DataStruct.h \
    ../../Common/LxTsspCommonDefinition.h \
    ../../Common/MCSDataStruct.h \
    ../../Common/TypeDefine.h \
    ../../Common/DeviceMap.h \
    ../../Common/Log.h \
    ../../Common/MCSUSBDataStruct.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    USBBBE_ZKsetGUIDlg.ui

#-------------------------------------------------
#
# Project created by QtCreator 2017-02-17 上午11:12
#
#-------------------------------------------------

QT       += network xml
QT       += gui
QT       += widgets

TARGET = DDTBBE_SysParaSet
TEMPLATE = lib

DEFINES += DDTBBE_SYSPARASETGUI_LIBRARY

SOURCES += src/DDTBBE_SysParaSetGUI.cpp \
    src/DDTBBE_SysParaSetGUIExport.cpp \
    src/DDTBBE_SysParaSetGUIDlg.cpp \
    ../Common/DeviceMap.cpp \
    ../Common/Log.cpp \
    ../Common/CommonFunction.cpp

HEADERS +=  src/DDTBBE_SysParaSetGUI.h\
         src/DDTBBE_SysParaSet_global.h \
    src/DDTBBE_SysParaSetGUIDlg.h \
    ../Common/DataStruct2311.h \
    ../Common/Dcu_common_DataStruct.h \
    ../Common/LxTsspCommonDefinition.h \
    ../Common/MCSDataStruct.h \
    ../Common/TypeDefine.h \
    ../Common/DeviceMap.h \
    ../Common/Log.h \
    ../Common/SMCUHeadFile.h \
    ../Common/CommonFunction.h \

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    src/DDTBBE_SysParaSetGUIDlg.ui

MOC_DIR = DDTBBE_SysParaSet/moc
RCC_DIR = DDTBBE_SysParaSet/rcc
UI_DIR = DDTBBE_SysParaSet/ui
OBJECTS_DIR = DDTBBE_SysParaSet/obj
DESTDIR += ../../

#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24 上午11:12
#
#-------------------------------------------------

QT       += network xml
QT       += gui
QT       += widgets


TARGET = SSBBE_TMParaSet
TEMPLATE = lib

DEFINES += SSBBE_TMPARASETGUI_LIBRARY

SOURCES += \
    ../Common/DeviceMap.cpp \
    ../Common/Log.cpp \
    src/SSBBE_TMParaSetGUI.cpp \
    src/SSBBE_TMParaSetGUIDlg.cpp \
    src/SSBBE_TMParaSetGUIExport.cpp \
    src/checkParam.cpp \
    ../Common/CommonFunction.cpp

HEADERS += \
    ../Common/DataStruct2311.h \
    ../Common/Dcu_common_DataStruct.h \
    ../Common/LxTsspCommonDefinition.h \
    ../Common/MCSDataStruct.h \
    ../Common/TypeDefine.h \
    ../Common/DeviceMap.h \
    ../Common/Log.h \
    ../Common/SMCUHeadFile.h \
    ../Common/SMCU_BBE_CDMAStruct.h \
    src/SSBBE_TMParaSetGUI.h \
    src/SSBBE_TMParaSetGUIDlg.h \
    src/SSBBE_TMParaSetGUI_global.h \
    ../Common/CommonFunction.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    src/SSBBE_TMParaSetGUIDlg.ui

MOC_DIR = SSBBE_TMParaSet/moc
RCC_DIR = SSBBE_TMParaSet/rcc
UI_DIR = SSBBE_TMParaSet/ui
OBJECTS_DIR = SSBBE_TMParaSet/obj
DESTDIR += ../../

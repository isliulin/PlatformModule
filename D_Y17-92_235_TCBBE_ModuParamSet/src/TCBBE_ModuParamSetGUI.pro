#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24 上午11:12
#
#-------------------------------------------------

QT       += network xml
QT       += gui
QT       += widgets


TARGET = TCBBE_ModuParamSet
TEMPLATE = lib

DEFINES += SS2BBE_MODUPARAMSETGUISHARED_LIBRARY

SOURCES += \
    ../../Common/DeviceMap.cpp \
    ../../Common/Log.cpp \
    ../../Common/CommonFunction.cpp \
    TCBBE_ModuParamSetGUI.cpp \
    TCBBE_ModuParamSetGUIExport.cpp \
    TCBBE_ModuParamSetGUIDlg.cpp

HEADERS += \
    ../../Common/DataStruct2311.h \
    ../../Common/Dcu_common_DataStruct.h \
    ../../Common/LxTsspCommonDefinition.h \
    ../../Common/TypeDefine.h \
    ../../Common/DeviceMap.h \
    ../../Common/Log.h \
    ../../Common/SMCUHeadFile.h \
    ../../Common/SMCUSSB2_BBE_CDMAStruct.h \
    ../../Common/CommonFunction.h \
    TCBBE_ModuParamSetGUI.h \
    TCBBE_ModuParamSetGUIDlg.h \
    TCBBE_ModuParamSetGUI_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    TCBBE_ModuParamSetGUIDlg.ui

MOC_DIR = TCBBE_ModuParamSet/moc
RCC_DIR = TCBBE_ModuParamSet/rcc
UI_DIR = TCBBE_ModuParamSet/ui
OBJECTS_DIR = TCBBE_ModuParamSet/obj

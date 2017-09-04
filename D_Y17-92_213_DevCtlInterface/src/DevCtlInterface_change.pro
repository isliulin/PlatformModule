#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24 上午11:12
#
#-------------------------------------------------

QT       += network xml
QT       += gui
QT       += widgets
TARGET = DevCtlInterface
TEMPLATE = lib

DEFINES += CDevCtlInterfaceGUISHARED_LIBRARY

SOURCES += \
    Global.cpp \
    XmlReader.cpp \
    DevCtlInterfaceExport.cpp \
    DevCtlInterfaceGUIDlg.cpp \
    DevCtlInterfaceGUI.cpp \
    ProcessModule.cpp\
    ../Common/DeviceMap.cpp \
    ../Common/Log.cpp \
    SearchStatusThread.cpp \
    RecvDataThread.cpp \
    cshowdatagram.cpp \
    util.cpp \
    connectstatus.cpp \
    platformresource.cpp \
    cbaseband.cpp \
    cddt.cpp \
    CSS1.cpp \
    CSS2.cpp \
    CTTC.cpp \
    devices.cpp \
    CTC.cpp \
    alter.cpp \
    adjustalter.cpp \
    ../Common/CommonFunction.cpp \
    CRM.cpp

HEADERS += \
    LxTsspCommonDefinition.h \
    TypeDefine.h \
    MCSDataStruct.h \
    Global.h \
    XmlReader.h \
    DevCtlInterface_global.h \
    DevCtlInterfaceGUIDlg.h \
    DevCtlInterfaceGUI.h \
    DataStructTTCDevCtlInterface.h \
    ProcessModule.h \
    SMCUHeadFile.h \
    ../Common/DeviceMap.h \
    ../Common/TypeDefine.h \
    ../Common/SMCUHeadFile.h \
    ../Common/MCSDataStruct.h \
    ../Common/LxTsspCommonDefinition.h \
    ../Common/Dcu_common_DataStruct.h \
    ../Common/DataStruct2311.h \
    ../Common/Log.h \
    SearchStatusThread.h \
    ../Common/MCSUSBDataStruct.h \
    RecvDataThread.h \
    cshowdatagram.h \
    ../Common/SMCUSSB2_BBE_CDMAStruct.h \
    util.h \
    connectstatus.h \
    ../Common/ddtTTCCtrlResultReport.h \
    ../Common/ss1trlResultReport.h \
    ../Common/ss2trlResultReport.h \
    platformresource.h \
    cbaseband.h \
    cddt.h \
    CSS1.h \
    CSS2.h \
    CTTC.h \
    devices.h \
    CTC.h \
    alter.h \
    adjustalter.h \
    ../Common/struct_header/Dcu_2311_AK_DataStruct.h \
    ../Common/CommonFunction.h \
    ../Common/const.h \
    CRM.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    DevCtlInterfaceGUIDlg.ui

MOC_DIR = DevCtlInterface/moc
RCC_DIR = DevCtlInterface/rcc
UI_DIR = DevCtlInterface/ui
OBJECTS_DIR = DevCtlInterface/obj

DESTDIR += ../../
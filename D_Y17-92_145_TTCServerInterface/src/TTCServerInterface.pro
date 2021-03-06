#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24 上午11:12
#
#-------------------------------------------------

QT       += network xml
QT       += gui
QT       += widgets
TARGET = TTCServerInterface
TEMPLATE = lib

DEFINES += CDevCtlInterfaceGUISHARED_LIBRARY

SOURCES += \
    Global.cpp \
    XmlReader.cpp \
    ../../Common/DeviceMap.cpp \
    ../../Common/Log.cpp \
    cshowdatagram.cpp \
    util.cpp \
    connectstatus.cpp \
    cddt.cpp \
    CSS1.cpp \
    CSS2.cpp \
    CTTC.cpp \
    devices.cpp \
    CTC.cpp \
    alter.cpp \
    adjustalter.cpp \
    ../../Common/GroupSwitch.cpp \
    cdatasource.cpp \
    TMServerInterfaceExport.cpp \
    TMServerInterfaceGUI.cpp \
    TMServerInterfaceGUIDlg.cpp \
    RecvDataThread_tmSrvr.cpp \
    cbaseband_tmSrvr.cpp \
    ../../Common/CommonFunction.cpp \
    SearchStatusThread_tmSrvr.cpp \
    ProcessModule_tmSrvr.cpp \
    platformresource_tmSrvr.cpp \
    CRM.cpp

HEADERS += \
    LxTsspCommonDefinition.h \
    TypeDefine.h \
    MCSDataStruct.h \
    Global.h \
    XmlReader.h \
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
    ../Common/MCSUSBDataStruct.h \
    cshowdatagram.h \
    ../Common/SMCUSSB2_BBE_CDMAStruct.h \
    util.h \
    connectstatus.h \
    ../Common/ddtTTCCtrlResultReport.h \
    ../Common/ss1trlResultReport.h \
    ../Common/ss2trlResultReport.h \
    cddt.h \
    CSS1.h \
    CSS2.h \
    CTTC.h \
    devices.h \
    CTC.h \
    alter.h \
    adjustalter.h \
    ../../Common/DataStruct2311.h \
    ../../Common/Dcu_common_DataStruct.h \
    ../../Common/ddtTTCCtrlResultReport.h \
    ../../Common/DeviceMap.h \
    ../../Common/GroupSwitch.h \
    ../../Common/Log.h \
    ../../Common/LxTsspCommonDefinition.h \
    ../../Common/MCSDataStruct.h \
    ../../Common/MCSUSBDataStruct.h \
    ../../Common/SMCU_BBE_CDMAStruct.h \
    ../../Common/SMCUHeadFile.h \
    ../../Common/SMCUSSB2_BBE_CDMAStruct.h \
    ../../Common/ss1trlResultReport.h \
    ../../Common/ss2trlResultReport.h \
    ../../Common/TypeDefine.h \
    TMServerInterface_global.h \
    TMServerInterfaceGUI.h \
    TMServerInterfaceGUIDlg.h \
    DataStructTMServerInterface.h \
    RecvDataThread_tmSrvr.h \
    cbaseband_tmSrvr.h \
    ../../Common/CommonFunction.h \
    SearchStatusThread_tmSrvr.h \
    ../../Common/interfaces/cdatasource.h \
    ../../Common/const.h \
    platformresource_tmSrvr.h \
    CRM.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
  TMServerInterfaceGUIDlg.ui

MOC_DIR = TTCServerInterface/moc
RCC_DIR = TTCServerInterface/rcc
UI_DIR = TTCServerInterface/ui
OBJECTS_DIR = TTCServerInterface/obj

DISTFILES += \
    TMServerInterfaceGUI.png \
    TMServerInterface.pro.user

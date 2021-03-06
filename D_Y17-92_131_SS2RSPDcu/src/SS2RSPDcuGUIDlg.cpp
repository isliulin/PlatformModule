﻿#include "SS2RSPDcuGUIDlg.h"
#include "ui_SS2RSPDcuGUIDlg.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDateTime>
#include "../../Common/Log.h"
#include <QDebug>
#include <QTextCodec>

extern quint64 GetID(ushort usStationID, uchar ucSubSysID, ushort usDeviceID, ushort usType,
              uchar ucSn);

SS2RSPDcuGUIDlg::SS2RSPDcuGUIDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SS2RSPDcuGUIDlg)
{
    ui->setupUi(this);
    m_pStateMgr = NULL;
    m_pILxTsspMessageManager = NULL;
    m_pILxTsspLogManager = NULL;
    m_pPublicInfo = NULL;
    m_pHAManager = NULL;

    m_pComPara = NULL;
    m_pComStatus = NULL;
    m_pModel = NULL;

    m_bOnLine = false;

    setAttribute(Qt::WA_DeleteOnClose);
    //setMinimumSize(800, 600);
    EnabledControls(false);
}

SS2RSPDcuGUIDlg::~SS2RSPDcuGUIDlg()
{
    killTimer(m_TimerID);
    delete ui;
}

void SS2RSPDcuGUIDlg::setText(const QString& strTitle, ParaStruct xmlsetText[3][60], QString MapMean[50][30],
                            int MapNum[50], uchar Type, ILxTsspParameterMacro* pMacroObj)
{
    m_strTitle = strTitle;
    setWindowTitle(strTitle);

    memcpy(m_DlgParaSetStruct, xmlsetText, sizeof(ParaStruct)*50*3);

    for(int i=0; i<50; i++)
    {
        for(int j=0; j<30; j++)
        {
            m_DlgMapMean[i][j] = MapMean[i][j];
        }
    }

    memset(DlgMapLineNum, 0, 50);
    for(int i=0; i<49; i++)
    {
        DlgMapLineNum[i] = MapNum[i];
    }

    //获取要发送的C和TID
    initCmdInfo(MODULENAME);

    //初始化TabWidget表格控件的显示
    initTableView();

    //初始化界面
    InitDLG(Type, pMacroObj);
}

//初始化对话框
void SS2RSPDcuGUIDlg::InitDLG(int Type, ILxTsspParameterMacro* pMacroObj)
{
    //设置界面的灯的默认颜色
    setLight(ui->label_DeviceLight, Light_empty);
    //
    ui->label_SaveCtrlCMD->hide();
    ui->comboBox_SaveCtrlCMD->hide();
    ui->pushButton_SaveCtrlCMD->hide();
    iParaSetFlag = Type;
    //设置输入格式
    InitInput();
    //显示变量名字和范围
    ShowControls();

    if(Type == 0)//取当前参数
    {
        //取当前设置的参数
       /* QByteArray byteArray;
        if(m_pHAManager != NULL)
        {
            m_pHAManager->GetParameter(m_ParaDDTBBECodeSynID,byteArray);
            memcpy(&m_DDTBBECodeSynPara,byteArray.data(),sizeof(m_DDTBBECodeSynPara));

            m_pHAManager->GetParameter(m_ParaDDTBBEFrameSynID,byteArray);
            memcpy(&m_DDTBBEFrameSynPara,byteArray.data(),sizeof(m_DDTBBEFrameSynPara));

            m_pHAManager->GetParameter(m_ParaDDTBBESubFrameSynID,byteArray);
            memcpy(&m_DDTBBESubFrameSynPara,byteArray.data(),sizeof(m_DDTBBESubFrameSynPara));
        }
      */
        if(m_pStateMgr != NULL)
        {
            //读取参数宏查询变量的数据
            DWORD stLen = sizeof(COMBTRANSPara);
            m_pComPara = (COMBTRANSPara*)m_pStateMgr->FindOneItem(m_ParaMarcoComSynID, stLen);
            if(m_pComPara == NULL || stLen != sizeof(COMBTRANSPara))
                return;

            //读取状态变量的的共享内存数据
            stLen = sizeof(COMBTRANSStatu);
            m_pComStatus = (COMBTRANSStatu*)m_pStateMgr->FindOneItem(m_StatusComSynID, stLen);
            if(m_pComStatus == NULL || stLen != sizeof(COMBTRANSStatu))
                return;
        }
        //启动定时器
        m_TimerID = startTimer(500);
        //不使能
        EnabledControls(false);
    }
    else if(Type == 1)//取宏里参数
    {
        if(pMacroObj != NULL)
        {
            QByteArray byteArray;
            pMacroObj->GetParameterBlock(m_ParaComSynID, byteArray);
            //memcpy(&m_ComPara, byteArray.data(), sizeof(RCLOCKDCUStatu));
            //切换控制命令
            ui->comboBox_CtrlCmd->setCurrentIndex(byteArray.at(0));
            //存盘控制命令
            ui->comboBox_SaveCtrlCMD->setCurrentIndex(byteArray.at(1));
            //模式切换命令
            ui->comboBox_ModuleCMD->setCurrentIndex(byteArray.at(2));
            //仿真控制命令
            ui->comboBox_FZCtrlCMD->setCurrentIndex(byteArray.at(3));
        }
        ui->pushButton_Modify->setVisible(false);
        ui->pushButton_Set->setVisible(false);

        //显示参数
        ShowPara(1);
    }
}

void SS2RSPDcuGUIDlg::initTableView()
{
    m_pModel = new QStandardItemModel();
    m_pModel->setColumnCount(4);
    m_pModel->setHeaderData(0, Qt::Horizontal, QString("参数宏号"));
    m_pModel->setHeaderData(1, Qt::Horizontal, QString("任务代号"));
    m_pModel->setHeaderData(2, Qt::Horizontal, QString("任务标识"));
    m_pModel->setHeaderData(3, Qt::Horizontal, QString("任务描述"));
    ui->tableView->setModel(m_pModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableView->setColumnHidden(Column_ID, true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);
    //表头信息显示居左
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //设置列宽不可变
    //ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    //ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0, 100);
    //ui->tableView->setColumnWidth(1, 120);
    //ui->tableView->setColumnWidth(2, 120);
    //隐藏中间两列
    ui->tableView->setColumnWidth(1, 0);
    ui->tableView->setColumnWidth(2, 0);
    //
    ui->tableView->setColumnWidth(3, 150);

#if 0
    //测试tableView代码
    for (int i = 0; i < 2; i++)
    {
        //参数宏号
        m_pModel->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
        //设置字符颜色
        //m_pModel->item(i, 0)->setForeground(QBrush(QColor(255, 0, 0)));
        //设置字符位置
        //m_pModel->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        //任务代号
        m_pModel->setItem(i, 1, new QStandardItem(QString("12345")));
        //任务标识
        m_pModel->setItem(i, 2, new QStandardItem(QString("111/222")));
        //任务描述
        m_pModel->setItem(i, 3, new QStandardItem(QString("1111")));
    }
#endif // 0
}

//设置输入格式
void SS2RSPDcuGUIDlg::InitInput()
{
    QRegExp regExp("[A-Fa-f0-9]{1,8}");
    //帧同步码组
//    ui->lineEdit_FrmSyncCode->setValidator(new QRegExpValidator(regExp, this));

//    //正整数
    QRegExp regExp2("[0-9]{1,16}");
    //输入射频频率
    //ui->lineEdit_InputFreq->setValidator(new QRegExpValidator(regExp2, this));
}

void SS2RSPDcuGUIDlg::ShowControls()
{
    //向下拉框控件中写入从xml中读取的内容
    //切换控制命令
    for (int i=0; i<DlgMapLineNum[0]; i++)
    {
        ui->comboBox_CtrlCmd->addItem(m_DlgMapMean[0][i]);
    }
    //模式切换命令
    for (int i=0; i<DlgMapLineNum[1]; i++)
    {
        ui->comboBox_ModuleCMD->addItem(m_DlgMapMean[1][i]);
    }
    //存盘控制命令
    for (int i=0; i<DlgMapLineNum[2]; i++)
    {
        ui->comboBox_SaveCtrlCMD->addItem(m_DlgMapMean[2][i]);
    }
    //仿真控制命令
    for (int i=0; i<DlgMapLineNum[3]; i++)
    {
        ui->comboBox_FZCtrlCMD->addItem(m_DlgMapMean[3][i]);
    }
    //载波输出状态
    for (int i=0; i<DlgMapLineNum[4]; i++)
    {
        //ui->comboBox_WaveInputStatus->addItem(m_DlgMapMean[4][i]);
    }
    //载波加调状态
    for (int i=0; i<DlgMapLineNum[5]; i++)
    {
        //ui->comboBox_WaveLoadStatus->addItem(m_DlgMapMean[5][i]);
    }
    //目标仿真类型
    for (int i=0; i<DlgMapLineNum[6]; i++)
    {
        //ui->comboBox_MBFZType->addItem(m_DlgMapMean[6][i]);
        //ui->comboBox_MBFZType_2->addItem(m_DlgMapMean[6][i]);
        //ui->comboBox_MBFZType_3->addItem(m_DlgMapMean[6][i]);
        //ui->comboBox_MBFZType_4->addItem(m_DlgMapMean[6][i]);
    }
    //目标仿真状态
    for (int i=0; i<DlgMapLineNum[7]; i++)
    {
        //ui->comboBox_MBFZStatus->addItem(m_DlgMapMean[7][i]);
        //ui->comboBox_MBFZStatus_2->addItem(m_DlgMapMean[7][i]);
        //ui->comboBox_MBFZStatus_3->addItem(m_DlgMapMean[7][i]);
        //ui->comboBox_MBFZStatus_4->addItem(m_DlgMapMean[7][i]);
    }
    //已仿真数据帧数
    //ui->lineEdit_FZDataFrame->setText();

    //设置显示状态控件为只读
    //载波输出状态
    ui->lineEdit_WaveInputStatus->setReadOnly(true);
    //载波加调状态
    ui->lineEdit_WaveLoadStatus->setReadOnly(true);
    //目标仿真类型
    ui->lineEdit_MBFZType->setReadOnly(true);
    ui->lineEdit_MBFZType_2->setReadOnly(true);
    ui->lineEdit_MBFZType_3->setReadOnly(true);
    ui->lineEdit_MBFZType_4->setReadOnly(true);
    //目标仿真状态
    ui->lineEdit_MBFZStatus->setReadOnly(true);
    ui->lineEdit_MBFZStatus_2->setReadOnly(true);
    ui->lineEdit_MBFZStatus_3->setReadOnly(true);
    ui->lineEdit_MBFZStatus_4->setReadOnly(true);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame->setReadOnly(true);
    ui->lineEdit_FZDataFrame_2->setReadOnly(true);
    ui->lineEdit_FZDataFrame_3->setReadOnly(true);
    ui->lineEdit_FZDataFrame_4->setReadOnly(true);
}

//显示参数
void SS2RSPDcuGUIDlg::ShowPara(int type)
{
    if(type == 0)       //参数设置
    {
        if(m_pComStatus != NULL)
        {
            //m_ComPara = m_pComStatus->tParam;
            //memcpy(&m_ComPara, m_pComStatus, sizeof(RCLOCKDCUStatu));
        }
        else
        {
            DWORD stLen = sizeof(COMBTRANSStatu);
            if(m_pStateMgr != NULL)
                m_pComStatus  = (COMBTRANSStatu*)m_pStateMgr->FindOneItem(m_StatusComSynID, stLen);

            stLen = sizeof(COMBTRANSPara);
            if(m_pStateMgr != NULL)
                m_pComPara  = (COMBTRANSPara*)m_pStateMgr->FindOneItem(m_ParaMarcoComSynID, stLen);
        }
    }
    else
    {    
        if (ui->pushButton_Modify->text() != "更改")
        {
            //EnabledControlsSubFrmType(ui->comboBox_SubFrmType->currentIndex());
        }
    }
    /********************显示后面灯的状态***********************/
    if (m_pComStatus == NULL)
        return;

    /********************设备是否在线************************/
    m_bOnLine = CCommFunc::getInstance()->getSS2RSPStatus();
    if (!m_bOnLine)
    {
        setOfflineStatusValue();
        return;
    }
    /********************设备是否在线************************/

    setLight(ui->label_DeviceLight, (m_pComStatus->usDevStat == 1 ? Light_green : Light_red));
    /********************显示后面灯的状态***********************/
    //载波输出状态
    ui->lineEdit_WaveInputStatus->setText(m_DlgMapMean[4][m_pComStatus->ucCarrOn]);
    //载波加调状态
    ui->lineEdit_WaveLoadStatus->setText(m_DlgMapMean[5][m_pComStatus->ucModuOn]);

    //区分目标进行显示
    //目标仿真类型
    ui->lineEdit_MBFZType->setText(m_DlgMapMean[6][m_pComStatus->ucSimuMode[0]]);
    //目标仿真状态
    if(0xFF == m_pComStatus->ucSimuOn[0])
        ui->lineEdit_MBFZStatus->setText("初态");
    else ui->lineEdit_MBFZStatus->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[0]]);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame->setText(QString::number(m_pComStatus->dwlFrameNum[0]));

    //目标仿真类型
    ui->lineEdit_MBFZType_2->setText(m_DlgMapMean[6][m_pComStatus->ucSimuMode[1]]);
    //目标仿真状态
    if(0xFF == m_pComStatus->ucSimuOn[0])
        ui->lineEdit_MBFZStatus_2->setText("初态");
    else ui->lineEdit_MBFZStatus_2->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[1]]);
    //ui->lineEdit_MBFZStatus_2->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[1]]);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame_2->setText(QString::number(m_pComStatus->dwlFrameNum[1]));

    //目标仿真类型
    ui->lineEdit_MBFZType_3->setText(m_DlgMapMean[6][m_pComStatus->ucSimuMode[2]]);
    //目标仿真状态
    if(0xFF == m_pComStatus->ucSimuOn[0])
        ui->lineEdit_MBFZStatus_3->setText("初态");
    else ui->lineEdit_MBFZStatus_3->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[2]]);
    //ui->lineEdit_MBFZStatus_3->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[2]]);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame_3->setText(QString::number(m_pComStatus->dwlFrameNum[2]));

    //目标仿真类型
    ui->lineEdit_MBFZType_4->setText(m_DlgMapMean[6][m_pComStatus->ucSimuMode[3]]);
    //目标仿真状态
    if(0xFF == m_pComStatus->ucSimuOn[0])
        ui->lineEdit_MBFZStatus_4->setText("初态");
    else ui->lineEdit_MBFZStatus_4->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[3]]);
    //ui->lineEdit_MBFZStatus_4->setText(m_DlgMapMean[7][m_pComStatus->ucSimuOn[3]]);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame_4->setText(QString::number(m_pComStatus->dwlFrameNum[3]));
}

void SS2RSPDcuGUIDlg::SetObjInfo(TLxTsspObjectInfo ObjectInfo)
{
    memcpy(&m_ObjectInfo, &ObjectInfo, sizeof(TLxTsspObjectInfo));
    //对象初始化
    //查找对象管理器
    LxTsspObjectManager* pObjectManager = LxTsspObjectManager::Instance();
    if(pObjectManager == NULL)
    {
        return;
    }
    m_pHAManager = pObjectManager->GetHAManager();

    bool bRet = m_DeviceMap.loadXML();
    if (!bRet)
    {
        QString strInfo = QString("load device map failed %1").arg(MODULENAME);
        CLog::addLog(strInfo);
        return;
    }

    ITEM *pItem = m_DeviceMap.getItem(MODULENAME);
    if (pItem != NULL && pItem->vSubItem.size() > 0)
    {
        //第1个作为接收设备工作状态使用
        SUBITEM sItem = pItem->vSubItem.at(0);
        /*************************框架**********************************/
        m_ParaComSynID = GetID(sItem.usStationID, sItem.ucSubSysID, sItem.usDeviceID,
                                     sItem.usTID, sItem.ucSN);
        /*************************框架**********************************/

        /*************************工作状态**********************************/
        m_StatusComSynID = GetID(sItem.usStationID, sItem.ucSubSysID, sItem.usDeviceID,
                                       sItem.usStateType, sItem.ucSN);
        /*************************工作状态状态**********************************/
    }

    //获取宏查询xml中的配置
    ITEM *pItemSearch = m_DeviceMap.getItem(RSP_MACRO_SEARCH);
    if (pItemSearch != NULL && pItemSearch->vSubItem.size() > 0)
    {
        //第2个作为接收宏查询返回的共享内存使用
        SUBITEM sItem = pItemSearch->vSubItem.at(1);
        m_usCMarcoSearch = sItem.usCMD;
        m_usTIDMarcoSearch = sItem.usTID;
        /*************************宏参数使用*****************************/
        m_ParaMarcoComSynID = GetID(sItem.usStationID, sItem.ucSubSysID, sItem.usDeviceID,
                                       sItem.usStateType, sItem.ucSN);
        /*************************宏参数使用*****************************/
    }
    //获取宏调用xml中的配置
    ITEM *pItemCall = m_DeviceMap.getItem(RSP_MACRO_CALL);
    if (pItemCall != NULL && pItemCall->vSubItem.size() > 0)
    {
        //第2个作为接收宏查询返回的共享内存使用
        SUBITEM sItem = pItemCall->vSubItem.at(1);
        m_usCMarcoCall = sItem.usCMD;
        m_usTIDMarcoCall = sItem.usTID;
    }

    //日志
    m_pILxTsspLogManager = pObjectManager->GetLogManager();
    m_pStateMgr = pObjectManager->GetStateManager();
    m_pILxTsspMessageManager = pObjectManager->GetMessageManager();
    m_pPublicInfo = pObjectManager->GetPublicInfoManager();
}

//使能或不使能控件
void SS2RSPDcuGUIDlg::EnabledControls(bool bFlag)
{
    //宏查询按钮
    ui->pushButton_MacroSearch->setEnabled(bFlag);
    //宏调用按钮
    ui->pushButton_MacroCall->setEnabled(bFlag);
    //切换控制命令
    ui->comboBox_CtrlCmd->setEnabled(bFlag);
    ui->pushButton_CtrlCMD->setEnabled(bFlag);
    //模式切换命令
    ui->comboBox_ModuleCMD->setEnabled(bFlag);
    ui->pushButton_ModuleCMD->setEnabled(bFlag);
    //存盘控制命令
    ui->comboBox_SaveCtrlCMD->setEnabled(bFlag);
    ui->pushButton_SaveCtrlCMD->setEnabled(bFlag);
    //仿真控制命令
    ui->comboBox_FZCtrlCMD->setEnabled(bFlag);
    ui->pushButton_FZCtrlCMD->setEnabled(bFlag);

    //将状态界面显示为不可用

    //时间设置按钮不可用 暂时不用设置
    //界面上以前预留的设置按钮不可用
    ui->pushButton_Set->hide();
}

int SS2RSPDcuGUIDlg::SaveToMacro(ILxTsspParameterMacro* pMacroObj)
{
    //切换控制命令
    UCHAR ucVal1 = ui->comboBox_CtrlCmd->currentIndex();
    //存盘控制命令
    UCHAR ucVal2 = ui->comboBox_SaveCtrlCMD->currentIndex();
    //模式切换命令
    UCHAR ucVal3 = ui->comboBox_ModuleCMD->currentIndex();
    //仿真控制命令
    UCHAR ucVal4 = ui->comboBox_FZCtrlCMD->currentIndex();

    //取当前设置的参数
    QByteArray byteArray;
    byteArray.resize(4);
    byteArray.append(ucVal1);
    byteArray.append(ucVal2);
    byteArray.append(ucVal3);
    byteArray.append(ucVal4);
    pMacroObj->UpdateParameterBlock(m_ParaComSynID, byteArray);

    //参数
    //byteArray.resize(sizeof(m_ComPara));
    //memcpy(byteArray.data(), &m_ComPara, sizeof(m_ComPara));
    //pMacroObj->UpdateParameterBlock(m_ParaComSynID, byteArray);

    //日志信息
    QString strLog = QString(tr("%1: 保存宏成功！")).arg(MODULENAME);
    CLog::addLog(strLog);
}

//定时器
void SS2RSPDcuGUIDlg::timerEvent(QTimerEvent *event)
{
    if (m_TimerID == event->timerId())
    {
        ShowPara(0);
    }

    if (m_MarcoSearchTimerID == event->timerId())
    {
        addData2TabView();
        killTimer(m_MarcoSearchTimerID);
    }
}

void SS2RSPDcuGUIDlg::on_pushButton_Modify_clicked()
{
    if (!m_bOnLine)
    {
        QString strMsg = QString(tr("%1处于离线状态，无法进行更改设置！")).arg(MODULENAME);
        QMessageBox::information(this, tr("信息提示"), strMsg);
        return;
    }

    if(ui->pushButton_Modify->text() == "更改")
    {
        EnabledControls(true);
        //killTimer(m_TimerID); //状态显示一直刷新将其注释
        ui->pushButton_Modify->setText("恢复");
    }
    else if(ui->pushButton_Modify->text() == "恢复")
    {
        EnabledControls(false);
        //m_TimerID = startTimer(1000);
        ui->pushButton_Modify->setText("更改");
    }
}

void SS2RSPDcuGUIDlg::on_pushButton_Set_clicked()
{
    //设置参数
    //取当前设置的参数
    if(m_pHAManager != NULL)
    {
        QByteArray byteArray;
        //参数
        //byteArray.resize(sizeof(m_ComPara));
        //memcpy(byteArray.data(), &m_ComPara, sizeof(m_ComPara));
        //m_pHAManager->SetParameter(m_ParaComSynID, byteArray);
        //SendMessage((char*)&m_ComPara, sizeof(RCLOCKDCUStatu), m_usC, m_usTID);
    }

    //日志信息
    QString strLog;
    strLog = QString("%1: 设置参数成功！").arg(MODULENAME);
    CLog::addLog(strLog);

    //不使能控件
    EnabledControls(false);
    //m_TimerID = startTimer(1000); //状态显示一直刷新将其注释
}

void SS2RSPDcuGUIDlg::closeEvent(QCloseEvent *event)
{
    event->accept();
}

QSize SS2RSPDcuGUIDlg::sizeHint() const
{
    return QSize(this->width(), this->height());
}

void SS2RSPDcuGUIDlg::SendMessage(char* pData, unsigned short  usLen, unsigned short usC, unsigned short usTID)
{
    static uint seq = 0;
    TLxTsspMessageHeader header = {0};
    TLxTsspSubMsgHeader subHeader = {0};
    header.uiSequence = seq++;
    header.uiHandle = 1;
    header.S = m_pPublicInfo->GetLocalDeviceID();
    header.L = sizeof(TLxTsspMessageHeader) +sizeof(TLxTsspSubMsgHeader) + usLen;
    header.D = m_pPublicInfo->GetLocalDeviceID();//0x09;
    header.T = 1;
    header.M = 1;
    header.P = 1;
    header.A = 0;
    header.R = 0;
    header.C = usC;

    subHeader.Len = usLen;
    subHeader.SID = m_pPublicInfo->GetLocalDeviceID();//m_DObjectInfo.usStationID;
    subHeader.DID = m_ObjectInfo.usLocalDID;
    subHeader.TID = usTID;
    subHeader.SN = m_ObjectInfo.ucSubSysID;
    subHeader.O = m_ObjectInfo.ucSN;
    TLxTsspMessage message;
    message.pData =
            new char[sizeof(TLxTsspMessageHeader) + sizeof(TLxTsspSubMsgHeader) + usLen];
    memcpy(message.pData, &header, sizeof(TLxTsspMessageHeader));
    memcpy(message.pData + sizeof(TLxTsspMessageHeader),
           &subHeader, sizeof(TLxTsspSubMsgHeader));
    memcpy(message.pData + sizeof(TLxTsspMessageHeader) + sizeof(TLxTsspSubMsgHeader),
           pData, usLen);
    message.iDataLen = sizeof(TLxTsspMessageHeader) + sizeof(TLxTsspSubMsgHeader) + usLen;

    int iRet = m_pILxTsspMessageManager->FireMessage(message);
    delete[] message.pData;
}

bool SS2RSPDcuGUIDlg::initCmdInfo(QString strModuleName)
{
    bool bRet = m_DeviceMap.loadXML();
    if (!bRet)
    {
        QString strInfo = QString("load device map failed %1").arg(MODULENAME);
        //qDebug() << strInfo;
        CLog::addLog(strInfo);
        return false;
    }
    ITEM* item = m_DeviceMap.getItem(strModuleName);
    if (item != NULL)
    {
        m_usC = item->vSubItem[0].usCMD;
        m_usTID = item->vSubItem[0].usTID;
    }
    ITEM* itemCmd = m_DeviceMap.getItem(RSPDCU_CMD);
    if (itemCmd != NULL)
    {
        m_vSubItem = itemCmd->vSubItem;
    }
    return true;
}

DWORDLONG SS2RSPDcuGUIDlg::StrToSynCode(const char * pStr)
{
    char c, C;
    const char*  p;
    DWORDLONG dwlValue = 0;
    char  DigVal = 0;
    USHORT nDigCount = 0, MAX_DIGITS = 16; //The synchronizing code should be not longer  than 16 digitals

    p = pStr;     //Point to the first char of the string
    c = *p++;   // read the first char
    while ((c != NULL) && (nDigCount <= MAX_DIGITS))//  Not the terminator of a string && total digitals is less than 16
    {
         if(isspace((int)(unsigned char)c))    c = *p++;// skip whitespace
         else if(c >= '0' && c <= '9')                        //the char is a number between 1 and 9
         {
             DigVal = c - '0';
             dwlValue = dwlValue*16 + DigVal;               //Accumulate the value of the hexal number so far to the current digital
             nDigCount++;
             c = *p++;
         }
         else if((C=toupper(c)) >= 'A' && C <= 'F')           //the char is a letter could be a digital of a hexal number
         {
             DigVal = C-'A'+10;
             dwlValue = dwlValue*16 + DigVal;               //Accumulate the value of the hexal number so far to the current digital
             nDigCount++;
             c = *p++;
         }
         else    break;                                 // An invalid char that is neither whitespace nor a hexal digital is encounted
                                                        // Conclude the scanning process
    }
    return dwlValue;
}

void SS2RSPDcuGUIDlg::sendCMD(QByteArray ba, QString strInfo)
{
    SendMessage(ba.data(), ba.size(), m_usC, m_usTID);
    //日志信息
    QString strLog;
    strLog = QString("%1: 设置%2成功！").arg(MODULENAME).arg(strInfo);
    CLog::addLog(strLog);

    //不使能控件
    EnabledControls(false);
    //m_TimerID = startTimer(500);
}

void SS2RSPDcuGUIDlg::setLight(QLabel *pLabel, int nState)
{
    QPixmap light;
    bool bRet = false;
    switch(nState)
    {
    case Light_empty:
        bRet = light.load(":/empty.png");
        break;
    case Light_green:
        bRet = light.load(QString(":/safe.png"));
        break;
    case Light_red:
        bRet = light.load(QString(":/denter.png"));
        break;
    default:
        bRet = light.load(":/empty.png");
        break;
    }
    pLabel->setPixmap(light);
    //pLabel->resize(24, 24);
}

void SS2RSPDcuGUIDlg::addData2TabView()
{
    qDebug() << "宏查询应答标志：" << m_pComPara->ucAnswerFlag;
    if (m_pComPara == NULL || m_pComPara->ucAnswerFlag != 0xFF)
    {
        return;
    }
    int nRowCount = m_pComPara->nParaCount;
    for (int i = 0; i < nRowCount; i++)
    {
        //参数宏号
        m_pModel->setItem(i, 0, new QStandardItem(QString::number(m_pComPara->paras[i].usMarcoNo, 16)));
        //设置字符颜色
        //m_pModel->item(i, 0)->setForeground(QBrush(QColor(255, 0, 0)));
        //设置字符位置
        //m_pModel->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        //任务代号
        //m_pModel->setItem(i, 1, new QStandardItem(QString(m_pComPara->paras[i].cTaskID)));
        //任务标识
        //m_pModel->setItem(i, 2, new QStandardItem(QString::number(m_pComPara->paras[i].uiM)));
        //任务描述
        char szBuf[100] = { 0 };
        memcpy(szBuf, m_pComPara->paras[i].cMarcoName, strlen(m_pComPara->paras[i].cMarcoName));
        QTextCodec *codec = QTextCodec::codecForName("GB2312");
        QString strDesc = codec->toUnicode(szBuf, strlen(szBuf));
        m_pModel->setItem(i, 3, new QStandardItem(strDesc));
    }
    killTimer(m_MarcoSearchTimerID);
}

void SS2RSPDcuGUIDlg::setOfflineStatusValue()
{
    setLight(ui->label_DeviceLight, 0);
    /********************显示后面灯的状态***********************/
    QString strEmpty("--");
    //载波输出状态
    ui->lineEdit_WaveInputStatus->setText(strEmpty);
    //载波加调状态
    ui->lineEdit_WaveLoadStatus->setText(strEmpty);

    //目标仿真类型
    ui->lineEdit_MBFZType->setText(strEmpty);
    //目标仿真状态
    ui->lineEdit_MBFZStatus->setText(strEmpty);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame->setText(strEmpty);

    //目标仿真类型
    ui->lineEdit_MBFZType_2->setText(strEmpty);
    //目标仿真状态
    ui->lineEdit_MBFZStatus_2->setText(strEmpty);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame_2->setText(strEmpty);

    //目标仿真类型
    ui->lineEdit_MBFZType_3->setText(strEmpty);
    //目标仿真状态
    ui->lineEdit_MBFZStatus_3->setText(strEmpty);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame_3->setText(strEmpty);

    //目标仿真类型
    ui->lineEdit_MBFZType_4->setText(strEmpty);
    //目标仿真状态
    ui->lineEdit_MBFZStatus_4->setText(strEmpty);
    //已仿真数据帧数
    ui->lineEdit_FZDataFrame_4->setText(strEmpty);
}

//宏查询按钮
void SS2RSPDcuGUIDlg::on_pushButton_MacroSearch_clicked()
{
    WORD wMarcoNo = 0;      //宏代号
    WORD wReserved = 0;      //保留
    //设置参数
    QByteArray byteArray;
    byteArray.append(wMarcoNo);
    byteArray.append(wReserved);
    unsigned short usC = 0x200;
    unsigned short usT = 0x0106;

    usC = m_usCMarcoSearch;
    usT = m_usTIDMarcoSearch;

    SendMessage(byteArray.data(), byteArray.size(), usC, usT);

    m_MarcoSearchTimerID = startTimer(1000);
}

//宏调用按钮
void SS2RSPDcuGUIDlg::on_pushButton_MacroCall_clicked()
{
    //获取TableWidget选中的一行数据
    int nCurRow = ui->tableView->currentIndex().row();
    if (nCurRow < 0)
    {
        QMessageBox::information(this, ("信息提示"), ("请选择一条需要调用的参数宏！"));
        return;
    }
    QAbstractItemModel  *pModel = ui->tableView->model();
    Q_ASSERT (pModel != NULL);
    QModelIndex indexTmp = pModel->index(nCurRow, 0);
    QVariant data = pModel->data(indexTmp);
    qDebug() << data.toString();
    //将选择的宏调用发送出去
    WORD wMarcoNo = data.toString().toShort();
    WORD wReserved = 0;         //预留信息
    QByteArray ba;
    ba.append(wMarcoNo);
    ba.append(wReserved);
    unsigned short usC = 0x200;
    unsigned short usT = 0x0206;

    usC = m_usCMarcoCall;
    usT = m_usTIDMarcoCall;

    SendMessage(ba.data(), ba.size(), usC, usT);
}

//模式切换命令按钮
void SS2RSPDcuGUIDlg::on_pushButton_ModuleCMD_clicked()
{
    int nIndex = ui->comboBox_ModuleCMD->currentIndex();
    QByteArray ba;
    if (nIndex == 0)    //选择USB
        ba.append(0x0A);
    else if (nIndex == 1)   //选择扩频
        ba.append(0x03);
    unsigned short usC = 0x046E;
    unsigned short usT = 0x046D;
    if (m_vSubItem.size() > 0)
    {
        usC = m_vSubItem.at(0).usCMD;
        usT = m_vSubItem.at(0).usTID;
    }
    SendMessage(ba.data(), ba.size(), usC, usT);
}

//仿真控制命令按钮
void SS2RSPDcuGUIDlg::on_pushButton_FZCtrlCMD_clicked()
{
    UCHAR ucVal = ui->comboBox_FZCtrlCMD->currentIndex();
    QByteArray ba;
    ba.append(ucVal);
    unsigned short usC = 0x046E;
    unsigned short usT = 0x046E;
    if (m_vSubItem.size() > 1)
    {
        usC = m_vSubItem.at(1).usCMD;
        usT = m_vSubItem.at(1).usTID;
    }
    SendMessage(ba.data(), ba.size(), usC, usT);
}

//存盘控制命令按钮
void SS2RSPDcuGUIDlg::on_pushButton_SaveCtrlCMD_clicked()
{
    UCHAR ucVal = ui->comboBox_SaveCtrlCMD->currentIndex();
    QByteArray ba;
    ba.append(ucVal);
    unsigned short usC = 0x046E;
    unsigned short usT = 0x046F;
    if (m_vSubItem.size() > 2)
    {
        usC = m_vSubItem.at(2).usCMD;
        usT = m_vSubItem.at(2).usTID;
    }
    SendMessage(ba.data(), ba.size(), usC, usT);
}

//切换控制命令设置按钮
void SS2RSPDcuGUIDlg::on_pushButton_CtrlCMD_clicked()
{
    //获取当前
    int nIndex = ui->comboBox_CtrlCmd->currentIndex();
    QByteArray ba;
    if (nIndex == 0)    //本控
        ba.append((char)0x00);
    else if (nIndex == 1)   //远控
        ba.append((char)0xFF);
    unsigned short usC = 0x046E;
    unsigned short usT = 0x0470;
    if (m_vSubItem.size() > 3)
    {
        usC = m_vSubItem.at(3).usCMD;
        usT = m_vSubItem.at(3).usTID;
    }
    SendMessage(ba.data(), ba.size(), usC, usT);
}

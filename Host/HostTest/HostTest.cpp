///////////////////////////////////////////////////////////
//
//  This file is part of RTArduLink
//
//  Copyright (c) 2014 richards-tech
//
//  Permission is hereby granted, free of charge,
//  to any person obtaining a copy of
//  this software and associated documentation files
//  (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute,
//  sublicense, and/or sell copies of the Software, and
//  to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice
//  shall be included in all copies or substantial portions
//  of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
//  ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
//  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include "HostTest.h"
#include "qboxlayout.h"
#include "qextserialenumerator.h"
#include "qdebug.h"

#include "EchoDialog.h"

BaudRateType speedMap[] = {BAUD9600, BAUD19200, BAUD38400, BAUD57600, BAUD115200};
const char *speedString[] = {"9600", "19200", "38400", "57600", "115200"};

HostTest::HostTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    layoutWindow();
    m_running = false;
    m_startButton->setDisabled(false);
    m_stopButton->setDisabled(true);
    m_link = new HostTestEcho(this);
    m_timerID = startTimer(100);

    for (int port = 0; port < RTARDULINKHOST_MAX_PORTS; port++)
        for (int address = 0; address < RTARDULINK_ADDRESSES; address++)
            m_row[port][address] = -1;
    m_nextRow = 0;

    connect(m_link, SIGNAL(RTArduLinkStatus(int, int, bool, char *, qint64, qint64)), this,
                    SLOT(RTArduLinkStatus(int, int, bool, char *, qint64, qint64)));
    connect(m_link, SIGNAL(RTArduLinkPortOpen(int)), SLOT(RTArduLinkPortOpen(int)));
    connect(m_link, SIGNAL(RTArduLinkPortClosed(int)), SLOT(RTArduLinkPortClosed(int)));
    connect(m_link, SIGNAL(RTArduLinkPortRX(int)), SLOT(RTArduLinkPortRX(int)));
    connect(m_link, SIGNAL(RTArduLinkPortTX(int)), SLOT(RTArduLinkPortTX(int)));
}

HostTest::~HostTest()
{

}

void HostTest::startButton()
{
    if (m_running)
        return;

    saveSettings();
    for (int i = 0; i < RTARDULINKHOST_MAX_PORTS; i++) {
        if (m_comPort[i]->currentText() == "Off")
            m_link->deletePort(i);
        else
            m_link->addPort(i, m_comPort[i]->currentText(), speedMap[m_comSpeed[i]->currentIndex()]);
    }

    m_link->begin();
    m_running = true;
    m_startButton->setDisabled(true);
    m_stopButton->setDisabled(false);
    m_echoButton->setDisabled(false);
}

void HostTest::stopButton()
{
    if (!m_running)
        return;
    m_link->end();
    m_running = false;
    m_startButton->setDisabled(false);
    m_stopButton->setDisabled(true);
    m_echoButton->setDisabled(true);
    for (int i = 0; i < RTARDULINKHOST_MAX_PORTS; i++)
        updateComState(i, false);
}

void HostTest::echoButton()
{
    EchoDialog *echoDialog;

    m_echoButton->setDisabled(true);
    echoDialog = new EchoDialog(this);
    connect(m_link, SIGNAL(updateEchoStatus(int, int, int, QString&, bool)),
        echoDialog, SLOT(updateEchoStatus(int, int, int, QString&, bool)));
    connect(echoDialog, SIGNAL(echoStart()), m_link, SLOT(echoStart()));
    connect(echoDialog, SIGNAL(echoStop()), m_link, SLOT(echoStop()));
    echoDialog->exec();
    delete echoDialog;
    m_echoButton->setDisabled(false);
}

void HostTest::RTArduLinkPortOpen(int port)
{
    updateComState(port, true);
}

void HostTest::RTArduLinkPortClosed(int port)
{
    updateComState(port, false);
}

void HostTest::RTArduLinkPortRX(int port)
{
    updateComRXTX(m_comRXLabel[port], true);
}

void HostTest::RTArduLinkPortTX(int port)
{
    updateComRXTX(m_comTXLabel[port], true);
}

void HostTest::timerEvent(QTimerEvent *)
{
    int port;

    for (port = 0; port < RTARDULINKHOST_MAX_PORTS; port++) {
        updateComRXTX(m_comRXLabel[port], false);
        updateComRXTX(m_comTXLabel[port], false);
    }
}

void HostTest::RTArduLinkStatus(int port, int address, bool active, char *identity, qint64 pollsIn, qint64 pollsOut)
{
    int row;

    row = m_row[port][address];
    if (row == -1) {                                        // no map for this port/address combo
        if (active) {
            if (m_nextRow == HOSTECHO_MAX_SUBSYSTEMS) {
                qDebug() << "Ran out of subsystem slots";
                return;
            }
            row = m_row[port][address] = m_nextRow++;
        } else {
            return;
        }
    }

    (m_subsystemStatus->item(row, RTARDULINK_COL_PORT))->setText(QString::number(port));
    (m_subsystemStatus->item(row, RTARDULINK_COL_ADDRESS))->setText(QString::number(address));
    (m_subsystemStatus->item(row, RTARDULINK_COL_STATUS))->setText(active ? "Active" : "...");
    (m_subsystemStatus->item(row, RTARDULINK_COL_IDENTITY))->setText(identity);
    if (pollsIn > 0)
        (m_subsystemStatus->item(row, RTARDULINK_COL_POLLSIN))->setText(QString::number(pollsIn));
    else
        (m_subsystemStatus->item(row, RTARDULINK_COL_POLLSIN))->setText("");
    if (pollsOut > 0)
        (m_subsystemStatus->item(row, RTARDULINK_COL_POLLSOUT))->setText(QString::number(pollsOut));
    else
        (m_subsystemStatus->item(row, RTARDULINK_COL_POLLSOUT))->setText("");
}


void HostTest::layoutWindow()
{
    QTableWidgetItem *item;
    QLabel *label;
    QHBoxLayout *horiz;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *vert = new QVBoxLayout(centralWidget);

    //  Set up com port line and controls

    horiz = NULL;
    for (int index = 0 ; index < RTARDULINKHOST_MAX_PORTS; index++) {
        if ((index & 1) == 0) {
            horiz = new QHBoxLayout();
            horiz->setAlignment(Qt::AlignLeft);
            vert->addLayout(horiz);
        } else {
            horiz->insertSpacing(6, 20);
        }

        m_comLabel[index] = new QLabel(QString("Com port %1:").arg(index));
        m_comLabel[index]->setAlignment(Qt::AlignLeft);
        m_comLabel[index]->setAlignment(Qt::AlignCenter);
        horiz->addWidget(m_comLabel[index]);

        m_comPort[index] = new QComboBox();
        populateComPorts(index);

        horiz->addWidget(m_comPort[index]);

        label = new QLabel(QString("Pt %1 spd:").arg(index));
        label->setAlignment(Qt::AlignLeft);
        label->setAlignment(Qt::AlignCenter);
        horiz->addWidget(label);

        m_comSpeed[index] = new QComboBox();

        for (int i = 0; i < 5; i++)
            m_comSpeed[index]->insertItem(i, speedString[i]);

        horiz->addWidget(m_comSpeed[index]);

        m_comRXLabel[index] = new QLabel(QString("RX"));
        m_comRXLabel[index]->setAlignment(Qt::AlignLeft);
        m_comRXLabel[index]->setAlignment(Qt::AlignCenter);
        horiz->addWidget(m_comRXLabel[index]);

        m_comTXLabel[index] = new QLabel(QString("TX"));
        m_comTXLabel[index]->setAlignment(Qt::AlignLeft);
        m_comTXLabel[index]->setAlignment(Qt::AlignCenter);
        horiz->addWidget(m_comTXLabel[index]);

    }

    //  Set up start and stop buttons

    horiz = new QHBoxLayout();
    horiz->setAlignment(Qt::AlignHCenter);
    vert->addLayout(horiz);

    m_startButton = new QPushButton("Start");
    connect(m_startButton, SIGNAL(pressed()), this, SLOT(startButton()));
    horiz->addWidget(m_startButton);
    m_startButton->setMaximumWidth(100);

    m_stopButton = new QPushButton("Stop");
    connect(m_stopButton, SIGNAL(pressed()), this, SLOT(stopButton()));
    horiz->addWidget(m_stopButton);
    m_stopButton->setMaximumWidth(100);

    horiz->insertSpacing(4, 50);

    m_echoButton = new QPushButton("Echo test");
    connect(m_echoButton, SIGNAL(pressed()), this, SLOT(echoButton()));
    horiz->addWidget(m_echoButton);
    m_echoButton->setMaximumWidth(100);
    m_echoButton->setDisabled(true);

    //  Add status table

    m_subsystemStatus = new QTableWidget();
    vert->addWidget(m_subsystemStatus);

    m_subsystemStatus->setColumnCount(RTARDULINK_COL_COUNT);
    m_subsystemStatus->setColumnWidth(RTARDULINK_COL_PORT, 70);
    m_subsystemStatus->setColumnWidth(RTARDULINK_COL_ADDRESS, 70);
    m_subsystemStatus->setColumnWidth(RTARDULINK_COL_STATUS, 100);
    m_subsystemStatus->setColumnWidth(RTARDULINK_COL_IDENTITY, 260);
    m_subsystemStatus->setColumnWidth(RTARDULINK_COL_POLLSIN, 110);
    m_subsystemStatus->setColumnWidth(RTARDULINK_COL_POLLSOUT, 110);

    m_subsystemStatus->verticalHeader()->setDefaultSectionSize(20);
    m_subsystemStatus->verticalHeader()->stretchLastSection();

    m_subsystemStatus->setHorizontalHeaderLabels(QStringList() << "Port" << "Address" << tr("Status")
            << tr("Identity") << tr("Polls in") << tr("Polls out"));

    m_subsystemStatus->setSelectionMode(QAbstractItemView::NoSelection);

    for (int row = 0; row < HOSTECHO_MAX_SUBSYSTEMS; row++) {
        m_subsystemStatus->insertRow(row);
        m_subsystemStatus->setRowHeight(row, 20);

        for (int col = 0; col < RTARDULINK_COL_COUNT; col++) {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
            item->setText("");
            item->setFlags(Qt::ItemIsEnabled);
            m_subsystemStatus->setItem(row, col, item);
        }
    }
    loadSettings();
    this->setMinimumSize(800, 200);
}

void HostTest::populateComPorts(int port)
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    m_comPort[port]->clear();
    m_comPort[port]->insertItem(0, "Off");
    for (int i = 1; i < ports.size(); i++)
#ifdef Q_OS_WIN
        m_comPort[port]->insertItem(i, ports.at(i).portName);
#else
        m_comPort[port]->insertItem(i, ports.at(i).physName);
#endif

}

void HostTest::updateComState(int port, bool open)
{
    QLabel *label = m_comLabel[port];

    QPalette pal = label->palette();

    if (!m_running)
        pal.setColor(label->backgroundRole(), Qt::lightGray);
    else {
        if (open)
            pal.setColor(label->backgroundRole(), Qt::green);
        else
            pal.setColor(label->backgroundRole(), Qt::red);
    }
    label->setPalette(pal);
    label->setAutoFillBackground(true);
}

void HostTest::updateComRXTX(QLabel *label, bool active)
{
    QPalette pal = label->palette();

    if (active)
        pal.setColor(label->backgroundRole(), Qt::green);
    else
        pal.setColor(label->backgroundRole(), Qt::lightGray);
    label->setPalette(pal);
    label->setAutoFillBackground(true);
}

void HostTest::loadSettings()
{
    int entryCount;
    int speed;
    int port;
    QString portString;

    m_settings = new QSettings("HostTest.ini", QSettings::IniFormat);
    entryCount = m_settings->beginReadArray(RTARDULINKHOST_SETTINGS_NAME);

    //	See if need to set defaults

    if (entryCount != RTARDULINKHOST_MAX_PORTS) {
        m_settings->endArray();
        m_settings->beginWriteArray(RTARDULINKHOST_SETTINGS_NAME);
        for (int i = 0; i < RTARDULINKHOST_MAX_PORTS; i++) {
            m_settings->setArrayIndex(i);
            m_settings->setValue(RTARDULINKHOST_SETTINGS_PORT, "Off");
            m_settings->setValue(RTARDULINKHOST_SETTINGS_SPEED, 4);
        }
        m_settings->endArray();
        entryCount = m_settings->beginReadArray(RTARDULINKHOST_SETTINGS_NAME);
    }

    //	Now read in values

    for (int i = 0; i < RTARDULINKHOST_MAX_PORTS; i++) {
        m_settings->setArrayIndex(i);
        portString = m_settings->value(RTARDULINKHOST_SETTINGS_PORT).toString();
        if ((port = m_comPort[i]->findText(portString)) != -1)
            m_comPort[i]->setCurrentIndex(port);

        speed = m_settings->value(RTARDULINKHOST_SETTINGS_SPEED).toInt();
        if ((speed >= 0) && (speed <= 4))
            m_comSpeed[i]->setCurrentIndex(speed);
    }
    m_settings->endArray();
}

void HostTest::saveSettings()
{
    m_settings->beginWriteArray(RTARDULINKHOST_SETTINGS_NAME);
    for (int i = 0; i < RTARDULINKHOST_MAX_PORTS; i++) {
        m_settings->setArrayIndex(i);
        m_settings->setValue(RTARDULINKHOST_SETTINGS_PORT, m_comPort[i]->currentText());
        m_settings->setValue(RTARDULINKHOST_SETTINGS_SPEED, m_comSpeed[i]->currentIndex());
    }
    m_settings->endArray();
}

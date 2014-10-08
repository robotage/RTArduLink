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

#include "HostDemo.h"
#include "HostDemoClient.h"

#include "qextserialenumerator.h"
#include "qdebug.h"

BaudRateType speedMap[] = {BAUD9600, BAUD19200, BAUD38400, BAUD57600, BAUD115200};
const char *speedString[] = {"9600", "19200", "38400", "57600", "115200"};

HostDemo::HostDemo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    layoutWindow();
    m_running = false;
    m_address = -1;
    m_link = new HostDemoClient(this);
    m_timerID = startTimer(100);

    connect(m_link, SIGNAL(RTArduLinkStatus(int, int, bool, QString, qint64, qint64)), this,
                    SLOT(RTArduLinkStatus(int, int, bool, QString, qint64, qint64)));
    connect(m_link, SIGNAL(RTArduLinkPortOpen(int)), SLOT(RTArduLinkPortOpen(int)));
    connect(m_link, SIGNAL(RTArduLinkPortClosed(int)), SLOT(RTArduLinkPortClosed(int)));
    connect(m_link, SIGNAL(RTArduLinkPortRX(int)), SLOT(RTArduLinkPortRX(int)));
    connect(m_link, SIGNAL(RTArduLinkPortTX(int)), SLOT(RTArduLinkPortTX(int)));
    connect(m_link, SIGNAL(updateInputStatus(int, bool)), this, SLOT(updateInputStatus(int, bool)));
}

HostDemo::~HostDemo()
{
    delete m_link;
}


void HostDemo::sendCommand()
{
    int channel;
    SERVO_CHANNEL *servoChannel;
    PWM_CHANNEL *pwmChannel;
    RTARDULINKDEMO_COMMAND command;

    if (m_address == -1)
        return;                                             // no subsystem yet

    //  set GPIO output data

    for (int output = 0; output < OUTPUT_COUNT; output++)
        command.outputValue[output] = m_output[output]->checkState() == Qt::Checked;

    //  Set servo data

    for (channel = 0; channel < SERVO_COUNT; channel++) {
        servoChannel = m_servo + channel;
        servoChannel->sliderPos = servoChannel->slider->value();
        servoChannel->sliderLabel->setText(QString::number(servoChannel->sliderPos));
        RTArduLinkConvertIntToUC2(servoChannel->sliderPos, command.servoPos[channel]);
    }

    //  Set PWM data

    for (channel = 0; channel < PWM_COUNT; channel++) {
        pwmChannel = m_pwm + channel;
        pwmChannel->sliderPos = pwmChannel->slider->value();
        pwmChannel->sliderLabel->setText(QString::number(pwmChannel->sliderPos));
        command.pwmValue[channel] = pwmChannel->sliderPos;
    }
    m_link->sendMessage(0, m_address, RTARDULINK_MESSAGE_CUSTOM, 0, (unsigned char *)&command,
                        sizeof(RTARDULINKDEMO_COMMAND));
}


//  Slots

void HostDemo::startButton()
{
    if (m_running)
        return;

    if (m_comPort->currentText() == "Off")
            m_link->deletePort(0);
        else
            m_link->addPort(0, m_comPort->currentText(), speedMap[m_comSpeed->currentIndex()]);

    m_link->begin();
    m_running = true;
    m_startButton->setDisabled(true);
    m_stopButton->setDisabled(false);
}

void HostDemo::stopButton()
{
    if (!m_running)
        return;
    m_link->end();
    m_running = false;
    m_startButton->setDisabled(false);
    m_stopButton->setDisabled(true);
    updateComState(false);
    m_address = -1;
    m_subsystem->clear();
    m_subsystem->insertItem(0, "None selected");
}

void HostDemo::sliderMoved(int)
{
    sendCommand();
}

void HostDemo::outputChanged(int)
{
    sendCommand();
}

void HostDemo::subsystemSelect(int index)
{
    m_identity = m_subsystem->itemText(index);
    m_address = m_subsystem->itemData(index).toInt();
    sendCommand();
}

void HostDemo::updateInputStatus(int inputNumber, bool value)
{
    if ((inputNumber < 0) || (inputNumber >= INPUT_COUNT))
        return;                                             // out of range input number
    m_input[inputNumber]->setChecked(value);
}

void HostDemo::RTArduLinkPortOpen(int )
{
    updateComState(true);
}

void HostDemo::RTArduLinkPortClosed(int )
{
    updateComState(false);
}

void HostDemo::RTArduLinkPortRX(int )
{
    updateComRXTX(m_comRXLabel, true);
}

void HostDemo::RTArduLinkPortTX(int )
{
    updateComRXTX(m_comTXLabel, true);
}

void HostDemo::timerEvent(QTimerEvent *)
{
    updateComRXTX(m_comRXLabel, false);
    updateComRXTX(m_comTXLabel, false);
}

void HostDemo::RTArduLinkStatus(int, int address, bool active, QString identity, qint64, qint64)
{
    int selIndex;

    selIndex = m_subsystem->findText(identity);             // get index in combo box

    if (active) {                                           // see if in subsystem list
        if (selIndex != -1)
            return;                                         // is in list already
        m_subsystem->addItem(identity, address);            // add it
        return;
    } else {                                                // check if need to delete from list
        if (selIndex == -1)
            return;                                         // not in list
        m_subsystem->removeItem(selIndex);                  // delete the item
        if (identity == m_identity)	{                       // and its the one in use
            m_address = -1;                                 // indicate not there
            m_identity = "";
        }
    }
}


//----------------------------------------------------------
//
//  GUI-related code

void HostDemo::layoutWindow()
{
    QLabel *label;
    QHBoxLayout *horiz;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *vert = new QVBoxLayout(centralWidget);

    //  Set up com port line and controls

    horiz = new QHBoxLayout();
    horiz->setAlignment(Qt::AlignLeft);
    vert->addLayout(horiz);
    m_comLabel = new QLabel("Com port:");
    m_comLabel->setAlignment(Qt::AlignLeft);
    m_comLabel->setAlignment(Qt::AlignCenter);
    horiz->addWidget(m_comLabel);

    m_comPort = new QComboBox(this);
    populateComPorts();

    horiz->addWidget(m_comPort);

    label = new QLabel("Port speed:");
    label->setAlignment(Qt::AlignLeft);
    label->setAlignment(Qt::AlignCenter);
    horiz->addWidget(label);

    m_comSpeed = new QComboBox(this);

    for (int i = 0; i < 5; i++)
        m_comSpeed->insertItem(i, speedString[i]);
    m_comSpeed->setCurrentIndex(4);
    horiz->addWidget(m_comSpeed);

    m_comRXLabel = new QLabel(QString("RX"));
    m_comRXLabel->setAlignment(Qt::AlignLeft);
    m_comRXLabel->setAlignment(Qt::AlignCenter);
    m_comRXLabel->setMaximumHeight(20);
    horiz->addWidget(m_comRXLabel);

    m_comTXLabel = new QLabel(QString("TX"));
    m_comTXLabel->setAlignment(Qt::AlignLeft);
    m_comTXLabel->setAlignment(Qt::AlignCenter);
    m_comTXLabel->setMaximumHeight(20);
    horiz->addWidget(m_comTXLabel);

    //  Set up start and stop buttons

    m_startButton = new QPushButton("Start");
    connect(m_startButton, SIGNAL(pressed()), this, SLOT(startButton()));
    horiz->addWidget(m_startButton);
    m_startButton->setMaximumWidth(100);

    m_stopButton = new QPushButton("Stop");
    connect(m_stopButton, SIGNAL(pressed()), this, SLOT(stopButton()));
    horiz->addWidget(m_stopButton);
    m_stopButton->setMaximumWidth(100);

    //  Set up subsystem selector

    horiz = new QHBoxLayout();
    horiz->setAlignment(Qt::AlignLeft);
    vert->addLayout(horiz);
    label = new QLabel("Subsystem:");
    label->setAlignment(Qt::AlignLeft);
    label->setAlignment(Qt::AlignCenter);
    horiz->addWidget(label);

    m_subsystem = new QComboBox(this);
    m_subsystem->setMinimumWidth(200);
    m_subsystem->insertItem(0, "None selected");
    horiz->addWidget(m_subsystem);
    connect(m_subsystem, SIGNAL(currentIndexChanged(int)), this, SLOT(subsystemSelect(int)));

    //  Set up GPIO outputs

    horiz = new QHBoxLayout();
    horiz->setAlignment(Qt::AlignLeft);
    vert->addLayout(horiz);

    for (int i = 0; i < OUTPUT_COUNT; i++) {
        m_output[i] = new QCheckBox(QString("Output %1").arg(i));
        m_output[i]->setMinimumWidth(100);
        connect(m_output[i], SIGNAL(stateChanged(int)), this, SLOT(outputChanged(int)));
        horiz->addWidget(m_output[i]);
    }

    //  Set up GPIO inputs

    horiz = new QHBoxLayout();
    horiz->setAlignment(Qt::AlignLeft);
    vert->addLayout(horiz);

    for (int i = 0; i < INPUT_COUNT; i++) {
        m_input[i] = new QCheckBox(QString("Input %1").arg(i));
        m_input[i]->setMinimumWidth(100);
        m_input[i]->setDisabled(true);
        horiz->addWidget(m_input[i]);
    }

    //  Set up servo sliders

    for (int i = 0; i < SERVO_COUNT; i++)
        layoutServoChannel(i, vert);

    //  Set up PWM sliders

    for (int i = 0; i < PWM_COUNT; i++)
        layoutPWMChannel(i, vert);

}

void HostDemo::layoutServoChannel(int channel, QVBoxLayout *vLayout)
{
    QSlider *slider;
    SERVO_CHANNEL *servoChannel;

    servoChannel= m_servo + channel;

    QHBoxLayout *hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    servoChannel->sliderPos = SERVO_CTR_VALUE;

    QLabel *label = new QLabel(QString("Servo channel %1: ").arg(channel));
    label->setMinimumWidth(100);
    hLayout->addWidget(label);

    servoChannel->sliderLabel = new QLabel();
    servoChannel->sliderLabel->setText(QString::number(SERVO_CTR_VALUE));
    servoChannel->sliderLabel->setMinimumWidth(30);
    hLayout->addWidget(servoChannel->sliderLabel);

    servoChannel->slider = new QSlider();
    slider = servoChannel->slider;
    slider->setOrientation(Qt::Horizontal);
    hLayout->addWidget(slider);
    slider->setMinimumWidth(150);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setRange(SERVO_MIN_VALUE, SERVO_MAX_VALUE);
    slider->setTickInterval((SERVO_MAX_VALUE - SERVO_MIN_VALUE) / 10);
    slider->setValue(SERVO_CTR_VALUE);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
}

void HostDemo::layoutPWMChannel(int channel, QVBoxLayout *vLayout)
{
    QSlider *slider;
    PWM_CHANNEL *pwmChannel;

    pwmChannel = m_pwm + channel;

    QHBoxLayout *hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    pwmChannel->sliderPos = PWM_CTR_VALUE;

    QLabel *label = new QLabel(QString("PWM channel %1: ").arg(channel));
    label->setMinimumWidth(100);
    hLayout->addWidget(label);

    pwmChannel->sliderLabel = new QLabel();
    pwmChannel->sliderLabel->setText(QString::number(PWM_CTR_VALUE));
    pwmChannel->sliderLabel->setMinimumWidth(30);
    hLayout->addWidget(pwmChannel->sliderLabel);

    pwmChannel->slider = new QSlider();
    slider = pwmChannel->slider;
    slider->setOrientation(Qt::Horizontal);
    hLayout->addWidget(slider);
    slider->setMinimumWidth(150);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setRange(PWM_MIN_VALUE, PWM_MAX_VALUE);
    slider->setTickInterval((PWM_MAX_VALUE - PWM_MIN_VALUE) / 10);
    slider->setValue(PWM_CTR_VALUE);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
}

void HostDemo::populateComPorts()
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    m_comPort->clear();
    m_comPort->insertItem(0, "Off");
    for (int i = 1; i < ports.size(); i++)
#ifdef Q_OS_WIN
        m_comPort->insertItem(i, ports.at(i).portName);
#else
        m_comPort->insertItem(i, ports.at(i).physName);
#endif

}

void HostDemo::updateComState(bool open)
{
    QPalette pal = m_comLabel->palette();

    if (!m_running)
        pal.setColor(m_comLabel->backgroundRole(), Qt::lightGray);
    else {
        if (open)
            pal.setColor(m_comLabel->backgroundRole(), Qt::green);
        else
            pal.setColor(m_comLabel->backgroundRole(), Qt::red);
    }
    m_comLabel->setPalette(pal);
    m_comLabel->setAutoFillBackground(true);
}

void HostDemo::updateComRXTX(QLabel *label, bool active)
{
    QPalette pal = label->palette();

    if (active)
        pal.setColor(label->backgroundRole(), Qt::green);
    else
        pal.setColor(label->backgroundRole(), Qt::lightGray);
    label->setPalette(pal);
    label->setAutoFillBackground(true);
}

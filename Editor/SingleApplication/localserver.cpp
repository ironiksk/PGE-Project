/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QFile>
#include <QStringList>
#include <QtDebug>

#include <common_features/mainwinconnect.h>
#include <networking/engine_intproc.h>

#include "localserver.h"

/**
 * @brief LocalServer::LocalServer
 *  Constructor
 */
LocalServer::LocalServer()
{
    qRegisterMetaType<QAbstractSocket::SocketState> ("QAbstractSocket::SocketState");
}

/**
 * @brief LocalServer::~LocalServer
 *  Destructor
 */
LocalServer::~LocalServer()
{
  server->close();
  for(int i = 0; i < clients.size(); ++i)
  {
    clients[i]->close();
  }
}

/**
 * -----------------------
 * QThread requred methods
 * -----------------------
 */

/**
 * @brief run
 *  Initiate the thread.
 */
void LocalServer::run()
{
  server = new QLocalServer();

  QObject::connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
  QObject::connect(this, SIGNAL(privateDataReceived(QString)), this, SLOT(slotOnData(QString)));

#ifdef Q_OS_UNIX
  // Make sure the temp address file is deleted
  QFile address(QString("/tmp/" LOCAL_SERVER_NAME));
  if(address.exists()){
    address.remove();
  }
#endif

  QString serverName = QString(LOCAL_SERVER_NAME);
  server->listen(serverName);
  while(server->isListening() == false){
    server->listen(serverName);
    msleep(100);
  }
  exec();
}

/**
 * @brief LocalServer::exec
 *  Keeps the thread alive. Waits for incomming connections
 */
void LocalServer::exec()
{
  while(server->isListening())
  {
    msleep(100);
    server->waitForNewConnection(100);
    for(int i = 0; i < clients.size(); ++i)
    {
      if(clients[i]->waitForReadyRead(100))
      {
        QByteArray data = clients[i]->readAll();
        emit privateDataReceived(QString::fromUtf8(data));
      }
      else
      {
          if(!clients[i]->isOpen())
          {
              QLocalSocket* tmp = clients[i];
              clients.removeAt(i);
              delete tmp;
              i--;
          }
      }
    }
  }
}



/**
 * -------
 * SLOTS
 * -------
 */



/**
 * @brief LocalServer::slotNewConnection
 *  Executed when a new connection is available
 */
void LocalServer::slotNewConnection()
{
  clients.push_front(server->nextPendingConnection());
}



/**
 * @brief LocalServer::slotOnData
 *  Executed when data is received
 * @param data
 */
void LocalServer::slotOnData(QString data)
{
  qDebug() << data;
  QStringList args = data.split('\n');
  foreach(QString c, args)
  {
      if(c.startsWith("CMD:", Qt::CaseInsensitive))
      {
        onCMD(c);
      }
      else
      {
        emit dataReceived(c);
      }
  }
}



/**
 * -------
 * Helper methods
 * -------
 */
void LocalServer::onCMD(QString data)
{
  //  Trim the leading part from the command
  if(data.startsWith("CMD:"))
  {
    data.remove("CMD:");

    qDebug()<<"Accepted data: "+data;

    QStringList commands;
    commands << "showUp";
    commands << "CONNECT_TO_ENGINE";
    commands << "ENGINE_CLOSED";

    if(MainWinConnect::pMainWin->continueLoad)
        switch(commands.indexOf(data))
        {
            case 0:
            {
                emit showUp();
                MainWinConnect::pMainWin->setWindowState((MainWinConnect::pMainWin->windowState()&
                                                         (~(MainWinConnect::pMainWin->windowState()&Qt::WindowMinimized)))
                                                          |Qt::WindowActive);
                if(MainWinConnect::pMainWin->isMinimized())
                {
                    MainWinConnect::pMainWin->raise();
                    MainWinConnect::pMainWin->activateWindow();
                    MainWinConnect::pMainWin->showNormal();
                }
                qApp->setActiveWindow(MainWinConnect::pMainWin);
                break;
            }
            case 1:
            {
                if(!IntEngine::isWorking())
                {
                    IntEngine::init();
                }

                IntEngine::sendLevelBuffer();
                //MainWinConnect::pMainWin->showMinimized();
                //IntEngine::engineSocket->sendLevelData(IntEngine::testBuffer);
                break;
            }
            case 2:
            {
                IntEngine::quit();
                //qDebug()<<"Set Window state";
                //MainWinConnect::pMainWin->setWindowState(MainWinConnect::pMainWin->windowState()&
                //                                         (~(MainWinConnect::pMainWin->windowState()&Qt::WindowMinimized)));
                //if(MainWinConnect::pMainWin->isMinimized())
                //{
                //    MainWinConnect::pMainWin->raise();
                //    MainWinConnect::pMainWin->showNormal();
                //}
                //qDebug()<<"Set active Window";
                //qApp->setActiveWindow(MainWinConnect::pMainWin);
                //qDebug()<<"Update menus";
                //MainWinConnect::pMainWin->updateMenus();
                //qDebug()<<"IntEngine::quit();";
                //IntEngine::quit();
                break;
            }
            default:
              emit acceptedCommand(data);
        }
  }
  else
      emit acceptedCommand(data);
}

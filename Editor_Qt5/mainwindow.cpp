/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "levelprops.h"
#include <QFileDialog>
#include <QtWidgets>
#include <QMdiArea>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QtMultimedia/QMediaPlayer>
#include <QMediaPlaylist>
#include <QLocale>
#include <QSplashScreen>
#include "childwindow.h"
#include "leveledit.h"
#include "npcedit.h"
#include "dataconfigs.h"
#include "musicfilelist.h"

QString LastOpenDir = ".";
bool LevelToolBoxVis = false;
bool WorldToolBoxVis = false;
bool SectionToolBoxVis = false;


dataconfigs configs;

QMediaPlayer * MusicPlayer;

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //QMenuBar * menuBar = new QMenuBar(0); // test for MacOS X

    MusicPlayer = new QMediaPlayer;

    QPixmap splashimg(":/images/splash.png");
    QSplashScreen splash(splashimg);
    splash.show();

    configs.loadconfigs(true);
    ui->setupUi(this);

    //Applay objects into tools
    setTools();

    #ifdef Q_OS_MAC
        this->setWindowIcon(QIcon(":/images/mac/mushroom.icns"));
    #endif

    //menuBar = ui->menuBar; // test for MacOS X
    //setMenuBar(menuBar); // test for MacOS X

    splash.finish(this);


    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    //resize(settings.value("size", size()).toSize());
    //move(settings.value("pos", pos()).toPoint());
    LastOpenDir = settings.value("lastpath", ".").toString();
    LevelToolBoxVis = settings.value("level-tb-visible", "false").toBool();
    WorldToolBoxVis = settings.value("world-tb-visible", "false").toBool();
    SectionToolBoxVis = settings.value("section-tb-visible", "false").toBool();
    //if(settings.value("maximased", "false")=="true") showMaximized();
    //"lvl-section-view", dockWidgetArea(ui->LevelSectionSettings)
    //dockWidgetArea();
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    settings.endGroup();

    ui->centralWidget->cascadeSubWindows();
    ui->WorldToolBox->hide();
    ui->LevelSectionSettings->hide();
    ui->LevelToolBox->hide();

    ui->menuView->setEnabled(0);
    ui->menuWindow->setEnabled(0);
    ui->menuLevel->setEnabled(0);
    ui->menuWorld->setEnabled(0);

    ui->actionLVLToolBox->setVisible(0);
    ui->actionSection_Settings->setVisible(0);
    ui->actionWLDToolBox->setVisible(0);

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMenus()
{
    int WinType = activeChildWindow(); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != 0);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

    ui->menuView->setEnabled( (hasSWindow) && (WinType!=2) );

    ui->actionSelect->setEnabled( (WinType==1) || (WinType==3));
    ui->actionEriser->setEnabled( (WinType==1) || (WinType==3));

    if(!(WinType==3)) WorldToolBoxVis = ui->WorldToolBox->isVisible(); //Save current visible status

    ui->WorldToolBox->setVisible( (WinType==3) && (WorldToolBoxVis)); //Restore saved visible status
    ui->menuWorld->setEnabled(( WinType==3) );
    ui->actionWLDToolBox->setVisible( (WinType==3));


    if(!(WinType==1)) LevelToolBoxVis = ui->LevelToolBox->isVisible();  //Save current visible status
    if(!(WinType==1)) SectionToolBoxVis = ui->LevelSectionSettings->isVisible();

    ui->LevelToolBox->setVisible( (WinType==1) && (LevelToolBoxVis)); //Restore saved visible status
    ui->LevelSectionSettings->setVisible( (WinType==1) && (SectionToolBoxVis));

    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->menuLevel->setEnabled( (WinType==1) );
    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->actionLevelProp->setEnabled( (WinType==1) );
    ui->actionLevNoBack->setEnabled( (WinType==1) );
    ui->actionLevOffScr->setEnabled( (WinType==1) );
    ui->actionLevWarp->setEnabled( (WinType==1) );
    ui->actionLevUnderW->setEnabled( (WinType==1) );
    ui->actionExport_to_image->setEnabled( (WinType==1) );

    ui->actionSection_1->setEnabled( (WinType==1) );
    ui->actionSection_2->setEnabled( (WinType==1) );
    ui->actionSection_3->setEnabled( (WinType==1) );
    ui->actionSection_4->setEnabled( (WinType==1) );
    ui->actionSection_5->setEnabled( (WinType==1) );
    ui->actionSection_6->setEnabled( (WinType==1) );
    ui->actionSection_7->setEnabled( (WinType==1) );
    ui->actionSection_8->setEnabled( (WinType==1) );
    ui->actionSection_9->setEnabled( (WinType==1) );
    ui->actionSection_10->setEnabled( (WinType==1) );
    ui->actionSection_11->setEnabled( (WinType==1) );
    ui->actionSection_12->setEnabled( (WinType==1) );
    ui->actionSection_13->setEnabled( (WinType==1) );
    ui->actionSection_14->setEnabled( (WinType==1) );
    ui->actionSection_15->setEnabled( (WinType==1) );
    ui->actionSection_16->setEnabled( (WinType==1) );
    ui->actionSection_17->setEnabled( (WinType==1) );
    ui->actionSection_18->setEnabled( (WinType==1) );
    ui->actionSection_19->setEnabled( (WinType==1) );
    ui->actionSection_20->setEnabled( (WinType==1) );
    ui->actionSection_21->setEnabled( (WinType==1) );

    if(WinType==1)
    {
        SetCurrentLevelSection(0, 1);
        setMusic( ui->LVLPropsMusicPlay->isChecked() );
    }

    ui->menuWindow->setEnabled( (WinType!=0) );

    /*
    closeAllAct->setEnabled(hasSWindow);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasSWindow);
    nextAct->setEnabled(hasSWindow);
    previousAct->setEnabled(hasSWindow);
    separatorAct->setVisible(hasSWindow);
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    */
}

void MainWindow::setTools()
{
    int i;
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    ui->LVLPropsBackImage->addItem("[No image]");
    ui->LVLPropsMusicNumber->addItem("[Silence]");

    for(i=0; i< configs.main_bg.size();i++)
        ui->LVLPropsBackImage->addItem(configs.main_bg[i].name);

    for(i=0; i< configs.main_music_lvl.size();i++)
        ui->LVLPropsMusicNumber->addItem(configs.main_music_lvl[i].name);

}

void MainWindow::save()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==2)
            saved = activeNpcEditWin()->save();
        if(WinType==1)
            saved = activeLvlEditWin()->save();

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_as()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==2)
            saved = activeNpcEditWin()->saveAs();
        if(WinType==1)
            saved = activeLvlEditWin()->saveAs();

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_all()
{
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
            ChildWindow0->save();
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
            ChildWindow2->save();
        }

    }
}

void MainWindow::close_sw()
{
    ui->centralWidget->activeSubWindow()->close();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        //qDebug() << "Dropped file:" << fileName;
        OpenFile(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->centralWidget->closeAllSubWindows();


    if (ui->centralWidget->currentSubWindow()) {
        event->ignore();

    }
    else
    {
    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    settings.setValue("pos", pos());
    settings.setValue("lastpath", LastOpenDir);
    settings.setValue("level-tb-visible", LevelToolBoxVis);
    settings.setValue("world-tb-visible", WorldToolBoxVis);
    settings.setValue("section-tb-visible", SectionToolBoxVis);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.endGroup();
    event->accept();
    }
}

///////////////////////////////////////////////////////


//////////////////////////////////////////////////////

void MainWindow::on_OpenFile_triggered()
{
     QString fileName_DATA = QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),LastOpenDir,
        tr("All SMBX files (*.LVL *.WLD npc-*.TXT *.INI)\n"
        "SMBX Level (*.LVL)\n"
        "SMBX World (*.WLD)\n"
        "SMBX NPC Config (npc-*.TXT)\n"
        "All Files (*.*)"));

        if(fileName_DATA==NULL) return;

        OpenFile(fileName_DATA);

}

void MainWindow::OpenFile(QString FilePath)
{

    QFile file(FilePath);


    QMdiSubWindow *existing = findMdiChild(FilePath);
            if (existing) {
                ui->centralWidget->setActiveSubWindow(existing);
                return;
            }


    if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("File open error"),
    tr("Can't open the file."), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    LastOpenDir = in_1.absoluteDir().absolutePath();

    if(in_1.suffix() == "lvl")
    {

        LevelData FileData = ReadLevelFile(file); //function in file_formats.cpp
        if( !FileData.ReadFileValid ) return;

        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();

        leveledit *child = createChild();
        if (child->loadFile(FilePath, FileData, configs)) {
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            child->show();
            SetCurrentLevelSection(0);
        } else {
            child->close();
        }
    }
    else
    if(in_1.suffix() == "wld")
    {
        WorldData FileData = ReadWorldFile(file);
        if( !FileData.ReadFileValid ) return;

        /*
        leveledit *child = createChild();
        if (child->loadFile(FilePath)) {
            statusBar()->showMessage(tr("World map file loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
        */

    }
    else
    if(in_1.suffix() == "txt")
    {
        NPCConfigFile FileData = ReadNpcTXTFile(file);
        if( !FileData.ReadFileValid ) return;

        npcedit *child = createNPCChild();
        if (child->loadFile(FilePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
    else
    {
    QMessageBox::warning(this, tr("Bad file"),
     tr("This file have unknown extension"),
     QMessageBox::Ok);
    return;
    }

/*
QMessageBox::information(this, tr("File checked"),
tr("This file is good"),
QMessageBox::Ok);
*/

}

//Edit NPC
npcedit *MainWindow::createNPCChild()
{
    npcedit *child = new npcedit;
    QMdiSubWindow *npcWindow = new QMdiSubWindow;
    npcWindow->setWidget(child);
    npcWindow->setFixedSize(520,640);
    npcWindow->setAttribute(Qt::WA_DeleteOnClose);
    npcWindow->setWindowFlags(Qt::WindowCloseButtonHint);
    npcWindow->move(rand()%100,rand()%50);
    ui->centralWidget->addSubWindow(npcWindow);

 /*   connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
 */

    return child;
}



//Edit LEVEL
leveledit *MainWindow::createChild()
{
    leveledit *child = new leveledit;
    //child->setWindowIcon(QIcon(QPixmap(":/lvl16.png")));
    ui->centralWidget->addSubWindow(child)->resize(QSize(800, 602));

        return child;
}



int MainWindow::activeChildWindow()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
    {
        if(QString(activeSubWindow->widget()->metaObject()->className())=="leveledit")
            return 1;
        if(QString(activeSubWindow->widget()->metaObject()->className())=="npcedit")
            return 2;
    }

    return 0;
}

npcedit *MainWindow::activeNpcEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<npcedit *>(activeSubWindow->widget());
    return 0;
}

leveledit *MainWindow::activeLvlEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<leveledit *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
        if (ChildWindow0->currentFile() == canonicalFilePath)
            return window;
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
        if (ChildWindow2->currentFile() == canonicalFilePath)
            return window;
        }

    }
    return 0;
}


void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    ui->centralWidget->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

/*
void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

*/


void MainWindow::SetCurrentLevelSection(int SctId, int open)
{
    int SectionId = SctId;
    int WinType = activeChildWindow();

    if ((open==1)&&(WinType==1)) // Only Set Checked section number without section select
    {
        SectionId = activeLvlEditWin()->LvlData.CurSection;
    }

    ui->actionSection_1->setChecked( (SectionId==0) );
    ui->actionSection_2->setChecked( (SectionId==1) );
    ui->actionSection_3->setChecked( (SectionId==2) );
    ui->actionSection_4->setChecked( (SectionId==3) );
    ui->actionSection_5->setChecked( (SectionId==4) );
    ui->actionSection_6->setChecked( (SectionId==5) );
    ui->actionSection_7->setChecked( (SectionId==6) );
    ui->actionSection_8->setChecked( (SectionId==7) );
    ui->actionSection_9->setChecked( (SectionId==8) );
    ui->actionSection_10->setChecked( (SectionId==9) );
    ui->actionSection_11->setChecked( (SectionId==10) );
    ui->actionSection_12->setChecked( (SectionId==11) );
    ui->actionSection_13->setChecked( (SectionId==12) );
    ui->actionSection_14->setChecked( (SectionId==13) );
    ui->actionSection_15->setChecked( (SectionId==14) );
    ui->actionSection_16->setChecked( (SectionId==15) );
    ui->actionSection_17->setChecked( (SectionId==16) );
    ui->actionSection_18->setChecked( (SectionId==17) );
    ui->actionSection_19->setChecked( (SectionId==18) );
    ui->actionSection_20->setChecked( (SectionId==19) );
    ui->actionSection_21->setChecked( (SectionId==20) );

    if ((WinType==1) && (open==0))
    {
       activeLvlEditWin()->setCurrentSection(SectionId);
    }

    if (WinType==1)
    {
        //Set Section Data in menu
        ui->actionLevNoBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->actionLevOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->actionLevUnderW->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->actionLevWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        //set data in Section Settings Widget
        ui->LVLProp_CurSect->setText(QString::number(SectionId+1));

        ui->LVLPropsNoTBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->LVLPropsOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->LVLPropsUnderWater->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->LVLPropsLevelWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        ui->LVLPropsMusicCustom->setText(activeLvlEditWin()->LvlData.sections[SectionId].music_file);

        ui->LVLPropsMusicNumber->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].music_id );
        ui->LVLPropsMusicCustomEn->setChecked( (activeLvlEditWin()->LvlData.sections[SectionId].music_id == configs.music_custom_id) );

        ui->LVLPropsBackImage->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].background );
    }
}


//////////////////SLOTS///////////////////////////

//Exit from application
void MainWindow::on_Exit_triggered()
{
    MainWindow::close();
    exit(0);
}

//Open About box
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog about;
    about.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    about.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, about.size(), qApp->desktop()->availableGeometry()));
    about.exec();
}


// Level tool box show/hide
void MainWindow::on_LevelToolBox_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionLVLToolBox->setChecked(true);
    }
     else
    {
        ui->actionLVLToolBox->setChecked(false);
    }
}

void MainWindow::on_actionLVLToolBox_triggered()
{
        if(ui->actionLVLToolBox->isChecked())
            ui->LevelToolBox->setVisible(true);
        else
            ui->LevelToolBox->setVisible(false);
}

// World tool box show/hide
void MainWindow::on_WorldToolBox_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionWLDToolBox->setChecked(true);
    }
     else
    {
        ui->actionWLDToolBox->setChecked(false);
    }
}

void MainWindow::on_actionWLDToolBox_triggered()
{
    if(ui->actionWLDToolBox->isChecked())
        ui->WorldToolBox->setVisible(true);
    else
        ui->WorldToolBox->setVisible(false);
}

// Level Section tool box show/hide
void MainWindow::on_LevelSectionSettings_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionSection_Settings->setChecked(true);
    }
     else
    {
        ui->actionSection_Settings->setChecked(false);
    }
}

void MainWindow::on_actionSection_Settings_triggered()
{
    if(ui->actionSection_Settings->isChecked())
        ui->LevelSectionSettings->setVisible(true);
    else
        ui->LevelSectionSettings->setVisible(false);
}


//Open Level Properties
void MainWindow::on_actionLevelProp_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelProps LevProps(activeLvlEditWin()->LvlData);
        LevProps.exec();
    }

}

void MainWindow::on_pushButton_4_clicked()
{
    //ui->xxxxlistWidget->insertItem(1, "test");
    //MyListItem *itm = new MyListItem;
    //insertItem(1, "test image");
    //itm->setText("My Item");
    //itm->icon_to_be_shown = "./data/graphics/worldmap/tile/tile-1.gif";
    //ui->TestItemBox->addItem(itm);

    //ui->TilesItemBox
    //ui->TilesItemBox->model()->setData(  ui->TilesItemBox->model()->index(1, 1),);

    QString first = "file-1.35.gif";
    QStringList mid1 = first.split(".",QString::SkipEmptyParts);
    QString second = mid1[0] + "m." + mid1[1];

    QMessageBox::information(this, tr("QRegExp test"),
       tr(QString("First: "+first+"\nSecond: "+second).toStdString().c_str()),
     QMessageBox::Ok);


}


void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSave_as_triggered()
{
    save_as();
}

void MainWindow::on_actionClose_triggered()
{
    close_sw();
}

void MainWindow::on_actionSave_all_triggered()
{
    save_all();
}

//////////////////////////////////// GoTo Section  ///////////////////////////////////////////////
void MainWindow::on_actionSection_1_triggered()
{
    SetCurrentLevelSection(0);
}

void MainWindow::on_actionSection_2_triggered()
{
    SetCurrentLevelSection(1);
}

void MainWindow::on_actionSection_3_triggered()
{
    SetCurrentLevelSection(2);
}

void MainWindow::on_actionSection_4_triggered()
{
    SetCurrentLevelSection(3);
}

void MainWindow::on_actionSection_5_triggered()
{
    SetCurrentLevelSection(4);
}

void MainWindow::on_actionSection_6_triggered()
{
    SetCurrentLevelSection(5);
}

void MainWindow::on_actionSection_7_triggered()
{
    SetCurrentLevelSection(6);
}

void MainWindow::on_actionSection_8_triggered()
{
    SetCurrentLevelSection(7);
}

void MainWindow::on_actionSection_9_triggered()
{
    SetCurrentLevelSection(8);
}

void MainWindow::on_actionSection_10_triggered()
{
    SetCurrentLevelSection(9);
}

void MainWindow::on_actionSection_11_triggered()
{
    SetCurrentLevelSection(10);
}

void MainWindow::on_actionSection_12_triggered()
{
    SetCurrentLevelSection(11);
}


void MainWindow::on_actionSection_13_triggered()
{
    SetCurrentLevelSection(12);
}

void MainWindow::on_actionSection_14_triggered()
{
    SetCurrentLevelSection(13);
}

void MainWindow::on_actionSection_15_triggered()
{
    SetCurrentLevelSection(14);
}

void MainWindow::on_actionSection_16_triggered()
{
    SetCurrentLevelSection(15);
}

void MainWindow::on_actionSection_17_triggered()
{
    SetCurrentLevelSection(16);
}

void MainWindow::on_actionSection_18_triggered()
{
    SetCurrentLevelSection(17);
}

void MainWindow::on_actionSection_19_triggered()
{
    SetCurrentLevelSection(18);
}

void MainWindow::on_actionSection_20_triggered()
{
    SetCurrentLevelSection(19);
}

void MainWindow::on_actionSection_21_triggered()
{
    SetCurrentLevelSection(20);
}



void MainWindow::on_actionLoad_configs_triggered()
{
    //Reload configs
    configs.loadconfigs();

    //Set tools from loaded configs
    setTools();

    QMessageBox::information(this, tr("Reload configuration"),
     tr("Configuration succesfully reloaded!"),
     QMessageBox::Ok);
}



void MainWindow::on_actionExport_to_image_triggered()
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->ExportToImage_fn();
    }
}

void MainWindow::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    unsigned int test = index;
    if(ui->LVLPropsMusicNumber->hasFocus())
    {
        ui->LVLPropsMusicCustomEn->setChecked(  test == configs.music_custom_id );
    }

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
    }

    setMusic(ui->LVLPropsMusicPlay->isChecked());
}

void MainWindow::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( configs.music_custom_id );
            if(activeChildWindow()==1)
            {
                activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
            }
        }
    }
}

void MainWindow::on_LVLPropsMusicCustomBrowse_clicked()
{
    QString dirPath;
    if(activeChildWindow()==1)
    {
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else return;

    MusicFileList musicList(dirPath);
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText(musicList.SelectedFile);
    }

}

void MainWindow::on_LVLPropsMusicPlay_toggled(bool checked)
{
    setMusic(checked);
}

void MainWindow::setMusic(bool checked)
{
    QString dirPath;
    QString musicFile;
    QString musicFilePath;
    bool silent;
    unsigned int CurMusNum;
    QMediaPlaylist * CurrentMusic = new QMediaPlaylist;

    if(activeChildWindow()==1)
    {
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else return;

    if(ui->LVLPropsMusicNumber->currentIndex()==0)
        silent=true;
    else
        silent=false;

    if(!silent)
        CurMusNum = ui->LVLPropsMusicNumber->currentIndex()-1;


    if(!silent)
    {
        if(CurMusNum==configs.music_custom_id-1)
        {
            musicFile = ui->LVLPropsMusicCustom->text();
            musicFilePath = dirPath + "/" + musicFile;
        }
        else
        {
            musicFile = configs.main_music_lvl[CurMusNum].file;
            dirPath = configs.dirs.music;
            musicFilePath = configs.dirs.music + musicFile;
            //QMessageBox::information(this, "test", "music is \n"+musicFile+"\n"+QString::number());
        }

        if(checked)
        {
            if(QFile::exists(musicFilePath) )
            {
                CurrentMusic->addMedia(QUrl::fromLocalFile( musicFilePath ));
                CurrentMusic->setPlaybackMode(QMediaPlaylist::Loop);
                MusicPlayer->stop();
                MusicPlayer->setPlaylist(CurrentMusic);
                MusicPlayer->setVolume(100);
                MusicPlayer->play();
            }
        }
        else
        {
            MusicPlayer->stop();
        }
    }
    else MusicPlayer->stop();
}



void MainWindow::on_LVLPropsMusicCustom_textChanged(const QString &arg1)
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file = arg1;
    }

    setMusic( ui->LVLPropsMusicPlay->isChecked() );
}
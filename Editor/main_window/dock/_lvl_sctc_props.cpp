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

#include <audio/music_player.h>
#include <editing/_dialogs/musicfilelist.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include "lvl_sctc_props.h"

// Level Section tool box show/hide
void MainWindow::on_LevelSectionSettings_visibilityChanged(bool visible)
{
        ui->actionSection_Settings->setChecked(visible);
}

void MainWindow::on_actionSection_Settings_triggered(bool checked)
{
    ui->LevelSectionSettings->setVisible(checked);
    if(checked) ui->LevelSectionSettings->raise();
}



bool lockSctSettingsProps=false;


// ////////////////Set LevelSection data//////////////////////////////////
void MainWindow::setLevelSectionData()
{
    lockSctSettingsProps=true;

    int i;
        WriteToLog(QtDebugMsg, QString("Set level Section Data"));
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    ui->LVLEvent_SctMus_List->clear(); //Music list in events
    ui->LVLEvent_SctBg_List->clear();  //Background list in events

    QPixmap empty(100,70);
    empty.fill(QColor(Qt::black));

    ui->LVLPropsBackImage->addItem(QIcon(empty), tr("[No image]"), "0" );
    ui->LVLEvent_SctBg_List->addItem(QIcon(empty), tr("[No image]"), "0" );
    ui->LVLPropsMusicNumber->addItem( tr("[Silence]"), "0" );
    ui->LVLEvent_SctMus_List->addItem( tr("[Silence]"), "0" );

    ui->LVLPropsBackImage->setIconSize(QSize(100,70));
    ui->LVLEvent_SctBg_List->setIconSize(QSize(100,70));

    QAbstractItemView *abVw = ui->LVLPropsBackImage->view();
            QListView *listVw = qobject_cast<QListView*>(abVw);
            if (listVw) {
                listVw->setSpacing(2);
                listVw->setViewMode(QListView::IconMode);
                listVw->setUniformItemSizes(true);
            }

    abVw = ui->LVLEvent_SctBg_List->view();
            listVw = qobject_cast<QListView*>(abVw);
            if (listVw) {
                listVw->setSpacing(2);
                listVw->setViewMode(QListView::IconMode);
                listVw->setUniformItemSizes(true);
            }


    for(i=0; i< configs.main_bg.size();i++)
    {
        QPixmap bgThumb(100,70);
        bgThumb.fill(QColor(Qt::white));
        QPainter xx(&bgThumb);

        QPixmap tmp;
        tmp = configs.main_bg[i].image.scaledToHeight(70);

        if (activeChildWindow()==1)
        {
            LevelEdit * edit = activeLvlEditWin();
            for(int q=0; q<edit->scene->uBGs.size();q++)
            {
                if(edit->scene->uBGs[q].id==configs.main_bg[i].id)
                {
                    if(!edit->scene->uBGs[q].image.isNull())
                        tmp = edit->scene->uBGs[q].image.scaledToHeight(70);
                    break;
                }
            }
        }

        if(!tmp.isNull())
        {
            int d=0;
            for(int i=0; i<100; i+=tmp.width() )
            {
                xx.drawPixmap(i,0, tmp.width(), tmp.height(), tmp);
                d+=tmp.width();
            }
            if(d<100)
            {
                xx.drawPixmap(d,0, tmp.width()-(100-d), tmp.height(), tmp);
            }
        }
        xx.end();

        ui->LVLPropsBackImage->addItem(QIcon(bgThumb), configs.main_bg[i].name, QString::number(configs.main_bg[i].id));
        ui->LVLEvent_SctBg_List->addItem(QIcon(bgThumb), configs.main_bg[i].name, QString::number(configs.main_bg[i].id));
    }

    for(i=0; i< configs.main_music_lvl.size();i++)
    {
        ui->LVLPropsMusicNumber->addItem(configs.main_music_lvl[i].name, QString::number(configs.main_music_lvl[i].id) );
        ui->LVLEvent_SctMus_List->addItem(configs.main_music_lvl[i].name, QString::number(configs.main_music_lvl[i].id) );
    }

    //Set current data
    if (activeChildWindow()==1)
    {
        LevelEdit * edit = activeLvlEditWin();
        //edit->LvlData.sections[edit->LvlData.CurSection].background
        //edit->LvlData.sections[edit->LvlData.CurSection].music_id
        ui->LVLPropsBackImage->setCurrentIndex(0);
        for(int i=0;i<ui->LVLPropsBackImage->count();i++)
        {
            if((unsigned long)ui->LVLPropsBackImage->itemData(i).toInt() ==
                    edit->LvlData.sections[edit->LvlData.CurSection].background)
            {
                ui->LVLPropsBackImage->setCurrentIndex(i); break;
            }
        }

        ui->LVLPropsMusicNumber->setCurrentIndex(0);
        for(int i=0;i<ui->LVLPropsMusicNumber->count();i++)
        {
            if((unsigned long)ui->LVLPropsMusicNumber->itemData(i).toInt() ==
                    edit->LvlData.sections[edit->LvlData.CurSection].music_id)
            {
                ui->LVLPropsMusicNumber->setCurrentIndex(i); break;
            }
        }

        ui->LVLPropsLevelWarp->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].IsWarp);
        ui->actionLevWarp->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].IsWarp);
        ui->LVLPropsOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        ui->actionLevOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        ui->LVLPropsNoTBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].noback);
        ui->actionLevNoBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].noback);
        ui->LVLPropsUnderWater->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);
        ui->actionLevUnderW->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);
        ui->LVLPropsMusicCustom->setText(edit->LvlData.sections[edit->LvlData.CurSection].music_file);
        ui->LVLPropsMusicCustomEn->setChecked((activeLvlEditWin()->LvlData.sections[edit->LvlData.CurSection].music_id == configs.music_custom_id));
    }

    lockSctSettingsProps=false;
}





// Level Section Settings
void MainWindow::on_LVLPropsLevelWarp_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECISWARP, QVariant(checked));
        ui->actionLevWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevWarp_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECISWARP, QVariant(checked));
        ui->LVLPropsLevelWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}


void MainWindow::on_LVLPropsOffScr_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        ui->actionLevOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevOffScr_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        ui->LVLPropsOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}



void MainWindow::on_LVLPropsNoTBack_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECNOBACK, QVariant(checked));
        ui->actionLevNoBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevNoBack_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECNOBACK, QVariant(checked));
        ui->LVLPropsNoTBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}


void MainWindow::on_LVLPropsUnderWater_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECUNDERWATER, QVariant(checked));
        ui->actionLevUnderW->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevUnderW_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECUNDERWATER, QVariant(checked));
        ui->LVLPropsUnderWater->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}







///////////////////////////////////////Resize section/////////////////////////////////////
void MainWindow::on_ResizeSection_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->setFocus();
        if(activeLvlEditWin()->scene->pResizer==NULL)
        {
            activeLvlEditWin()->scene->setSectionResizer(true);
            ui->ResizeSection->setVisible(false);
            ui->applyResize->setVisible(true);
            ui->cancelResize->setVisible(true);
            ui->ResizingToolbar->setVisible(true);
        }
    }
}

void MainWindow::on_applyResize_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setSectionResizer(false, true);
        ui->ResizeSection->setVisible(true);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);
        ui->ResizingToolbar->setVisible(false);
    }
}

void MainWindow::on_cancelResize_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setSectionResizer(false, false);
        ui->ResizeSection->setVisible(true);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);
        ui->ResizingToolbar->setVisible(false);
    }
}


// ////////////////////////////////////////////////////////////////////////////////


void MainWindow::on_LVLPropsBackImage_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    if(configs.main_bg.size()==0)
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for backgrounds not loaded"));
        return;
    }

    //if(ui->LVLPropsBackImage->hasFocus())
    //{
    ui->LVLPropsBackImage->setEnabled(false);
    WriteToLog(QtDebugMsg, "Change BG to "+QString::number(index));
    if (activeChildWindow()==1)
    {
        QList<QVariant> backData;
        backData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].background);
        backData.push_back(ui->LVLPropsBackImage->currentData().toInt());
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECBACKGROUNDIMG, QVariant(backData));
        activeLvlEditWin()->scene->ChangeSectionBG(ui->LVLPropsBackImage->currentData().toInt());
        activeLvlEditWin()->LvlData.modified = true;
    }
    ui->LVLPropsBackImage->setEnabled(true);
//    }
//    else
//    {
//        if (activeChildWindow()==1)
//        {
//           ui->LVLPropsBackImage->setCurrentIndex(
//                       activeLvlEditWin()->LvlData.sections[
//                       activeLvlEditWin()->LvlData.CurSection].background);
//        }
//    }
}





void MainWindow::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    unsigned int test = index;
//    if(ui->LVLPropsMusicNumber->hasFocus())
//    {
        ui->LVLPropsMusicCustomEn->setChecked(  test == configs.music_custom_id );
//    }

    if(activeChildWindow()==1)
    {
        QList<QVariant> musicData;
        musicData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id);
        musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECMUSIC, QVariant(musicData));
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        if(ui->LVLPropsMusicNumber->hasFocus()) activeLvlEditWin()->LvlData.modified = true;
    }

    WriteToLog(QtDebugMsg, "Call to Set Music if playing");
    setMusic(ui->actionPlayMusic->isChecked());
}

void MainWindow::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(lockSctSettingsProps) return;

    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( configs.music_custom_id );
            if(activeChildWindow()==1)
            {
                QList<QVariant> musicData;
                musicData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id);
                musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
                activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECMUSIC, QVariant(musicData));
                activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
                activeLvlEditWin()->LvlData.modified = true;
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

    if(activeLvlEditWin()->isUntitled)
    {
        QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
        return;
    }

    MusicFileList musicList( dirPath, ui->LVLPropsMusicCustom->text() );
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText( musicList.SelectedFile );
        ui->LVLPropsMusicCustom->setModified(true);
        on_LVLPropsMusicCustom_editingFinished();
    }
}

void MainWindow::on_LVLPropsMusicCustom_editingFinished()//_textChanged(const QString &arg1)
{
    if(lockSctSettingsProps) return;
    if(!ui->LVLPropsMusicCustom->isModified()) return;
    ui->LVLPropsMusicCustom->setModified(false);

    if(activeChildWindow()==1)
    {
        QString arg1 = ui->LVLPropsMusicCustom->text();

        QList<QVariant> cusMusicData;
        cusMusicData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file);
        cusMusicData.push_back(arg1.simplified().remove('\"'));
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECCUSTOMMUSIC, QVariant(cusMusicData));
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file = arg1.simplified().remove('\"');
    }

    setMusic( ui->actionPlayMusic->isChecked() );
}
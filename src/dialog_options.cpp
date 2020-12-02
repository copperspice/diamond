/**************************************************************************
*
* Copyright (c) 2012-2020 Barbara Geller
*
* Diamond Editor is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 2
* as published by the Free Software Foundation.
*
* Diamond is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
***************************************************************************/

#include "dialog_options.h"
#include "util.h"
#include "overlord.h"
#include "dialog_edt_help.h"
#include "diamondlimits.h"

#include <QFileDialog>
#include <QKeySequence>
#include <QLineEdit>
#include <QString>
#include <QToolButton>
#include <QStringParser>

#include <qglobal.h>

Dialog_Options::Dialog_Options( QWidget *parent )
    : QDialog( parent ), m_ui( new Ui::Dialog_Options )
{
    m_options  = Overlord::getInstance()->pullLocalCopyOfOptions();

    m_ui->setupUi( this );
    this->setWindowIcon( QIcon( "://resources/diamond.png" ) );
    m_ui->keypad_label->setPixmap( QPixmap( "://resources/EDT-Keypad.png" ) );

    QValidator *validator = new QIntValidator( 3, DiamondLimits::BACKUP_VERSION_MAX, this );
    m_ui->maxVersions->setValidator( validator );

    initData();

    connect( m_ui->backupDirectory_TB,  &QToolButton::clicked, this, &Dialog_Options::pick_BackupDirectory );
    connect( m_ui->dictMain_TB,         &QToolButton::clicked, this, &Dialog_Options::pick_Main );
    connect( m_ui->dictUser_TB,         &QToolButton::clicked, this, &Dialog_Options::pick_User );
    connect( m_ui->syntax_TB,           &QToolButton::clicked, this, &Dialog_Options::pick_Syntax );

    connect( m_ui->reset_PB,            &QPushButton::clicked, this, &Dialog_Options::reset_StandardKey );
    connect( m_ui->save_PB,             &QPushButton::clicked, this, &Dialog_Options::save );
    connect( m_ui->cancel_PB,           &QPushButton::clicked, this, &Dialog_Options::cancel );
    connect( m_ui->edtHelp_PB,          &QPushButton::clicked, this, &Dialog_Options::show_help );

    m_ui->tabWidget->setCurrentIndex( 0 );
}

Dialog_Options::~Dialog_Options()
{
    delete m_ui;
}

void Dialog_Options::initData()
{
    QStringList list;
    int index;

    // ** tab one

    // 1
    list << "MM/dd/yyyy" << "dd/MM/yyyy" << "MMM dd, yyyy"
         << "MMMM dd, yyyy" << "yyyyMMdd";

    m_ui->dateFormat_CB->addItems( list );
    m_ui->dateFormat_CB->setEditable( false );

    index = m_ui->dateFormat_CB->findText( m_options.formatDate() );
    m_ui->dateFormat_CB->setCurrentIndex( index );

    // 2
    list.clear();
    list << "hh:mm" << "hh:mm:ss" << "h:m:s ap" << "h:mm ap";

    m_ui->timeFormat_CB->addItems( list );
    m_ui->timeFormat_CB->setEditable( false );

    index = m_ui->timeFormat_CB->findText( m_options.formatTime() );
    m_ui->timeFormat_CB->setCurrentIndex( index );

    // 3
    list.clear();
    list << "3" << "4" << "8";
    m_ui->tabSpacing_CB->addItems( list );
    m_ui->tabSpacing_CB->setEditable( false );

    QString mxV = QString( "%1" ).formatArg( m_options.maxVersions() );
    m_ui->maxVersions->setText( mxV );

    index = m_ui->tabSpacing_CB->findText( QString::number( m_options.tabSpacing() ) );
    m_ui->tabSpacing_CB->setCurrentIndex( index );

    m_ui->useSpaces_CKB->setChecked( m_options.useSpaces() );

    m_ui->removeSpace_CKB->setChecked( m_options.removeSpaces() );

    m_ui->autoLoad_CKB->setChecked( m_options.autoLoad() );

    m_ui->enable_EDT_CKB->setChecked( m_options.keys().edtEnabled() );

    m_ui->astyle_CKB->setChecked( m_options.astyleOnSave() );
    m_ui->createBackups_CKB->setChecked( m_options.makeBackups() );

    m_ui->backupDirectory->setText( m_options.backupDirectory() );
    m_ui->dictMain->setText( m_options.mainDictionary() );
    m_ui->dictMain->setCursorPosition( 0 );

    m_ui->dictUser->setText( m_options.userDictionary() );
    m_ui->dictUser->setCursorPosition( 0 );

    m_ui->syntax->setText( m_options.syntaxPath() );
    m_ui->syntax->setCursorPosition( 0 );

    m_ui->clipper_CKB->setChecked( m_options.preloadClipper() );
    m_ui->cmake_CKB->setChecked( m_options.preloadCmake() );
    m_ui->cpp_CKB->setChecked( m_options.preloadCpp() );
    m_ui->css_CKB->setChecked( m_options.preloadCss() );
    m_ui->doxy_CKB->setChecked( m_options.preloadDoxy() );
    m_ui->errlog_CKB->setChecked( m_options.preloadErrLog() );
    m_ui->html_CKB->setChecked( m_options.preloadHtml() );
    m_ui->java_CKB->setChecked( m_options.preloadJava() );
    m_ui->js_CKB->setChecked( m_options.preloadJs() );
    m_ui->json_CKB->setChecked( m_options.preloadJson() );
    m_ui->make_CKB->setChecked( m_options.preloadMake() );
    m_ui->none_CKB->setChecked( m_options.preloadNone() );
    m_ui->nsi_CKB->setChecked( m_options.preloadNSI() );
    m_ui->php_CKB->setChecked( m_options.preloadPhp() );
    m_ui->pl_CKB->setChecked( m_options.preloadPl() );
    m_ui->py_CKB->setChecked( m_options.preloadPy() );
    m_ui->sh_CKB->setChecked( m_options.preloadSh() );
    m_ui->txt_CKB->setChecked( m_options.preloadTxt() );
    m_ui->xml_CKB->setChecked( m_options.preloadXml() );


    // ** tab two
    m_ui->key_open->setText( m_options.keys().open() );
    m_ui->key_close->setText( m_options.keys().close() );
    m_ui->key_save->setText( m_options.keys().save() );
    m_ui->key_saveAs->setText( m_options.keys().saveAs() );
    m_ui->key_print->setText( m_options.keys().print() );
    m_ui->key_undo->setText( m_options.keys().undo() );
    m_ui->key_redo->setText( m_options.keys().redo() );
    m_ui->key_cut->setText( m_options.keys().cut() );
    m_ui->key_copy->setText( m_options.keys().copy() );
    m_ui->key_paste->setText( m_options.keys().paste() );
    m_ui->key_selectAll->setText( m_options.keys().selectAll() );
    m_ui->key_find->setText( m_options.keys().find() );
    m_ui->key_replace->setText( m_options.keys().replace() );
    m_ui->key_findNext->setText( m_options.keys().findNext() );
    m_ui->key_findPrev->setText( m_options.keys().findPrev() );
    m_ui->key_goTop->setText( m_options.keys().goTop() );
    m_ui->key_goBottom->setText( m_options.keys().goBottom() );
    m_ui->key_newTab->setText( m_options.keys().newTab() );

    // ** tab three
    m_ui->key_printPreview->setText( m_options.keys().printPreview() );
    m_ui->key_reload->setText( m_options.keys().reload() );
    m_ui->key_selectLine->setText( m_options.keys().selectLine() );
    m_ui->key_selectWord->setText( m_options.keys().selectWord() );
    m_ui->key_selectBlock->setText( m_options.keys().selectBlock() );
    m_ui->key_upper->setText( m_options.keys().upper() );
    m_ui->key_lower->setText( m_options.keys().lower() );
    m_ui->key_indentIncrement->setText( m_options.keys().indentIncrement() );
    m_ui->key_indentDecrement->setText( m_options.keys().indentDecrement() );
    m_ui->key_deleteLine->setText( m_options.keys().deleteLine() );
    m_ui->key_deleteToEOL->setText( m_options.keys().deleteToEOL() );
    m_ui->key_deleteThroughEOL->setText( m_options.keys().deleteThroughEOL() );
    m_ui->key_columnMode->setText( m_options.keys().columnMode() );
    m_ui->key_goLine->setText( m_options.keys().gotoLine() );
    m_ui->key_show_Spaces->setText( m_options.keys().showSpaces() );
    m_ui->key_show_Breaks->setText( m_options.keys().showBreaks() );
    m_ui->key_macroPlay->setText( m_options.keys().macroPlay() );
    m_ui->key_spellCheck->setText( m_options.keys().spellCheck() );
    m_ui->key_copyBuffer->setText( m_options.keys().copyBuffer() );

    // ** tab four
    m_ui->key_EDT_GotoLine->setText( m_options.keys().edtGotoLine() );
    m_ui->numLock_RB->setChecked( m_options.keys().numLockGold() );
    m_ui->scrollLock_RB->setChecked( m_options.keys().scrollLockGold() );
    m_ui->edtWordCtrlMeta_RB->setChecked( m_options.keys().edtWordCtrlMeta() );
    m_ui->edtWordAltOption_RB->setChecked( m_options.keys().edtWordAltOption() );
    m_ui->key_EDT_Copy->setText( m_options.keys().edtCopy() );
    m_ui->key_EDT_InsertFile->setText( m_options.keys().edtInsertFile() );
    m_ui->key_EDT_UpperCase->setText( m_options.keys().edtUpperCase() );
    m_ui->key_EDT_LowerCase->setText( m_options.keys().edtLowerCase() );
    m_ui->key_EDT_SplitHorizontal->setText( m_options.keys().edtSplitHorizontal() );
    m_ui->key_EDT_SplitVertical->setText( m_options.keys().edtSplitVertical() );
    m_ui->key_EDT_SaveFile->setText( m_options.keys().edtSaveFile() );
    m_ui->key_EDT_Astyle->setText( m_options.keys().edtAstyle() );
    m_ui->f12AsBackspace_CKB->setChecked( m_options.keys().f12AsBackspace() );

}

void Dialog_Options::save()
{
    // ** tab 1
    m_options.set_formatDate( m_ui->dateFormat_CB->currentText() );
    m_options.set_formatTime( m_ui->timeFormat_CB->currentText() );
    m_options.set_backupDirectory( m_ui->backupDirectory->text() );
    m_options.set_mainDictionary( m_ui->dictMain->text() );
    m_options.set_userDictionary( m_ui->dictUser->text() );
    m_options.set_syntaxPath( m_ui->syntax->text() );

    QString value = m_ui->tabSpacing_CB->currentText();
    m_options.set_tabSpacing( value.toInteger<int>() );

    m_options.set_useSpaces( m_ui->useSpaces_CKB->isChecked() );
    m_options.set_removeSpaces( m_ui->removeSpace_CKB->isChecked() );
    m_options.set_autoLoad( m_ui->autoLoad_CKB->isChecked() );
    m_options.keys().set_edtEnabled( m_ui->enable_EDT_CKB->isChecked() );
    m_options.set_astyleOnSave( m_ui->astyle_CKB->isChecked() );
    m_options.set_makeBackups( m_ui->createBackups_CKB->isChecked() );
    value = m_ui->maxVersions->text();
    m_options.set_maxVersions( value.toInteger<int>() );

    m_options.set_preloadClipper( m_ui->clipper_CKB->isChecked() );
    m_options.set_preloadCmake( m_ui->cmake_CKB->isChecked() );
    m_options.set_preloadCpp( m_ui->cpp_CKB->isChecked() );
    m_options.set_preloadCss( m_ui->css_CKB->isChecked() );
    m_options.set_preloadDoxy( m_ui->doxy_CKB->isChecked() );
    m_options.set_preloadErrLog( m_ui->errlog_CKB->isChecked() );
    m_options.set_preloadHtml( m_ui->html_CKB->isChecked() );
    m_options.set_preloadJava( m_ui->java_CKB->isChecked() );
    m_options.set_preloadJs( m_ui->js_CKB->isChecked() );
    m_options.set_preloadJson( m_ui->json_CKB->isChecked() );
    m_options.set_preloadMake( m_ui->make_CKB->isChecked() );
    m_options.set_preloadNone( m_ui->none_CKB->isChecked() );
    m_options.set_preloadNSI( m_ui->nsi_CKB->isChecked() );
    m_options.set_preloadPhp( m_ui->php_CKB->isChecked() );
    m_options.set_preloadPl( m_ui->pl_CKB->isChecked() );
    m_options.set_preloadPy( m_ui->py_CKB->isChecked() );
    m_options.set_preloadSh( m_ui->sh_CKB->isChecked() );
    m_options.set_preloadTxt( m_ui->txt_CKB->isChecked() );
    m_options.set_preloadXml( m_ui->xml_CKB->isChecked() );

    // ** tab 2
    m_options.keys().set_open( m_ui->key_open->text() );
    m_options.keys().set_close( m_ui->key_close->text() );
    m_options.keys().set_save( m_ui->key_save->text() );
    m_options.keys().set_saveAs( m_ui->key_saveAs->text() );
    m_options.keys().set_print( m_ui->key_print->text() );
    m_options.keys().set_undo( m_ui->key_undo->text() );
    m_options.keys().set_redo( m_ui->key_redo->text() );
    m_options.keys().set_cut( m_ui->key_cut->text() );
    m_options.keys().set_copy( m_ui->key_copy->text() );
    m_options.keys().set_paste( m_ui->key_paste->text() );
    m_options.keys().set_selectAll( m_ui->key_selectAll->text() );
    m_options.keys().set_find( m_ui->key_find->text() );
    m_options.keys().set_replace( m_ui->key_replace->text() );
    m_options.keys().set_findNext( m_ui->key_findNext->text() );
    m_options.keys().set_findPrev( m_ui->key_findPrev->text() );
    m_options.keys().set_goTop( m_ui->key_goTop->text() );
    m_options.keys().set_goBottom( m_ui->key_goBottom->text() );
    m_options.keys().set_newTab( m_ui->key_newTab->text() );

    // ** tab 3
    m_options.keys().set_printPreview( m_ui->key_printPreview->text() );
    m_options.keys().set_reload( m_ui->key_reload->text() );
    m_options.keys().set_selectLine( m_ui->key_selectLine->text() );
    m_options.keys().set_selectWord( m_ui->key_selectWord->text() );
    m_options.keys().set_selectBlock( m_ui->key_selectBlock->text() );
    m_options.keys().set_upper( m_ui->key_upper->text() );
    m_options.keys().set_lower( m_ui->key_lower->text() );
    m_options.keys().set_indentIncrement( m_ui->key_indentIncrement->text() );
    m_options.keys().set_indentDecrement( m_ui->key_indentDecrement->text() );
    m_options.keys().set_deleteLine( m_ui->key_deleteLine->text() );
    m_options.keys().set_deleteToEOL( m_ui->key_deleteToEOL->text() );
    m_options.keys().set_deleteThroughEOL( m_ui->key_deleteThroughEOL->text() );
    m_options.keys().set_columnMode( m_ui->key_columnMode->text() );
    m_options.keys().set_gotoLine( m_ui->key_goLine->text() );
    m_options.keys().set_showSpaces( m_ui->key_show_Spaces->text() );
    m_options.keys().set_showBreaks( m_ui->key_show_Breaks->text() );
    m_options.keys().set_macroPlay( m_ui->key_macroPlay->text() );
    m_options.keys().set_spellCheck( m_ui->key_spellCheck->text() );
    m_options.keys().set_copyBuffer( m_ui->key_copyBuffer->text() );

    // ** tab 4
    m_options.keys().set_edtGotoLine( m_ui->key_EDT_GotoLine->text() );
    m_options.keys().set_numLockGold( m_ui->numLock_RB->isChecked() );
    m_options.keys().set_scrollLockGold( m_ui->scrollLock_RB->isChecked() );
    m_options.keys().set_edtCopy( m_ui->key_EDT_Copy->text() );
    m_options.keys().set_edtInsertFile( m_ui->key_EDT_InsertFile->text() );
    m_options.keys().set_edtUpperCase( m_ui->key_EDT_UpperCase->text() );
    m_options.keys().set_edtLowerCase( m_ui->key_EDT_LowerCase->text() );
    m_options.keys().set_edtSplitHorizontal( m_ui->key_EDT_SplitHorizontal->text() );
    m_options.keys().set_edtSplitVertical( m_ui->key_EDT_SplitVertical->text() );
    m_options.keys().set_edtSaveFile( m_ui->key_EDT_SaveFile->text() );
    m_options.keys().set_edtAstyle( m_ui->key_EDT_Astyle->text() );
    m_options.keys().set_f12AsBackspace( m_ui->f12AsBackspace_CKB->isChecked() );

    Overlord::getInstance()->updateOptionsFromLocalCopy( m_options );

    this->done( QDialog::Accepted );
}

void Dialog_Options::cancel()
{
    this->done( QDialog::Rejected );
}

void Dialog_Options::pick_Main()
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString fileName = QFileDialog::getOpenFileName( this, tr( "Select Main Dictionary" ),
                       m_ui->dictMain->text(), tr( "Dictionary File (*.dic)" ), &selectedFilter, options );

    if ( ! fileName.isEmpty() )
    {
        m_ui->dictMain->setText( fileName );
    }
}

void Dialog_Options::pick_BackupDirectory()
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString dir = QFileDialog::getExistingDirectory( this, tr( "Open Directory" ),
                  QDir::homePath(),
                  QFileDialog::ShowDirsOnly
                  | QFileDialog::DontResolveSymlinks );

    if ( ! dir.isEmpty() )
    {
        m_ui->backupDirectory->setText( dir );
    }
}

void Dialog_Options::pick_User()
{
    QString selectedFilter;
    QFileDialog::Options options;

    // force windows 7 and 8 to honor initial path
    options = QFileDialog::ForceInitialDir_Win7;

    QString fileName = QFileDialog::getOpenFileName( this, tr( "Select User Dictionary" ),
                       m_ui->dictUser->text(), tr( "User Dictionary File (*.txt)" ), &selectedFilter, options );

    if ( ! fileName.isEmpty() )
    {
        m_ui->dictUser->setText( fileName );
    }
}

void Dialog_Options::pick_Syntax()
{
    QString msg  = tr( "Select Diamond Syntax Folder" );
    QString path = m_ui->syntax->text();

    path = get_DirPath( this, msg, path );

    if ( ! path.isEmpty() )
    {
        m_ui->syntax->setText( path );
    }
}

void Dialog_Options::reset_StandardKey()
{
    m_options.keys().setDefaultKeyValues();
    initData();
}

void Dialog_Options::show_help()
{
    Dialog_Edt_Help *dw = new Dialog_Edt_Help( this );
    dw->exec();

    delete dw;
}

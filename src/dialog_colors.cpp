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

#include "dialog_colors.h"
#include "util.h"
#include "non_gui_functions.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QLineEdit>
#include <QPalette>
#include <QString>
#include <QStringList>
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

Dialog_Colors::Dialog_Colors( QWidget *parent )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_Colors )
{
    m_ui->setupUi( this );
    this->setWindowIcon( QIcon( "://resources/diamond.png" ) );

    m_localSettings = Overlord::getInstance()->pullLocalSettingsCopy();
    m_syntaxFname  = m_localSettings.syntaxPath() + "syn_cpp.json";

    m_ui->sample->runSyntax( m_syntaxFname );

    connect( m_ui->text_TB,           &QToolButton::clicked, this, &Dialog_Colors::text_TB       );
    connect( m_ui->back_TB,           &QToolButton::clicked, this, &Dialog_Colors::back_TB       );
    connect( m_ui->gutterText_TB,     &QToolButton::clicked, this, &Dialog_Colors::gutterText_TB );
    connect( m_ui->gutterBack_TB,     &QToolButton::clicked, this, &Dialog_Colors::gutterBack_TB );
    connect( m_ui->current_TB,        &QToolButton::clicked, this, &Dialog_Colors::current_TB    );

    connect( m_ui->key_TB,            &QToolButton::clicked, this, &Dialog_Colors::key_TB      );
    connect( m_ui->type_TB,           &QToolButton::clicked, this, &Dialog_Colors::type_TB     );
    connect( m_ui->class_TB,          &QToolButton::clicked, this, &Dialog_Colors::class_TB    );
    connect( m_ui->func_TB,           &QToolButton::clicked, this, &Dialog_Colors::func_TB     );
    connect( m_ui->quote_TB,          &QToolButton::clicked, this, &Dialog_Colors::quote_TB    );
    connect( m_ui->comment_TB,        &QToolButton::clicked, this, &Dialog_Colors::comment_TB  );
    connect( m_ui->mline_TB,          &QToolButton::clicked, this, &Dialog_Colors::mline_TB    );
    connect( m_ui->const_TB,          &QToolButton::clicked, this, &Dialog_Colors::const_TB    );

    connect( m_ui->key_Bold_CB,       &QCheckBox::clicked,   this, &Dialog_Colors::key_bold       );
    connect( m_ui->key_Italic_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::key_italic     );
    connect( m_ui->type_Bold_CB,      &QCheckBox::clicked,   this, &Dialog_Colors::type_bold      );
    connect( m_ui->type_Italic_CB,    &QCheckBox::clicked,   this, &Dialog_Colors::type_italic    );
    connect( m_ui->class_Bold_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::class_bold     );
    connect( m_ui->class_Italic_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::class_italic   );
    connect( m_ui->func_Bold_CB,      &QCheckBox::clicked,   this, &Dialog_Colors::func_bold      );
    connect( m_ui->func_Italic_CB,    &QCheckBox::clicked,   this, &Dialog_Colors::func_italic    );
    connect( m_ui->quote_Bold_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::quote_bold     );
    connect( m_ui->quote_Italic_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::quote_italic   );
    connect( m_ui->comment_Bold_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::comment_bold   );
    connect( m_ui->comment_Italic_CB, &QCheckBox::clicked,   this, &Dialog_Colors::comment_italic );
    connect( m_ui->mline_Bold_CB,     &QCheckBox::clicked,   this, &Dialog_Colors::mline_bold     );
    connect( m_ui->mline_Italic_CB,   &QCheckBox::clicked,   this, &Dialog_Colors::mline_italic   );

    connect( m_ui->save_PB,           &QPushButton::clicked, this, &Dialog_Colors::save          );
    connect( m_ui->cancel_PB,         &QPushButton::clicked, this, &Dialog_Colors::cancel        );
    connect( m_ui->copy_button,       &QPushButton::clicked, this, &Dialog_Colors::copyClicked   );
    connect( m_ui->delete_button,     &QPushButton::clicked, this, &Dialog_Colors::deleteClicked );
    connect( m_ui->export_button,     &QPushButton::clicked, this, &Dialog_Colors::exportClicked );
    connect( m_ui->import_button,     &QPushButton::clicked, this, &Dialog_Colors::importClicked );

    connect( m_ui->sample, &DiamondTextEdit::cursorPositionChanged, m_ui->sample, &DiamondTextEdit::moveBar );

    rebuildComboBox( false );
}

Dialog_Colors::~Dialog_Colors()
{
    delete m_ui;
}

void Dialog_Colors::colorBox( QLineEdit *field, QColor color )
{
    QPalette temp;

    temp = field->palette();
    temp.setColor( QPalette::Base, color );
    field->setPalette( temp );
}

void Dialog_Colors::themeChanged( const QString &themeName )
{
    if ( themeName.isEmpty() )
    {
        qDebug() << "**** themeName was empty";
        return;
    }

    m_localSettings.set_activeTheme( themeName );

    m_ui->sample->changeSettings( &m_localSettings );
    // ordinary text
    m_ui->text_Color->setReadOnly( true );
    colorBox( m_ui->text_Color, m_localSettings.currentTheme()->colorText() );

    m_ui->back_Color->setReadOnly( true );
    colorBox( m_ui->back_Color, m_localSettings.currentTheme()->colorBack() );

    // line number margin/gutter
    m_ui->gutterText_Color->setReadOnly( true );
    colorBox( m_ui->gutterText_Color, m_localSettings.currentTheme()->gutterText() );

    m_ui->gutterBack_Color->setReadOnly( true );
    colorBox( m_ui->gutterBack_Color, m_localSettings.currentTheme()->gutterBack() );

    // current line background color
    m_ui->current_Color->setReadOnly( true );
    colorBox( m_ui->current_Color, m_localSettings.currentTheme()->currentLineBack() );

    /* Key */
    m_ui->key_Color->setReadOnly( true );
    colorBox( m_ui->key_Color, m_localSettings.currentTheme()->syntaxKey().color() );

    if ( m_localSettings.currentTheme()->syntaxKey().weight() == QFont::Bold )
    {
        m_ui->key_Bold_CB->setChecked( true );
    }

    m_ui->key_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxKey().italic() );

    /* Type */
    m_ui->type_Color->setReadOnly( true );
    colorBox( m_ui->type_Color, m_localSettings.currentTheme()->syntaxType().color() );

    if ( m_localSettings.currentTheme()->syntaxType().weight() == QFont::Bold )
    {
        m_ui->type_Bold_CB->setChecked( true );
    }

    m_ui->type_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxType().italic() );

    /* Class */
    m_ui->class_Color->setReadOnly( true );
    colorBox( m_ui->class_Color, m_localSettings.currentTheme()->syntaxClass().color() );

    if ( m_localSettings.currentTheme()->syntaxClass().weight() == QFont::Bold )
    {
        m_ui->class_Bold_CB->setChecked( true );
    }

    m_ui->class_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxClass().italic() );

    /* Func */
    m_ui->func_Color->setReadOnly( true );
    colorBox( m_ui->func_Color, m_localSettings.currentTheme()->syntaxFunc().color() );

    if ( m_localSettings.currentTheme()->syntaxFunc().weight() == QFont::Bold )
    {
        m_ui->func_Bold_CB->setChecked( true );
    }

    m_ui->func_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxFunc().italic() );

    /* Quote */
    m_ui->quote_Color->setReadOnly( true );
    colorBox( m_ui->quote_Color, m_localSettings.currentTheme()->syntaxQuote().color() );

    if ( m_localSettings.currentTheme()->syntaxQuote().weight() == QFont::Bold )
    {
        m_ui->quote_Bold_CB->setChecked( true );
    }

    m_ui->quote_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxQuote().italic() );

    /* Comment */
    m_ui->comment_Color->setReadOnly( true );
    colorBox( m_ui->comment_Color, m_localSettings.currentTheme()->syntaxComment().color() );

    if ( m_localSettings.currentTheme()->syntaxComment().weight() == QFont::Bold )
    {
        m_ui->comment_Bold_CB->setChecked( true );
    }

    m_ui->comment_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxComment().italic() );

    /* MLine comment*/
    m_ui->mline_Color->setReadOnly( true );
    colorBox( m_ui->mline_Color, m_localSettings.currentTheme()->syntaxMLine().color() );

    if ( m_localSettings.currentTheme()->syntaxMLine().weight() == QFont::Bold )
    {
        m_ui->mline_Bold_CB->setChecked( true );
    }

    m_ui->mline_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxMLine().italic() );

    /* Constant */
    m_ui->const_Color->setReadOnly( true );
    colorBox( m_ui->const_Color, m_localSettings.currentTheme()->syntaxConstant().color() );

    if ( m_localSettings.currentTheme()->syntaxConstant().weight() == QFont::Bold )
    {
        m_ui->constant_Bold_CB->setChecked( true );
    }

    m_ui->constant_Italic_CB->setChecked( m_localSettings.currentTheme()->syntaxConstant().italic() );

    if ( m_localSettings.currentTheme()->isProtected() )
    {
        m_ui->delete_button->setEnabled( false );
    }
    else
    {
        m_ui->delete_button->setEnabled( true );
    }


}

void Dialog_Colors::copyClicked()
{
    QString destName;
    bool okFlag = false;

    //  TODO:: Really need to get an AlphaNumeric hint added
    destName = QInputDialog::getText( this, "New Theme name", "Name: ", QLineEdit::Normal,
                                      destName, &okFlag );

    if ( okFlag && !destName.isEmpty() )
    {
        bool abortFlag = false;

        while ( nameCollision( destName, abortFlag ) && !abortFlag );

        m_localSettings.copyTheme( m_ui->theme_comboBox->currentText(), destName );
        rebuildComboBox( true );
    }
}

void Dialog_Colors::deleteClicked()
{
    qDebug() << "Theme: " << m_localSettings.currentTheme()->name() << "  protected: "
             << m_localSettings.currentTheme()->isProtected();

    if ( m_localSettings.currentTheme()->isProtected() )
    {
        QMessageBox::warning( this, "Delete", "Cannot delete pre-installed theme",
                              QMessageBox::Ok );
        return;
    }

    int btn = QMessageBox::question( this, "Confirm Deletion", "Really delete selected theme?",
                                     QMessageBox::Yes, QMessageBox::No );

    if ( btn == QMessageBox::Yes )
    {
        m_localSettings.deleteTheme( m_ui->theme_comboBox->currentText() );
        rebuildComboBox( true );
    }
}

void Dialog_Colors::exportClicked()
{
    QFileDialog *qfd = new QFileDialog( this, tr( "Destination File" ),
                                        QDir::homePath(), tr( "Diamond Theme Files (*.diamond_theme)" ) );

    qfd->setDefaultSuffix( "diamond_theme" );

    int rslt = qfd->exec();

    if ( rslt == QDialog::Accepted )
    {
        QStringList names = qfd->selectedFiles();

        if ( !names.isEmpty() )
        {
            QString fileName = names[0];
            QJsonObject object;

            QJsonArray list;

            // dummy
            list.append( 0 );
            list.append( true );
            list.append( 0 );

            object.insert( "theme-name", m_localSettings.currentTheme()->name() );
            object.insert( "theme-protected", m_localSettings.currentTheme()->isProtected() );
            object.insert( "theme-color-text", getRGBString( m_localSettings.currentTheme()->colorText() ) );
            object.insert( "theme-color-back", getRGBString( m_localSettings.currentTheme()->colorBack() ) );
            object.insert( "theme-color-gutterText", getRGBString( m_localSettings.currentTheme()->gutterText() ) );
            object.insert( "theme-color-gutterBack", getRGBString( m_localSettings.currentTheme()->gutterBack() ) );
            object.insert( "theme-color-currentLineBack", getRGBString( m_localSettings.currentTheme()->currentLineBack() ) );

            list.replace( 0, m_localSettings.currentTheme()->syntaxKey().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxKey().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxKey().color() ) );
            object.insert( "theme-syntax-key", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxType().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxType().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxType().color() ) );
            object.insert( "theme-syntax-type", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxClass().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxClass().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxClass().color() ) );
            object.insert( "theme-syntax-class", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxFunc().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxFunc().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxFunc().color() ) );
            object.insert( "theme-syntax-func", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxQuote().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxQuote().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxQuote().color() ) );
            object.insert( "theme-syntax-quote", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxComment().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxComment().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxComment().color() ) );
            object.insert( "theme-syntax-comment", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxMLine().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxMLine().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxMLine().color() ) );
            object.insert( "theme-syntax-mline", list );

            list.replace( 0, m_localSettings.currentTheme()->syntaxConstant().weight() );
            list.replace( 1, m_localSettings.currentTheme()->syntaxConstant().italic() );
            list.replace( 2, getRGBString( m_localSettings.currentTheme()->syntaxConstant().color() ) );
            object.insert( "theme-syntax-constant", list );

            QJsonDocument doc( object );
            QByteArray data = doc.toJson();

            QString path = pathName( fileName );
            QDir directory( path );

            if ( ! directory.exists() )
            {
                directory.mkpath( path );
            }

            QFile file( fileName );

            if ( ! file.open( QFile::WriteOnly | QFile::Text ) )
            {
                const QString msg = tr( "Unable to save export theme to: " )
                                    +  fileName + " : " + file.errorString();
                csError( tr( "Export Theme" ), msg );
                return;
            }

            file.write( data );
            file.close();
        }

    }
}

void Dialog_Colors::importClicked()
{
    QFileDialog *qfd = new QFileDialog( this, tr( "Import File" ),
                                        QDir::homePath(), tr( "Diamond Theme Files (*.diamond_theme)" ) );

    qfd->setDefaultSuffix( "diamond_theme" );

    int rslt = qfd->exec();

    if ( rslt == QDialog::Accepted )
    {
        QStringList names = qfd->selectedFiles();

        if ( !names.isEmpty() )
        {
            QString fileName = names[0];

            QByteArray data;

            QFile file( fileName );

            if ( ! file.open( QFile::ReadWrite | QFile::Text ) )
            {
                const QString msg = tr( "Unable to open Theme File: " ) +  fileName + " : " + file.errorString();
                csError( tr( "Read Theme" ), msg );
                return;
            }

            file.seek( 0 );
            data = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson( data );
            QJsonObject object = doc.object();
            QJsonValue value;

            QJsonArray list;
            TextAttributes attrib;

            // dummy
            list.append( 0 );
            list.append( true );
            list.append( 0 );

            QString themeName = object.value( "theme-name" ).toString();

            if ( themeName.isEmpty() )
            {
                csError( tr( "Theme Import" ), "Missing theme name in file" );
                return;
            }

            bool abortFlag = false;

            while ( nameCollision( themeName, abortFlag ) && !abortFlag );

            Themes *theme = new Themes( themeName, false );

            theme->set_colorBack( colorFromValueString( object.value( "theme-color-back" ).toString() ) );
            theme->set_currentLineBack( colorFromValueString( object.value( "theme-color-currentLineBack" ).toString() ) );
            theme->set_gutterBack( colorFromValueString( object.value( "theme-color-gutterBack" ).toString() ) );
            theme->set_gutterText( colorFromValueString( object.value( "theme-color-gutterText" ).toString() ) );
            theme->set_colorText( colorFromValueString( object.value( "theme-color-text" ).toString() ) );
            // TextAttribute array written as weight, italic-bool, color string
            //

            // syntax key
            //
            QJsonArray syn = object.value( "theme-syntax-key" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxKey( attrib );
            }

            // syntax type
            //
            syn = object.value( "theme-syntax-type" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxType( attrib );
            }

            // syntax class
            //
            syn = object.value( "theme-syntax-class" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxClass( attrib );
            }

            // syntax func
            //
            syn = object.value( "theme-syntax-func" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxFunc( attrib );
            }

            // syntax quote
            //
            syn = object.value( "theme-syntax-quote" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxQuote( attrib );
            }

            // syntax comment
            //
            syn = object.value( "theme-syntax-comment" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxComment( attrib );
            }

            // syntax mline
            //
            syn = object.value( "theme-syntax-mline" ).toArray();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxMLine( attrib );
            }

            syn = object.value( "theme-syntax-constant" ).toArray();

            qDebug() << "syn.count(): " << syn.count();

            if ( syn.count() > 2 )
            {
                attrib.set_weight( syn[0].toInt() );
                attrib.set_italic( syn[1].toBool() );
                qDebug() << "color: " << syn[2].toString();
                attrib.set_color( colorFromValueString( syn[2].toString() ) );
                theme->set_syntaxConstant( attrib );
            }

            m_localSettings.add_theme( theme );
            delete theme;
            rebuildComboBox( true );
        }
    }
}

void Dialog_Colors::rebuildComboBox( bool needToDisconnect )
{
    if ( needToDisconnect )
    {
        // stop needless screen updates while rebuilding combo
        disconnect( m_ui->theme_comboBox,
                    static_cast<void ( QComboBox::* )( const QString & )>( &QComboBox::currentIndexChanged ),
                    this,
                    &Dialog_Colors::themeChanged );

    }

    m_ui->theme_comboBox->clear();
    m_ui->theme_comboBox->addItems( m_localSettings.availableThemes() );
    m_ui->theme_comboBox->setCurrentIndex(
        m_ui->theme_comboBox->findText( m_localSettings.activeTheme() ) );

    // Do not connect before assigning data as that will cause syntax update with
    // ill-formed m_localSettings.
    //
    connect( m_ui->theme_comboBox,
             static_cast<void ( QComboBox::* )( const QString & )>( &QComboBox::currentIndexChanged ),
             this,
             &Dialog_Colors::themeChanged );

    themeChanged( m_localSettings.activeTheme() );
}

bool Dialog_Colors::nameCollision( QString &themeName, bool &abortFlag )
{

    bool retVal = false;

    if ( m_localSettings.themeNameExists( themeName ) )
    {
        abortFlag = false;

        //  TODO:: Really need to get an AlphaNumeric hint added
        themeName = QInputDialog::getText( this,
                                           "Theme name exists - New Theme name",
                                           "Name: ",
                                           QLineEdit::Normal,
                                           themeName, &abortFlag, Qt::Dialog, Qt::ImhNone );
    }

    return retVal;
}

QColor Dialog_Colors::pickColor( QColor oldColor )
{
    QColor color = oldColor;

    if ( false )       // (useNative)
    {
        color = QColorDialog::getColor( color, this );
    }
    else
    {
        color = QColorDialog::getColor( color, this, "Select Color", QColorDialog::DontUseNativeDialog );
    }

    if ( color.isValid() )
    {
        return color;
    }

    return oldColor;
}

void Dialog_Colors::text_TB()
{
    QColor color = m_localSettings.currentTheme()->colorText();
    m_localSettings.currentTheme()->set_colorText( pickColor( color ) );

    colorBox( m_ui->text_Color, m_localSettings.currentTheme()->colorText() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::back_TB()
{
    QColor color = m_localSettings.currentTheme()->colorBack();
    m_localSettings.currentTheme()->set_colorBack( pickColor( color ) );

    colorBox( m_ui->back_Color, m_localSettings.currentTheme()->colorBack() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::gutterText_TB()
{
    QColor color = m_localSettings.currentTheme()->gutterText();
    m_localSettings.currentTheme()->set_gutterText( pickColor( color ) );

    colorBox( m_ui->gutterText_Color, m_localSettings.currentTheme()->gutterText() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::gutterBack_TB()
{
    QColor color = m_localSettings.currentTheme()->gutterBack();
    m_localSettings.currentTheme()->set_gutterBack( pickColor( color ) );

    colorBox( m_ui->gutterBack_Color, m_localSettings.currentTheme()->gutterBack() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::current_TB()
{
    QColor color = m_localSettings.currentTheme()->currentLineBack();
    m_localSettings.currentTheme()->set_currentLineBack( pickColor( color ) );

    colorBox( m_ui->current_Color, m_localSettings.currentTheme()->currentLineBack() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::key_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxKey().color();
    m_localSettings.currentTheme()->syntaxKey().set_color( pickColor( color ) );

    colorBox( m_ui->key_Color, m_localSettings.currentTheme()->syntaxKey().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::type_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxType().color();
    m_localSettings.currentTheme()->syntaxType().set_color( pickColor( color ) );

    colorBox( m_ui->type_Color, m_localSettings.currentTheme()->syntaxType().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::class_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxClass().color();
    m_localSettings.currentTheme()->syntaxClass().set_color( pickColor( color ) );

    colorBox( m_ui->class_Color, m_localSettings.currentTheme()->syntaxClass().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::func_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxFunc().color();
    m_localSettings.currentTheme()->syntaxFunc().set_color( pickColor( color ) );

    colorBox( m_ui->func_Color, m_localSettings.currentTheme()->syntaxFunc().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::quote_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxQuote().color();
    m_localSettings.currentTheme()->syntaxQuote().set_color( pickColor( color ) );

    colorBox( m_ui->quote_Color, m_localSettings.currentTheme()->syntaxQuote().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::comment_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxComment().color();
    m_localSettings.currentTheme()->syntaxComment().set_color( pickColor( color ) );

    colorBox( m_ui->comment_Color, m_localSettings.currentTheme()->syntaxComment().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::mline_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxMLine().color();
    m_localSettings.currentTheme()->syntaxMLine().set_color( pickColor( color ) );

    colorBox( m_ui->mline_Color, m_localSettings.currentTheme()->syntaxMLine().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::const_TB()
{
    QColor color = m_localSettings.currentTheme()->syntaxConstant().color();
    m_localSettings.currentTheme()->syntaxConstant().set_color( pickColor( color ) );

    colorBox( m_ui->const_Color, m_localSettings.currentTheme()->syntaxConstant().color() );

    m_ui->sample->changeSettings( &m_localSettings );
}


void Dialog_Colors::key_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->key_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxKey().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::key_italic()
{
    bool b = false;

    if ( m_ui->key_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxKey().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::type_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->type_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxType().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::type_italic()
{
    bool b = false;

    if ( m_ui->type_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxType().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::class_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->class_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxClass().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::class_italic()
{
    bool b = false;

    if ( m_ui->class_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxClass().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::func_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->func_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxFunc().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::func_italic()
{
    bool b = false;

    if ( m_ui->func_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxFunc().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::quote_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->quote_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxQuote().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::quote_italic()
{
    bool b = false;

    if ( m_ui->quote_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxQuote().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::comment_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->comment_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxComment().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::comment_italic()
{
    bool b = false;

    if ( m_ui->comment_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxComment().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::mline_bold()
{
    QFont::Weight f = QFont::Normal;

    if ( m_ui->mline_Bold_CB->isChecked() )
    {
        f = QFont::Bold;
    }

    m_localSettings.currentTheme()->syntaxMLine().set_weight( f );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::mline_italic()
{
    bool b = false;

    if ( m_ui->mline_Italic_CB->isChecked() )
    {
        b = true;
    }

    m_localSettings.currentTheme()->syntaxMLine().set_italic( b );

    m_ui->sample->changeSettings( &m_localSettings );
}

void Dialog_Colors::save()
{
    // update Overlord
    //
    Overlord::getInstance()->updateSettingsFromLocalCopy( m_localSettings );
    this->done( QDialog::Accepted );
}

void Dialog_Colors::cancel()
{
    this->done( QDialog::Rejected );
}

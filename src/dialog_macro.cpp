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

#include "dialog_macro.h"
#include "util.h"
#include "overlord.h"

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QTableView>

Dialog_Macro::Dialog_Macro( QWidget *parent, MacroEnum enumValue )
    : QDialog( parent )
    , m_ui( new Ui::Dialog_Macro )
    , m_enum( enumValue )
{
    m_ui->setupUi( this );
    setWindowIcon( QIcon( "://resources/diamond.png" ) );

    setupTitle();
    setUpView();

    // alter the hightlight color
    QPalette temp = m_ui->tableView->palette();
    temp.setColor( QPalette::Highlight, QColor( 206, 206, 206, 255 ) );
    temp.setColor( QPalette::HighlightedText, Qt::black );
    m_ui->tableView->setPalette( temp );

    // highlight cell
    QModelIndex index = m_model->index( 0, 1 );
    m_ui->tableView->setCurrentIndex( index );

    // resize the dialog widget after the text has been displayed
    adjustSize();

    connect( m_ui->ok_PB,     &QPushButton::clicked, this, &Dialog_Macro::processEdits );
    connect( m_ui->view_PB,   &QPushButton::clicked, this, &Dialog_Macro::view );
    connect( m_ui->cancel_PB, &QPushButton::clicked, this, &Dialog_Macro::cancel );
}

Dialog_Macro::~Dialog_Macro()
{
    delete m_ui;
}

void Dialog_Macro::setupTitle()
{
    switch ( m_enum )
    {
        case MACRO_LOAD:
            setWindowTitle( "Load Macro" );
            break;

        case MACRO_MANAGE:
            setWindowTitle( "Edit Macro Names" );
            break;
    }
}

void Dialog_Macro::setUpView()
{
    m_model = new QStandardItemModel( this );
    m_model->setColumnCount( 2 );
    m_model->setHeaderData( 0, Qt::Horizontal, tr( "Delete" ) );
    m_model->setHeaderData( 1, Qt::Horizontal, tr( "Macro Name" ) );

    //
    m_ui->tableView->setModel( m_model );
    m_ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection );
    m_ui->tableView->setEditTriggers( QAbstractItemView::NoEditTriggers );

    m_ui->tableView->setColumnWidth( 1, 250 );

    // resize the last column
    m_ui->tableView->horizontalHeader()->setStretchLastSection( true );
    QBrush brush = QColor( 0,0,255 );

    // add data
    QStringList namesList = Overlord::getInstance()->macroNames();

    int row = 0;

    for ( QString nameStr : namesList )
    {
        QStandardItem *item0 = new QStandardItem( "" );
        item0->setCheckable( true );
        item0->setCheckState( Qt::Unchecked );
        item0->setTextAlignment( Qt::AlignHCenter );

        QStandardItem *item1 = new QStandardItem( nameStr );
        item1->setData( nameStr, ORIGINAL_ROLE );
        item1->setForeground( brush );
        item1->setEnabled( true );

        m_model->setItem( row, 0, item0 );
        m_model->setItem( row, 1, item1 );
        row++;
    }

    m_ui->tableView->setEditTriggers( QAbstractItemView::DoubleClicked );

    connect( m_model, &QStandardItemModel::dataChanged, this, &Dialog_Macro::tableDataChanged );
}

void Dialog_Macro::tableDataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    Q_UNUSED( topLeft )
    Q_UNUSED( bottomRight )
    m_updateNames = true;
}

void Dialog_Macro::processEdits()
{
    if ( m_updateNames )
    {
        for ( int row = 0; row < m_model->rowCount(); row++ )
        {
            QStandardItem *item0 = m_model->item( row, 0 );
            QStandardItem *item1 = m_model->item( row, 1 );
            QString currentText  = item1->data( Qt::DisplayRole ).toString();
            QString originalText = item1->data( ORIGINAL_ROLE ).toString();

            // see if user wants to delete
            //
            if ( item0->checkState() == Qt::Checked )
            {
                Overlord::getInstance()->deleteMacro( originalText );
            }
            else if ( currentText != originalText )
            {
                Overlord::getInstance()->renameMacro( originalText, currentText );
            }
        }
    }

    done( QDialog::Accepted );
}

void Dialog_Macro::cancel()
{
    done( QDialog::Rejected );
}

void Dialog_Macro::view()
{
    QString macro = get_Macro();

    if ( macro.length() < 1 )
    {
        csError( "View Macro", "No macro selected" );
        return;
    }

    // get the macro
    QList<MacroStruct *> data;
    data = Overlord::getInstance()->viewMacro( macro );


    QString msg = "<table style=margin-right:35>";

    for ( int k = 0; k < data.size(); ++k )
    {

        int key         = data.at( k )->m_key;
        int modifier    = data.at( k )->m_modifier;
        QString textAsc = data.at( k )->m_text;

        msg += "<tr><td width=150>Modifier: &nbsp;";

        switch ( modifier )
        {

            case 0:
                msg += " ";
                break;

            case Qt::SHIFT:
                msg += "Shift";
                break;

            case Qt::CTRL:
                msg += "Control";
                break;

            case Qt::META:
                msg += "Meta";
                break;

            case Qt::ALT:
                msg += "Alt";
                break;

            case Qt::KeypadModifier:
                msg += "Key-Pad";
                break;

            case Qt::GroupSwitchModifier:
                msg += "Group-Switch";
                break;

            default:
                msg += "(modifier)" + QString::number( modifier );
        }

        msg += "</td> <td width=150>Key: &nbsp;";

        // part 2
        bool isKey = false;

        switch ( key )
        {

            case Qt::Key_Tab:
                msg += "Tab";
                isKey = true;
                break;

            case Qt::Key_Backtab:
                msg += "Back-Tab";
                isKey = true;
                break;

            case Qt::Key_Backspace:
                msg += "Backspace";
                isKey = true;
                break;

            case Qt::Key_Return:
            case Qt::Key_Enter:
                msg += "Return";
                isKey = true;
                break;

            case Qt::Key_Insert:
                msg += "Insert";
                isKey = true;
                break;

            case Qt::Key_Delete:
                msg += "Delete";
                isKey = true;
                break;

            case Qt::Key_Shift:
                msg += " ";
                isKey = true;
                break;

            case Qt::Key_Space:
                msg += "Space";
                isKey = true;
                break;

            case Qt::Key_Home:
                msg += "Home";
                isKey = true;
                break;

            case Qt::Key_End:
                msg += "End";
                isKey = true;
                break;

            case Qt::Key_Left:
                msg += "Left";
                isKey = true;
                break;

            case Qt::Key_Right:
                msg += "Right";
                isKey = true;
                break;

            case Qt::Key_Up:
                msg += "Up";
                isKey = true;
                break;

            case Qt::Key_Down:
                msg += "Down";
                isKey = true;
                break;

            case Qt::Key_PageUp:
                msg += "Page-Up";
                isKey = true;
                break;

            case Qt::Key_PageDown:
                msg += "Page-Down";
                isKey = true;
                break;

            default:
                if ( textAsc.isEmpty() )
                {
                    msg += QString::number( key );
                    isKey = true;
                }

                break;
        }

        // part 3
        if ( ! isKey )
        {
            int pos = msg.size();
            msg = msg.left( pos-11 ) + "Text: &nbsp;" + textAsc;
        }

        msg += "</td></tr>";
    }

    // blank line
    msg += "<tr></tr>";

    QDialog msgB;
    msgB.setWindowIcon( QIcon( "://resources/diamond.png" ) );
    msgB.setWindowTitle( "View Macro - " + macro );
    msgB.setMinimumWidth( 120 );

    //
    QLabel *label = new QLabel;

    QFont font = label->font();
    font.setPointSize( 10 );
    label->setFont( font );

    label->setText( msg );

    //
    QPushButton *button = new QPushButton();

    font = button->font();
    font.setPointSize( 10 );
    button->setFont( font );

    button->setText( "Ok" );

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutButton->addStretch();
    layoutButton->addWidget( button );
    layoutButton->addStretch();

    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->addWidget( label );
    layoutMain->addLayout( layoutButton );

    msgB.setLayout( layoutMain );

    QObject::connect( button, &QPushButton::clicked, &msgB, &QDialog::accept );

    msgB.exec();
}

QString Dialog_Macro::get_Macro()
{
    QModelIndex index = m_ui->tableView->currentIndex();

    if ( ! index.isValid() )
    {
        return QString( "" );
    }

    // return the macro name
    QStandardItem *item = m_model->item( index.row(), 1 );
    QString data = item->data( Qt::DisplayRole ).toString();

    return data;
}


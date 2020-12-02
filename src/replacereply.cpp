#include "replacereply.h"

#include <QBoxLayout>
#include <QKeyEvent>
#include <QLabel>

// *
ReplaceReply::ReplaceReply( QWidget *parent )
    : QDialog( parent )
{
    QLabel *label_1= new QLabel();
    label_1->setText( "Replace?" );

    QFont font = label_1->font();
    font.setPointSize( 10 );
    label_1->setFont( font );

    QLabel *label_2= new QLabel();
    label_2->setText( "Yes / No / All / Once / Stop" );

    font = label_2->font();
    font.setPointSize( 10 );
    label_2->setFont( font );

    QBoxLayout *layout = new QVBoxLayout();
    layout->addWidget( label_1 );
    layout->addWidget( label_2 );

    setLayout( layout );

    //
    setWindowTitle( "Search & Replace" );

    QSize size = QSize( 235, 60 );
    resize( size );
}

ReplaceReply::~ReplaceReply()
{
}

void ReplaceReply::keyPressEvent( QKeyEvent *event )
{
    m_replaceReply = event->key();

    if ( m_replaceReply == Qt::Key_Escape )
    {
        this->done( 0 );

    }
    else
    {
        this->done( 1 );

    }
}

int ReplaceReply::getKey()
{
    return m_replaceReply;
}

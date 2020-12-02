#ifndef REPLACEREPLY_H
#define REPLACEREPLY_H

#include <QDialog>

class ReplaceReply: public QDialog
{
    CS_OBJECT( ReplaceReply )

public:
    ReplaceReply( QWidget *parent );
    ~ReplaceReply();

    int getKey();

protected:
    void keyPressEvent( QKeyEvent *event );

private:
    int m_replaceReply;
};


#endif
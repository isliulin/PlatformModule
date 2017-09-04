#ifndef DRAWLABEL_H
#define DRAWLABEL_H

#include <QLabel>

class DrawLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DrawLabel(QWidget *parent = 0);
    explicit DrawLabel(int pointSize, int drawShapeWidth,
                       const QString &text, QWidget *parent=0);

    void SetFontSize(int nSize = 10);

protected:
    void paintEvent(QPaintEvent *);
    void drawLabelText(int nSize);

    int m_pointSize;
    int m_drawShapeWidth;

};

#endif // DRAWLABEL_H

#include "zoomIndicator.h"
#include "utils/baseutils.h"
#include "utils/tempfile.h"

#include <QCursor>
#include <QTextOption>
#include <QDebug>
#include <QRgb>

namespace {
const QSize BACKGROUND_SIZE = QSize(59, 59);
const int SCALE_VALUE = 4;
const int IMG_WIDTH =  12;
const int INDICATOR_WIDTH = 49;
const int CENTER_RECT_WIDTH = 12;
const int BOTTOM_RECT_HEIGHT = 14;
}
ZoomIndicator::ZoomIndicator(QWidget *parent)
    : QLabel(parent)
    , m_updatePos(true)
{
    setFixedSize(BACKGROUND_SIZE);
    setStyleSheet(getFileContent(":/resources/qss/zoomindicator.qss"));
    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_centerRect = QRect((BACKGROUND_SIZE.width() - CENTER_RECT_WIDTH)/2 + 1,
                             (BACKGROUND_SIZE.width() - CENTER_RECT_WIDTH)/2 + 1,
                             CENTER_RECT_WIDTH, CENTER_RECT_WIDTH);
}

ZoomIndicator::~ZoomIndicator() {}


void ZoomIndicator::paintEvent(QPaintEvent *) {
//    using namespace utils;
    QPoint centerPos =  this->cursor().pos();
    centerPos = QPoint(centerPos.x() - this->window()->x(), centerPos.y());
    QPainter painter(this);
    if (m_updatePos) {
        QString fullscreenImgFile = TempFile::instance()->getFullscreenFileName();
        QRgb centerRectRgb = QImage(fullscreenImgFile).pixel(centerPos);
        QPixmap zoomPix = QPixmap(fullscreenImgFile).copy(centerPos.x() - IMG_WIDTH/2,
                                                          centerPos.y() - IMG_WIDTH/2, IMG_WIDTH, IMG_WIDTH);

        zoomPix = zoomPix.scaled(QSize(INDICATOR_WIDTH,  INDICATOR_WIDTH),
                                 Qt::KeepAspectRatio);
        m_lastPic = zoomPix;

        painter.drawPixmap(QRect(5, 5, INDICATOR_WIDTH, INDICATOR_WIDTH), zoomPix);


        painter.drawPixmap(m_centerRect, QPixmap(":/resources/images/action/center_rect.png"));
        m_lastCenterPosBrush = QBrush(QColor(qRed(centerRectRgb),
                                             qGreen(centerRectRgb), qBlue(centerRectRgb)));
        painter.fillRect(QRect(INDICATOR_WIDTH/2 + 2, INDICATOR_WIDTH/2 + 2,
                               CENTER_RECT_WIDTH - 4, CENTER_RECT_WIDTH - 4), m_lastCenterPosBrush);

        painter.fillRect(QRect(5, INDICATOR_WIDTH - 9, INDICATOR_WIDTH, BOTTOM_RECT_HEIGHT),
                         QBrush(QColor(0, 0, 0, 125)));
        QFont posFont;
        posFont.setPixelSize(9);
        painter.setFont(posFont);
        painter.setPen(QColor(Qt::white));
        QTextOption posTextOption;
        posTextOption.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        painter.drawText(QRectF(5, INDICATOR_WIDTH - 10, INDICATOR_WIDTH, INDICATOR_WIDTH),
                         QString("%1, %2").arg(centerPos.x()).arg(centerPos.y()), posTextOption);
        m_updatePos = false;
    } else {
        painter.drawPixmap(QRect(5, 5, INDICATOR_WIDTH, INDICATOR_WIDTH), m_lastPic);
        painter.drawPixmap(m_centerRect, QPixmap(":/resources/images/action/center_rect.png"));

        painter.fillRect(QRect(INDICATOR_WIDTH/2 + 2, INDICATOR_WIDTH/2 + 2,
                               CENTER_RECT_WIDTH - 4, CENTER_RECT_WIDTH - 4), m_lastCenterPosBrush);
    }
}

void ZoomIndicator::updatePaintEvent() {
    m_updatePos = true;
    update();
}

void ZoomIndicator::showMagnifier(QPoint pos) {
    this->show();

    this->move(pos);
}

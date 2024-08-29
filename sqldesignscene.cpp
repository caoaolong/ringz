#include "sqldesignscene.h"
#include "ringz.h"
#include "structtable.h"
#include <QMimeData>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QApplication>

SqlDesignScene::SqlDesignScene(QSqlDatabase db, QWidget *view, QObject *parent)
    : QGraphicsScene{parent}
{
    auto colors = Ringz::getTheme("colors").toObject();
    this->db = db;
    this->view = view;
    setBackgroundBrush(QBrush(QColor(colors["editor.background"].toString())));
    this->tableContentBgColor = QColor(colors["button.background"].toString());
    this->tableHeaderBgColor = QColor(colors["button.secondaryBackground"].toString());
    this->tableBorderColor = QColor(colors["button.secondaryForeground"].toString());
    this->anchorBorderColor = QColor(colors["activityBar.foreground"].toString());
    this->lineColor = QColor(colors["textLink.foreground"].toString());
    this->anchorBgColor = this->lineColor;
}

void SqlDesignScene::createTable(QPointF position, QString table, QStringList columns)
{
    // 计算最大宽度
    QFont font = QApplication::font();
    QFont hf(font);
    hf.setPixelSize(16);
    QFont cf(font);
    cf.setPixelSize(12);
    cf.setBold(false);
    cf.setItalic(true);
    QFontMetrics hfm(hf), cfm(cf);
    int maxWidth = hfm.horizontalAdvance(table);
    for (int i = 0; i < columns.count(); i++) {
        auto item = columns.at(i);
        maxWidth = qMax(cfm.horizontalAdvance(item), maxWidth);
    }
    QRect headerRect(position.x(), position.y(),
                     maxWidth + LINE_HEIGHT,
                     hfm.height() + LINE_HEIGHT);
    QRect bodyRect(position.x(), position.y() + headerRect.height(),
                   headerRect.width(),
                   (columns.count() + 1)* LINE_HEIGHT);
    QGraphicsRectItem *hbox = new QGraphicsRectItem(headerRect);
    hbox->setBrush(QBrush(tableHeaderBgColor));
    hbox->setPen(QPen(Qt::transparent));
    QGraphicsRectItem *cbox = new QGraphicsRectItem(bodyRect);
    cbox->setBrush(QBrush(tableContentBgColor));
    cbox->setPen(QPen(Qt::transparent));

    QGraphicsTextItem *header = new QGraphicsTextItem(table);
    header->setPos(position.x() + 4, position.y() + 4);
    header->setFont(hf);
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    group->addToGroup(hbox);
    group->addToGroup(cbox);
    group->addToGroup(header);

    QBrush anchorBg(anchorBgColor);
    QPen anchorBorder(anchorBorderColor);
    for (int i = 0; i < columns.count(); i++) {
        QString item = columns.at(i);
        QGraphicsTextItem *column = new QGraphicsTextItem(item);
        qreal y = bodyRect.y() + 4 + (i * LINE_HEIGHT);
        column->setPos(bodyRect.x() + 4, y);
        QGraphicsEllipseItem *anchorLeft = new QGraphicsEllipseItem(
            bodyRect.x() - 4,
            y + (LINE_HEIGHT / 2), ANCHOR_SIZE, ANCHOR_SIZE);
        anchorLeft->setBrush(anchorBg);
        anchorLeft->setPen(anchorBorder);
        anchorLeft->setData(ItemType, ItemAnchor);
        anchorLeft->setCursor(Qt::PointingHandCursor);
        QGraphicsEllipseItem *anchorRight = new QGraphicsEllipseItem(
            bodyRect.x() + bodyRect.width() - 4,
            y + (LINE_HEIGHT / 2), ANCHOR_SIZE, ANCHOR_SIZE);
        anchorRight->setBrush(anchorBg);
        anchorRight->setPen(anchorBorder);
        anchorRight->setData(ItemType, ItemAnchor);
        anchorRight->setCursor(Qt::PointingHandCursor);
        column->setFont(cf);
        group->addToGroup(column);
        group->addToGroup(anchorLeft);
        group->addToGroup(anchorRight);
    }
    group->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    addItem(group);
}

void SqlDesignScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
    QGraphicsScene::dragEnterEvent(event);
}

void SqlDesignScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    auto data = event->mimeData();
    QString table = data->data("table");
    QStringList columns;
    if (StructTable::showTable(this->view, this->db, table, &columns)) {
        createTable(event->scenePos(), table, columns);
    }
}

void SqlDesignScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void SqlDesignScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem *click = itemAt(event->scenePos(), QTransform());
    if (click && click->data(ItemType) == ItemAnchor) {
        click->group()->setFlag(QGraphicsItem::ItemIsMovable, false);
        operation = DrawLine;
        QGraphicsLineItem *line = new QGraphicsLineItem();
        QPen pen(lineColor);
        pen.setWidth(2);
        line->setPen(pen);
        TableLine *tl = new TableLine(line);
        tl->updateFrom(click);
        lines.append(tl);
        this->addItem(line);
    }
    event->accept();
    QGraphicsScene::mousePressEvent(event);
}

void SqlDesignScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    bool finished = false;
    QList<QGraphicsItem*> clickItems = items(event->scenePos());
    if (operation == DrawLine) {
        auto item = lines.last();
        for (auto click : clickItems) {
            auto data = click->data(ItemType);
            if (data.isValid() && data == ItemAnchor && click->group() != item->getFrom()) {
                item->updateTo(click);
                finished = true;
                break;
            }
        }
        if (!finished) {
            this->removeItem(item->getLine());
            lines.pop_back();
        }
        item->getFrom()->setFlag(QGraphicsItem::ItemIsMovable, true);
    }
    operation = SeeView;
    event->accept();
    QGraphicsScene::mouseReleaseEvent(event);
}

void SqlDesignScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    for (auto item : lines)
        item->update();

    if (operation == DrawLine) {
        lines.last()->updateDraw(event->scenePos());
    }
    event->accept();
    QGraphicsScene::mouseMoveEvent(event);
}

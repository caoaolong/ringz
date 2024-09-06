#include "sqldesignscene.h"
#include "ringz.h"
#include "structtable.h"
#include <QMimeData>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QApplication>

#define TABLE_HEADER_FONT_SIZE  16
#define TABLE_COLUMNS_FONT_SIZE 12

#define LINE_FROM   1
#define LINE_TO     2
#define LINE_WIDTH  4

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
    this->lineActiveColor = QColor(colors["textPreformat.foreground"].toString());
    this->anchorBgColor = this->lineColor;

    this->defaultLinePen.setColor(this->lineColor);
    this->defaultLinePen.setWidth(LINE_WIDTH);
    this->defaultLinePen.setJoinStyle(Qt::RoundJoin);
    this->defaultLinePen.setCapStyle(Qt::RoundCap);

    this->activeLinePen.setColor(this->lineActiveColor);
    this->activeLinePen.setWidth(LINE_WIDTH + 2);
    this->activeLinePen.setJoinStyle(Qt::RoundJoin);
    this->activeLinePen.setCapStyle(Qt::RoundCap);
}

TableRect SqlDesignScene::tableRect(QPointF position, QString table, QStringList columns)
{
    // 计算最大宽度
    QFont font = QApplication::font();
    QFont hf(font);
    hf.setPixelSize(TABLE_HEADER_FONT_SIZE);
    QFont cf(font);
    cf.setPixelSize(TABLE_COLUMNS_FONT_SIZE);
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
    return TableRect{headerRect, bodyRect, hf, cf, maxWidth};
}

void SqlDesignScene::createTable(QPointF position, QString table, QStringList columns)
{
    TableRect tr = tableRect(position, table, columns);
    QGraphicsItemGroup *group = appendTable(nullptr, tr, table, columns);
    QBrush anchorBg(anchorBgColor);
    QPen anchorBorder(anchorBorderColor);
    for (int i = 0; i < columns.count(); i++) {
        QString item = columns.at(i);
        appendColumn(group, tr.bodyRect, item, i, anchorBg, anchorBorder, tr.cf);
    }
    group->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    addItem(group);
}

void SqlDesignScene::editTable(QGraphicsItemGroup *group, QString table, QStringList columns)
{
    auto r = rects[group];
    // 删除原有节点
    auto items = group->childItems();
    for (int i = 0; i < items.count(); i++) {
        auto item = items.at(i);
        group->removeFromGroup(item);
        removeItem(item);
    }
    removeItem(group);
    rects.remove(group);
    QPointF position(r.headerRect.left(), r.headerRect.top());
    createTable(position, table, columns);
}

TableLine* SqlDesignScene::line(QGraphicsEllipseItem *anchor, int *type)
{
    auto name = anchor->data(ItemColumn).toString();
    auto position = anchor->data(ItemPosition).toInt();
    for (auto item : lines) {
        if (item->getFromName() == name && item->getSource()->data(ItemPosition) == position) {
            *type = LINE_FROM;
            return item;
        }
        if (item->getToName() == name && item->getTarget()->data(ItemPosition) == position) {
            *type = LINE_TO;
            return item;
        }
    }
    return nullptr;
}

void SqlDesignScene::setLine(QGraphicsEllipseItem *anchor, TableLine *line, int lineType)
{
    if (lineType == LINE_FROM) {
        line->updateFrom(anchor);
    } else if (lineType == LINE_TO) {
        line->updateTo(anchor);
    }
}

void SqlDesignScene::appendColumn(QGraphicsItemGroup *group, QRect bodyRect, QString item, int i,
                                  QBrush anchorBg, QPen anchorBorder, QFont cf)
{
    QGraphicsTextItem *column = new QGraphicsTextItem(item);
    qreal y = bodyRect.y() + 4 + (i * LINE_HEIGHT);
    column->setPos(bodyRect.x() + 4, y);
    column->setData(ItemColumn, item);
    column->setData(ItemType, ItemEntry);
    QGraphicsEllipseItem *anchorLeft = new QGraphicsEllipseItem(
        bodyRect.x() - 4,
        y + (LINE_HEIGHT / 2), ANCHOR_SIZE, ANCHOR_SIZE);
    anchorLeft->setBrush(anchorBg);
    anchorLeft->setPen(anchorBorder);
    anchorLeft->setData(ItemType, ItemAnchor);
    anchorLeft->setData(ItemPosition, ItemLeft);
    anchorLeft->setData(ItemColumn, item);
    anchorLeft->setCursor(Qt::PointingHandCursor);
    int lineType = 0;
    auto lLine = line(anchorLeft, &lineType);
    if (lLine)
        setLine(anchorLeft, lLine, lineType);

    QGraphicsEllipseItem *anchorRight = new QGraphicsEllipseItem(
        bodyRect.x() + bodyRect.width() - 4,
        y + (LINE_HEIGHT / 2), ANCHOR_SIZE, ANCHOR_SIZE);
    anchorRight->setBrush(anchorBg);
    anchorRight->setPen(anchorBorder);
    anchorRight->setData(ItemType, ItemAnchor);
    anchorRight->setData(ItemPosition, ItemRight);
    anchorRight->setData(ItemColumn, item);
    anchorRight->setCursor(Qt::PointingHandCursor);
    auto rLine = line(anchorRight, &lineType);
    if (rLine)
        setLine(anchorRight, rLine, lineType);
    column->setFont(cf);
    group->addToGroup(column);
    group->addToGroup(anchorLeft);
    group->addToGroup(anchorRight);
}

QGraphicsItemGroup *SqlDesignScene::appendTable(QGraphicsItemGroup *group, TableRect tr, QString table, QStringList columns)
{
    QGraphicsRectItem *hbox = new QGraphicsRectItem(tr.headerRect);
    hbox->setBrush(QBrush(tableHeaderBgColor));
    hbox->setData(ItemType, ItemTable);
    hbox->setData(ItemName, table);
    hbox->setData(ItemData, columns.join(","));
    hbox->setPen(QPen(Qt::transparent));
    QGraphicsRectItem *cbox = new QGraphicsRectItem(tr.bodyRect);
    cbox->setBrush(QBrush(tableContentBgColor));
    cbox->setData(ItemType, ItemColumns);
    cbox->setPen(QPen(Qt::transparent));

    QGraphicsTextItem *header = new QGraphicsTextItem(table);
    header->setPos(tr.headerRect.left() + 4, tr.headerRect.top() + 4);
    header->setFont(tr.hf);
    if (!group)
        group = new QGraphicsItemGroup();
    group->addToGroup(hbox);
    group->addToGroup(cbox);
    group->addToGroup(header);
    rects.insert(group, tr);
    return group;
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
        QGraphicsPathItem *line = new QGraphicsPathItem();
        line->setPen(defaultLinePen);
        TableLine *tl = new TableLine(line);
        tl->updateFrom(click);
        lines.append(tl);
        this->addItem(line);
    }
    // 重新绘制线条
    for (auto line : lines) {
        auto item = line->getLine();
        item->setPen(click == item ? activeLinePen : defaultLinePen);
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
        if (clickItems.count() == 1) {
            // 创建新节点
            auto type = item->getSource()->data(ItemType).toInt();
            switch (type) {
            case ItemAnchor:
                // 创建条件

                break;
            }
        } else {
            for (auto click : clickItems) {
                auto data = click->data(ItemType);
                if (data.isValid() && data == ItemAnchor && click->group() != item->getFrom()) {
                    item->updateTo(click);
                    finished = true;
                    break;
                }
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

void SqlDesignScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem*> clickItems = items(event->scenePos());
    if (!clickItems.empty()) {
        for (auto item : clickItems) {
            if (item->data(ItemType) == ItemTable) {
                auto table = item->data(ItemName).toString();
                auto columns = QStringList::fromList(item->data(ItemData).toString().split(",").toList());
                if (StructTable::showTable(this->view, this->db, item->data(ItemName).toString(), &columns)) {
                    editTable(item->group(), table, columns);
                    break;
                }
            }
        }
    }
    event->accept();
    QGraphicsScene::mouseDoubleClickEvent(event);
}

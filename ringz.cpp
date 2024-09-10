#include "ringz.h"
#include "rz.h"
#include "ui_ringz.h"
#include "sqldesignview.h"
#include "buildmodel.h"
#include "project.h"
#include "datasource.h"
#include "texteditor.h"
#include "dataview.h"
#include "preferences.h"
#include "tableview.h"
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QLabel>
#include <QAction>

QJsonObject Ringz::preferences = QJsonObject();
QJsonObject Ringz::data = QJsonObject();
QJsonObject Ringz::theme = QJsonObject();

QMap<QString, QString> Ringz::templates = QMap<QString, QString>();
QMap<QString, QWidget*> Ringz::windows = QMap<QString, QWidget*>();
QMap<QString, QMap<QString, QString>> Ringz::typeMappings = QMap<QString, QMap<QString, QString>>();

Ringz::Ringz(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Ringz)
{
    ui->setupUi(this);
    // 读取配置文件
    this->loadPreferences();
    // 读取主题
    this->loadTheme();
    // 加载模板
    this->loadTemplates();
    // 加载右键菜单
    this->loadMenu();
    // 成员变量
    this->connections = new QList<DatabaseConnection*>();
    this->projects = new QList<ProjectInfo*>();
    // Icons
    this->databaseIcon = QIcon(":/ui/icons/database.png");
    this->databaseActiveIcon = QIcon(":/ui/icons/database-active.png");
    this->tableIcon = QIcon(":/ui/icons/table.png");
    this->keysIcon = QIcon(":/ui/icons/folder.png");
    this->primaryKeyIcon = QIcon(":/ui/icons/primary_key.png");
    this->indexIcon = QIcon(":/ui/icons/key.png");
    this->columnIcon = QIcon(":/ui/icons/column.png");
    this->folderIcon = QIcon(":/ui/icons/folder.png");
    this->fileIcon = QIcon(":/ui/icons/file.png");

    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    ui->mdiArea->setTabsClosable(true);
    ui->mdiArea->setTabsMovable(true);
    // 菜单代理
    ui->dbTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->dbTree->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->projectTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->projectTree->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->actionDsView->setChecked(!ui->dbDock->isHidden());
    ui->actionProView->setChecked(!ui->projectDock->isHidden());

    this->tabifyDockWidget(ui->dbDock, ui->projectDock);

    // 加载用户数据
    this->loadUserData();
}

Ringz::~Ringz()
{
    delete ui;
}

QJsonValue Ringz::getPreference(QString key)
{
    return preferences[key];
}

QJsonValue Ringz::getTheme(QString key)
{
    return theme[key];
}

QJsonValue Ringz::getData(QString key)
{
    return data[key];
}

QString Ringz::getTemplate(QString key)
{
    return templates[key];
}

void Ringz::on_actionDbCreate_triggered()
{
    DatasourceInfo *info = new DatasourceInfo();
    if (Datasource::create(info))
        createDatasource(info);
}

void Ringz::on_actionDsView_triggered(bool checked)
{
    if (checked) {
        ui->dbDock->show();
        ui->dbDock->raise();
    } else {
        ui->dbDock->hide();
    }
}

void Ringz::on_actionProView_toggled(bool checked)
{
    if (checked) {
        ui->projectDock->show();
        ui->projectDock->raise();
    } else {
        ui->projectDock->hide();
    }
}

void Ringz::on_actionProOpen_triggered()
{
    QString projectDir = QFileDialog::getExistingDirectory(this, "打开项目", QDir::homePath());
    if (projectDir.isEmpty()) return;

    ProjectInfo *info = new ProjectInfo(projectDir);
    createProject(info);
}

void Ringz::showProjectTree(QTreeWidgetItem *parent, ProjectItem *item)
{
    for (auto item : *item->getChildren()) {
        if (item->getType() == Folder) {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(FolderItem);
            treeItem->setText(0, item->getName());
            treeItem->setIcon(0, folderIcon);
            parent->addChild(treeItem);
            this->showProjectTree(treeItem, item);
        } else {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(FileItem);
            treeItem->setText(0, item->getName());
            treeItem->setIcon(0, fileIcon);
            parent->addChild(treeItem);
        }
    }
}

void Ringz::createProject(ProjectInfo *info)
{
    this->projects->append(info);
    QTreeWidgetItem *project = new QTreeWidgetItem(ProjectFolderItem);
    project->setIcon(ColumnLabel, ProjectInfo::getIcon(info->getType()));
    project->setText(ColumnLabel, info->getRoot()->getName());
    project->setData(ColumnLabel, DataRole, info->getWorkspace());
    if (info->getActive()) {
        project->setForeground(ColumnLabel, Qt::green);
    }
    ui->projectTree->addTopLevelItem(project);
    this->showProjectTree(project, info->getRoot());
}

void Ringz::loadPreferences()
{
    QFile preferencesFile(QString(RINGZ_HOME).append(RINGZ_CONFIG));
    if (!preferencesFile.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取配置文件");
        return;
    }
    QByteArray preferencesData = preferencesFile.readAll();
    preferencesFile.close();
    QJsonDocument preferenceDoc = QJsonDocument::fromJson(preferencesData);
    if (preferenceDoc.isObject()) {
        this->preferences = preferenceDoc.object();
    }
    auto mapping = this->getPreference("mapping").toObject();
    for (const auto &db : mapping.keys()) {
        auto dbType = mapping[db].toObject();
        QStringList entries;
        for (const auto &type : dbType.keys()) {
            entries.append(dbType[type].toString());
        }
    }
    // 设置全局字体
    auto apperence = this->getPreference("apperence").toObject();
    QApplication::setFont(Rz::parseFont(apperence["font"].toString()));
}

void Ringz::loadUserData()
{
    QFile file(QString(RINGZ_HOME).append(RINGZ_DATA));

    if (!file.exists()) return;
    if(!file.open(QIODevice::ReadWrite)) return;

    QByteArray userData = file.readAll();
    QJsonDocument dataDoc = QJsonDocument::fromJson(userData);
    if (dataDoc.isObject()) {
        this->data = dataDoc.object();
    }
    // 加载数据源
    loadDatasource(data["datasources"].toArray());
    // 加载项目
    loadProject(data["projects"].toArray());
}

void Ringz::loadTheme()
{
    auto apperence = this->preferences["apperence"].toObject();
    QFile themeFile(
        QString(RINGZ_HOME).append(RINGZ_THEME).
        append(apperence["theme"].toString().replace(" ", "-").append(".json")));
    if (!themeFile.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取配置文件");
        return;
    }
    QByteArray themeData = themeFile.readAll();
    themeFile.close();
    QJsonDocument themeDoc = QJsonDocument::fromJson(themeData);
    if (themeDoc.isObject()) {
        this->theme = themeDoc.object();
    }
}

void Ringz::loadTemplates()
{
    QDir dir(QString(RINGZ_HOME).append(RINGZ_TEMPLATE));
    QFileInfoList templates = dir.entryInfoList(QDir::NoDotAndDotDot);
    for (const auto &tpl : templates) {
        QFile file(tpl.filePath());
        this->templates.insert(tpl.fileName(), QString::fromUtf8(file.readAll()));
        file.close();
    }
}

void Ringz::loadMenu()
{
    // table menu
    QAction *structAction = new QAction(QIcon(":/ui/icons/struct.png"), "结构", this);
    connect(structAction, &QAction::triggered, this, &Ringz::createTableView);
    QAction *buildAction = new QAction(QIcon(":/ui/icons/build.png"), "构建", this);
    connect(buildAction, &QAction::triggered, this, &Ringz::buildModel);
    QAction *activeAction = new QAction(QIcon(":/ui/icons/active.png"), "激活", this);
    connect(activeAction, &QAction::triggered, this, &Ringz::useProject);

    this->tableMenu.addAction(structAction);
    this->tableMenu.addAction(buildAction);

    this->projectMenu.addAction(structAction);
    this->projectMenu.addAction(activeAction);
}

void Ringz::loadDatasource(QJsonArray connections)
{
    for (auto item : connections) {
        DatasourceInfo *info = new DatasourceInfo(item.toObject());
        createDatasource(info);
    }
}

void Ringz::loadProject(QJsonArray projects)
{
    for (auto item : projects) {
        auto entry = item.toObject();
        ProjectInfo *info = new ProjectInfo(entry["path"].toString());
        info->setActive(entry["active"].toBool());
        if (info->getActive()) {
            this->activeProject = info;
        }
        createProject(info);
    }
}

void Ringz::createEditorView(EditorType type, QFile *fp)
{
    QString key = "";
    if (fp) {
        key = fp->fileName();
    } else {
        key = QString::number(QDateTime::currentMSecsSinceEpoch());
    }
    TextEditor *editor = new TextEditor(key, type, fp);
    if (fp) {
        editor->setWindowTitle(fp->fileName());
        editor->appendContent(fp->readAll());
    } else {
        editor->setWindowTitle("untitled");
    }
    if (type == SqlEditor) {
        editor->setConn(this->activeConnection->get());
    }
    connect(editor, &TextEditor::windowClosed, this, [=](QString key){
        this->windows.remove(key);
    });
    this->windows.insert(key, editor);
    ui->mdiArea->addSubWindow(editor);
    editor->show();
}

void Ringz::createDataView(QTreeWidgetItem *item, int column)
{
    QString key = QString(item->parent()->text(column)).append(".").append(item->text(column));
    if (this->windows.contains(key)) return;

    DataView *window = new DataView(this->activeConnection->get(), item->text(0), this);
    window->setWindowTitle(key);
    this->windows.insert(key, window);
    ui->mdiArea->addSubWindow(window);
    window->show();
}

void Ringz::createTableView()
{
    auto item = ui->dbTree->currentItem();
    QString database = item->parent()->text(ColumnLabel);
    QString table = item->text(ColumnLabel);
    QString key = QString("表结构[%1.%2]").arg(database, table);
    if (this->windows.contains(key)) return;

    TableView *window = new TableView(activeConnection->get(), table, activeProject, this);
    window->setWindowTitle(key);
    this->windows.insert(key, window);
    ui->mdiArea->addSubWindow(window);
    window->show();
}

void Ringz::createDesignView()
{
    SqlDesignView *window = new SqlDesignView(activeConnection->get());
    window->setWindowTitle("新建设计图");
    this->windows.insert(QString::number(QDateTime::currentMSecsSinceEpoch()), window);
    ui->mdiArea->addSubWindow(window);
    window->show();
}

void Ringz::useProject()
{
    auto project = ui->projectTree->selectedItems()[0];
    if (activeProjectItem) {
        activeProjectItem->setForeground(ColumnLabel, Qt::black);
    }
    this->activeProjectItem = project;
    this->activeProjectItem->setForeground(ColumnLabel, Qt::green);

    for (int i = 0; i < projects->length(); i++) {
        if (projects->at(i)->getWorkspace() == project->data(ColumnLabel, DataRole)) {
            this->activeProject = projects->at(i);
            this->activeProject->setActive(true);
            break;
        }
    }
}

void Ringz::buildModel()
{
    QTreeWidgetItem *item = ui->dbTree->selectedItems()[0];
    if(BuildModel::showTable(this, this->activeConnection->get(), item->text(ColumnLabel), this->projects)) {
        qDebug() << "build";
    }
}

void Ringz::createDatasource(DatasourceInfo *info)
{
    DatabaseConnection *conn = new DatabaseConnection(info);
    this->connections->append(conn);
    QTreeWidgetItem *database = new QTreeWidgetItem(DatabaseItem);
    if (info->getActive()) {
        database->setIcon(0, databaseActiveIcon);
        this->showDatasourceTree(database, conn);
        this->activeConnection = conn;
    } else {
        database->setIcon(0, databaseIcon);
    }
    database->setText(0, info->getDatabase());
    ui->dbTree->addTopLevelItem(database);
}

void Ringz::showDatasourceTree(QTreeWidgetItem *parent, DatabaseConnection *conn)
{
    auto tables = conn->tables();
    for (Table* &item : *tables) {
        QTreeWidgetItem *table = new QTreeWidgetItem(TableItem);
        table->setIcon(ColumnLabel, tableIcon);
        table->setText(ColumnLabel, item->getName());

        QTreeWidgetItem *primaryKeysItem = new QTreeWidgetItem(PrimaryKeysItem);
        primaryKeysItem->setIcon(0, keysIcon);
        primaryKeysItem->setText(0, "键");
        auto primaryKeys = item->getPrimaryKeys();
        for (TableKey* &key : *primaryKeys) {
            QTreeWidgetItem *keyItem = new QTreeWidgetItem(PrimaryKeyItem);
            keyItem->setIcon(ColumnLabel, primaryKeyIcon);
            keyItem->setText(ColumnLabel, key->getName());
            primaryKeysItem->addChild(keyItem);
        }
        table->addChild(primaryKeysItem);

        QTreeWidgetItem *indexesItem = new QTreeWidgetItem(IndexesItem);
        indexesItem->setIcon(ColumnLabel, keysIcon);
        indexesItem->setText(ColumnLabel, "索引");
        auto indexes = item->getIndexes();
        for (TableKey* &key : *indexes) {
            QTreeWidgetItem *keyItem = new QTreeWidgetItem(IndexItem);
            keyItem->setIcon(ColumnLabel, indexIcon);
            keyItem->setText(ColumnLabel, key->getName());
            indexesItem->addChild(keyItem);
        }
        table->addChild(indexesItem);

        QTreeWidgetItem *columnItem = new QTreeWidgetItem(ColumnsItem);
        columnItem->setIcon(ColumnLabel, keysIcon);
        columnItem->setText(ColumnLabel, "字段");
        auto columns = item->getColumns();
        for (TableColumn* &column : *columns) {
            QTreeWidgetItem *keyItem = new QTreeWidgetItem(ColumnItem);
            keyItem->setIcon(ColumnLabel, columnIcon);
            keyItem->setText(ColumnLabel, column->getName());
            columnItem->addChild(keyItem);
        }
        table->addChild(columnItem);

        parent->addChild(table);
    }
}

void Ringz::on_dbTree_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    if (ui->dbTree->selectedItems().isEmpty()) return;
    auto item = ui->dbTree->selectedItems().at(0);
    switch (item->type()) {
    case TableItem:
        this->tableMenu.exec(QCursor::pos());
        break;
    }
}

void Ringz::on_projectTree_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    if (ui->projectTree->selectedItems().isEmpty()) return;
    auto item = ui->projectTree->selectedItems().at(0);
    switch (item->type()) {
    case ProjectFolderItem:
        this->projectMenu.exec(QCursor::pos());
        break;
    }
}

void Ringz::on_actionMdCreate_triggered()
{

}


void Ringz::on_actionSqlCreate_triggered()
{
    createEditorView(SqlEditor, nullptr);
}

void Ringz::on_actionSettings_triggered()
{
    Preferences *window = new Preferences();
    window->show();
}

void Ringz::on_actionFileOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
        this, "打开文件", QDir::homePath(),
        "文本文件(*.txt *.sql *.java *.xml *.yaml *.yml);;"
        "SQL文件(*.sql);;"
        "Java文件(*.sql);;"
        "Xml文件(*.xml);;"
        "Yaml文件(*.yaml *.yml)");
    if (filename.isEmpty()) return;
    QFile *file = new QFile(filename);
    if (!file->exists()) return;
    if (this->windows.contains(filename)) return;
    if (!file->open(QIODevice::ReadWrite | QIODevice::ExistingOnly)) {
        QMessageBox::warning(this, "错误", "文件打开失败");
        return;
    }
    createEditorView(SqlEditor, file);
}

void Ringz::on_dbTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item->type() != TableItem) return;
    createDataView(item, column);
}


void Ringz::on_actionSqlDesign_triggered()
{
    createDesignView();
}



//    fileComboBox = createComboBox(tr("*"));
//    textComboBox = createComboBox();
//    directoryComboBox = createComboBox(QDir::currentPath());




//! [2]
void Window::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Find Files"), QDir::currentPath());

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}
//! [2]

static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

void Window::find()
{
    filesTable->setRowCount(0);

    QString fileName = fileComboBox->currentText();
    QString text = textComboBox->currentText();
    QString path = directoryComboBox->currentText();

    updateComboBox(fileComboBox);
    updateComboBox(textComboBox);
    updateComboBox(directoryComboBox);

    currentDir = QDir(path);
    QStringList files;
    if (fileName.isEmpty())
        fileName = "*";
    files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);

    if (!text.isEmpty())
        files = findFiles(files, text);
    showFiles(files);
}

QStringList Window::findFiles(const QStringList &files, const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));

    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();


        if (progressDialog.wasCanceled())
            break;

        QFile file(currentDir.absoluteFilePath(files[i]));

        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text)) {
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
    return foundFiles;
}

void Window::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }

    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()) +
                             (" (Double click on a file to open it)"));

    filesFoundLabel->setWordWrap(true);
}


QComboBox *Window::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    return comboBox;
}

void Window::createFilesTable()
{
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);

    connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFileOfItem(int,int)));
}

void Window::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);
    QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
}


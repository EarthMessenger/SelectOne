#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QDrag>
#include <QFile>
#include <QFileDialog>
#include <QMimeData>
#include <QPushButton>
#include <QRandomGenerator>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <cassert>
#include <qchar.h>
#include <qdebug.h>
#include <qevent.h>
#include <qglobal.h>
#include <qobjectdefs.h>
#include <qspinbox.h>
#include <qwidget.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this)),
      selecting(false), numberOfPeople(0)
{
  ui->setupUi(this);

  updateCandidateStatus();
  updateStartOrStopButton();

  connect(ui->fileSelectorButton, &QPushButton::clicked, this,
          &MainWindow::handleFileSelectorButton);
  connect(timer, &QTimer::timeout, this,
          QOverload<>::of(&MainWindow::updateCandidate));
  connect(ui->startOrStopButton, &QPushButton::clicked, this,
          &MainWindow::handleSelectionStartOrStop);
  connect(ui->numberOfPeopleSpinBox, qOverload<int>(&QSpinBox::valueChanged),
          this, &MainWindow::handleNumberOfPeopleChange);

  setAcceptDrops(true);
}

MainWindow::~MainWindow() { delete ui; }

QString MainWindow::selectOne()
{
  assert(0 <= numberOfPeople && numberOfPeople <= candidates.size());

  for (int i = 0; i < numberOfPeople; i++) {
    std::swap(
        candidates[i],
        candidates[QRandomGenerator::global()->bounded(i, candidates.size())]);
  }

  QStringList result{ candidates.begin(), candidates.begin() + numberOfPeople };
  return result.join("\n");
}

void MainWindow::handleFileSelectorButton()
{
  QString file_name = QFileDialog::getOpenFileName(this);
  if (!file_name.isEmpty()) setFile(file_name);
}

void MainWindow::setFile(QString fileName)
{
  QFile file(fileName);
  QStringList new_candidates;
  if (file.open(QIODevice::ReadOnly)) {
    while (!file.atEnd()) {
      QString line = file.readLine().trimmed();
      if (!line.isEmpty()) new_candidates.push_back(line);
    }
  }
  candidates = std::move(new_candidates);
  ui->numberOfPeopleSpinBox->setRange(0, candidates.size());
  if (!candidates.empty()) {
    ui->numberOfPeopleSpinBox->setValue(
        std::max(1, ui->numberOfPeopleSpinBox->value()));
  }
  updateCandidateStatus();
}

void MainWindow::handleNumberOfPeopleChange(int newValue)
{
  numberOfPeople = newValue;
}

void MainWindow::updateCandidateStatus()
{
  if (candidates.empty()) {
    ui->candidateStatus->setText(QString("没有候选！"));
  } else {
    ui->candidateStatus->setText(
        QString("有 %1 个候选").arg(candidates.size()));
  }
}

void MainWindow::updateStartOrStopButton()
{
  if (selecting) {
    ui->startOrStopButton->setText("停止");
  } else {
    ui->startOrStopButton->setText("开始");
  }
}

void MainWindow::handleSelectionStartOrStop()
{
  if (selecting) {
    timer->stop();
  } else {
    timer->start(20);
  }
  selecting = !selecting;
  updateStartOrStopButton();
}

void MainWindow::updateCandidate() { ui->selectResult->setText(selectOne()); }

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasUrls() &&
      event->mimeData()->urls().front().isLocalFile()) {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent(QDropEvent *event)
{
  const auto url = event->mimeData()->urls().front();
  setFile(url.toLocalFile());
}

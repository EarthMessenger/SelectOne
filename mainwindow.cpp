#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QRandomGenerator>
#include <QString>
#include <QStringList>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent) , ui(new Ui::MainWindow), timer(new QTimer(this)), selecting(false)
{
  ui->setupUi(this);

  updateCandidateStatus();
  updateStartOrStopButton();
  connect(ui->file_selector_button, &QPushButton::clicked, this, &MainWindow::handleFileSelectorButton);
  connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::updateCandidate));
  connect(ui->start_or_stop_button, &QPushButton::clicked, this, &MainWindow::handleSelectionStartOrStop);
}

MainWindow::~MainWindow()
{
  delete ui;
}

QString MainWindow::selectOne()
{
  if (candidates.empty()) return "";
  if (candidates.size() > 1) {
    std::swap(candidates[0],
              candidates[QRandomGenerator::global()->bounded(0, candidates.size())]);
  }
  return candidates[0];
}

void MainWindow::handleFileSelectorButton()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    QStringList new_candidates;
    if (file.open(QIODevice::ReadOnly)) {
      while (!file.atEnd()) {
        QString line = file.readLine().trimmed();
        if (!line.isEmpty()) {
          new_candidates.push_back(line);
        }
      }
    }
    candidates = std::move(new_candidates);
    updateCandidateStatus();
  }
}

void MainWindow::updateCandidateStatus()
{
  if (candidates.empty()) {
    ui->candidate_status->setText(QString("没有项目！"));
  } else {
    ui->candidate_status->setText(QString("有 %1 个项目").arg(candidates.size()));
  }
}

void MainWindow::updateStartOrStopButton() {
  if (selecting) {
    ui->start_or_stop_button->setText("停止");
  } else {
    ui->start_or_stop_button->setText("开始");
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

void MainWindow::updateCandidate()
{
  ui->selected_candidate->setText(selectOne());
}

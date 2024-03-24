#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRandomGenerator>
#include <QString>
#include <QStringList>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  QStringList candidates;
  QTimer *timer;
  bool selecting;

  QString selectOne();

  void handleFileSelectorButton();
  void updateCandidateStatus();
  void updateStartOrStopButton();
  void updateCandidate();
  void handleSelectionStartOrStop();
};
#endif // MAINWINDOW_H

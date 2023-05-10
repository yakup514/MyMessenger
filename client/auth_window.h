#ifndef AUTH_WINDOW_H
#define AUTH_WINDOW_H

#include <QDialog>

namespace Ui {
class AuthWindow;
}

class AuthWindow : public QDialog {
  Q_OBJECT

 public:
  explicit AuthWindow(QWidget *parent = nullptr);
  ~AuthWindow();
 private slots:

 public:
  Ui::AuthWindow *ui;
};

#endif  // AUTH_WINDOW_H

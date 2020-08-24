#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
//      QString timedata=" ";
  MainWindow w;
  w.show();

  return a.exec();
}


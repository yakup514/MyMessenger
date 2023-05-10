#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include <unordered_map>

#include "auth_window.h"
#include "mainwindow.h"
/*!
 * Класс сервер, создает TCP клиент
 *
 */
class Client : public QObject {
  Q_OBJECT
 public:
  static const quint8 msg_ok = 0;
  static const quint8 auth_mes = 1;
  static const quint8 auth_ok = 10;
  static const quint8 auth_not = 11;
  static const quint8 reg_mes = 2;
  static const quint8 reg_ok = 20;
  static const quint8 reg_not = 21;
  static const quint8 send_clients = 3;
  static const quint8 send_his = 4;
  static const quint8 file_err = 5;
  static const quint8 client_alreday_exist_err = 6;
  static const quint8 send_msg = 7;

 public:
  /**
   * @brief конструктор класса
   */
  Client();
 private slots:
  /**
   * @brief слот при нажатии кнопки connect
   */
  void SlotConnnectButtonClicked();
  /**
   * @brief слот при нажатии кнопки disconnect
   */
  void SlotDisconnectButtonClicked();
  /**
   * @brief слот при нажатии кнопки send
   */
  void SlotSendButtonClicked();
  /**
   * @brief слот при соединении ссервером
   */
  void SlotSocketConnected();
  /**
   * @brief слот при нажатии кнопки авторизация
   */
  void SlotAuthButtonClicked();
  /**
   * @brief слот при нажатии кнопки регистрация
   */
  void SlotRegButtonClicked();
  /**
   * @brief слот при выборе клиента кликом в списке клиентов
   */
  void SlotBuddyChoosed(QListWidgetItem*);
  /**
   * @brief слот при отсоединении от сервера
   */
  void SlotSocketDisconnected();

 public slots:
  void SlotReadyRead();

 private:
  /**
   * @brief окно авторизации вызывается после соединения с сервером
   */
  AuthWindow* aw;
  /**
   * @brief mainwindow
   */
  MainWindow* mw;
  QTcpSocket* socket_;
  /**
   * \brief массив для данных, заполняется при отправке
   */
  QByteArray data_;
  /**
   * \brief функция для отправки серверу служебной информации
   * \param str - информация если необходимо
   * \param mess_type тип сообщения
   */
  void SendToServer(QString str, quint8 mess_type);
  /**
   * \brief функция для сообщений другим коиентам, имя клиента которому
   * отправляется сообщение берется исходя из того какой item listWidget сейчас
   * активен \param msg - сообщение
   */
  void SendToClient(QString msg);
  /**
   * \brief параметр задающий размер следующего блока данных
   */
  uint next_block_size = 0;
  /**
   * \brief имя клиента
   */
  QString my_name_;
  /**
   * \brief имя клиента которому отправляем сообщение
   */
  QString buddy_name_;
  bool is_auth = false;
  /**
   * \brief функция для получения списка клиентов
   */
  void GetClientsList(QDataStream& in);
  /**
   * \brief функция для проверки соответсвия имени тербованиям
   */
  bool CheckName(const QString& name);
  /**
   * \brief функция для проверки соответсвия пароля тербованиям
   */
  bool CheckPass(const QString& pass);
  /**
   * \brief функция для записи сообщения в файл истории
   * \param msg - сообщение
   * \param name - кому
   * \param is_my_msg - если сообщение исходящее в историю запишется
   * отпаравитель me
   */
  void WriteMessageToHistory(QString msg, QString name, bool is_my_msg);
  /**
   * \brief функция для вывода истории сообщений с текущим собеседником на экран
   */
  void SetHistory();
  /**
   * \brief словарь для сопоставления имени клиента и item в listWidget
   * для отображения пришедшего сообщения
   */
  std::unordered_map<QString, QListWidgetItem*> names_to_items_;
  /**
   * \brief словарь для сопоставления имени клиента и его статуса
   */
  std::unordered_map<QString, bool> clients_list_;
};

#endif  // CLIENT_H

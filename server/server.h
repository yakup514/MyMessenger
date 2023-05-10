#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <unordered_map>
#include <unordered_set>

#include "mainwindow.h"
/*!
 * Класс сервер, создает TCP сервер
 *
 */
class Server : public QTcpServer {
  Q_OBJECT
 public:
  /**
   * @brief конструктор ксласса
   */
  Server();

 private:
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
  /**
   * \brief виджет для отображения клиентов и их статуса
   */
  MainWindow* mw;
  /**
   * \brief словарь для сопоставления имени клиента и сокета который ему
   * соответсвует
   */
  std::unordered_map<QString, QTcpSocket*> names_to_sockets_;
  /**
   * \brief словарь для сопоставления сокета клиента и имени которое ему
   * соответсвует
   */
  std::unordered_map<QTcpSocket*, QString> sockets_to_names_;
  /**
   * \brief словарь для сопоставления имени клиента и его статуса, содержит
   * имена всех клиентов которые были зарегистрированы
   */
  std::unordered_map<QString, bool> clients_list_;
  /**
   * \brief массив для данных, заполняется при отправке
   */
  QByteArray data_;
  /**
   * \brief функция которая считывает имена всех клиентов которые были
   * зарегистрированы
   */
  void MakeClientList();
  /**
   * \brief функция отображения клиенто в MainWindow
   */
  void SetClientList();
  /**
   * \brief функция для регистрации нового пользователя, возвращает
   * -file_err при ошибке открытия файла
   * -reg_ok при успешной регистрации
   * -client_alreday_exist_err при совпадении имени клиента с имеющимся
   * \param name - имя клиента
   * \param pass - хэш пароля
   */
  int RegNewUser(QString name, QString pass);
  /**
   * \brief функция для авторизации пользователя, возвращает
   * -true при успешной авторизации
   * -false при ошибке
   * \param name - имя клиента
   * \param pass - хэш пароля
   */
  bool AuthNewUser(QString name, QString pass);
  /**
   * \brief функция вызывается при получении от клиента auth_mes
   * при успешной авторизации отправляет клиенту сообщение  auth_ok, затем с
   * помощью SendClientsList список пользователей и их статус и обновляет статус
   * клиентов на сервере \param in - входящие данные \param socket - сокет
   */
  void AutorizationRequest(QDataStream& in, QTcpSocket* socket);
  /**
   * \brief функция вызывается при получении от клиента auth_mes
   * при успешной регистрации отправляет клиенту сообщение  reg_ok
   * \param in - входящие данные
   * \param socket - сокет
   */
  void RegistrationRequest(QDataStream& in, QTcpSocket* socket);
  /**
   * \brief функция для отправки клиенту списка клиентов и х статуса
   * \param socket - сокет
   */
  void SendClientsList(QTcpSocket* socket);
  /**
   * \brief функция для отправки кленту служебной информации
   * \param socket - сокет
   * \param str - информация если необходимо
   * \param mess_type тип сообщения
   */
  void SendToClient(QTcpSocket* socket, QString str, quint8 mess_type);
  /**
   * \brief функция для отправки кленту сообщений от других клиентов, тип
   * сообщения send_msg \param socket - сокет \param from - от кого \param msg
   * сообщение
   */
  void SendMsgToClient(QTcpSocket* socket, QString from, QString msg);
  /**
   * \brief функция для отправки кленту сообщений от других клиентов, тип
   * сообщения для дальнейшей добработки, если необходимо отправляеть сообщения
   * клиентам оффлайн
   */
  void WriteMessageToHistory(QString msg, QString from, QString to);
  /**
   * \brief параметр задающий размер следующего блока данных
   */
  uint next_block_size = 0;
  QTcpSocket* socket_;

 private slots:
  void incomingConnection(qintptr socket_descriptor);
  void SlotReadyRead();
  void SlotClientDisconected();
  void SlotStartButtonClicked();
};

#endif  // SERVER_H

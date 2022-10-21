#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "../common.hpp"
#include "../json.hpp"

using boost::asio::ip::tcp;




void SendMessage(tcp::socket&, nlohmann::json&);

//void ShowBids(tcp::socket&, const std::string&, const std::string&);
std::string ReadMessage(tcp::socket&);
// Мы предполагаем, что для идентификации пользователя будет использоваться ID.
// Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
// Этот ID далее используется при отправке запросов.
bool ProcessRegistration(const std::string&, const std::string&);
void SendBid(const std::string&, const std::string&, const std::string&);
void RejectBid(const std::string&);
void ShowQuotes(tcp::socket&);
void GetData(tcp::socket&);
void CheckExecutedBids(tcp::socket&);
void CheckActiveBids(tcp::socket&);



#endif // CLIENT_H

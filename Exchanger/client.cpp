//============================================================================
// Name        : Exch
// Author      : Koksharov Sergey
// Version     :
// Copyright   : my cprght
// Description : Test for NTProg
//============================================================================

#include "client.h"

#include "mainwindow.h"
#include <QApplication>
#include <algorithm>

boost::asio::io_service io_service;
boost::mutex global_stream_lock;

tcp::resolver resolver(io_service);
tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
tcp::resolver::iterator iterator = resolver.resolve(query);

tcp::socket s(io_service);
tcp::socket s1(io_service);
tcp::socket s2(io_service);
tcp::socket s3(io_service);
tcp::socket s4(io_service);

extern bool authorization;
std::string name = "";
std::string id = "";
std::string rub = "";
std::string usd = "";
std::vector<std::vector<std::string>> pullExecBids;
std::vector<std::vector<std::string>> pullActiveBids;
std::vector<std::string> idsExecBids;
std::vector<std::string> idsActiveBids;
std::vector<std::string> answerSendBid;
std::vector<std::string> quotes (10, " ");

int main(int argc, char *argv[])
{

    s.connect(*iterator);
    s1.connect(*iterator);
    s2.connect(*iterator);
    s3.connect(*iterator);
    s4.connect(*iterator);

    boost::thread_group thread_group;
    thread_group.create_thread(boost::bind(&GetData, boost::ref(s1)));
    thread_group.create_thread(boost::bind(&ShowQuotes, boost::ref(s2)));
    thread_group.create_thread(boost::bind(&CheckExecutedBids, boost::ref(s3)));
    thread_group.create_thread(boost::bind(&CheckActiveBids, boost::ref(s4)));

    io_service.run();

    std::cout << "all threads active" << std::endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    return 0;
}

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aRequestType,
    const std::string& aMessage)
{
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;
    std::string request = req.dump();
    boost::lock_guard<boost::mutex> lock(global_stream_lock);
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::lock_guard<boost::mutex> lock(global_stream_lock);

    std::string line;
    boost::asio::read_until(aSocket, boost::asio::dynamic_string_buffer(line), "\0");

    // Попытки прочитать более 512 бит
//    boost::asio::streambuf b;
//    boost::asio::read(aSocket, b, boost::asio::transfer_at_least(aSocket.available()));

//    boost::asio::streambuf b;
//    boost::asio::read_until(aSocket, b, "\0");
//    std::istream is(&b);
//    std::string line(std::istreambuf_iterator<char>(is), {});

    std::cout << "Cout: " << line << std::endl;
    return line;
}

void SendBid(
    const std::string& aBidType,
    const std::string& aBidValue,
    const std::string& aBidPrice)
{
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = Requests::Bid;
    req["BidType"] = aBidType;
    req["BidValue"] = aBidValue;
    req["BidPrice"] = aBidPrice;

    std::string request = req.dump();
    boost::asio::write(s, boost::asio::buffer(request, request.size()));
    std::string response = ReadMessage(s);
    if(!response.empty()) {
        boost::split(answerSendBid, response, boost::is_any_of(","), boost::token_compress_on);
    }
}

void RejectBid(const std::string& aId)
{
    nlohmann::json req;
    req["ReqType"] = Requests::RejectBid;
    req["BidId"] = aId;

    std::string request = req.dump();
    boost::asio::write(s, boost::asio::buffer(request, request.size()));
}

void ShowQuotes(tcp::socket& aSocket)
{
    while(true){
        nlohmann::json req;
        req["ReqType"] = Requests::ShowQuotes;
        std::string request = req.dump();
        global_stream_lock.lock();
        boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
        global_stream_lock.unlock();

        // Функция должна получать 5 заявок покупки и 5 заявок продажи по минимальным ценам
        std::string response = ReadMessage(aSocket);
        if(!response.empty()) {
            boost::split(quotes, response, boost::is_any_of(","), boost::token_compress_on);
        }
        boost::this_thread::sleep(boost::posix_time::seconds( 5 ));
    }
}

void ShowBids(
    tcp::socket& aSocket,
    const std::string& aRequestType,
    const std::string& aShowType)
{
    nlohmann::json req;
    req["UserId"] = id;
    req["ReqType"] = aRequestType;
    req["ShowType"] = aShowType;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// "Создаём" пользователя, получаем его ID.
bool ProcessRegistration(const std::string& aLogin, const std::string& aPass)
{
    nlohmann::json req;
    req["ReqType"] = Requests::Registration;
    req["Login"] = aLogin;
    req["Pass"] = aPass;

    std::string request = req.dump();
    boost::asio::write(s, boost::asio::buffer(request, request.size()));

    // Для регистрации Id не нужен, заполним его нулём

    std::string response = ReadMessage(s);
    if (response.find("Wrong") == std::string::npos) {
        id = response;
        return true;
    } else return false;
}

void GetData(tcp::socket& aSocket){
    while(true){
        if(!authorization) continue;
        try{
            // TODO: parser
            SendMessage(aSocket, Requests::GetData, "");
            std::string srub = "";
            std::string susd = "";
            std::string msg = ReadMessage(aSocket);
            bool writeRub = true;
            for(auto&& c : msg)
            {
                if (c == 'r') continue;
                if (c == 'd') {
                    writeRub = false;
                    continue;
                }
                if(writeRub)srub += c;
                else susd += c;
            }
            rub = srub;
            usd = susd;
            boost::this_thread::sleep(boost::posix_time::seconds( 1 ));
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception in cli GetData: " << e.what() << "\n";
        }
    }
}
// Проверка исполнения заявок
// Асинхронный вызов
void CheckExecutedBids(tcp::socket& aSocket){
    while(true){
        try{
            SendMessage(aSocket, Requests::CheckExecBids, "");
            std::string response = ReadMessage(aSocket);
            std::cout << "RESPONSE: " << response << std::endl;
            if(response != " ") {
                std::vector<std::string> toCout;
                std::cout << "response: " << response << std::endl;
                boost::split(toCout, response, boost::is_any_of(","), boost::token_compress_on);
                pullExecBids.push_back(toCout);
                idsExecBids.push_back(toCout.at(0));
            }
            boost::this_thread::sleep(boost::posix_time::seconds( 5 ));
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception in CEB: " << e.what() << "\n";
        }
    }
}

// Получениеe активных заявок
// Асинхронный вызов
void CheckActiveBids(tcp::socket& aSocket){
    while(true){
        if(!authorization) continue;
        try{
            SendMessage(aSocket, Requests::CheckActiveBids, "");
            std::string response = ReadMessage(aSocket);
            pullActiveBids.clear();
            if(response != " ") {
                std::vector<std::string> fullResponse;
                boost::split(fullResponse, response, boost::is_any_of(";"), boost::token_compress_on);
                std::vector<std::string> toCout;
                for(auto&& row : fullResponse){
                    boost::split(toCout, row, boost::is_any_of(","), boost::token_compress_on);
                    pullActiveBids.push_back(toCout);
                    idsActiveBids.push_back(toCout.at(0));
                }
            }
            boost::this_thread::sleep(boost::posix_time::seconds( 5 ));
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception in CAB: " << e.what() << "\n";
        }
    }
}

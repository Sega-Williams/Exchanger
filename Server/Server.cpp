#include <cstdlib>
#include <iostream>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include "../json.hpp"
#include <pqxx/pqxx>
#include <cmath>
#include <map>
#include "../common.hpp"

using boost::asio::ip::tcp;

pqxx::connection c(conn_set.c_str());
pqxx::connection connectionCheckBD(conn_set.c_str());
boost::mutex global_stream_lock;

class Core
{
public:
    // "Регистрирует" пользователя, если он не зарегистрирован и возвращает его id.
    std::string RegisterNewUser(const std::string& aUserName, const std::string& aUserPass)
    {

    	//TODO: реализовать проверку на символы. При вводе символа ' в конце имени приводит к прекращению работы клиента и сервера.
		pqxx::row response;
		try{
			//start_worker{
			pqxx::work tx{c};
			response = tx.exec1("SELECT id FROM users WHERE username = '" + aUserName +"';");
			tx.commit();
			//end_worker}

			try{
				//start_worker{
				pqxx::work txn{c};
				response = txn.exec1(std::string("SELECT id FROM users WHERE id = ") + response["id"].c_str() + " AND psw = '" + aUserPass + "';");
				txn.commit();
				//end_worker}
			}
		    catch (std::exception& e)
		    {
		        std::cerr << "Exception: " << e.what() << "\n";

		        std::cout << "id: " << response["id"].c_str() << "pass:  " << aUserPass << "Wrong login or password" << std::endl;
		        return "Wrong login or password";
		    }
		}
	    catch (std::exception& e)
	    {
	        std::cerr << "Exception: " << e.what() << "\n";
			//start_worker{
			pqxx::work tx{c};
			response = tx.exec1(
					"INSERT INTO users("
					"username, psw, rub , usd)"
					"VALUES ("
					"'" + aUserName +"',"
					"'" + aUserPass +"',"
					"'" + std::to_string(1000) + "',"
					"'" + std::to_string(10) + "'"
					") RETURNING id;"
					);
			tx.commit();
			//end_worker}
	    }
	    std::cout << "selected id: " << response["id"].c_str()  << std::endl;

        return std::string(response["id"].c_str());
    }

    // Запрос имени клиента по ID
    std::string GetUserName(const std::string& aUserId)
    {
/*        const auto userIt = mUsers.find(std::stoi(aUserId));
        if (userIt == mUsers.cend())
        {
            return "Error! Unknown User";
        }
        else
        {
            return userIt->second;
        }*/
    	std::cout << "Name : TODO" << std::endl;
    	return "Name";
    }
    // Запрос списка пользователей
    std::string GetUsers()
    {
    	/*
    		    pqxx::result response = tx.exec("SELECT * FROM users");
    			for (size_t i = 0; i < response.size(); i++)
    			{
    				std::cout << "Id: " << response[i][0] << " Username: " << response[i][1] << " Password: " << response[i][2] << " Email: " << response[i][3] << std::endl;
    			}
    	*/

    	std::cout << "Users: TODO" << std::endl;
    	return "List";
    }

    // Регистрация заявки на покупку/продажу
    std::string Bid(const std::string& aId, const std::string& aBidType,
    		const std::string& aBidValue, const std::string& aBidPrice)
    {
    	// TODO: Логика размещения заявки.

    	/*
    	 insert into bids (id_user, bid_type, value, price, status)
							values (
							1, 'buy', 5, 55, 'active');
    	 *
    	 */
		pqxx::row response;
		try{
			//start_worker{
			pqxx::work tx{c};

			response = tx.exec1(
					"INSERT INTO bids (id_user, bid_type, value, remain_value, price, status, id_deal_with, showed, date) "
					"VALUES ("
					+ aId + ", "
					"'" + aBidType +"',"
					+ aBidValue + ", "
					+ aBidValue + ", "
					+ aBidPrice + ", "
					+ "'active', "
					+ " 0, "
					+ " false, "
					+ " now() "
					") RETURNING id, bid_type, value, price, status;"
					);
			tx.commit();
			//end_worker}

			return std::string(response["id"].c_str())
					+ "," + response["bid_type"].c_str()
					+ "," + response["value"].c_str()
					+ "," + response["price"].c_str()
					+ "," + response["status"].c_str();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
			return "";
		}
    }

    // Отмена заявки
    void RejectBid(const std::string& aBidId)
    {
		try{
			//start_worker{
			pqxx::work tx{c};
			tx.exec0(std::string("UPDATE bids SET status = 'rejected' WHERE id = ") + aBidId);
			tx.commit();
			//end_worker}
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception Reject: " << e.what() << "\n";
			return;
		}
    }

    // Просмотр котировок
    std::string ShowQuotes()
    {
    	std::string toReturn = "";
    	pqxx::result response;

		try{
			//start_worker{
			pqxx::work tx{c};
			response = tx.exec("SELECT price FROM bids WHERE bid_type = 'buy' "
					"and status = 'active' GROUP BY price ORDER BY price DESC LIMIT 5;");
			tx.commit();
			//end_worker}

			for (auto && row : response)
			{
				toReturn += std::string(row["price"].c_str()) + ",";
			}
			if(response.size() < 5){
				int count = 5 - response.size();
				for(int i = 0; i < count; i++){
					toReturn += " ,";
				}
			}
			//start_worker{
			pqxx::work txn{c};
			response = txn.exec("SELECT price FROM bids WHERE bid_type = 'sell' "
					"and status = 'active' GROUP BY price ORDER BY price ASC LIMIT 5;");
			txn.commit();
			//end_worker}

			for (auto && row : response)
			{
				toReturn += std::string(row["price"].c_str()) + ",";
			}
			if(response.size() < 5){
				int count = 5 - response.size();
				for(int i = 0; i < count; i++){
					toReturn += " ,";
				}
			}

			return toReturn;
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
			return "";
		}
    }


    // Просмотр заявок
    std::string ShowBids(const std::string& aId, const std::string& aShowType)
    {
    	std::string toReturn = "";
    	pqxx::result response;

		try{
			//start_worker{
			pqxx::work tx{c};
			if(aShowType == "my"){
				response = tx.exec(
						"SELECT id, bid_type, value, price, status FROM bids WHERE id_user = " + aId + " ORDER BY status;"
						);
			}
			else{
				response = tx.exec(
						"SELECT id, bid_type, value, price FROM bids WHERE id_user != " + aId + " and status = 'active' ORDER BY price;"
						);
			}
			tx.commit();
			//end_worker}


			for (auto && row : response)
			{
				toReturn += std::string("\nid of bid: ") + row["id"].c_str()
							+ "; Bid Type: " + row["bid_type"].c_str()
							+ "; Value: " + row["value"].c_str()
							+ "; Price: " + row["price"].c_str();
				if(aShowType == "my") toReturn += std::string("; Status: ") + row["status"].c_str();
			}

			return toReturn;
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
			return "ERROR Show";
		}
    }



    std::map <int,int> pullIdsForCheckBids;
	// Проверка выполненных заявок
	std::string CheckExecBids(const std::string& aId)
	{

		if(aId.empty())return " ";
		int id = std::stoi(aId);
		if( pullIdsForCheckBids.find(id) == pullIdsForCheckBids.end() ) pullIdsForCheckBids[id] = 0;
		pqxx::row response;
		std::string toReturn = "";
		try{
			//start_worker{
			pqxx::work tx{c};
				response = tx.exec1("SELECT * FROM bids WHERE id_user = " + aId
						+ " AND status = 'executed' ORDER BY id ASC LIMIT 1 OFFSET "
						+ std::to_string(pullIdsForCheckBids[id])  + ";");
			tx.commit();
			//end_worker}

			std::cout << "checkExecBidsIterator added = " << pullIdsForCheckBids[id]++ <<  std::endl;

			toReturn += 			std::string(response["id"].c_str())
						+ "," + response["bid_type"].c_str()
						+ "," + response["value"].c_str()
						+ "," + response["price"].c_str()
						+ "," + response["status"].c_str();

			std::cout << toReturn << std::endl;
			return toReturn;
		}
		catch (std::exception& e)
		{
			return " ";
		}
    }

	// Проверка активных заявок
	std::string CheckActiveBids(const std::string& aId)
	{
		pqxx::result response;
    	std::string toReturn = "";
		try{
			//start_worker{
			pqxx::work tx{c};
				response = tx.exec("SELECT * FROM bids WHERE id_user = " + aId + " AND status = 'active' ORDER BY id DESC;");
			tx.commit();
			//end_worker}

			std::cout << "CAB commited" << std::endl;

			for(auto&& row : response){

				toReturn += 			std::string(row["id"].c_str())
							+ "," + row["bid_type"].c_str()
							+ "," + row["value"].c_str()
							+ "," + row["price"].c_str()
							+ "," + row["status"].c_str();
				if(row != response.back()) toReturn += ";";
			}
			std::cout << toReturn << std::endl;
			return toReturn;
		}
		catch (std::exception& e)
		{
			return " ";
		}
    }

	// Получение данных
	std::string GetData(const std::string& aId)
	{
		try{
			pqxx::row response;
			pqxx::work tx{c};
				response = tx.exec1("SELECT rub, usd FROM users WHERE id = " + aId);
			tx.commit();

			return std::string("r") + response["rub"].c_str() + "d" + response["usd"].c_str();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
			return "GetData Error";
		}
	}



private:
};

Core& GetCore()
{
    static Core core;
    return core;
}



class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            data_[bytes_transferred] = '\0';

            // Парсим json, который пришёл нам в сообщении.
            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];

            std::string reply = "Error! Unknown request type";
            if (reqType == Requests::Registration)
            {
                // Это реквест на регистрацию пользователя.
                // Добавляем нового пользователя и возвращаем его ID.
                reply = GetCore().RegisterNewUser(j["Login"], j["Pass"]);
            }

            else if (reqType == Requests::Bid)
            {
            	//TODO заявка на покупку/продажу

        		reply = GetCore().Bid(j["UserId"], j["BidType"], j["BidValue"], j["BidPrice"]);
            }
            else if (reqType == Requests::RejectBid)
            {
            	//TODO заявка на покупку/продажу

        		GetCore().RejectBid(j["BidId"]);
            }
            else if (reqType == Requests::ShowQuotes)
            {
            	// показать список заявок
            	reply = GetCore().ShowQuotes();
            }
            else if (reqType == Requests::ShowBids)
            {
            	// показать список заявок
            	reply = GetCore().ShowBids(j["UserId"], j["ShowType"]);
            }
            else if (reqType == Requests::CheckExecBids)
            {
            	// Проверка исполнения заявок
            	reply = GetCore().CheckExecBids(j["UserId"]);
            }
            else if (reqType == Requests::CheckActiveBids)
            {
            	// Проверка исполнения заявок
            	reply = GetCore().CheckActiveBids(j["UserId"]);
            }
            else if (reqType == Requests::GetData)
            {
            	// Проверка исполнения заявок
            	reply = GetCore().GetData(j["UserId"]);
            }

            // Попытки передать более 512 символов
//        	reply = std::to_string(int(ceil(reply.size()/512.0))) + "\n" + reply;
            std::cout << "REQ: " << reqType << "; reply: " << reply << std::endl;

            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

// Мэтчинг ордеров
void CheckBD(){

	double valueBuy;
	double valueSell;
	double priceBuy;
	double priceSell;
	double value;
	double price;

	while(true){
        boost::this_thread::sleep(boost::posix_time::seconds( 5 ));
        // Получение списка id ордеров на покупку
		pqxx::work txid{connectionCheckBD};
		pqxx::result responseId = txid.exec("SELECT id FROM bids WHERE status = 'active' "
				"and bid_type = 'buy' ORDER BY price DESC, date ASC;");
		txid.commit();

		for(auto&& idOfBuyBid : responseId){
			try{
				// Получаем данные заявки на покупку с самой высокой ценой
				pqxx::work txb{connectionCheckBD};
				pqxx::row responseBuy = txb.exec1(std::string("SELECT id, id_user, remain_value, price FROM bids "
						"WHERE id = ") + idOfBuyBid["id"].c_str() + ";");
				txb.commit();

				// Получаем данные заявки на продажу с самой низкой ценой
				pqxx::work txs{connectionCheckBD};
				pqxx::row responseSell = txs.exec1(std::string("select id, id_user, remain_value, price from bids ")
						+ "WHERE id_user != " + responseBuy["id_user"].c_str() + " and status = 'active' "
						+ "and bid_type = 'sell' and price <= " + responseBuy["price"].c_str() + " ORDER BY price DESC, date ASC LIMIT 1;");
				txs.commit();




				if(!std::string(responseSell["id"].c_str()).empty()){
					std::string request = "";

					valueBuy = std::stod(responseBuy["remain_value"].c_str());
					valueSell = std::stod(responseSell["remain_value"].c_str());
					priceBuy = std::stod(responseBuy["price"].c_str());
					priceSell = std::stod(responseSell["price"].c_str());

					// Помечаем заявку покупателя как исполненую
					if ((valueBuy - valueSell) <= 0){
						request += (std::string("") +
								"UPDATE bids SET status = 'executed' WHERE id = " + responseBuy["id"].c_str() + ";");
						value = valueBuy;
						std::cout << "valueBuy= " << std::to_string(value) << std::endl;
					}
					// Помечаем заявку продавца как исполненую
					if ((valueSell - valueBuy) <= 0){
						request += (std::string("") +
								"UPDATE bids SET status = 'executed' WHERE id = " + responseSell["id"].c_str() + ";");
						value = valueSell;
						std::cout << "valueSell= " << std::to_string(value) << std::endl;
					}

					// Проводим сделку

					request += (std::string("") +
							+ "UPDATE bids SET remain_value = remain_value - " + std::to_string(value)
							+ " WHERE id = " + responseSell["id"].c_str() + ";");

					request += (std::string("") +
							+ "UPDATE bids SET remain_value = remain_value - " + std::to_string(value)
							+ " WHERE id = " + responseBuy["id"].c_str() + ";");


					//Обновляем счёт rub и usd у продавца
					request += (std::string("")
							+ "UPDATE users SET (rub, usd) = (rub + " + std::to_string(priceSell * value) + ","
							" usd - " + std::to_string(valueBuy) + ") WHERE id = " + responseSell["id_user"].c_str() + ";");

					//Обновляем счёт rub и usd у покупателя
					request += (std::string("")
							+ "UPDATE users SET (rub, usd) = (rub - " + std::to_string(priceSell * value) + ","
							" usd + " + std::to_string(valueBuy) + ") WHERE id = " + responseBuy["id_user"].c_str() + ";");

					// Вписываем в поле продавца id заявки, с которой совершалась сделка
					request += (std::string("") +
							+ "UPDATE bids SET id_deal_with = " + responseBuy["id"].c_str()
							+ " WHERE id = " + responseSell["id"].c_str() + ";");

					// Вписываем в поле покупателя id заявки, с которой совершалась сделка
					request += (std::string("") +
							+ "UPDATE bids SET id_deal_with = " + responseSell["id"].c_str()
							+ " WHERE id = " + responseBuy["id"].c_str() + ";");

					std::cout << "request = \n" << request << std::endl;

					pqxx::work txe{connectionCheckBD};
					txe.exec0(request);
					txe.commit();
				}
			}
			catch (std::exception& e)
			{
				// TODO: появился баг. Одновременно пытаются сработать 3 обращения к базе данных из данной функции.
				std::cerr << "Exception in CheckDB(): " << e.what() << "\n";
			}
		}


	}
}

int main()
{
    try
    {
        boost::asio::io_service io_service;
        boost::thread_group worker_threads;
        static Core core;
        server s(io_service);
        worker_threads.create_thread(&CheckBD);

        std::cout << "Connected to " << c.dbname() << '\n';
        io_service.run();


    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

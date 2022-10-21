# Exchanger
Test job

Программы собираются на CMake.

Биржа по обмену RUB/USD.
Состоит из двух программ: Сервер и Клиент(Exchanger).
Сервер работает с базой данных(В частности, использовалась PostgreSQL) посредством библиотеки pqxx(для увязки БД и С++).
Exchanger представляет собой GUI, написанный на QT, включающий в себя такие функции, как:
Регистрация по имени и паролю в предварительном окне, предотвращающая дальнейший взаимодействие с программой в случае неверного ввода данных.
Получение id, имени и значений средств на счёте клиента для отображения значений вверху окна Клиента.
Получение Активных заявок и отображение в соответствующем GroupBox с возможностью просмотра каждого ордера в отдельном окне по клику и дальнейшей отмены по запросу.
Получение Выполненных заявок и отображение в соответствующем GroupBox для информирования пользователя о выполненных ранее и "свежих" ордерах на покупку/продажу с возможностью просмотра в отдельном окне по клику.
Мониторинг котировок, разделённый на продажу и покупку.
Каждый ордер, отображаемый в окне клиента имеет соответсвующий цвет(зелёный, красный).
Адаптивное окно клиента.


Порядок сборки:

Установить PostgreeSQL; 
Настроить базу данных в соответствии со следующими настройками:

CREATE DATABASE имя;

CREATE TABLE users(
  id bigserial not null primary key,
  username varchar(50) not null,
  rub real,
  usd real,
  psw varchar(18)
);

CREATE TABLE bids(
  id bigserial not null primary key,
  id_user bigserial not null,
  bid_type varchar(4) not null,
  value real not null,
  price real not null,
  status varchar(20),
  id_deal_with bigserial,
  showed bool,
  date timestamp without time zone,
  remain_value real not null
);



Установить библиотеку libpqxx:
mkdir lib
cd lib
Clone repo libpqxx in dir
cmake CMakeList.txt 
make
cmake --build .
cmake --install .


#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 8828;
static std::string conn_set = "host=localhost port=5432 dbname=checkbefore user=postgres password =qwerty";

namespace Requests
{
    static std::string Registration = "Reg";
    static std::string Bid = "Bid";
    static std::string RejectBid = "RjB";
    static std::string ShowQuotes = "ShQ";
    static std::string ShowBids = "ShB";
    static std::string CheckExecBids = "CEB";
    static std::string CheckActiveBids = "CAB";
    static std::string GetData = "ChD";
}

#endif //CLIENSERVERECN_COMMON_HPP

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <forward_list> // std::forward_list
#include "../include/hashtbl.h"

/*!
 * To illustrate the concept of a checking account
 */
struct Account{
    std::string client_name;
    int bank_code;
    int agency;
    int account;
    float balance;
    int m_balance;
    using AcctKey = std::pair < std::string, int >;

    Account(std::string name, int code, int agency, int account, float balance)
    : client_name(name), bank_code(code), agency(agency), account(account), balance(balance)
    {/*empty*/}

    Account(){
        client_name = " ";
        bank_code = 0;
        agency = 0;
        account = 0;
        balance = 0;
        m_balance = 0;
    }

    std::pair < std::string, int > getKey(){
        return {client_name, bank_code};
    }
};

/*!
 * Primary hash function to be applied on the key to get a
 * positive integer of type std::size_t.
 */
struct KeyHash{
    std::size_t operator()(const Account::AcctKey& k_) const{
        return std::hash<std::string>()(k_.first) xor
               std::hash<int>()(k_.second);
    }
};

/*!
 * A way to compare keys by equality so that it is possible to
 * search for equal keys during the linear search performed
 * on the list of collisions, when necessary.
 */
struct KeyEqual{
    bool operator()(const Account::AcctKey& lhs_, const Account::AcctKey& rhs_) const{
        return lhs_.first == rhs_.first and lhs_.second == rhs_.second;
    }
};

/// Checks if the contents of lhs and rhs are equal.
bool operator==(const Account &lhs, const Account &rhs){
    return lhs.account == rhs.account;
}

#endif
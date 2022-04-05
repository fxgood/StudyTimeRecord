#ifndef DATABASE_H
#define DATABASE_H

#include<QSql>
#include<QtDebug>
#include<QSqlQuery>
#include<QSqlError>

class Database
{
public:
    Database();
     bool getConnection();
     void disConnect();
private:
    QSqlDatabase db;
};

#endif // DATABASE_H

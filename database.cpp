#include "database.h"


//QSqlDatabase Database::db=QSqlDatabase::addDatabase("QODBC");   //创建静态数据库实例

Database::Database()
{
    db=QSqlDatabase::addDatabase("QODBC");
}


bool Database::getConnection(){

    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("worktimerecord");   //odbc数据源中设置的名字
    db.setUserName("root");
    db.setPassword("123456");
    return db.open();
}

void Database::disConnect(){
    db.close();
}

#ifndef DBTABLE_H
#define DBTABLE_H

#include <QString>

QString tableAccount = "TableAccount";
QString createTableAccount =
        "CREATE TABLE[TableAccount](\
        [account] VARCHAR(256) NOT NULL,\
        [pwd] VARCHAR(16) NOT NULL,\
        PRIMARY KEY([account]))";

#endif // DBTABLE_H

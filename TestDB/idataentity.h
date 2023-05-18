#ifndef IDATAENTITY_H
#define IDATAENTITY_H


class IDataEntity
{
public:
    virtual ~IDataEntity()
    {

    }
    virtual void release(){delete this;}
};

#endif // IDATAENTITY_H

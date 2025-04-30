#ifndef SALEABLE_H
#define SALEABLE_H

class Saleable
{
public:
    virtual void sell() = 0;
    virtual ~Saleable() {}
};

#endif
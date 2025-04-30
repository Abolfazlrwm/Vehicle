#ifndef STORESYSTEM_H
#define STORESYSTEM_H

#include "Vehicle.h"
using namespace std;
class StoreSystem
{
private:
    struct Node
    {
        Vehicle *data;
        Node *next;
        Node(Vehicle *v) : data(v), next(nullptr) {}
    };

    Node *head;

public:
    StoreSystem();
    void addVehicle(Vehicle *);
    void showAllItems();
    void runMenu();
    ~StoreSystem();
};

#endif
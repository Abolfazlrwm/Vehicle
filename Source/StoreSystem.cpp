#include "../Header/StoreSystem.h"
#include "../Header/Saleable.h"
#include "../Header/Rentable.h"
#include <iostream>
#include <cstdlib>
#include <limits>

StoreSystem::StoreSystem() : head(nullptr) {}

StoreSystem::~StoreSystem()
{
    Node *current = head;
    while (current)
    {
        Node *next = current->next;
        delete current->data;
        delete current;
        current = next;
    }
}

void StoreSystem::addVehicle(Vehicle *item)
{
    Node *newNode = new Node(item);
    if (!head)
        head = newNode;
    else
    {
        Node *temp = head;
        while (temp->next)
            temp = temp->next;
        temp->next = newNode;
    }
}

void StoreSystem::showAllItems()
{
    Node *temp = head;
    int index = 0;
    while (temp)
    {
        cout << "\nItem " << index++ << ":" << endl;
        temp->data->showInfo();
        cout << "--------------------------" << endl;
        temp = temp->next;
    }
}

void StoreSystem::runMenu()
{
    while (true)
    {
        system("cls");

        cout << "--- Vehicle Management System ---" << endl;
        cout << "1. Show All Items" << endl;
        cout << "2. Sell Item" << endl;
        cout << "3. Rent Item" << endl;
        cout << "4. Test Drive" << endl;
        cout << "5. Exit" << endl;

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "is NOT valid input!" << endl;
            cout << "Press Enter to continue...";
            cin.ignore().get();
            continue;
        }

        switch (choice)
        {
        case 1:
            showAllItems();
            cout << "\nPress Enter to continue...";
            cin.ignore().get();
            break;

        case 2:
        case 3:
        case 4:
        {
            int index;
            cout << "Enter item index: ";
            cin >> index;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid index!" << endl;
                cout << "Press Enter to continue...";
                cin.ignore().get();
                continue;
            }

            Node *current = head;
            int i = 0;
            while (current && i < index)
            {
                current = current->next;
                ++i;
            }

            if (!current)
            {
                cout << "is NOT valid index!";
                cout << "Press Enter to continue...";
                cin.ignore().get();
                continue;
            }

            if (choice == 2)
            {
                Saleable *s = dynamic_cast<Saleable *>(current->data);
                if (s)
                {
                    s->sell();
                    if (current == head)
                        head = head->next;
                    else
                    {
                        Node *prev = head;
                        while (prev->next != current)
                            prev = prev->next;
                        prev->next = current->next;
                    }

                    delete current->data;
                    delete current;
                }
                else
                    cout << "This item is NOT for sale." << endl;

                cout << "Press Enter to continue...";
                cin.ignore().get();
                break;
            }

            if (choice == 3)
            {
                Rentable *r = dynamic_cast<Rentable *>(current->data);
                if (r)
                {
                    int days;
                    cout << "How many days? ";
                    cin >> days;

                    if (cin.fail())
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "is NOT valid day count!" << endl;
                        cout << "Press Enter to continue...";
                        cin.ignore().get();
                        continue;
                    }
                    r->rent(days);
                }
                else
                    cout << "This item is not rentable." << endl;

                cout << "Press Enter to continue...";
                cin.ignore().get();
                break;
            }

            if (choice == 4)
            {
                current->data->drive();
                cout << "Press Enter to continue...";
                cin.ignore().get();
                break;
            }
            break;
        }

        case 5:
            cout << "Exiting..." << endl;
            return;

        default:
            cout << "Invalid option, try again." << endl;
            cout << "Press Enter to continue...";
            cin.ignore().get();
            break;
        }
    }
}
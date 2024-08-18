#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <sstream>
using namespace std;

#define BLUE "\033[34m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define GREEN "\033[32m"

bool flag = false;
int max_width = 100;
int max_height = 17;

void gotoxy();
void displayBox();
void choice();
void welcome();
void displayScreen();

void gotoxy(int x, int y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void displayBox()
{
    system("cls");
    cout << CYAN;
    for (int i = 0; i < max_width + 2; i++)
    {
        gotoxy(i, 0);
        cout << "x";
        gotoxy(i, max_height + 1);
        cout << "x";
    }
    for (int i = 1; i < max_height + 1; i++)
    {
        gotoxy(0, i);
        cout << "x";
        gotoxy(max_width + 1, i);
        cout << "x";
    }
    cout << RESET;
}

void welcome()
{
    cout << MAGENTA;
    gotoxy(30, 7);
    cout << "WELCOME TO OUT RAILWAY MANAGEMENT SYSTEM!!!";
    gotoxy(30, 8);
    cout << "Press any key to continue... ";
    while (_getch() != '\r')
        ;
    flag = true;
    cout << RESET;
}

void displayScreen()
{
    displayBox();
    welcome();
    choice();
}

class User
{
protected:
    string username;
    string password;

public:
    User() {}
    User(string uname, string pwd)
    {
        username = uname;
        password = pwd;
    }
    virtual bool login(const string &uname, const string &pwd) = 0;
};

struct TrainDetails
{
    string date;
    int totalSeats;
    int availableSeats;
    int seatsBooked;
};

struct passenger
{
    string name;
    int age;
    char gender;
};

struct Ticket
{
    string pnr;
    string departure;
    string arrival;
    string date;
    int numTickets;
    double fare;
    vector<passenger> passengers;
};

class RMS
{
protected:
    vector<Ticket> Tickets;
    vector<passenger> passengers;
    map<string, TrainDetails> a_trains;
    map<string, passenger> t_details;
    static const string trainfile;
    static const string bookingfile;

public:
    bool checkAvailability(string departure, string arrival, string date, int numTickets)
    {
        ifstream infile(trainfile);
        if (!infile)
        {
            cerr << "Error: Unable to open train file for reading." << endl;
            return false;
        }

        string line;
        while (getline(infile, line))
        {
            stringstream ss(line);
            string route, routeDate;
            int totalSeats, availableSeats, seatsBooked;

            ss >> route >> routeDate >> totalSeats >> availableSeats >> seatsBooked;

            if (route == departure + "-" + arrival)
            {
                if (routeDate == date && availableSeats >= numTickets)
                {
                    cout << "Tickets are available for " << numTickets << " passengers!!!..." << endl;
                    infile.close();
                    return true;
                }
                else if (routeDate != date)
                {
                    cout << "No train available for the specified date!!!.." << endl;
                    infile.close();
                    return false;
                }
                else if (availableSeats < numTickets)
                {
                    cout << "Insufficient seats available!!..." << endl;
                    infile.close();
                    return false;
                }
            }
        }

        cout << "Route not available." << endl;
        infile.close();
        return false;
    }

    // funciton fo r calculating ticket price
    double calculateprice(string departure, string arrival, int numTickets)
    {
        double baseprice = 1000;
        double totalprice = baseprice * numTickets;
        return totalprice;
    }

    // function for booking ticket
    void bookTicket(string departure, string arrival, string date, int numTickets)
    {
        if (!checkAvailability(departure, arrival, date, numTickets))
        {
            gotoxy(30, 11);
            cout << "Sorry, the requested tickets are not available." << endl;
            return;
        }

        double fare = calculateprice(departure, arrival, numTickets);

        passengers.clear();

        displayBox();
        gotoxy(30, 3);
        cout << "==PASSENGER DETAIL==";
        for (int i = 0; i < numTickets; ++i)
        {
            passenger p;

            gotoxy(10, 7 + i * 3);
            cout << "Passenger " << i + 1 << " Name: ";
            cin >> p.name;
            gotoxy(10, 8 + i * 3);
            cout << "Age: ";
            cin >> p.age;
            gotoxy(10, 9 + i * 3);
            cout << "Gender: ";
            cin >> p.gender;
            passengers.push_back(p);
        }
        srand(time(nullptr));
        string pnr = "PNR" + to_string(rand() % 800000 + 100000);

        Ticket ticket;
        ticket.pnr = pnr;
        ticket.departure = departure;
        ticket.arrival = arrival;
        ticket.date = date;
        ticket.numTickets = numTickets;
        ticket.fare = fare;
        ticket.passengers = passengers;

        Tickets.push_back(ticket);

        updateSeatsBooked(departure + "-" + arrival, numTickets);
        storebooking(pnr, departure, arrival, date, passengers);

        displayBox();
        gotoxy(30, 3);
        cout << "==Booking Details==";
        gotoxy(10, 7);
        cout << "PNR: " << pnr << endl;
        gotoxy(10, 8);
        cout << "From: " << departure << endl;
        gotoxy(10, 9);
        cout << "To: " << arrival << endl;
        gotoxy(10, 10);
        cout << "Date: " << date << endl;
        gotoxy(10, 11);
        cout << "Number of Tickets: " << numTickets << endl;
        gotoxy(10, 12);
        cout << "Fare per Ticket: Rs" << fare << endl;
        gotoxy(10, 13);

        gotoxy(30, 10);
        cout << "Total Fare: Rs" << fare * numTickets << endl;
    }

    // function for storing train details
    void storetraindetails()
    {
        ofstream outFile(trainfile, ios::app);
        if (!outFile)
        {
            gotoxy(30, 12);
            cerr << "Error opening trains file for writing." << endl;
            return;
        }

        for (const auto &train : a_trains)
        {
            outFile << train.first << " ";
            outFile << train.second.date << " ";
            outFile << train.second.totalSeats << " ";
            outFile << train.second.availableSeats << " ";
            outFile << train.second.seatsBooked << endl;
        }

        outFile.close();
    }

    // function for storing booking detail
    void storebooking(const string &pnr, const string &departure, const string &arrival, const string &date, const vector<passenger> &passengerDetails)
    {
        ofstream outFile(bookingfile, ios::app);
        if (!outFile)
        {
            cerr << "Error opening bookings file for writing." << endl;
            return;
        }

        outFile << "PNR: " << pnr << endl;
        outFile << "Route: " << departure << " to " << arrival << endl;
        outFile << "Date of Travel: " << date << endl;
        outFile << "Passenger Details:" << endl;
        for (const auto &passenger : passengerDetails)
        {
            outFile << "Name: " << passenger.name << ", Age: " << passenger.age << ", Gender: " << passenger.gender << endl;
        }
        outFile << endl;

        outFile.close();
    }

    void displayPassenger(const string &pnr)
    {
        ifstream infile(bookingfile);
        if (!infile)
        {
            cerr << "Error: Unable to open booking file for reading." << endl;
            return;
        }

        string line;
        bool found = false;
        while (getline(infile, line))
        {
            size_t pos = line.find("PNR: ");
            if (pos != string::npos)
            {
                string temp = line.substr(pos + 5);
                if (temp == pnr)
                {
                    gotoxy(10, 8);
                    cout << "PNR details: " << pnr << endl;
                    found = true;
                    int i = 0;
                    while (getline(infile, line) && !line.empty())
                    {
                        gotoxy(10, i + 9);
                        cout << line << endl;
                    }
                    break;
                }
            }
        }

        if (!found)
        {
            gotoxy(40, 9);
            cout << "no data found for for PNR: " << pnr << endl;
        }

        infile.close();
    }

    // function for updating seat or adding new train
    // fo r admin access
    void updateSeatsBooked(const string &route, int numTickets)
    {
        ofstream infile(trainfile);
        if (!infile)
        {
            gotoxy(30, 12);
            cerr << "Error opening trains file for writing." << endl;
            return;
        }
    }

    void routeCheck(const string &departure, const string &arrival)
    {
        ifstream infile(trainfile);
        if (!infile)
        {
            cerr << "Error: Unable to open train file for reading." << endl;
            return;
        }

        displayBox();
        gotoxy(30, 4);
        cout << "Trains Available for Route: " << departure << " to " << arrival << endl;
        gotoxy(10, 7);
        cout << "Departure" << "\t" << "Arrival" << "\t" << "Date" << "\t" << "Total Seats" << "\t" << "Available Seats" << endl;

        string line;
        int i = 0;
        while (getline(infile, line))
        {
            stringstream ss(line);
            string route, routeDate;
            int totalSeats, availableSeats, seatsBooked;
            ss >> route >> routeDate >> totalSeats >> availableSeats >> seatsBooked;

            if (route.find(departure) != string::npos && route.find(arrival) != string::npos)
            {
                gotoxy(10, i + 8);
                cout << departure << "\t" << arrival << "\t" << routeDate << "\t" << totalSeats << "\t" << availableSeats << endl;
                i++;
            }
        }

        infile.close();
    }
};
const string RMS::trainfile = "train.txt";
const string RMS::bookingfile = "booking.txt";

class Admin : public User,
              public RMS
{
private:
    static const string adminfile;

public:
    Admin(string uname, string pwd) : User(uname, pwd) {}

    bool login(const string &uname, const string &pwd)
    {
        ifstream file(adminfile);
        if (!file.is_open())
        {
            gotoxy(30, 12);
            cerr << "Error: File not found." << endl;
            return false;
        }

        string line;
        while (getline(file, line))
        {
            if (line == username + " " + password)
            {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    void changeTicketAvailability()
    {
        displayBox();
        string departure, arrival, date;
        int seat;

        gotoxy(40, 4);
        cout << "==Change ticket availability==";
        gotoxy(10, 7);
        cout << "Enter departure city: ";
        cin >> departure;
        gotoxy(10, 8);
        cout << "Enter arrival city: ";
        cin >> arrival;
        gotoxy(10, 9);
        cout << "Enter date (DD/MM/YYYY): ";
        cin >> date;

        string route = departure + "-" + arrival;

        gotoxy(10, 13);
        cout << "Enter number of seats (-1 to add a new route): ";
        cin >> seat;

        if (seat == -1)
        {
            TrainDetails newRoute;
            int seats;

            displayBox();
            gotoxy(10, 7);
            cout << "Enter number of seats for new route: ";
            cin >> seats;
            newRoute.date = date;
            newRoute.totalSeats = seats;
            newRoute.availableSeats = seats;
            newRoute.seatsBooked = 0;
            a_trains[route] = newRoute;

            displayBox();
            gotoxy(30, 10);
            cout << "New route added successfully!" << endl;
            gotoxy(30, 12);
            cout << "press any key to continue.. ";
            cin.get();
        }
        else
        {
            if (a_trains.find(route) == a_trains.end())
            {
                gotoxy(30, 10);
                cout << "Route not available. Cannot change availability." << endl;
                return;
            }

            TrainDetails &existingRoute = a_trains[route];
            existingRoute.availableSeats = seat;

            cout << "Ticket availability updated successfully!" << endl;
        }

        storetraindetails();
    }
};
const string Admin::adminfile = "admin.txt";

class Passenger : public User, public RMS
{
public:
    static const string passengerfile;

public:
    Passenger(string uname, string pwd)
        : User(uname, pwd)
    {
    }

    bool login(const string &uname, const string &pwd)
    {
        ifstream file(passengerfile);
        if (!file.is_open())
        {
            gotoxy(30, 12);
            cerr << "Error: File not found." << endl;
            return false;
        }

        string line;
        while (getline(file, line))
        {
            if (line == username + " " + password)
            {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
};
const string Passenger::passengerfile = "passenger.txt";

bool createAccount(string uname, string pwd, string cpwd)
{
    const string passengerfile = "passenger.txt";
    if (pwd != cpwd)
    {
        gotoxy(30, 12);
        cout << "Password didn't match!!..";
        gotoxy(30, 13);
        cout << "Press Enter to continue.. ";
        cin.ignore();
        cin.get();
        return 0;
    }
    ofstream outfile(passengerfile, ios::app);
    if (!outfile.is_open())
    {
        gotoxy(30, 12);
        cerr << "Error: File not found." << endl;
        return 0;
    }

    outfile << uname + " " + pwd << endl;
    outfile.close();
    return true;
}

void choice()
{
    while (true)
    {
        displayBox();
        gotoxy(40, 4);
        cout << BLUE << "=== Login Menu ===" << endl;
        gotoxy(10, 7);
        cout << "1. Admin Login" << endl;
        gotoxy(10, 8);
        cout << "2. Passenger Login" << endl;
        gotoxy(10, 9);
        cout << "3. Create new Account" << endl;
        gotoxy(10, 10);
        cout << "4. Exit" << endl;
        gotoxy(10, 11);
        cout << "Enter your choice: " << RESET;

        int choice;
        cin >> choice;
        string pnr;

        switch (choice)
        {
        case 1:
        {
            string adminUsername, adminPassword;
            displayBox();

            gotoxy(40, 4);
            cout << GREEN << "=== Login Menu ===" << endl;
            gotoxy(10, 7);
            cout << "Enter admin username: ";
            cin >> adminUsername;
            gotoxy(10, 8);
            cout << "Enter admin password: ";
            cin >> adminPassword;
            cout << RESET;

            Admin admin(adminUsername, adminPassword);

            if (admin.login(adminUsername, adminPassword))
            {
                gotoxy(30, 10);
                cout << "Admin logged in successfully!!!" << endl;

                bool isLoggedIn = true;
                while (isLoggedIn)
                {
                    displayBox();
                    cout << GREEN;
                    gotoxy(40, 4);
                    cout << "=== Admin Menu ===" << endl;
                    gotoxy(10, 7);
                    cout << "1. Change Ticket Availability" << endl;
                    gotoxy(10, 8);
                    cout << "2. Logout" << endl;
                    gotoxy(10, 9);
                    cout << "Enter your choice: " << RESET;

                    int adminChoice;
                    cin >> adminChoice;

                    switch (adminChoice)
                    {
                    case 1:
                        admin.changeTicketAvailability();
                        break;
                    case 2:
                        isLoggedIn = false;
                        goto logout;
                    default:
                        gotoxy(40, 11);
                        cout << "Invalid choice. Please enter a valid option." << endl;
                    }
                    gotoxy(40, 12);
                    cout << "Press Enter to continue...";
                    cin.ignore();
                    cin.get();
                }
            }
            else
            {
                gotoxy(40, 11);
                cout << "Invalid admin credentials." << endl;
            }
            break;
        }

        case 2:
        {
            displayBox();
            gotoxy(40, 4);
            cout << GREEN << "=== Login Menu ===" << endl;
            string passengerUsername, passengerPassword;
            gotoxy(10, 7);
            cout << "Enter passenger username: ";
            cin >> passengerUsername;
            gotoxy(10, 8);
            cout << "Enter passenger password: ";
            cin >> passengerPassword;
            cout << RESET;

            Passenger passenger(passengerUsername, passengerPassword);
            if (passenger.login(passengerUsername, passengerPassword))
            {
                displayBox();
                gotoxy(3, 3);
                cout << "Passenger logged in successfully!" << endl;

                bool isLoggedIn = true;
                while (isLoggedIn)
                {
                    displayBox();
                    gotoxy(40, 4);
                    cout << "=== Passenger Menu ===" << endl;
                    gotoxy(10, 7);
                    cout << "1. Book Tickets" << endl;
                    gotoxy(10, 8);
                    cout << "2. Display Passenger Details" << endl;
                    gotoxy(10, 9);
                    cout << "3. Check available trains..." << endl;
                    gotoxy(10, 10);
                    cout << "3. Logout" << endl;
                    gotoxy(10, 11);
                    cout << "Enter your choice: ";

                    int passengerChoice;
                    cin >> passengerChoice;

                    switch (passengerChoice)
                    {
                    case 1:
                    {
                        string departure, arrival, date;
                        int numTickets;

                        displayBox();
                        gotoxy(40, 3);
                        cout << "BOOK TICKETS";
                        gotoxy(10, 6);
                        cout << "Enter departure city: ";
                        cin >> departure;
                        gotoxy(10, 7);
                        cout << "Enter arrival city: ";
                        cin >> arrival;
                        gotoxy(10, 8);
                        cout << "Enter date (DD/MM/YYYY): ";
                        cin >> date;
                        gotoxy(10, 9);
                        cout << "Enter number of tickets: ";
                        cin >> numTickets;

                        passenger.bookTicket(departure, arrival, date, numTickets);
                        break;
                    }
                    case 2:
                    {
                        displayBox();
                        gotoxy(30, 4);
                        cout << "==GET THE PASSENGER DETAILS==";
                        gotoxy(10, 7);
                        cout << "Enter the PNR number: ";
                        cin >> pnr;
                        passenger.displayPassenger(pnr);
                        break;
                    }
                    case 3:
                    {
                        string departure, arrival;
                        displayBox();
                        gotoxy(30, 4);
                        cout << GREEN << "=== Check Train Schedule and Availability ===" << endl;
                        gotoxy(10, 7);
                        cout << "Enter departure city: ";
                        cin >> departure;
                        gotoxy(10, 8);
                        cout << "Enter arrival city: ";
                        cin >> arrival;
                        cout << RESET;

                        passenger.routeCheck(departure, arrival);
                        break;
                    }
                    case 4:
                        isLoggedIn = false;
                        goto logout;
                    default:
                        gotoxy(40, 11);
                        cout << "Invalid choice. Please enter a valid option." << endl;
                    }

                    gotoxy(40, 12);
                    cout << "Press Enter to continue...";
                    cin.ignore();
                    cin.get();
                }
            }
            else
            {
                gotoxy(40, 11);
                cout << "Invalid passenger credentials." << endl;
            }
            break;
        }
        case 3:
        {
            string uname, pwd, cpwd;
            displayBox();
            gotoxy(30, 4);
            cout << "==Create new passenger account==";
            gotoxy(10, 7);
            cout << "Enter new Username: ";
            cin >> uname;
            gotoxy(10, 8);
            cout << "Enter Password: ";
            cin >> pwd;
            gotoxy(10, 9);
            cout << "Confirm Password: ";
            cin >> cpwd;

            bool acc = createAccount(uname, pwd, cpwd);
            if (!acc)
            {
                gotoxy(30, 13);
                cout << "ACCOUNT NOT CREATED!!! ";
            }
            gotoxy(30, 13);
            cout << "ACCOUNT CREATED SUCCESSFULLY!!!.. ";
        }
        case 4:
            return;
        default:
            gotoxy(40, 11);
            cout << "Invalid choice. Please enter a valid option." << endl;
        }
        gotoxy(40, 12);
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    logout:
        gotoxy(40, 12);
        cout << "logging out..";
        gotoxy(40, 13);
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

int main()
{
    displayScreen();
    gotoxy(40, 13);
    cout << "Exiting the program...";
    gotoxy(40,14);
    cout<<"Press Enter to Continue...// ";
    cin.ignore();
    cin.get();
    system("cls");
    return 0;
}
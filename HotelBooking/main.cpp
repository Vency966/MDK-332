#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <random>
#include <algorithm>
#include <stdexcept>

using namespace std;

// ============================================================
// Date utility (classic time.h)
// ============================================================
class DateUtils {
public:
    static time_t parseDate(const string& s) {
        int y = 0, m = 0, d = 0;
        char dash1 = 0, dash2 = 0;
        stringstream ss(s);
        ss >> y >> dash1 >> m >> dash2 >> d;
        if (ss.fail() || dash1 != '-' || dash2 != '-') {
            throw invalid_argument("Bad date format. Use YYYY-MM-DD");
        }
        tm t{};
        t.tm_year = y - 1900;
        t.tm_mon = m - 1;
        t.tm_mday = d;
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        t.tm_isdst = -1;
        time_t result = mktime(&t);
        if (result == -1) throw invalid_argument("Invalid date");
        return result;
    }

    static string formatDate(time_t t) {
        tm* lt = localtime(&t);
        char buf[16];
        strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
        return string(buf);
    }

    static time_t today() {
        time_t now = time(nullptr);
        tm* lt = localtime(&now);
        lt->tm_hour = 0;
        lt->tm_min = 0;
        lt->tm_sec = 0;
        return mktime(lt);
    }

    static int nightsBetween(time_t from, time_t to) {
        return (int)((to - from) / (60 * 60 * 24));
    }

    static time_t addDays(time_t t, int days) {
        return t + (time_t)days * 60 * 60 * 24;
    }
};

// ============================================================
// Room type
// ============================================================
enum class RoomType { Standard, Comfort, Suite, President };

string roomTypeName(RoomType t) {
    switch (t) {
        case RoomType::Standard:  return "Standard";
        case RoomType::Comfort:   return "Comfort";
        case RoomType::Suite:     return "Suite";
        case RoomType::President: return "President";
    }
    return "?";
}

double roomTypePrice(RoomType t) {
    switch (t) {
        case RoomType::Standard:  return 3000.0;
        case RoomType::Comfort:   return 5000.0;
        case RoomType::Suite:     return 9000.0;
        case RoomType::President: return 20000.0;
    }
    return 0.0;
}

// ============================================================
// Room
// ============================================================
struct Room {
    int number;
    RoomType type;
    int floor;

    Room(int n, RoomType t, int f) : number(n), type(t), floor(f) {}
};

// ============================================================
// Booking
// ============================================================
enum class BookingStatus { Active, CheckedIn, CheckedOut, Cancelled };

string bookingStatusName(BookingStatus s) {
    switch (s) {
        case BookingStatus::Active:     return "Active";
        case BookingStatus::CheckedIn:  return "CheckedIn";
        case BookingStatus::CheckedOut: return "CheckedOut";
        case BookingStatus::Cancelled:  return "Cancelled";
    }
    return "?";
}

struct Booking {
    int id;
    string guestName;
    string passport;
    int roomNumber;
    time_t checkIn;
    time_t checkOut;
    BookingStatus status;

    int nights() const {
        return DateUtils::nightsBetween(checkIn, checkOut);
    }
};

// ============================================================
// Utility class: booking generator (task for "4")
// ============================================================
class BookingGenerator {
public:
    static Booking generate(int id, const vector<Room>& rooms, mt19937& rng) {
        const vector<string>& names = firstNamesRef();
        const vector<string>& surnames = surnamesRef();

        uniform_int_distribution<int> roomDist(0, (int)rooms.size() - 1);
        uniform_int_distribution<int> nameDist(0, (int)names.size() - 1);
        uniform_int_distribution<int> surnameDist(0, (int)surnames.size() - 1);
        uniform_int_distribution<int> offsetDist(0, 30);
        uniform_int_distribution<int> stayDist(1, 10);
        uniform_int_distribution<int> passportDist(100000, 999999);

        Booking b;
        b.id = id;
        b.guestName = names[nameDist(rng)] + " " + surnames[surnameDist(rng)];
        b.passport = to_string(passportDist(rng)) + " " + to_string(passportDist(rng));
        b.roomNumber = rooms[roomDist(rng)].number;

        time_t today = DateUtils::today();
        b.checkIn = DateUtils::addDays(today, offsetDist(rng));
        b.checkOut = DateUtils::addDays(b.checkIn, stayDist(rng));
        b.status = BookingStatus::Active;
        return b;
    }

private:
    static const vector<string>& firstNamesRef() {
        static const vector<string> v = {
            "Ivan", "Petr", "Sergey", "Alexey", "Dmitry",
            "Anna", "Maria", "Olga", "Ekaterina", "Natalia"
        };
        return v;
    }
    static const vector<string>& surnamesRef() {
        static const vector<string> v = {
            "Ivanov", "Petrov", "Sidorov", "Smirnov", "Kuznetsov",
            "Popov", "Sokolov", "Lebedev", "Kozlov", "Novikov"
        };
        return v;
    }
};

// ============================================================
// Hotel
// ============================================================
class Hotel {
private:
    vector<Room> rooms;
    vector<Booking> bookings;
    int nextBookingId;

    bool isRoomFree(int roomNumber, time_t from, time_t to) const {
        for (size_t i = 0; i < bookings.size(); ++i) {
            const Booking& b = bookings[i];
            if (b.roomNumber != roomNumber) continue;
            if (b.status == BookingStatus::Cancelled) continue;
            if (b.status == BookingStatus::CheckedOut) continue;

            if (from < b.checkOut && to > b.checkIn) {
                return false;
            }
        }
        return true;
    }

public:
    Hotel() : nextBookingId(1) {
        for (int i = 1; i <= 100; ++i) {
            RoomType t;
            if (i <= 40) t = RoomType::Standard;
            else if (i <= 70) t = RoomType::Comfort;
            else if (i <= 90) t = RoomType::Suite;
            else t = RoomType::President;

            int floor = (i - 1) / 10 + 1;
            rooms.push_back(Room(i, t, floor));
        }
    }

    const vector<Room>& getRooms() const { return rooms; }
    const vector<Booking>& getBookings() const { return bookings; }

    void showAvailableRooms(time_t from, time_t to) const {
        cout << "\n=== Available rooms from " << DateUtils::formatDate(from)
             << " to " << DateUtils::formatDate(to) << " ===\n";
        cout << left << setw(8) << "Room"
             << setw(20) << "Type"
             << setw(8) << "Floor"
             << setw(12) << "Price/night" << "\n";
        cout << string(50, '-') << "\n";

        int count = 0;
        for (size_t i = 0; i < rooms.size(); ++i) {
            const Room& r = rooms[i];
            if (isRoomFree(r.number, from, to)) {
                cout << left << setw(8) << r.number
                     << setw(20) << roomTypeName(r.type)
                     << setw(8) << r.floor
                     << setw(12) << roomTypePrice(r.type) << "\n";
                ++count;
            }
        }
        if (count == 0) {
            cout << "No free rooms for these dates.\n";
        } else {
            cout << "\nTotal available: " << count << "\n";
        }
    }

    bool bookRoom(const string& guest, const string& passport,
                  int roomNumber, time_t from, time_t to) {
        int roomIdx = -1;
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i].number == roomNumber) { roomIdx = (int)i; break; }
        }
        if (roomIdx == -1) {
            cout << "Error: room " << roomNumber << " does not exist.\n";
            return false;
        }
        if (to <= from) {
            cout << "Error: check-out date must be after check-in date.\n";
            return false;
        }
        if (!isRoomFree(roomNumber, from, to)) {
            cout << "Error: room " << roomNumber << " is occupied for these dates.\n";
            return false;
        }

        Booking b;
        b.id = nextBookingId++;
        b.guestName = guest;
        b.passport = passport;
        b.roomNumber = roomNumber;
        b.checkIn = from;
        b.checkOut = to;
        b.status = BookingStatus::Active;
        bookings.push_back(b);

        cout << "Booking #" << b.id << " created. Room "
             << roomNumber << ", " << b.nights() << " nights, price: "
             << b.nights() * roomTypePrice(rooms[roomIdx].type) << " RUB\n";
        return true;
    }

    bool cancelBooking(int id) {
        for (size_t i = 0; i < bookings.size(); ++i) {
            Booking& b = bookings[i];
            if (b.id == id) {
                if (b.status == BookingStatus::Cancelled) {
                    cout << "Booking already cancelled.\n";
                    return false;
                }
                if (b.status == BookingStatus::CheckedIn) {
                    cout << "Cannot cancel: guest already checked in.\n";
                    return false;
                }
                b.status = BookingStatus::Cancelled;
                cout << "Booking #" << id << " cancelled.\n";
                return true;
            }
        }
        cout << "Booking #" << id << " not found.\n";
        return false;
    }

    bool checkIn(int id) {
        for (size_t i = 0; i < bookings.size(); ++i) {
            Booking& b = bookings[i];
            if (b.id == id) {
                if (b.status != BookingStatus::Active) {
                    cout << "Cannot check in: status is "
                         << bookingStatusName(b.status) << ".\n";
                    return false;
                }
                b.status = BookingStatus::CheckedIn;
                cout << "Guest " << b.guestName << " checked in to room "
                     << b.roomNumber << ".\n";
                return true;
            }
        }
        cout << "Booking #" << id << " not found.\n";
        return false;
    }

    bool checkOut(int id) {
        for (size_t i = 0; i < bookings.size(); ++i) {
            Booking& b = bookings[i];
            if (b.id == id) {
                if (b.status != BookingStatus::CheckedIn) {
                    cout << "Cannot check out: guest not checked in (status "
                         << bookingStatusName(b.status) << ").\n";
                    return false;
                }
                b.status = BookingStatus::CheckedOut;

                double pricePerNight = 0;
                for (size_t j = 0; j < rooms.size(); ++j) {
                    if (rooms[j].number == b.roomNumber) {
                        pricePerNight = roomTypePrice(rooms[j].type);
                        break;
                    }
                }
                double price = b.nights() * pricePerNight;

                cout << "Guest " << b.guestName << " checked out from room "
                     << b.roomNumber << ".\n";
                cout << "   Total: " << b.nights() << " nights * "
                     << pricePerNight << " RUB = "
                     << price << " RUB\n";
                return true;
            }
        }
        cout << "Booking #" << id << " not found.\n";
        return false;
    }

    void showAllBookings() const {
        cout << "\n=== All bookings ===\n";
        if (bookings.empty()) {
            cout << "No bookings yet.\n";
            return;
        }
        cout << left << setw(5) << "ID"
             << setw(20) << "Guest"
             << setw(8) << "Room"
             << setw(12) << "CheckIn"
             << setw(12) << "CheckOut"
             << setw(8) << "Nights"
             << setw(12) << "Price"
             << setw(12) << "Status" << "\n";
        cout << string(90, '-') << "\n";

        for (size_t i = 0; i < bookings.size(); ++i) {
            const Booking& b = bookings[i];
            double price = 0;
            for (size_t j = 0; j < rooms.size(); ++j) {
                if (rooms[j].number == b.roomNumber) {
                    price = b.nights() * roomTypePrice(rooms[j].type);
                    break;
                }
            }
            cout << left << setw(5) << b.id
                 << setw(20) << b.guestName
                 << setw(8) << b.roomNumber
                 << setw(12) << DateUtils::formatDate(b.checkIn)
                 << setw(12) << DateUtils::formatDate(b.checkOut)
                 << setw(8) << b.nights()
                 << setw(12) << price
                 << setw(12) << bookingStatusName(b.status) << "\n";
        }
    }

    void calcPrice(int id) const {
        for (size_t i = 0; i < bookings.size(); ++i) {
            const Booking& b = bookings[i];
            if (b.id == id) {
                for (size_t j = 0; j < rooms.size(); ++j) {
                    if (rooms[j].number == b.roomNumber) {
                        cout << "\nBooking #" << id << " (" << b.guestName << ")\n"
                             << "Room: " << rooms[j].number << " ("
                             << roomTypeName(rooms[j].type) << ")\n"
                             << "Period: " << DateUtils::formatDate(b.checkIn)
                             << " - " << DateUtils::formatDate(b.checkOut) << "\n"
                             << "Nights: " << b.nights() << "\n"
                             << "Price per night: " << roomTypePrice(rooms[j].type) << " RUB\n"
                             << "TOTAL: " << b.nights() * roomTypePrice(rooms[j].type) << " RUB\n";
                        return;
                    }
                }
            }
        }
        cout << "Booking #" << id << " not found.\n";
    }

    void addGenerated(const Booking& b) {
        bookings.push_back(b);
        if (b.id >= nextBookingId) nextBookingId = b.id + 1;
    }
};

// ============================================================
// Menu
// ============================================================
void printMenu() {
    cout << "\n========== HOTEL ==========\n"
         << "1. Show available rooms for dates\n"
         << "2. Book a room\n"
         << "3. Cancel booking\n"
         << "4. Check-in\n"
         << "5. Check-out\n"
         << "6. Show all bookings\n"
         << "7. Calculate booking price\n"
         << "8. Generate test bookings (utility)\n"
         << "0. Exit\n"
         << "Choice: ";
}

int main() {
    Hotel hotel;

    random_device rd;
    mt19937 rng(rd());

    cout << "Welcome to the hotel booking system!\n";
    cout << "Today: " << DateUtils::formatDate(DateUtils::today()) << "\n";

    int choice;
    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        try {
            if (choice == 0) {
                cout << "Goodbye!\n";
                break;
            }
            else if (choice == 1) {
                string d1, d2;
                cout << "Check-in date (YYYY-MM-DD): "; cin >> d1;
                cout << "Check-out date (YYYY-MM-DD): "; cin >> d2;
                hotel.showAvailableRooms(DateUtils::parseDate(d1), DateUtils::parseDate(d2));
            }
            else if (choice == 2) {
                string name, passport, d1, d2;
                int num;
                cin.ignore();
                cout << "Guest name: "; getline(cin, name);
                cout << "Passport: "; getline(cin, passport);
                cout << "Room number: "; cin >> num;
                cout << "Check-in date (YYYY-MM-DD): "; cin >> d1;
                cout << "Check-out date (YYYY-MM-DD): "; cin >> d2;
                hotel.bookRoom(name, passport, num,
                               DateUtils::parseDate(d1), DateUtils::parseDate(d2));
            }
            else if (choice == 3) {
                int id; cout << "Booking ID: "; cin >> id;
                hotel.cancelBooking(id);
            }
            else if (choice == 4) {
                int id; cout << "Booking ID: "; cin >> id;
                hotel.checkIn(id);
            }
            else if (choice == 5) {
                int id; cout << "Booking ID: "; cin >> id;
                hotel.checkOut(id);
            }
            else if (choice == 6) {
                hotel.showAllBookings();
            }
            else if (choice == 7) {
                int id; cout << "Booking ID: "; cin >> id;
                hotel.calcPrice(id);
            }
            else if (choice == 8) {
                int n;
                cout << "How many bookings to generate? "; cin >> n;
                for (int i = 0; i < n; ++i) {
                    Booking b = BookingGenerator::generate(
                        (int)hotel.getBookings().size() + i + 1,
                        hotel.getRooms(), rng);
                    hotel.addGenerated(b);
                }
                cout << "Generated " << n << " bookings.\n";
            }
            else {
                cout << "Invalid menu option.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}
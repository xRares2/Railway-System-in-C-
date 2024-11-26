#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <regex>

using namespace std;

// parole - criptare și verificare
class PasswordUtils
{
public:
    static string encrypt(const string &text, const string &key)
    {
        string encryptedText;
        int keyLength = key.length();

        for (size_t i = 0; i < text.length(); i++)
        {
            char c = text[i];
            if (isalpha(c))
            {
                char offset = isupper(c) ? 'A' : 'a';
                char keyChar = key[i % keyLength] - offset;
                c = (c - offset + keyChar) % 26 + offset;
            }
            encryptedText += c;
        }
        return encryptedText;
    }

    static string decrypt(const string &text, const string &key)
    {
        string decryptedText;
        int keyLength = key.length();

        for (size_t i = 0; i < text.length(); i++)
        {
            char c = text[i];
            if (isalpha(c))
            {
                char offset = isupper(c) ? 'A' : 'a';
                char keyChar = key[i % keyLength] - offset;
                c = (c - offset - keyChar + 26) % 26 + offset;
            }
            decryptedText += c;
        }
        return decryptedText;
    }

    static string evaluatePasswordStrength(const string &password)
    {
        if (password.length() < 8)
            return "weak";
        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;

        for (char c : password)
        {
            if (isupper(c))
                hasUpper = true;
            else if (islower(c))
                hasLower = true;
            else if (isdigit(c))
                hasDigit = true;
            else
                hasSpecial = true;
        }

        if (hasUpper && hasLower && hasDigit && hasSpecial)
            return "good";
        if (hasUpper && hasLower && hasDigit)
            return "ok";
        return "weak";
    }
};

class User
{
private:
    string email;
    string encryptedPassword;
    const string encryptionKey = "Cheie"; // cheia pentru Vigenère Cipher

public:
    User() = default;

    User(const string &email, const string &password)
    {
        this->email = email;
        this->encryptedPassword = PasswordUtils::encrypt(password, encryptionKey);
    }

    static bool isValidEmail(const string &email)
    {
        regex emailRegex("^[a-zA-Z0-9_+&*-]+(?:\\.[a-zA-Z0-9_+&*-]+)*@(?:[a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,7}$");
        return regex_match(email, emailRegex);
    }

    static User login(const string &email, const string &password)
    {
        ifstream file("users.csv");
        string line, fileEmail, filePassword;

        while (getline(file, line))
        {
            stringstream ss(line);
            getline(ss, fileEmail, ',');
            getline(ss, filePassword, ',');

            if (fileEmail == email && filePassword == PasswordUtils::encrypt(password, "Cheie"))
            {
                cout << "Login reușit pentru utilizator: " << email << endl;
                return User(email, password);
            }
        }

        throw invalid_argument("Email sau parolă incorectă!");
    }

    void saveToFile() const
    {
        ofstream file("users.csv", ios::app);
        file << email << "," << encryptedPassword << endl;
        file.close();
    }

    static void registerUser(const string &email, const string &password)
    {
        if (!isValidEmail(email))
        {
            throw invalid_argument("Email invalid!");
        }

        string strength = PasswordUtils::evaluatePasswordStrength(password);
        cout << "Parola ta este: " << strength << endl;
        if (strength == "weak")
        {
            throw invalid_argument("Parola este prea slabă! Te rugăm să o modifici.");
        }

        string passwordRepeat;
        cout << "Repetă parola: ";
        cin >> passwordRepeat;

        if (password != passwordRepeat)
        {
            throw invalid_argument("Parolele nu coincid! Te rugăm să le introduci din nou.");
        }

        User newUser(email, password);
        newUser.saveToFile();
        cout << "Cont creat cu succes!" << endl;
    }
};

// Clasa pentru gestionarea curselor
class Trip
{
private:
    string departureCity;
    string arrivalCity;
    string date;
    string time;

public:
    Trip(const string &departure, const string &arrival, const string &date, const string &time)
        : departureCity(departure), arrivalCity(arrival), date(date), time(time) {}

    void saveToFile() const
    {
        ofstream file("trips.csv", ios::app);
        file << departureCity << "," << arrivalCity << "," << date << "," << time << endl;
        file.close();
    }

    static void listTrips()
    {
        ifstream file("trips.csv");
        string line;

        cout << "Curse disponibile:" << endl;
        while (getline(file, line))
        {
            cout << line << endl;
        }
    }

    static bool isValidCityName(const string &city)
    {
        for (char c : city)
        {
            if (!isalpha(c) && c != ' ')
                return false;
        }
        return true;
    }

    static bool isValidDate(const string &date)
    {
        regex dateRegex("^(\\d{4})-(\\d{2})-(\\d{2})$");
        return regex_match(date, dateRegex);
    }

    static bool isValidTime(const string &time)
    {
        regex timeRegex("^(\\d{2}):(\\d{2})$");
        return regex_match(time, timeRegex);
    }

    static void addTrip(const string &departure, const string &arrival, const string &date, const string &time)
    {
        if (!isValidCityName(departure) || !isValidCityName(arrival))
        {
            throw invalid_argument("Orașul conține caractere nevalide!");
        }

        if (!isValidDate(date))
        {
            throw invalid_argument("Data este într-un format invalid!");
        }

        if (!isValidTime(time))
        {
            throw invalid_argument("Ora este într-un format invalid!");
        }

        Trip trip(departure, arrival, date, time);
        trip.saveToFile();
        cout << "Cursa a fost adăugată!" << endl;
    }

    static void deleteTrip(const string &departure, const string &arrival, const string &date, const string &time)
    {
        ifstream file("trips.csv");
        ofstream tempFile("temp_trips.csv");
        string line;

        bool found = false;
        while (getline(file, line))
        {
            stringstream ss(line);
            string dep, arr, dat, tim;
            getline(ss, dep, ',');
            getline(ss, arr, ',');
            getline(ss, dat, ',');
            getline(ss, tim, ',');

            if (dep == departure && arr == arrival && dat == date && tim == time)
            {
                found = true; 
                continue;
            }
            tempFile << line << endl;
        }

        file.close();
        tempFile.close();

        if (found)
        {
            remove("trips.csv");
            rename("temp_trips.csv", "trips.csv");
            cout << "Cursa a fost ștearsă!" << endl;
        }
        else
        {
            remove("temp_trips.csv");
            throw invalid_argument("Cursa nu a fost găsită!");
        }
    }
};

int main()
{
    int choice;
    string role;
    cout << "Sunteti operator sau utilizator? (operator/utilizator): ";
    cin >> role;

    if (role == "utilizator")
    {
        cout << "1. Înregistrare utilizator\n2. Autentificare utilizator\n3. Listare curse\n4. Rezervare loc\nAlege opțiunea: ";
        cin >> choice;

        try
        {
            if (choice == 1)
            {
                string email, password;
                cout << "Email: ";
                cin >> email;
                cout << "Parolă: ";
                cin >> password;

                User::registerUser(email, password);
            }
            else if (choice == 2)
            {
                string email, password;
                cout << "Email: ";
                cin >> email;
                cout << "Parolă: ";
                cin >> password;

                User user = User::login(email, password);
            }
            else if (choice == 3)
            {
                Trip::listTrips();
            }
            else if (choice == 4)
            {
                string departure, arrival, date, time, trainClass;
                cout << "Oraș plecare: ";
                cin >> departure;
                cout << "Oraș sosire: ";
                cin >> arrival;
                cout << "Data (yyyy-mm-dd): ";
                cin >> date;
                cout << "Ora (hh:mm): ";
                cin >> time;
                cout << "Clasa trenului (1, 2): ";
                cin >> trainClass;

                cout << "Rezervare realizată pentru cursa " << departure << " -> " << arrival << endl;
            }
            else
            {
                cout << "Opțiune invalidă!" << endl;
            }
        }
        catch (const exception &e)
        {
            cerr << "Eroare: " << e.what() << endl;
        }
    }
    else if (role == "operator")
    {
        cout << "1. Adăugare cursă\n2. Ștergere cursă\n3. Listare curse\nAlege opțiunea: ";
        cin >> choice;

        try
        {
            if (choice == 1)
            {
                string departure, arrival, date, time;
                cout << "Oraș plecare: ";
                cin >> departure;
                cout << "Oraș sosire: ";
                cin >> arrival;
                cout << "Data (yyyy-mm-dd): ";
                cin >> date;
                cout << "Ora (hh:mm): ";
                cin >> time;

                Trip::addTrip(departure, arrival, date, time);
            }
            else if (choice == 2)
            {
                string departure, arrival, date, time;
                cout << "Oraș plecare: ";
                cin >> departure;
                cout << "Oraș sosire: ";
                cin >> arrival;
                cout << "Data (yyyy-mm-dd): ";
                cin >> date;
                cout << "Ora (hh:mm): ";
                cin >> time;

                Trip::deleteTrip(departure, arrival, date, time);
            }
            else if (choice == 3)
            {
                Trip::listTrips();
            }
            else
            {
                cout << "Opțiune invalidă!" << endl;
            }
        }
        catch (const exception &e)
        {
            cerr << "Eroare: " << e.what() << endl;
        }
    }
    else
    {
        cout << "Răspuns invalid!" << endl;
    }

    return 0;
}

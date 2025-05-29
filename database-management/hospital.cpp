#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

class InvalidCommandException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: invalid command";
    }
};

class DuplicateDoctorException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: doctor added twice";
    }
};

class InvalidWeekdayException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: invalid weekday";
    }
};

class NegativePatientLimitException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: number of patients can not be negative";
    }
};

class DoctorNotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: no doctor with this name exist";
    }
};

class NoDoctorForSpecialtyException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: no doctor for this problem exist";
    }
};

class DoctorBusyException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: doctors with this specialty are busy";
    }
};

class PatientNotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "Error: no patient with this name exist";
    }
};

const vector<string> validDays = {
    "saturday", "sunday", "monday", "tuesday", "wednesday"
};

bool isValidDay(const string& day) {
    return find(validDays.begin(), validDays.end(), day) != validDays.end();
}

struct Patient {
    string name;
    string doctor;
    string day;
};

struct Doctor {
    string name;
    string specialty;
    int patientLimit;
    set<string> workingDays;
    map<string, vector<string>> scheduledPatients; 

    void updatePatientLimit(int newLimit) {
        if (newLimit < 0) throw NegativePatientLimitException();
        patientLimit = newLimit;

        for (auto& [day, patients] : scheduledPatients) {
            while ((int)patients.size() > patientLimit) {
                patients.pop_back();
            }
        }
    }

    void updateWorkingDays(const set<string>& newDays) {
        for (const string& d : newDays) {
            if (!isValidDay(d)) throw InvalidWeekdayException();
        }

        workingDays = newDays;

        for (auto it = scheduledPatients.begin(); it != scheduledPatients.end(); ) {
            if (workingDays.count(it->first) == 0) {
                it = scheduledPatients.erase(it);
            } else {
                ++it;
            }
        }
    }

    bool isAvailable(string& assignedDay) {
        for (const string& d : validDays) {
            if (workingDays.count(d)) {
                if ((int)scheduledPatients[d].size() < patientLimit) {
                    assignedDay = d;
                    return true;
                }
            }
        }
        return false;
    }

    bool assignPatient(const string& pname, string& assignedDay) {
        if (isAvailable(assignedDay)) {
            scheduledPatients[assignedDay].push_back(pname);
            return true;
        }
        return false;
    }

    bool removePatient(const string& pname, string& removedDay) {
        for (auto& [day, patients] : scheduledPatients) {
            auto it = find(patients.begin(), patients.end(), pname);
            if (it != patients.end()) {
                patients.erase(it);
                removedDay = day;
                return true;
            }
        }
        return false;
    }
};

map<string, Doctor> doctors;
map<string, Patient> patients;

bool inputHandler(string line) {
    stringstream ss(line);
    string cmd;
    ss >> cmd;

    if (cmd == "exit") return false;

    if (cmd == "add") {
        string type;
        ss >> type;
        if (type == "doctor") {
            string name, specialty;
            int limit;
            ss >> name >> specialty >> limit;

            vector<string> days;
            string day;
            while (ss >> day) days.push_back(day);
            if (doctors.count(name)) throw DuplicateDoctorException();
            if (limit < 0) throw NegativePatientLimitException();
            for (const string& d : days) {
                if (!isValidDay(d)) throw InvalidWeekdayException();
            }

            Doctor doc;
            doc.name = name;
            doc.specialty = specialty;
            doc.patientLimit = limit;
            doc.workingDays = set<string>(days.begin(), days.end());
            doctors[name] = doc;

            cout << "doctor " << name << " added with specialty " << specialty << " with " << limit << " patients" << endl;
        }
        else if (type == "patient") {
            string pname, spec;
            ss >> pname >> spec;

            if (patients.count(pname)) return true; 

            for (auto& [docname, doc] : doctors) {
                if (doc.specialty == spec) {
                    string assignedDay;
                    if (doc.assignPatient(pname, assignedDay)) {
                        Patient p = { pname, docname, assignedDay };
                        patients[pname] = p;
                        cout << "appointment set on day " << assignedDay << " doctor " << docname << endl;
                        return true;
                    }
                }
            }

            bool specialtyExists = any_of(doctors.begin(), doctors.end(),
                [&](auto& d) { return d.second.specialty == spec; });

            if (specialtyExists) throw DoctorBusyException();
            else throw NoDoctorForSpecialtyException();
        } else {
            throw InvalidCommandException();
        }
    }

    else if (cmd == "change") {
        string temp;
        ss >> temp;
        if (temp == "the") {
            string what;
            ss >> what;

            if (what == "number") {
                string token;
                string doctorName;
                while (ss >> token && token != "doctor");
                while (ss >> token && token != "to") {
                    if (!doctorName.empty()) doctorName += " ";
                    doctorName += token;
                }

                int newLimit;
                ss >> newLimit;

                if (!doctors.count(doctorName)) {throw DoctorNotFoundException();}
                doctors[doctorName].updatePatientLimit(newLimit);
                cout << "number of patients changed" << endl;
            }

            else if (what == "working") {
                string token;
                string doctorName;

                while (ss >> token && token != "of");
                ss >> token;

                while (ss >> token && token != "to") {
                    if (!doctorName.empty()) doctorName += " ";
                    doctorName += token;
                }

                vector<string> newDays;
                string d;
                while (ss >> d) newDays.push_back(d);

                if (!doctors.count(doctorName)) {throw DoctorNotFoundException();}
                set<string> newDaySet(newDays.begin(), newDays.end());
                doctors[doctorName].updateWorkingDays(newDaySet);
                cout << "working days changed" << endl;
            }

        } else {
            throw InvalidCommandException();
        }
    }

    else if (cmd == "delete") {
        string entity;
        ss >> entity;
        if (entity == "patient") {
            string name;
            ss >> name;
            if (!patients.count(name)) throw PatientNotFoundException();
            Patient p = patients[name];
            string removedDay;
            doctors[p.doctor].removePatient(name, removedDay);
            patients.erase(name);
            cout << "appointment deleted on day " << removedDay << " doctor " << p.doctor << endl;
        } else {
            throw InvalidCommandException();
        }
    }

    else if (cmd == "patients") {
        string view;
        ss >> view;
        if (view != "list") throw InvalidCommandException();

        for (const string& day : validDays) {
            cout << day << ":" << endl;
            int count = 1;
            for (const auto& [pname, p] : patients) {
                if (p.day == day) {
                    cout << "patient" << count++ << " " << pname << " : doctor " << p.doctor << endl;
                }
            }
        }
    }

    else throw InvalidCommandException();

    return true;
}

int main() {
    string line;
    bool cond = true;
    while (cond) {
        getline(cin, line);
        try {
            cond = inputHandler(line);
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    return 0;
}


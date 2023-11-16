#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <windows.h>

using namespace std;

// Class to represent a user/patient
class Patient
{
public:
    string name;
    string password;
    int age;
    string healthReport;
    string lastExaminedDate;
    string lastCheckedDoctor;
    string examiningDate;
    string currentDoctor;
    string status; // 'booked' or 'checked'

    // Constructor
    Patient(string n, string password, int a, string hr, string last, string lastDoctor, string examDate, string currentDoc, string stat)
        : name(n), password(password), age(a), healthReport(hr), lastExaminedDate(last), lastCheckedDoctor(lastDoctor), examiningDate(examDate),
          currentDoctor(currentDoc), status(stat) {}
};

// Class to represent a doctor
class Doctor
{
public:
    string name;
    int charge;
    string speciality;
    vector<string> patientsToExamine;

    // Constructor
    Doctor(string n, int charge, string spec, const vector<string> &patients)
        : name(n), charge(charge), speciality(spec), patientsToExamine(patients) {}
};

class Admin
{
public:
    string name;
    string password;

    Admin(string n, string password) : name(n), password(password) {}
};

// Function to read patient data from file
vector<Patient> readPatientData();

// Function to read doctor data from file
vector<Doctor> readDoctorData();

// Function to read admin data from file
vector<Admin> readAdminData();

// Function to book an appointment
void bookAppointment(Patient &patient, vector<Doctor> &doctors, vector<Patient> &patients);

// Function to display user/patient dashboard
void patientDashboard(Patient &patient, vector<Doctor> &doctors, vector<Patient> &patients);

// Function to update patient details
void updatePatientDetails(vector<Patient> &patients, vector<Doctor> &doctors);

// Function to display doctor dashboard
void doctorDashboard(const Doctor &doctor, const vector<Patient> &patients);

// Function to display admin dashboard
void adminDashboard(vector<Doctor> &doctors, vector<Patient> &patients, vector<Admin> &admins);

// Function to display admin dashboard
void patientLogin(vector<Doctor> &doctors, vector<Patient> &patients);

// Function to display admin dashboard
void adminLogin(vector<Doctor> &doctors, vector<Patient> &patients, vector<Admin> &admins);

// Function to display admin dashboard
void doctorDetails(vector<Doctor> &doctors, vector<Patient> &patients);

int main()
{
    // Read data from files
    vector<Patient> patients = readPatientData();
    vector<Doctor> doctors = readDoctorData();
    vector<Admin> admins = readAdminData();

    char choice;
    do
    {
        system("cls");
        system("color 0A");
        cout << "\n\n      ************************************************\n";
        cout << "      ********** Hospital Management System **********\n";
        cout << "      ************************************************\n\n";
        cout << "                Press 'p' to login as patient\n";
        cout << "                Press 'a' to login as admin\n";
        cout << "                Press 'd' to login as doctor\n";
        cout << "                    Press 's' to exit\n\n";
        cout << "      ********************************************\n\n";
        cout << "      Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 'p':
            patientLogin(doctors, patients);
            break;

        case 'a':
            adminLogin(doctors, patients, admins);
            break;

        case 'd':
            doctorDetails(doctors, patients);
            break;

        case 's':
            // Exit the program
            break;

        default:
            cout << "           Invalid choice. Please try again.\n";
        }
    } while (choice != 's');

    return 0;
}

vector<Patient> readPatientData()
{
    vector<Patient> patients;
    ifstream file("user.txt");

    if (file.is_open())
    {
        string name, password, healthReport, examiningDate, last, lastDoctor, currentDoctor, status;
        int age;

        while (file >> name >> password >> age >> healthReport >> last >> lastDoctor >> examiningDate >> currentDoctor >> status)
        {
            // cout << name << " " << age << " " << healthReport << " " << last << " " << lastDoctor << " " << examiningDate << " " << currentDoctor << " " << status << "\n";
            patients.push_back(Patient(name, password, age, healthReport, last, lastDoctor, examiningDate, currentDoctor, status));
        }

        file.close();
    }

    return patients;
}

vector<Doctor> readDoctorData()
{
    vector<Doctor> doctors;
    ifstream file("doctor.txt");

    if (file.is_open())
    {
        string name, speciality;
        int charge;
        vector<string> patientsToExamine;

        while (file >> name >> charge >> speciality)
        {
            // Read patient names to be examined into a vector
            string patientName;
            while (file >> patientName && patientName != "-")
            {
                patientsToExamine.push_back(patientName);
            }

            doctors.push_back(Doctor(name, charge, speciality, patientsToExamine));
            patientsToExamine.clear(); // Clear for the next doctor
        }

        file.close();
    }

    return doctors;
}

vector<Admin> readAdminData()
{
    vector<Admin> admins;
    ifstream file("admin.txt");

    if (file.is_open())
    {
        string name, password;

        while (file >> name >> password)
        {
            admins.push_back(Admin(name, password));
        }

        file.close();
    }

    return admins;
}

void bookAppointment(Patient &patient, vector<Doctor> &doctors, vector<Patient> &patients)
{
    // Book a new examination
    cout << "\n       Enter doctor's name to book an appointment: ";
    string doctorName;
    cin >> doctorName;

    // Find the doctor and add patient to the examination list
    for (Doctor &doctor : doctors)
    {
        if (doctor.name == doctorName)
        {
            // checking if the doctor's patient list is full or not
            if (doctor.patientsToExamine.size() >= 5)
            {
                cout << "\n      Doctor's patient list is full, please try on another day\n";
                break;
            }

            // payment logic
            char paid = 'n';
            cout << "      The amount to be paid is : " << doctor.charge << "\n";
            cout << "      paid the amount (y/n) : ";
            cin >> paid;
            if (paid == 'n')
                break;

            // booking appointment
            patient.status = "booked";
            patient.currentDoctor = doctorName;

            // appending the new patient to the doctor object
            doctor.patientsToExamine.push_back(patient.name);

            // Save updated doctor data to the file
            ofstream doctorFile("doctor.txt");
            for (const Doctor &d : doctors)
            {
                doctorFile << d.name << " " << d.charge << " " << d.speciality;
                for (const string &patient : d.patientsToExamine)
                {
                    doctorFile << " " << patient;
                }
                doctorFile << " -\n";
            }

            // Set examination date for a new patient
            time_t now = time(0);
            tm *date = localtime(&now);
            patient.examiningDate = to_string(date->tm_mday) + "-" + to_string(date->tm_mon + 1) + "-" + to_string(date->tm_year + 1900);

            // updating the patient data in the patients vector as well
            for (Patient &p : patients)
            {
                if (p.name == patient.name)
                {
                    p.examiningDate = patient.examiningDate;
                    p.currentDoctor = patient.currentDoctor;
                    p.status = patient.status;
                    break;
                }
            }
            // Save updated patient data to the file
            ofstream patientFile("user.txt");
            for (const Patient &p : patients)
            {
                patientFile << p.name << " " << p.password << " " << p.age << " " << p.healthReport << " " << p.lastExaminedDate << " " << p.lastCheckedDoctor << " " << p.examiningDate
                            << " " << p.currentDoctor << " " << p.status << "\n";
            }
            patientFile.close();

            cout << "\n                  Appointment booked successfully!\n\n";
            break;
        }
    }
}

void patientDashboard(Patient &patient, vector<Doctor> &doctors, vector<Patient> &patients)
{
    system("cls");
    cout << "\n\n      ************************************************\n";
    cout << "      ********** Hospital Management System **********\n";
    cout << "      ************************************************\n\n";
    cout << "      =============== Patient Dashboard ==============\n\n";
    cout << "                       Welcome, " << patient.name << "!\n";
    cout << "       Age: " << patient.age << "\n";
    // convert the underscore back to spaces
    for (int i = 0; i < patient.healthReport.length(); i++)
    {
        if (patient.healthReport[i] == '_')
        {
            patient.healthReport[i] = ' ';
        }
    }
    cout << "       Health Report: " << patient.healthReport << "\n";
    cout << "       Last Examined Date: " << patient.lastExaminedDate << "\n";
    cout << "       Last Checked Doctor: " << patient.lastCheckedDoctor << "\n";
    if (patient.status == "booked")
        cout << "       Status: " << patient.status << "\n";

    if (patient.examiningDate == "-")
    {
        cout << "\n             No Appointments scheduled.\n";
        cout << "               1. Book an Appointment\n";
        cout << "               2. Exit\n\n";
        cout << "               Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            bookAppointment(patient, doctors, patients);
            break;

        case 2:
            // Exit the patient dashboard
            break;

        default:
            cout << "\n      Invalid choice. Please try again.\n";
        }
    }
    else
    {
        // Examination is scheduled
        cout << "      Appointment Date: " << patient.examiningDate << "\n";
        cout << "      Doctor Booked : " << patient.currentDoctor << "\n\n";
    }

    cout << "============================================================\n\n";
    system("pause");
}

void doctorDashboard(const Doctor &doctor, const vector<Patient> &patients)
{
    system("cls");
    cout << "\n\n      ************************************************\n";
    cout << "      ********** Hospital Management System **********\n";
    cout << "      ************************************************\n\n";
    cout << "      =============== Doctor Dashboard ===============\n\n";
    cout << "                    Welcome, Dr. " << doctor.name << "!\n\n";
    cout << "                  Speciality: " << doctor.speciality << "\n";
    cout << "                        Charge: " << doctor.charge << "\n";

    if (!doctor.patientsToExamine.empty())
        cout << "            Number of patients to Examine Today: " << doctor.patientsToExamine.size() << "\n";
    else
        cout << "              No patients to examine today.\n\n";

    cout << "============================================================\n\n\n";
    system("pause");
}

void updatePatientDetails(vector<Patient> &patients, vector<Doctor> &doctors)
{
    // update patient booking status
    string patientName;
    cout << "\n              Enter patient's name: ";
    cin >> patientName;

    Patient patient("", "", 0, "", "", "", "", "", "");
    int found = 0;
    for (Patient &p : patients)
    {
        if (p.name == patientName)
        {
            found = 1;
            patient = p;
            break;
        }
    }
    if (found == 0)
    {
        cout << "          Patient not found \n\n";
        system("pause");
        return;
    }

    // Update health report
    if (patient.status == "booked")
    {
        // Mark appointment as 'checked'
        patient.status = "checked";
    }

    cout << "\n           Update Health Report:\n";
    cout << "           Enter new health report: ";
    cin.ignore(); // Ignore newline from previous input
    getline(cin, patient.healthReport);

    // replace the spaces with underscore
    for (int i = 0; i < patient.healthReport.length(); i++)
    {
        if (patient.healthReport[i] == ' ')
        {
            patient.healthReport[i] = '_';
        }
    }

    // updating the patient data in the patients vector as well
    for (Patient &p : patients)
    {
        if (p.name == patient.name)
        {
            p.healthReport = patient.healthReport;
            p.lastExaminedDate = patient.examiningDate;
            p.lastCheckedDoctor = patient.currentDoctor;
            p.examiningDate = "-";
            p.currentDoctor = "-";
            p.status = patient.status;
            break;
        }
    }

    // Save updated patient data to the file
    ofstream patientFile("user.txt");
    for (const Patient &p : patients)
    {
        patientFile << p.name << " " << p.password << " " << p.age << " " << p.healthReport << " " << p.lastExaminedDate << " " << p.lastCheckedDoctor << " " << p.examiningDate
                    << " " << p.currentDoctor << " " << p.status << "\n";
    }
    patientFile.close();

    // also remove the patient from the doctor's list
    for (Doctor &doctor : doctors)
    {
        if (doctor.name == patient.currentDoctor)
        {
            for (int i = 0; i < doctor.patientsToExamine.size(); i++)
            {
                if (doctor.patientsToExamine[i] == patient.name)
                {
                    doctor.patientsToExamine.erase(doctor.patientsToExamine.begin() + i);
                    break;
                }
            }
            break;
        }
    }

    // Save updated doctor data to the file
    ofstream doctorFile("doctor.txt");
    for (const Doctor &d : doctors)
    {
        doctorFile << d.name << " " << d.charge << " " << d.speciality;
        for (const string &patient : d.patientsToExamine)
        {
            doctorFile << " " << patient;
        }
        doctorFile << " -\n";
    }

    cout << "           Health report updated successfully!\n\n";
    system("pause");
}

void adminDashboard(vector<Doctor> &doctors, vector<Patient> &patients, vector<Admin> &admins)
{
    // Simulate admin login
    system("cls");
    cout << "\n\n      ************************************************\n";
    cout << "      ********** Hospital Management System **********\n";
    cout << "      ************************************************\n\n";
    cout << "               Welcome to the admin dashboard \n";
    cout << "              1. update patient examining details\n";
    cout << "              2. add new doctors\n";
    cout << "              3. exit\n\n";

    int choice;
    cout << "             Enter your choice : ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        updatePatientDetails(patients, doctors);
        break;

    case 2:
    {
        // adding new doctor
        cout << "          Enter doctor details : \n";

        string name, speciality;
        int charge;
        vector<string> patientsToExamine;

        cout << "          Enter doctor's name: ";
        cin >> name;
        cout << "          Enter doctor's speciality: ";
        cin >> speciality;
        cout << "          Enter doctor's charge: ";
        cin >> charge;

        doctors.push_back(Doctor(name, charge, speciality, patientsToExamine));

        // Save the new doctor data to the file
        ofstream file("doctor.txt");
        for (const Doctor &d : doctors)
        {
            file << d.name << " " << d.charge << " " << d.speciality;
            for (const string &patient : d.patientsToExamine)
            {
                file << " " << patient;
            }
            file << " -\n";
        }
        file.close();

        cout << "        Doctor details created successfully!\n";
        break;
    }
    case 3:
        // Exit the program
        break;

    default:
        cout << "          Invalid choice. Please try again.\n";
    }
}

void patientLogin(vector<Doctor> &doctors, vector<Patient> &patients)
{
    // Simulate user login (patient)
    string patientName;
    cout << "\n      Enter patient's name: ";
    cin >> patientName;
    cout << "      Enter patient's password: ";
    string patientPassword;
    cin >> patientPassword;
    Patient currentPatient("", "", 0, "", "", "", "", "", "");

    int found = 0;
    for (Patient &patient : patients)
    {
        if (patient.name == patientName && patient.password == patientPassword)
        {
            found = 1;
            currentPatient = patient;
            break;
        }
    }

    if (currentPatient.name.empty())
    {
        char ch = 'n';
        cout << "\n      Patient record not found , want to signUp (y/n) : ";
        cin >> ch;
        if (ch == 'n')
            return;

        // New patient, create a new entry
        currentPatient.name = patientName;
        currentPatient.healthReport = "-";
        currentPatient.lastExaminedDate = "-";
        currentPatient.lastCheckedDoctor = "-";
        currentPatient.examiningDate = "-";
        currentPatient.currentDoctor = "-";
        currentPatient.status = "-";
        int age;

        cout << "\n      Enter age: ";
        cin >> age;
        currentPatient.age = age;

        cout << "      create a password : ";
        cin >> currentPatient.password;

        patients.push_back(currentPatient);

        // Save the new patient data to the file
        ofstream file("user.txt");
        for (const Patient &p : patients)
        {
            file << p.name << " " << p.password << " " << p.age << " " << p.healthReport << " " << p.lastExaminedDate << " " << p.lastCheckedDoctor << " " << p.examiningDate
                 << " " << p.currentDoctor << " " << p.status << "\n";
        }
        file.close();

        cout << "\n      Patient details created successfully!\n\n";
        system("pause");
    }

    // Display patient dashboard
    patientDashboard(currentPatient, doctors, patients);
}

void adminLogin(vector<Doctor> &doctors, vector<Patient> &patients, vector<Admin> &admins)
{
    cout << "\n      Enter admin name : ";
    string adminName;
    cin >> adminName;
    cout << "      Enter admin password : ";
    string adminPassword;
    cin >> adminPassword;

    int found = 0;
    for (Admin &admin : admins)
    {
        if (admin.name == adminName && admin.password == adminPassword)
        {
            found = 1;
            break;
        }
    }
    if (found == 0)
    {
        cout << "       Invalid admin credentials.\n";
        return;
    }

    adminDashboard(doctors, patients, admins);
}

void doctorDetails(vector<Doctor> &doctors, vector<Patient> &patients)
{
    // Simulate doctor login
    string doctorName;
    cout << "                 Enter doctor's name: ";
    cin >> doctorName;

    Doctor currentDoctor("", 0, "", {});
    for (const Doctor &doctor : doctors)
    {
        if (doctor.name == doctorName)
        {
            currentDoctor = doctor;
            break;
        }
    }

    if (currentDoctor.name.empty())
    {
        cout << "           Invalid doctor credentials.\n";
        return;
    }

    // Display doctor dashboard
    doctorDashboard(currentDoctor, patients);
}
# 🏥 HMS Nexus Pro

> A production-ready **Hospital Management System** built with **C++17 & Qt6** — designed for clinics, hospitals, and diagnostic laboratories in Pakistan and beyond.

![Version](https://img.shields.io/badge/version-2.0-06b6d4?style=flat-square)
![Language](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=cplusplus)
![Framework](https://img.shields.io/badge/Qt-6-41CD52?style=flat-square&logo=qt)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)

---

## 📸 Overview

HMS Nexus Pro is a **fully offline, file-based hospital management desktop application** built entirely in C++ with the Qt6 framework. It was designed to be lightweight, fast, and deployable on any machine without needing a database server — making it ideal for small-to-medium clinics and diagnostic labs in regions where internet or server infrastructure is limited.

The system covers the full patient journey: **registration → appointment → treatment → lab test → billing → discharge.**

---

## ✨ Features at a Glance

| Module | Capabilities |
|---|---|
| 👥 **Patients** | Add, Edit, Delete, Search — auto-generated IDs, blood group, balance tracking |
| 👨‍⚕️ **Doctors** | Manage doctors with specialty, experience, available days, consultation fee |
| 📅 **Appointments** | Schedule, mark Completed / No-Show / Cancelled, colour-coded status |
| 💊 **Treatments & Billing** | Add treatments per patient, mark as Paid, print professional bills |
| 🧪 **Lab Tests** | Order from 20+ common tests, enter results, track pending tests |
| 📦 **Inventory** | Track medicines and supplies, red-flag low stock (< 20 units) |
| 📊 **Analytics** | Revenue bar chart, age distribution, gender pie chart |
| 📄 **Reports** | Printable HTML summary report with all key stats |
| 📤 **CSV Export** | Export patients, doctors, and treatments to spreadsheet |
| 💾 **Backup & Restore** | One-click timestamped backup to any folder |
| 🔐 **Role-based Access** | Admin (full access) vs Receptionist (limited access) |
| 🔍 **Live Search** | Filters any table in real-time as you type |

---

## 🛠️ Tech Stack

- **Language:** C++17
- **GUI Framework:** Qt6 (Widgets, Charts, PrintSupport)
- **Build System:** CMake 3.16+
- **Storage:** Plain text files (no database required)
- **OS Support:** Linux, Windows, macOS

---

## 📁 Project Structure

```
hms-nexus-pro/
├── main.cpp              ← Entire application (single-file architecture)
├── CMakeLists.txt        ← Build configuration
├── hmsnexuspro.svg       ← App icon
├── build_package.sh      ← Linux .deb packaging script
└── README.md
```

> Data files are auto-generated at first run:
> `patients.txt`, `doctors.txt`, `appointments.txt`, `treatments.txt`,
> `labtests.txt`, `inventory.txt`, `users.txt`

---

## 🚀 Getting Started

### Prerequisites

Make sure you have the following installed:

```bash
# Ubuntu / Debian
sudo apt install cmake g++ qt6-base-dev qt6-charts-dev libqt6printsupport6

# Fedora
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtcharts-devel

# Windows
# Install Qt6 via https://www.qt.io/download and CMake via https://cmake.org
```

### Build & Run

```bash
# Clone the repo
git clone https://github.com/farhanalikalyani/HMS-App.git
cd HMS-App

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./HMSNexusPro
```

### Default Login Credentials

| Role | Username | Password |
|---|---|---|
| Admin | `admin` | `admin123` |
| Receptionist | `reception` | `rec123` |

> ⚠️ Change these credentials in `users.txt` before deploying commercially.

---

## 🗺️ Full Learning Roadmap

> **Want to study, extend, or rebuild this project yourself?** Follow this step-by-step roadmap. Each phase builds on the previous one.

---

### Phase 1 — C++ Foundations (Week 1–2)

Before touching Qt, make sure you're solid on these C++ concepts used heavily in this project:

- **Structs** — `PatientData`, `DoctorData`, `AppointmentData`, etc. are all plain C++ structs
- **Vectors** — `std::vector<PatientData> patients` — adding, removing, iterating
- **String streams** — `stringstream ss(line); getline(ss, field, '#')` — used in all file I/O
- **File I/O** — `ifstream`, `ofstream`, reading/writing line by line
- **Lambdas** — `[this](){}` — used in every Qt signal connection
- **STL algorithms** — `std::find_if`, `std::count_if` — used in search and stats
- **Range-based for** — `for (auto& p : patients)`

**Practice task:** Write a console program that stores 5 students in a vector of structs, saves them to a `.txt` file with `#` as delimiter, and reads them back.

---

### Phase 2 — Qt6 Basics (Week 2–3)

Learn the Qt building blocks used in this project:

**Widgets you must understand:**
- `QMainWindow` — the outer shell
- `QWidget` — every panel and page is a QWidget
- `QStackedWidget` — how the sidebar navigation works (pages switch without reopening windows)
- `QTableWidget` — patients, doctors, appointments are all displayed here
- `QDialog` — every "Add Patient", "Add Doctor" popup is a QDialog
- `QFormLayout`, `QVBoxLayout`, `QHBoxLayout` — layout system
- `QPushButton`, `QLineEdit`, `QComboBox`, `QSpinBox`, `QDateEdit` — input widgets
- `QLabel` — text display

**Core Qt concepts:**
- **Signals & Slots** — `connect(button, &QPushButton::clicked, this, &MainWindow::addPatient)`
- **QSS (Qt Style Sheets)** — how the dark theme is applied, very similar to CSS
- **Parent-child ownership** — Qt deletes children automatically

**Practice task:** Build a simple window with a sidebar of 3 buttons that switches between 3 pages using `QStackedWidget`.

---

### Phase 3 — Understanding This Codebase (Week 3–4)

Read `main.cpp` in this order:

**Step 1 — Data layer (top of file)**
```
PatientData, DoctorData, AppointmentData, TreatmentData, LabTestData, InventoryData
```
Understand what each struct stores and why.

**Step 2 — File I/O functions**
```
loadPatients() / savePatients()
loadDoctors()  / saveDoctors()
... and so on for each module
```
These are plain read/write functions. Study the `#` delimiter pattern.

**Step 3 — Helper functions**
```
styledBtn()     ← creates a styled QPushButton
styledTable()   ← creates a styled QTableWidget
styleInput()    ← applies dark theme to any input widget
statCard()      ← creates a dashboard stat card
```

**Step 4 — MainWindow class**

This is the main application. Read it in this order:
1. `refreshPatientTable()` — how data loads into the table
2. `addPatient()` — the full add dialog flow
3. `editPatient()` — how selected row maps back to vector
4. `printBill()` — HTML bill generation
5. `showRevenueChart()` — Qt Charts usage
6. `MainWindow()` constructor — how the entire UI is assembled

**Step 5 — main() at the bottom**
```
loadUsers() → LoginDialog → MainWindow
```

---

### Phase 4 — Extend the Project (Week 4–6)

Once you understand the codebase, try these extensions in order of difficulty:

#### 🟢 Beginner Extensions
- Add a **Notes** field to Patient records
- Add a **Discharge Date** to treatments
- Add more blood groups or time slots to dropdowns
- Add a **print** button to the patient list

#### 🟡 Intermediate Extensions
- **Patient history view** — click a patient → see all their appointments + treatments + lab tests in one dialog
- **Doctor schedule view** — show all appointments for a selected doctor by date
- **Invoice with multiple treatments** — combine multiple treatments into one printable bill
- **Search by date range** — filter appointments between two dates
- **Password change dialog** — let users change their own password

#### 🔴 Advanced Extensions
- **SQLite database** — replace `.txt` files with Qt's `QSqlDatabase` + SQLite
- **PDF export** — use Qt's `QPdfWriter` to export bills and reports as PDF files
- **Email bill** — integrate SMTP to email bills directly to patients
- **Multi-branch support** — add a "Branch" field to all records and filter by branch
- **Dark/Light theme toggle** — switch between dark navy and a clean white theme at runtime
- **REST API backend** — replace file I/O with HTTP calls to a Node.js or Python backend

---

### Phase 5 — Packaging & Deployment

#### Linux (.deb package)
```bash
chmod +x build_package.sh
./build_package.sh
```

#### Windows
1. Build with Qt6 on Windows using CMake
2. Run `windeployqt HMSNexusPro.exe` to bundle Qt DLLs
3. Use NSIS or Inno Setup to create an installer

#### macOS
```bash
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6
cmake --build .
macdeployqt HMSNexusPro.app
```

---

## 🔐 Role Permissions Reference

| Action | Admin | Receptionist |
|---|---|---|
| Add / Edit Patient | ✅ | ✅ |
| Delete Patient | ✅ | ❌ |
| Add / Edit Doctor | ✅ | ❌ |
| Delete Doctor | ✅ | ❌ |
| Schedule Appointment | ✅ | ✅ |
| Add Treatment | ✅ | ✅ |
| Mark Payment | ✅ | ✅ |
| Order Lab Test | ✅ | ✅ |
| Manage Inventory | ✅ | ❌ |
| Backup & Restore | ✅ | Backup only |
| View Reports & Charts | ✅ | ✅ |

---

## 📊 Data File Format Reference

All data is stored in plain text files using `#` as the field delimiter.

**patients.txt**
```
ID#Name#Age#Gender#Phone#Balance#BloodGroup#Address
1001#Ahmed Raza#28#Male#03001234567#15000#A+#House 5 Gulberg Lahore
```

**doctors.txt**
```
ID#Name#Specialty#Experience#Days#Phone#ConsultFee
2001#Dr. Kamran Malik#Cardiologist#12#Mon,Wed,Fri#03001111111#2000
```

**appointments.txt**
```
ID#PatientID#DoctorID#PatientName#DoctorName#Date#Time#Status#Notes
1#1001#2001#Ahmed Raza#Dr. Kamran Malik#2025-05-15#10:00 AM#Scheduled#Follow-up
```

**treatments.txt**
```
ID#PatientID#PatientName#Description#Cost#Date#Paid(0/1)#Category
1#1001#Ahmed Raza#ECG Test#5000#2025-05-15#1#Cardiology
```

---

## 🤝 Contributing

Contributions are welcome! If you're a student practising Qt or C++, this is a great project to fork and extend.

1. Fork the repo
2. Create a feature branch: `git checkout -b feature/patient-history`
3. Commit your changes: `git commit -m "Add patient history view"`
4. Push: `git push origin feature/patient-history`
5. Open a Pull Request

---

## 👤 Author

**Farhan Ali**
- GitHub: [@farhanalikalyani](https://github.com/farhanalikalyani)
- Project: HMS Nexus Pro — built as a commercial-grade portfolio project

---

## 📄 License

This project is licensed under the MIT License — you are free to use, modify, and distribute it with attribution.

---

> Built with ❤️ using C++17 and Qt6 — no database, no server, no internet required.

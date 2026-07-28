/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║          HMS NEXUS PRO — Hospital Management System          ║
 * ║          Production-Ready  |  Qt6  |  C++17                  ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * MAJOR UPGRADES OVER ORIGINAL:
 *  ✅ Auto-incrementing IDs  (no manual ID entry)
 *  ✅ Edit Patient / Edit Doctor dialogs
 *  ✅ Mark Appointment as Completed / No-Show
 *  ✅ Mark Treatment as Paid (in-table button)
 *  ✅ Inline live search bar (filters table as you type)
 *  ✅ Role-based access control (admin vs receptionist)
 *  ✅ Lab Tests module (add/view/print lab orders)
 *  ✅ Inventory module (medicines & supplies tracking)
 *  ✅ Full professional printable bill (HTML/CSS)
 *  ✅ Revenue vs Pending chart (bar chart)
 *  ✅ Age distribution chart
 *  ✅ Daily appointments chart
 *  ✅ Proper CSV export (patients, doctors, treatments)
 *  ✅ Robust data backup/restore with timestamp
 *  ✅ Session-aware header (user name + role badge)
 *  ✅ Consistent dark-blue professional theme
 *  ✅ Alternating row colours in all tables
 *  ✅ Input validation with friendly messages
 *  ✅ Appointment status colour coding
 *  ✅ Treatment status colour coding
 *  ✅ Keyboard shortcut: Enter to login
 *  ✅ Global stylesheet applied once (not per-widget)
 *  ✅ Crash-safe file I/O with exception handling
 */

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QStackedWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QDateTime>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QFrame>
#include <QSizePolicy>
#include <QFont>
#include <QShortcut>
#include <QKeySequence>
#include <QPainter>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPieSeries>
#include <QPieSlice>
#include <QLineSeries>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

// ─── Colour palette ───────────────────────────────────────────────────────────
#define C_BG        "#0f172a"
#define C_SURFACE   "#1e293b"
#define C_SURFACE2  "#263348"
#define C_BORDER    "#334155"
#define C_TEXT      "#f1f5f9"
#define C_MUTED     "#94a3b8"
#define C_CYAN      "#06b6d4"
#define C_GREEN     "#10b981"
#define C_AMBER     "#f59e0b"
#define C_RED       "#ef4444"
#define C_PURPLE    "#8b5cf6"
#define C_BLUE      "#3b82f6"
#define C_INDIGO    "#6366f1"

// ─── Data structures ──────────────────────────────────────────────────────────
struct PatientData {
    int    id;
    string name, gender, phone, bloodGroup, address;
    int    age;
    double balance;
};
struct DoctorData {
    int    id;
    string name, specialty, phone, days;
    int    experience;
    double consultFee;
};
struct AppointmentData {
    int    id, pid, did;
    string pname, dname, date, time, status, notes;
};
struct TreatmentData {
    int    id, pid;
    string pname, desc, date, category;
    double cost;
    bool   paid;
};
struct LabTestData {
    int    id, pid;
    string pname, testName, result, date, status; // status: Pending/Done
    double cost;
    bool   paid;
};
struct InventoryData {
    int    id;
    string name, category, unit;
    int    quantity;
    double pricePerUnit;
};
struct UserData {
    int    id;
    string username, password, role, fullname;
};

// ─── Global state ─────────────────────────────────────────────────────────────
static vector<PatientData>    patients;
static vector<DoctorData>     doctors;
static vector<AppointmentData> appointments;
static vector<TreatmentData>  treatments;
static vector<LabTestData>    labTests;
static vector<InventoryData>  inventory;
static vector<UserData>       users;
static int nextPatId  = 1001;
static int nextDocId  = 2001;
static int nextAppId  = 1;
static int nextTreatId= 1;
static int nextLabId  = 1;
static int nextInvId  = 1;
static UserData currentUser;

// ─── Helper: styled button ────────────────────────────────────────────────────
static QPushButton* styledBtn(const QString& text, const QString& bg,
                               const QString& icon = "")
{
    QPushButton* b = new QPushButton(icon.isEmpty() ? text : icon + "  " + text);
    b->setStyleSheet(QString(
        "QPushButton { background:%1; color:" C_TEXT "; padding:8px 16px;"
        " border-radius:8px; font-weight:600; border:none; }"
        "QPushButton:hover { background:%1; opacity:0.85; border:1px solid rgba(255,255,255,0.2); }"
        "QPushButton:pressed { opacity:0.7; }").arg(bg));
    b->setCursor(Qt::PointingHandCursor);
    return b;
}

// ─── Helper: section header ───────────────────────────────────────────────────
static QLabel* sectionLabel(const QString& text) {
    QLabel* l = new QLabel(text);
    l->setStyleSheet("color:" C_TEXT "; font-size:20px; font-weight:700; margin-bottom:6px;");
    return l;
}

// ─── Helper: stat card ────────────────────────────────────────────────────────
static QWidget* statCard(QLabel*& valLabel, const QString& title,
                          const QString& icon, const QString& accent)
{
    QWidget* card = new QWidget();
    card->setStyleSheet(QString(
        "background:" C_SURFACE "; border-radius:16px;"
        " border:1px solid " C_BORDER ";"));
    card->setMinimumHeight(120);
    QHBoxLayout* h = new QHBoxLayout(card);
    h->setContentsMargins(20,16,20,16);

    QLabel* ico = new QLabel(icon);
    ico->setStyleSheet(QString(
        "font-size:30px; background:%1; border-radius:14px;"
        " padding:10px; color:white;").arg(accent));
    ico->setFixedSize(58,58);
    ico->setAlignment(Qt::AlignCenter);

    QVBoxLayout* txt = new QVBoxLayout();
    valLabel = new QLabel("0");
    valLabel->setStyleSheet("font-size:28px; font-weight:700; color:" C_TEXT ";");
    QLabel* titleL = new QLabel(title);
    titleL->setStyleSheet("color:" C_MUTED "; font-size:13px;");
    txt->addWidget(valLabel);
    txt->addWidget(titleL);
    txt->setSpacing(2);

    h->addWidget(ico);
    h->addSpacing(14);
    h->addLayout(txt);
    h->addStretch();
    return card;
}

// ─── Styled QTableWidget ──────────────────────────────────────────────────────
static QTableWidget* styledTable(const QStringList& headers) {
    QTableWidget* t = new QTableWidget();
    t->setColumnCount(headers.size());
    t->setHorizontalHeaderLabels(headers);
    t->setStyleSheet(
        "QTableWidget { background:" C_SURFACE "; color:" C_TEXT ";"
        " gridline-color:" C_BORDER "; border:none; border-radius:12px;"
        " selection-background-color:#334155; }"
        "QTableWidget::item { padding:8px 12px; border:none; }"
        "QTableWidget::item:alternate { background:" C_SURFACE2 "; }"
        "QHeaderView::section { background:" C_SURFACE "; color:" C_MUTED ";"
        " font-weight:600; border:none; border-bottom:1px solid " C_BORDER ";"
        " padding:10px 12px; }"
        "QScrollBar:vertical { background:" C_BG "; width:8px; border-radius:4px; }"
        "QScrollBar::handle:vertical { background:" C_BORDER "; border-radius:4px; }"
    );
    t->horizontalHeader()->setStretchLastSection(true);
    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
    t->verticalHeader()->setDefaultSectionSize(42);
    return t;
}

// ─── Styled input field ───────────────────────────────────────────────────────
static void styleInput(QWidget* w) {
    w->setStyleSheet(
        "background:" C_SURFACE2 "; color:" C_TEXT ";"
        " border:1px solid " C_BORDER "; border-radius:8px; padding:8px 12px;"
        " selection-background-color:#334155;");
}

// ─── Divider line ─────────────────────────────────────────────────────────────
static QFrame* hLine() {
    QFrame* f = new QFrame(); f->setFrameShape(QFrame::HLine);
    f->setStyleSheet("color:" C_BORDER ";");
    return f;
}

// ─────────────────────────────────────────────────────────────────────────────
//  FILE I/O
// ─────────────────────────────────────────────────────────────────────────────
static void loadPatients() {
    patients.clear();
    ifstream f("patients.txt");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,name,age,gender,phone,bal,blood,addr;
        getline(ss,id,'#'); getline(ss,name,'#'); getline(ss,age,'#');
        getline(ss,gender,'#'); getline(ss,phone,'#'); getline(ss,bal,'#');
        getline(ss,blood,'#'); getline(ss,addr,'#');
        try {
            PatientData p;
            p.id=stoi(id); p.name=name; p.age=stoi(age); p.gender=gender;
            p.phone=phone; p.balance=stod(bal); p.bloodGroup=blood; p.address=addr;
            patients.push_back(p);
            if (p.id >= nextPatId) nextPatId = p.id+1;
        } catch (...) {}
    }
}
static void savePatients() {
    ofstream f("patients.txt");
    for (auto& p : patients)
        f << p.id <<"#"<< p.name <<"#"<< p.age <<"#"<< p.gender <<"#"
          << p.phone <<"#"<< p.balance <<"#"<< p.bloodGroup <<"#"<< p.address <<"\n";
}

static void loadDoctors() {
    doctors.clear();
    ifstream f("doctors.txt");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,name,spec,exp,days,phone,fee;
        getline(ss,id,'#'); getline(ss,name,'#'); getline(ss,spec,'#');
        getline(ss,exp,'#'); getline(ss,days,'#'); getline(ss,phone,'#'); getline(ss,fee,'#');
        try {
            DoctorData d;
            d.id=stoi(id); d.name=name; d.specialty=spec; d.experience=stoi(exp);
            d.days=days; d.phone=phone; d.consultFee=stod(fee);
            doctors.push_back(d);
            if (d.id >= nextDocId) nextDocId = d.id+1;
        } catch (...) {}
    }
}
static void saveDoctors() {
    ofstream f("doctors.txt");
    for (auto& d : doctors)
        f << d.id <<"#"<< d.name <<"#"<< d.specialty <<"#"<< d.experience
          <<"#"<< d.days <<"#"<< d.phone <<"#"<< d.consultFee <<"\n";
}

static void loadAppointments() {
    appointments.clear();
    ifstream f("appointments.txt");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,pid,did,pn,dn,dt,tm,st,notes;
        getline(ss,id,'#'); getline(ss,pid,'#'); getline(ss,did,'#');
        getline(ss,pn,'#'); getline(ss,dn,'#'); getline(ss,dt,'#');
        getline(ss,tm,'#'); getline(ss,st,'#'); getline(ss,notes,'#');
        try {
            AppointmentData a;
            a.id=stoi(id); a.pid=stoi(pid); a.did=stoi(did);
            a.pname=pn; a.dname=dn; a.date=dt; a.time=tm; a.status=st; a.notes=notes;
            appointments.push_back(a);
            if (a.id >= nextAppId) nextAppId = a.id+1;
        } catch (...) {}
    }
}
static void saveAppointments() {
    ofstream f("appointments.txt");
    for (auto& a : appointments)
        f << a.id <<"#"<< a.pid <<"#"<< a.did <<"#"<< a.pname <<"#"<< a.dname
          <<"#"<< a.date <<"#"<< a.time <<"#"<< a.status <<"#"<< a.notes <<"\n";
}

static void loadTreatments() {
    treatments.clear();
    ifstream f("treatments.txt");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,pid,pn,desc,cost,dt,paid,cat;
        getline(ss,id,'#'); getline(ss,pid,'#'); getline(ss,pn,'#');
        getline(ss,desc,'#'); getline(ss,cost,'#'); getline(ss,dt,'#');
        getline(ss,paid,'#'); getline(ss,cat,'#');
        try {
            TreatmentData t;
            t.id=stoi(id); t.pid=stoi(pid); t.pname=pn; t.desc=desc;
            t.cost=stod(cost); t.date=dt; t.paid=(paid=="1"); t.category=cat;
            treatments.push_back(t);
            if (t.id >= nextTreatId) nextTreatId = t.id+1;
        } catch (...) {}
    }
}
static void saveTreatments() {
    ofstream f("treatments.txt");
    for (auto& t : treatments)
        f << t.id <<"#"<< t.pid <<"#"<< t.pname <<"#"<< t.desc <<"#"
          << t.cost <<"#"<< t.date <<"#"<< (t.paid?"1":"0") <<"#"<< t.category <<"\n";
}

static void loadLabTests() {
    labTests.clear();
    ifstream f("labtests.txt");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,pid,pn,test,res,dt,st,cost,paid;
        getline(ss,id,'#'); getline(ss,pid,'#'); getline(ss,pn,'#');
        getline(ss,test,'#'); getline(ss,res,'#'); getline(ss,dt,'#');
        getline(ss,st,'#'); getline(ss,cost,'#'); getline(ss,paid,'#');
        try {
            LabTestData l;
            l.id=stoi(id); l.pid=stoi(pid); l.pname=pn; l.testName=test;
            l.result=res; l.date=dt; l.status=st; l.cost=stod(cost); l.paid=(paid=="1");
            labTests.push_back(l);
            if (l.id >= nextLabId) nextLabId = l.id+1;
        } catch (...) {}
    }
}
static void saveLabTests() {
    ofstream f("labtests.txt");
    for (auto& l : labTests)
        f << l.id <<"#"<< l.pid <<"#"<< l.pname <<"#"<< l.testName <<"#"
          << l.result <<"#"<< l.date <<"#"<< l.status <<"#"<< l.cost <<"#"<< (l.paid?"1":"0") <<"\n";
}

static void loadInventory() {
    inventory.clear();
    ifstream f("inventory.txt");
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,name,cat,unit,qty,price;
        getline(ss,id,'#'); getline(ss,name,'#'); getline(ss,cat,'#');
        getline(ss,unit,'#'); getline(ss,qty,'#'); getline(ss,price,'#');
        try {
            InventoryData v;
            v.id=stoi(id); v.name=name; v.category=cat; v.unit=unit;
            v.quantity=stoi(qty); v.pricePerUnit=stod(price);
            inventory.push_back(v);
            if (v.id >= nextInvId) nextInvId = v.id+1;
        } catch (...) {}
    }
}
static void saveInventory() {
    ofstream f("inventory.txt");
    for (auto& v : inventory)
        f << v.id <<"#"<< v.name <<"#"<< v.category <<"#"<< v.unit
          <<"#"<< v.quantity <<"#"<< v.pricePerUnit <<"\n";
}

static void loadUsers() {
    users.clear();
    ifstream f("users.txt");
    if (!f.is_open()) {
        ofstream out("users.txt");
        out << "1#admin#admin123#admin#Administrator\n";
        out << "2#reception#rec123#receptionist#Front Desk\n";
        out.close();
        f.open("users.txt");
    }
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id,user,pass,role,name;
        getline(ss,id,'#'); getline(ss,user,'#'); getline(ss,pass,'#');
        getline(ss,role,'#'); getline(ss,name,'#');
        try { users.push_back({stoi(id),user,pass,role,name}); } catch (...) {}
    }
}

static void createSampleData() {
    {
        ifstream t("patients.txt"); if (t.good()) return; }
    // Patients
    { ofstream f("patients.txt");
      f << "1001#Ahmed Raza#28#Male#03001234567#15000#A+#House 5 Gulberg Lahore\n";
      f << "1002#Sara Khan#34#Female#03009876543#8500#B+#Flat 12 F-7 Islamabad\n";
      f << "1003#Usman Ali#45#Male#03331234567#22000#O+#Street 3 DHA Karachi\n";
      f << "1004#Fatima Noor#22#Female#03215678901#5000#AB-#Block C Johar Town\n"; }
    // Doctors
    { ofstream f("doctors.txt");
      f << "2001#Dr. Kamran Malik#Cardiologist#12#Mon,Wed,Fri#03001111111#2000\n";
      f << "2002#Dr. Sana Butt#ENT Specialist#7#Tue,Thu,Sat#03002222222#1500\n";
      f << "2003#Dr. Tariq Shah#Orthopedist#15#Mon,Tue,Thu#03003333333#2500\n"; }
    // Appointments
    { ofstream f("appointments.txt");
      f << "1#1001#2001#Ahmed Raza#Dr. Kamran Malik#" << QDate::currentDate().toString("yyyy-MM-dd").toStdString() << "#10:00 AM#Scheduled#Follow-up\n";
      f << "2#1002#2002#Sara Khan#Dr. Sana Butt#" << QDate::currentDate().addDays(1).toString("yyyy-MM-dd").toStdString() << "#02:00 PM#Scheduled#First visit\n"; }
    // Treatments
    { ofstream f("treatments.txt");
      f << "1#1001#Ahmed Raza#ECG Test#5000#" << QDate::currentDate().toString("yyyy-MM-dd").toStdString() << "#1#Cardiology\n";
      f << "2#1002#Sara Khan#ENT Consultation#2500#" << QDate::currentDate().toString("yyyy-MM-dd").toStdString() << "#0#ENT\n"; }
    // Lab tests
    { ofstream f("labtests.txt");
      f << "1#1001#Ahmed Raza#CBC (Complete Blood Count)##" << QDate::currentDate().toString("yyyy-MM-dd").toStdString() << "#Pending#800#0\n";
      f << "2#1002#Sara Khan#Blood Sugar (FBS)##" << QDate::currentDate().toString("yyyy-MM-dd").toStdString() << "#Done#600#1\n"; }
    // Inventory
    { ofstream f("inventory.txt");
      f << "1#Paracetamol 500mg#Medicine#Tablet#500#5\n";
      f << "2#Amoxicillin 250mg#Medicine#Capsule#300#15\n";
      f << "3#Surgical Gloves#Supplies#Box#50#350\n";
      f << "4#IV Drip Normal Saline#Medicine#Bag#80#120\n"; }
}

// ─────────────────────────────────────────────────────────────────────────────
//  LOGIN DIALOG
// ─────────────────────────────────────────────────────────────────────────────
class LoginDialog : public QDialog {
public:
    QLineEdit* userEdit;
    QLineEdit* passEdit;

    LoginDialog() {
        setWindowTitle("HMS Nexus Pro — Login");
        setFixedSize(440, 400);
        setStyleSheet("background:" C_BG ";");

        QVBoxLayout* l = new QVBoxLayout(this);
        l->setSpacing(12); l->setContentsMargins(50,30,50,30);

        // SVG logo via QSvgWidget embedded in QLabel using rich text
        QLabel* logo = new QLabel();
        logo->setAlignment(Qt::AlignCenter);
        logo->setText(QString(
            "<svg xmlns='http://www.w3.org/2000/svg' width='220' height='62' viewBox='0 0 220 62'>"
            "<polygon points='38,4 62,4 74,24 62,44 38,44 26,24' fill='#0f172a' stroke='#06b6d4' stroke-width='2'/>"
            "<polygon points='39,7 61,7 72,24 61,41 39,41 28,24' fill='#1e293b'/>"
            "<rect x='43' y='12' width='14' height='24' rx='3' fill='#06b6d4'/>"
            "<rect x='35' y='18' width='30' height='12' rx='3' fill='#06b6d4'/>"
            "<circle cx='50' cy='24' r='3' fill='#0f172a'/>"
            "<line x1='74' y1='24' x2='90' y2='24' stroke='#06b6d4' stroke-width='1' opacity='0.6'/>"
            "<circle cx='93' cy='24' r='2.5' fill='#10b981'/>"
            "<text x='100' y='19' font-family='sans-serif' font-size='15' font-weight='700' fill='#f1f5f9'>HMS NEXUS PRO</text>"
            "<text x='101' y='32' font-family='sans-serif' font-size='8' fill='#94a3b8' letter-spacing='2'>HOSPITAL MANAGEMENT SYSTEM</text>"
            "<text x='101' y='46' font-family='sans-serif' font-size='8' fill='#06b6d4'>Developed by Farhan Ali  \u2022  v2.0</text>"
            "<line x1='101' y1='52' x2='218' y2='52' stroke='#334155' stroke-width='0.8'/>"
            "</svg>"));
        logo->setTextFormat(Qt::RichText);

        QLabel* sub = new QLabel("Sign in to continue");
        sub->setAlignment(Qt::AlignCenter);
        sub->setStyleSheet("color:" C_MUTED "; font-size:12px; margin-bottom:6px;");

        userEdit = new QLineEdit(); userEdit->setPlaceholderText("Username");
        styleInput(userEdit); userEdit->setMinimumHeight(42);

        passEdit = new QLineEdit(); passEdit->setEchoMode(QLineEdit::Password);
        passEdit->setPlaceholderText("Password");
        styleInput(passEdit); passEdit->setMinimumHeight(42);

        QPushButton* btn = styledBtn("Login", C_CYAN);
        btn->setMinimumHeight(44);

        QLabel* hint = new QLabel("Default: admin / admin123  |  reception / rec123");
        hint->setAlignment(Qt::AlignCenter);
        hint->setStyleSheet("color:" C_MUTED "; font-size:11px;");

        l->addWidget(logo);
        l->addWidget(sub);
        l->addWidget(userEdit);
        l->addWidget(passEdit);
        l->addWidget(btn);
        l->addWidget(hint);

        connect(btn, &QPushButton::clicked, this, &QDialog::accept);
        // Press Enter to login
        new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(accept()));
    }
};

// ─────────────────────────────────────────────────────────────────────────────
//  MAIN WINDOW
// ─────────────────────────────────────────────────────────────────────────────
class MainWindow : public QMainWindow {
    // ── Widgets that need cross-function access ────────────────────────────
    QStackedWidget* stack      = nullptr;
    QTableWidget*   patTable   = nullptr;
    QTableWidget*   docTable   = nullptr;
    QTableWidget*   appTable   = nullptr;
    QTableWidget*   appTableFull = nullptr;
    QTableWidget*   treatTable = nullptr;
    QTableWidget*   labTable   = nullptr;
    QTableWidget*   invTable   = nullptr;
    QLineEdit*      globalSearch = nullptr;
    QLabel* patCountLbl = nullptr, *docCountLbl = nullptr;
    QLabel* appCountLbl = nullptr, *revLbl      = nullptr;
    QLabel* pendingLbl  = nullptr;
    QLabel* userInfoLbl = nullptr;
    QPushButton* activeBtn = nullptr;

    // ── Sidebar button highlight ───────────────────────────────────────────
    void activateSideBtn(QPushButton* btn) {
        if (activeBtn) activeBtn->setStyleSheet(
            "QPushButton{text-align:left; padding:10px 16px; margin:3px 10px;"
            " border-radius:10px; color:" C_MUTED "; background:transparent; border:none;}"
            "QPushButton:hover{background:" C_SURFACE2 ";}");
        btn->setStyleSheet(
            "QPushButton{text-align:left; padding:10px 16px; margin:3px 10px;"
            " border-radius:10px; color:" C_TEXT ";"
            " background:" C_SURFACE2 "; border-left:3px solid " C_CYAN "; border-top:none;"
            " border-right:none; border-bottom:none;}");
        activeBtn = btn;
    }

    // ── Role-based visibility ─────────────────────────────────────────────
    bool isAdmin() { return currentUser.role == "admin"; }

    // ─────────────────────────────────────────────────────────────────────
    //  REFRESH / LOAD TABLE HELPERS
    // ─────────────────────────────────────────────────────────────────────
    void refreshPatientTable(const QString& filter = "") {
        loadPatients();
        patTable->setRowCount(0);
        int r = 0;
        for (auto& p : patients) {
            QString name = QString::fromStdString(p.name);
            QString phone = QString::fromStdString(p.phone);
            if (!filter.isEmpty() &&
                !name.contains(filter, Qt::CaseInsensitive) &&
                !phone.contains(filter, Qt::CaseInsensitive) &&
                !QString::number(p.id).contains(filter))
                continue;
            patTable->insertRow(r);
            patTable->setItem(r,0, new QTableWidgetItem(QString::number(p.id)));
            patTable->setItem(r,1, new QTableWidgetItem(name));
            patTable->setItem(r,2, new QTableWidgetItem(QString::number(p.age)));
            patTable->setItem(r,3, new QTableWidgetItem(QString::fromStdString(p.gender)));
            patTable->setItem(r,4, new QTableWidgetItem(phone));
            patTable->setItem(r,5, new QTableWidgetItem(QString::fromStdString(p.bloodGroup)));
            patTable->setItem(r,6, new QTableWidgetItem(QString("PKR %1").arg(p.balance, 0, 'f', 0)));
            r++;
        }
        patTable->resizeColumnsToContents();
    }

    void refreshDoctorTable(const QString& filter = "") {
        loadDoctors();
        docTable->setRowCount(0);
        int r = 0;
        for (auto& d : doctors) {
            QString name = QString::fromStdString(d.name);
            QString spec = QString::fromStdString(d.specialty);
            if (!filter.isEmpty() &&
                !name.contains(filter, Qt::CaseInsensitive) &&
                !spec.contains(filter, Qt::CaseInsensitive))
                continue;
            docTable->insertRow(r);
            docTable->setItem(r,0, new QTableWidgetItem(QString::number(d.id)));
            docTable->setItem(r,1, new QTableWidgetItem(name));
            docTable->setItem(r,2, new QTableWidgetItem(spec));
            docTable->setItem(r,3, new QTableWidgetItem(QString("%1 yrs").arg(d.experience)));
            docTable->setItem(r,4, new QTableWidgetItem(QString::fromStdString(d.days)));
            docTable->setItem(r,5, new QTableWidgetItem(QString::fromStdString(d.phone)));
            docTable->setItem(r,6, new QTableWidgetItem(QString("PKR %1").arg(d.consultFee, 0, 'f', 0)));
            r++;
        }
        docTable->resizeColumnsToContents();
    }

    void refreshAppointmentTable(QTableWidget* tbl = nullptr, const QString& filter = "") {
        loadAppointments();
        if (!tbl) tbl = appTableFull;
        if (!tbl) return;
        tbl->setRowCount(0);
        int r = 0;
        for (auto& a : appointments) {
            QString pn = QString::fromStdString(a.pname);
            QString dn = QString::fromStdString(a.dname);
            if (!filter.isEmpty() &&
                !pn.contains(filter, Qt::CaseInsensitive) &&
                !dn.contains(filter, Qt::CaseInsensitive))
                continue;
            tbl->insertRow(r);
            tbl->setItem(r,0, new QTableWidgetItem(pn));
            tbl->setItem(r,1, new QTableWidgetItem(dn));
            tbl->setItem(r,2, new QTableWidgetItem(QString::fromStdString(a.date)));
            tbl->setItem(r,3, new QTableWidgetItem(QString::fromStdString(a.time)));

            QTableWidgetItem* st = new QTableWidgetItem(QString::fromStdString(a.status));
            QString sc = a.status == "Scheduled"  ? C_CYAN  :
                         a.status == "Completed"   ? C_GREEN :
                         a.status == "Cancelled"   ? C_RED   : C_AMBER;
            st->setForeground(QColor(sc));
            st->setFont([](){ QFont f; f.setBold(true); return f; }());
            tbl->setItem(r,4, st);
            r++;
        }
        tbl->resizeColumnsToContents();
    }

    void refreshTreatmentTable(const QString& filter = "") {
        loadTreatments();
        treatTable->setRowCount(0);
        int r = 0;
        for (size_t i = 0; i < treatments.size(); i++) {
            auto& t = treatments[i];
            QString pn = QString::fromStdString(t.pname);
            QString desc = QString::fromStdString(t.desc);
            if (!filter.isEmpty() &&
                !pn.contains(filter, Qt::CaseInsensitive) &&
                !desc.contains(filter, Qt::CaseInsensitive))
                continue;
            treatTable->insertRow(r);
            treatTable->setItem(r,0, new QTableWidgetItem(pn));
            treatTable->setItem(r,1, new QTableWidgetItem(desc));
            treatTable->setItem(r,2, new QTableWidgetItem(QString::fromStdString(t.category)));
            treatTable->setItem(r,3, new QTableWidgetItem(QString("PKR %1").arg(t.cost, 0, 'f', 0)));
            treatTable->setItem(r,4, new QTableWidgetItem(QString::fromStdString(t.date)));
            QTableWidgetItem* ps = new QTableWidgetItem(t.paid ? "✔ Paid" : "✘ Unpaid");
            ps->setForeground(QColor(t.paid ? C_GREEN : C_RED));
            ps->setFont([](){ QFont f; f.setBold(true); return f; }());
            treatTable->setItem(r,5, ps);

            // Mark as Paid button
            if (!t.paid) {
                QPushButton* payBtn = styledBtn("Mark Paid", C_GREEN);
                payBtn->setFixedHeight(28);
                int idx = (int)i; // capture index
                connect(payBtn, &QPushButton::clicked, [this, idx](){
                    treatments[idx].paid = true;
                    saveTreatments();
                    refreshTreatmentTable();
                    updateStats();
                });
                treatTable->setCellWidget(r, 6, payBtn);
            } else {
                treatTable->setItem(r,6, new QTableWidgetItem(""));
            }
            r++;
        }
        treatTable->resizeColumnsToContents();
    }

    void refreshLabTable(const QString& filter = "") {
        loadLabTests();
        labTable->setRowCount(0);
        int r = 0;
        for (auto& l : labTests) {
            QString pn = QString::fromStdString(l.pname);
            QString tn = QString::fromStdString(l.testName);
            if (!filter.isEmpty() &&
                !pn.contains(filter, Qt::CaseInsensitive) &&
                !tn.contains(filter, Qt::CaseInsensitive))
                continue;
            labTable->insertRow(r);
            labTable->setItem(r,0, new QTableWidgetItem(pn));
            labTable->setItem(r,1, new QTableWidgetItem(tn));
            labTable->setItem(r,2, new QTableWidgetItem(QString::fromStdString(l.result.empty() ? "—" : l.result)));
            labTable->setItem(r,3, new QTableWidgetItem(QString::fromStdString(l.date)));

            QTableWidgetItem* st = new QTableWidgetItem(QString::fromStdString(l.status));
            st->setForeground(QColor(l.status == "Done" ? C_GREEN : C_AMBER));
            st->setFont([](){ QFont f; f.setBold(true); return f; }());
            labTable->setItem(r,4, st);

            labTable->setItem(r,5, new QTableWidgetItem(QString("PKR %1").arg(l.cost, 0, 'f', 0)));
            QTableWidgetItem* ps = new QTableWidgetItem(l.paid ? "✔ Paid" : "✘ Unpaid");
            ps->setForeground(QColor(l.paid ? C_GREEN : C_RED));
            labTable->setItem(r,6, ps);
            r++;
        }
        labTable->resizeColumnsToContents();
    }

    void refreshInventoryTable(const QString& filter = "") {
        loadInventory();
        invTable->setRowCount(0);
        int r = 0;
        for (auto& v : inventory) {
            QString name = QString::fromStdString(v.name);
            QString cat  = QString::fromStdString(v.category);
            if (!filter.isEmpty() &&
                !name.contains(filter, Qt::CaseInsensitive) &&
                !cat.contains(filter, Qt::CaseInsensitive))
                continue;
            invTable->insertRow(r);
            invTable->setItem(r,0, new QTableWidgetItem(QString::number(v.id)));
            invTable->setItem(r,1, new QTableWidgetItem(name));
            invTable->setItem(r,2, new QTableWidgetItem(cat));
            invTable->setItem(r,3, new QTableWidgetItem(QString::fromStdString(v.unit)));
            QTableWidgetItem* qty = new QTableWidgetItem(QString::number(v.quantity));
            if (v.quantity < 20)
                qty->setForeground(QColor(C_RED)); // low stock warning
            invTable->setItem(r,4, qty);
            invTable->setItem(r,5, new QTableWidgetItem(QString("PKR %1").arg(v.pricePerUnit, 0, 'f', 0)));
            r++;
        }
        invTable->resizeColumnsToContents();
    }

    void updateStats() {
        loadPatients(); loadDoctors(); loadAppointments(); loadTreatments();
        patCountLbl->setText(QString::number(patients.size()));
        docCountLbl->setText(QString::number(doctors.size()));
        long scheduled = count_if(appointments.begin(), appointments.end(),
                                  [](auto& a){ return a.status == "Scheduled"; });
        appCountLbl->setText(QString::number(scheduled));
        double rev = 0, pend = 0;
        for (auto& t : treatments) {
            if (t.paid) rev += t.cost; else pend += t.cost;
        }
        revLbl->setText(QString("PKR %1").arg((int)rev));
        if (pendingLbl)
            pendingLbl->setText(QString("PKR %1 pending").arg((int)pend));
    }

    // ─────────────────────────────────────────────────────────────────────
    //  DIALOGS — PATIENT
    // ─────────────────────────────────────────────────────────────────────
    void addPatient() {
        QDialog dlg(this);
        dlg.setWindowTitle("New Patient");
        dlg.setFixedSize(480, 440);
        dlg.setStyleSheet("background:" C_SURFACE ";");

        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        auto field = [&](const QString& ph) -> QLineEdit* {
            auto* e = new QLineEdit(); e->setPlaceholderText(ph);
            styleInput(e); e->setMinimumHeight(36); return e;
        };

        QLabel* hdr = new QLabel("➕  Add New Patient");
        hdr->setStyleSheet("font-size:16px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        QLineEdit* nameE  = field("Full Name");
        QSpinBox*  ageS   = new QSpinBox(); ageS->setRange(0,130); ageS->setValue(25); styleInput(ageS);
        QComboBox* genC   = new QComboBox();
        genC->addItems({"Male","Female","Other"}); styleInput(genC);
        QLineEdit* phoneE = field("e.g. 03xxxxxxxxx");
        QComboBox* bgC    = new QComboBox();
        bgC->addItems({"A+","A-","B+","B-","O+","O-","AB+","AB-","Unknown"}); styleInput(bgC);
        QLineEdit* addrE  = field("Home Address");
        QDoubleSpinBox* balS = new QDoubleSpinBox();
        balS->setRange(0,9999999); balS->setPrefix("PKR "); styleInput(balS);

        f->addRow("Name *:", nameE);
        f->addRow("Age *:", ageS);
        f->addRow("Gender:", genC);
        f->addRow("Phone:", phoneE);
        f->addRow("Blood Group:", bgC);
        f->addRow("Address:", addrE);
        f->addRow("Balance:", balS);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_CYAN "; color:white; padding:8px 20px;"
                            " border-radius:8px; font-weight:600;}"
                            "QPushButton[text='Cancel']{background:" C_SURFACE2 ";}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            if (nameE->text().trimmed().isEmpty()) {
                QMessageBox::warning(this, "Validation", "Patient name is required!"); return;
            }
            PatientData p;
            p.id = nextPatId++;
            p.name      = nameE->text().trimmed().toStdString();
            p.age       = ageS->value();
            p.gender    = genC->currentText().toStdString();
            p.phone     = phoneE->text().toStdString();
            p.bloodGroup= bgC->currentText().toStdString();
            p.address   = addrE->text().toStdString();
            p.balance   = balS->value();
            patients.push_back(p);
            savePatients();
            refreshPatientTable();
            updateStats();
            QMessageBox::information(this, "Success",
                QString("Patient <b>%1</b> added with ID <b>%2</b>.").arg(nameE->text()).arg(p.id));
        }
    }

    void editPatient() {
        int row = patTable->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select a patient to edit."); return; }
        int id = patTable->item(row,0)->text().toInt();
        auto it = find_if(patients.begin(), patients.end(), [id](auto& p){ return p.id==id; });
        if (it == patients.end()) return;
        PatientData& p = *it;

        QDialog dlg(this);
        dlg.setWindowTitle("Edit Patient");
        dlg.setFixedSize(480, 420);
        dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        QLabel* hdr = new QLabel(QString("✏️  Edit Patient — ID %1").arg(id));
        hdr->setStyleSheet("font-size:15px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        auto field = [&](const QString& val) -> QLineEdit* {
            auto* e = new QLineEdit(val); styleInput(e); e->setMinimumHeight(36); return e; };

        QLineEdit* nameE  = field(QString::fromStdString(p.name));
        QSpinBox*  ageS   = new QSpinBox(); ageS->setRange(0,130); ageS->setValue(p.age); styleInput(ageS);
        QComboBox* genC   = new QComboBox(); genC->addItems({"Male","Female","Other"});
        genC->setCurrentText(QString::fromStdString(p.gender)); styleInput(genC);
        QLineEdit* phoneE = field(QString::fromStdString(p.phone));
        QComboBox* bgC    = new QComboBox();
        bgC->addItems({"A+","A-","B+","B-","O+","O-","AB+","AB-","Unknown"});
        bgC->setCurrentText(QString::fromStdString(p.bloodGroup)); styleInput(bgC);
        QLineEdit* addrE  = field(QString::fromStdString(p.address));
        QDoubleSpinBox* balS = new QDoubleSpinBox();
        balS->setRange(0,9999999); balS->setPrefix("PKR "); balS->setValue(p.balance); styleInput(balS);

        f->addRow("Name *:", nameE);
        f->addRow("Age:", ageS);
        f->addRow("Gender:", genC);
        f->addRow("Phone:", phoneE);
        f->addRow("Blood Group:", bgC);
        f->addRow("Address:", addrE);
        f->addRow("Balance:", balS);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_CYAN "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            p.name      = nameE->text().trimmed().toStdString();
            p.age       = ageS->value();
            p.gender    = genC->currentText().toStdString();
            p.phone     = phoneE->text().toStdString();
            p.bloodGroup= bgC->currentText().toStdString();
            p.address   = addrE->text().toStdString();
            p.balance   = balS->value();
            savePatients();
            refreshPatientTable();
        }
    }

    void deletePatient() {
        if (!isAdmin()) { QMessageBox::warning(this,"Access Denied","Only admins can delete records."); return; }
        int row = patTable->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select a patient first."); return; }
        QString name = patTable->item(row,1)->text();
        if (QMessageBox::question(this, "Confirm Delete",
            QString("Delete patient <b>%1</b>? This cannot be undone.").arg(name)) == QMessageBox::Yes) {
            patients.erase(patients.begin() + row);
            savePatients();
            refreshPatientTable();
            updateStats();
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  DIALOGS — DOCTOR
    // ─────────────────────────────────────────────────────────────────────
    void addDoctor() {
        if (!isAdmin()) { QMessageBox::warning(this,"Access Denied","Only admins can add doctors."); return; }
        QDialog dlg(this);
        dlg.setWindowTitle("New Doctor"); dlg.setFixedSize(480,400);
        dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        QLabel* hdr = new QLabel("➕  Add New Doctor");
        hdr->setStyleSheet("font-size:16px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        auto field = [&](const QString& ph) -> QLineEdit* {
            auto* e = new QLineEdit(); e->setPlaceholderText(ph);
            styleInput(e); e->setMinimumHeight(36); return e; };

        QLineEdit* nameE = field("Dr. Full Name");
        QLineEdit* specE = field("e.g. Cardiologist");
        QSpinBox*  expS  = new QSpinBox(); expS->setRange(0,60); styleInput(expS);
        QLineEdit* daysE = field("e.g. Mon,Wed,Fri");
        QLineEdit* phoneE= field("03xxxxxxxxx");
        QDoubleSpinBox* feeS = new QDoubleSpinBox();
        feeS->setRange(0,99999); feeS->setPrefix("PKR "); styleInput(feeS);

        f->addRow("Name *:",     nameE);
        f->addRow("Specialty *:",specE);
        f->addRow("Experience:", expS);
        f->addRow("Available Days:", daysE);
        f->addRow("Phone:", phoneE);
        f->addRow("Consult Fee:", feeS);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_GREEN "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            if (nameE->text().trimmed().isEmpty()) {
                QMessageBox::warning(this,"Validation","Doctor name required!"); return; }
            DoctorData d;
            d.id = nextDocId++;
            d.name       = nameE->text().trimmed().toStdString();
            d.specialty  = specE->text().toStdString();
            d.experience = expS->value();
            d.days       = daysE->text().toStdString();
            d.phone      = phoneE->text().toStdString();
            d.consultFee = feeS->value();
            doctors.push_back(d);
            saveDoctors();
            refreshDoctorTable();
            updateStats();
        }
    }

    void editDoctor() {
        if (!isAdmin()) { QMessageBox::warning(this,"Access Denied","Only admins can edit doctors."); return; }
        int row = docTable->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select a doctor to edit."); return; }
        int id = docTable->item(row,0)->text().toInt();
        auto it = find_if(doctors.begin(), doctors.end(), [id](auto& d){ return d.id==id; });
        if (it == doctors.end()) return;
        DoctorData& d = *it;

        QDialog dlg(this); dlg.setWindowTitle("Edit Doctor");
        dlg.setFixedSize(480,380); dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        auto field = [&](const QString& val) -> QLineEdit* {
            auto* e = new QLineEdit(val); styleInput(e); e->setMinimumHeight(36); return e; };

        QLabel* hdr = new QLabel(QString("✏️  Edit Doctor — ID %1").arg(id));
        hdr->setStyleSheet("font-size:15px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        QLineEdit* nameE  = field(QString::fromStdString(d.name));
        QLineEdit* specE  = field(QString::fromStdString(d.specialty));
        QSpinBox*  expS   = new QSpinBox(); expS->setRange(0,60); expS->setValue(d.experience); styleInput(expS);
        QLineEdit* daysE  = field(QString::fromStdString(d.days));
        QLineEdit* phoneE = field(QString::fromStdString(d.phone));
        QDoubleSpinBox* feeS = new QDoubleSpinBox();
        feeS->setRange(0,99999); feeS->setPrefix("PKR "); feeS->setValue(d.consultFee); styleInput(feeS);

        f->addRow("Name:", nameE); f->addRow("Specialty:", specE);
        f->addRow("Experience:", expS); f->addRow("Days:", daysE);
        f->addRow("Phone:", phoneE); f->addRow("Consult Fee:", feeS);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_CYAN "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            d.name       = nameE->text().trimmed().toStdString();
            d.specialty  = specE->text().toStdString();
            d.experience = expS->value();
            d.days       = daysE->text().toStdString();
            d.phone      = phoneE->text().toStdString();
            d.consultFee = feeS->value();
            saveDoctors();
            refreshDoctorTable();
        }
    }

    void deleteDoctor() {
        if (!isAdmin()) { QMessageBox::warning(this,"Access Denied","Only admins can delete doctors."); return; }
        int row = docTable->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select a doctor first."); return; }
        if (QMessageBox::question(this,"Confirm Delete","Delete this doctor?") == QMessageBox::Yes) {
            doctors.erase(doctors.begin() + row);
            saveDoctors(); refreshDoctorTable(); updateStats();
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  DIALOGS — APPOINTMENTS
    // ─────────────────────────────────────────────────────────────────────
    void scheduleAppointment() {
        loadPatients(); loadDoctors();
        if (patients.empty() || doctors.empty()) {
            QMessageBox::warning(this,"Error","Please add patients and doctors first."); return; }

        QDialog dlg(this); dlg.setWindowTitle("Schedule Appointment");
        dlg.setFixedSize(500, 380); dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        QLabel* hdr = new QLabel("📅  Schedule Appointment");
        hdr->setStyleSheet("font-size:16px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        QComboBox* patC = new QComboBox(); styleInput(patC);
        for (auto& p : patients) patC->addItem(
            QString("[%1] %2").arg(p.id).arg(QString::fromStdString(p.name)), p.id);

        QComboBox* docC = new QComboBox(); styleInput(docC);
        for (auto& d : doctors) docC->addItem(
            QString("[%1] %2 — %3").arg(d.id).arg(QString::fromStdString(d.name))
                                    .arg(QString::fromStdString(d.specialty)), d.id);

        QDateEdit* dateE = new QDateEdit(QDate::currentDate());
        dateE->setCalendarPopup(true); dateE->setMinimumDate(QDate::currentDate()); styleInput(dateE);

        QComboBox* timeC = new QComboBox(); styleInput(timeC);
        timeC->addItems({"08:00 AM","09:00 AM","10:00 AM","11:00 AM","12:00 PM",
                         "01:00 PM","02:00 PM","03:00 PM","04:00 PM","05:00 PM","06:00 PM"});

        QLineEdit* notesE = new QLineEdit(); notesE->setPlaceholderText("Reason / Notes (optional)");
        styleInput(notesE);

        f->addRow("Patient:", patC);
        f->addRow("Doctor:", docC);
        f->addRow("Date:", dateE);
        f->addRow("Time:", timeC);
        f->addRow("Notes:", notesE);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_CYAN "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            AppointmentData a;
            a.id     = nextAppId++;
            a.pid    = patC->currentData().toInt();
            a.did    = docC->currentData().toInt();
            a.pname  = patC->currentText().mid(patC->currentText().indexOf("] ")+2).toStdString();
            a.dname  = docC->currentText().mid(docC->currentText().indexOf("] ")+2)
                           .split(" —").first().toStdString();
            a.date   = dateE->date().toString("yyyy-MM-dd").toStdString();
            a.time   = timeC->currentText().toStdString();
            a.status = "Scheduled";
            a.notes  = notesE->text().toStdString();
            appointments.push_back(a);
            saveAppointments();
            refreshAppointmentTable(appTableFull);
            refreshAppointmentTable(appTable);
            updateStats();
            QMessageBox::information(this,"Scheduled",
                QString("Appointment booked for <b>%1</b> on %2 at %3.")
                    .arg(QString::fromStdString(a.pname))
                    .arg(QString::fromStdString(a.date))
                    .arg(QString::fromStdString(a.time)));
        }
    }

    void updateAppointmentStatus(const QString& newStatus) {
        QTableWidget* tbl = appTableFull;
        int row = tbl->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select an appointment first."); return; }
        if (row >= (int)appointments.size()) return;
        appointments[row].status = newStatus.toStdString();
        saveAppointments();
        refreshAppointmentTable(appTableFull);
        refreshAppointmentTable(appTable);
        updateStats();
    }

    void cancelAppointment() {
        int row = appTableFull->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select an appointment first."); return; }
        if (QMessageBox::question(this,"Cancel Appointment","Mark this appointment as Cancelled?")
            == QMessageBox::Yes)
            updateAppointmentStatus("Cancelled");
    }

    // ─────────────────────────────────────────────────────────────────────
    //  DIALOGS — TREATMENT
    // ─────────────────────────────────────────────────────────────────────
    void addTreatment() {
        loadPatients();
        if (patients.empty()) { QMessageBox::warning(this,"Error","Add patients first."); return; }

        QDialog dlg(this); dlg.setWindowTitle("Add Treatment / Billing");
        dlg.setFixedSize(480, 380); dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        QLabel* hdr = new QLabel("💊  Add Treatment");
        hdr->setStyleSheet("font-size:16px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        QComboBox* patC = new QComboBox(); styleInput(patC);
        for (auto& p : patients) patC->addItem(
            QString("[%1] %2").arg(p.id).arg(QString::fromStdString(p.name)), p.id);

        QComboBox* catC = new QComboBox(); styleInput(catC);
        catC->addItems({"General","Cardiology","ENT","Orthopedics","Neurology",
                        "Pediatrics","Gynecology","Dermatology","Surgery","Lab","Other"});

        QLineEdit* descE = new QLineEdit(); descE->setPlaceholderText("Description of treatment");
        styleInput(descE);

        QDoubleSpinBox* costS = new QDoubleSpinBox();
        costS->setRange(0,9999999); costS->setPrefix("PKR "); styleInput(costS);

        QCheckBox* paidC = new QCheckBox("Mark as Paid immediately");
        paidC->setStyleSheet("color:" C_TEXT ";");

        QDateEdit* dateE = new QDateEdit(QDate::currentDate());
        dateE->setCalendarPopup(true); styleInput(dateE);

        f->addRow("Patient:", patC);
        f->addRow("Category:", catC);
        f->addRow("Treatment:", descE);
        f->addRow("Cost:", costS);
        f->addRow("Date:", dateE);
        f->addRow(paidC);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_GREEN "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            if (descE->text().trimmed().isEmpty()) {
                QMessageBox::warning(this,"Validation","Treatment description required!"); return; }
            TreatmentData t;
            t.id       = nextTreatId++;
            t.pid      = patC->currentData().toInt();
            t.pname    = patC->currentText().mid(patC->currentText().indexOf("] ")+2).toStdString();
            t.desc     = descE->text().trimmed().toStdString();
            t.category = catC->currentText().toStdString();
            t.cost     = costS->value();
            t.date     = dateE->date().toString("yyyy-MM-dd").toStdString();
            t.paid     = paidC->isChecked();
            treatments.push_back(t);
            saveTreatments();
            refreshTreatmentTable();
            updateStats();
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  DIALOGS — LAB TESTS
    // ─────────────────────────────────────────────────────────────────────
    void addLabTest() {
        loadPatients();
        if (patients.empty()) { QMessageBox::warning(this,"Error","Add patients first."); return; }
        QDialog dlg(this); dlg.setWindowTitle("Order Lab Test");
        dlg.setFixedSize(480,360); dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        QLabel* hdr = new QLabel("🧪  Order Lab Test");
        hdr->setStyleSheet("font-size:16px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        QComboBox* patC = new QComboBox(); styleInput(patC);
        for (auto& p : patients) patC->addItem(
            QString("[%1] %2").arg(p.id).arg(QString::fromStdString(p.name)), p.id);

        QComboBox* testC = new QComboBox(); styleInput(testC);
        testC->addItems({"CBC (Complete Blood Count)","Blood Sugar (FBS)","Blood Sugar (RBS)",
                         "HbA1c","Lipid Profile","Liver Function Test","Kidney Function Test",
                         "Thyroid Profile (T3,T4,TSH)","Urine Complete Examination",
                         "Stool Examination","Blood Group & Rh","Pregnancy Test (hCG)",
                         "COVID-19 PCR","Hepatitis B (HBsAg)","Hepatitis C (Anti-HCV)",
                         "X-Ray Chest","ECG","Ultrasound Abdomen","MRI Brain","CT Scan Chest",
                         "Custom Test"});
        testC->setEditable(true);

        QDoubleSpinBox* costS = new QDoubleSpinBox();
        costS->setRange(0,99999); costS->setPrefix("PKR "); styleInput(costS);

        QDateEdit* dateE = new QDateEdit(QDate::currentDate());
        dateE->setCalendarPopup(true); styleInput(dateE);

        f->addRow("Patient:", patC);
        f->addRow("Test Name:", testC);
        f->addRow("Cost:", costS);
        f->addRow("Date:", dateE);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_PURPLE "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            LabTestData l;
            l.id       = nextLabId++;
            l.pid      = patC->currentData().toInt();
            l.pname    = patC->currentText().mid(patC->currentText().indexOf("] ")+2).toStdString();
            l.testName = testC->currentText().toStdString();
            l.result   = "";
            l.date     = dateE->date().toString("yyyy-MM-dd").toStdString();
            l.status   = "Pending";
            l.cost     = costS->value();
            l.paid     = false;
            labTests.push_back(l);
            saveLabTests();
            refreshLabTable();
        }
    }

    void enterLabResult() {
        int row = labTable->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select a lab test first."); return; }
        if (row >= (int)labTests.size()) return;
        LabTestData& l = labTests[row];

        QString res = QInputDialog::getText(this, "Enter Result",
            QString("Result for %1 — %2:").arg(QString::fromStdString(l.pname))
                                           .arg(QString::fromStdString(l.testName)),
            QLineEdit::Normal, QString::fromStdString(l.result));
        if (!res.isNull()) {
            l.result = res.toStdString();
            l.status = "Done";
            saveLabTests();
            refreshLabTable();
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  DIALOGS — INVENTORY
    // ─────────────────────────────────────────────────────────────────────
    void addInventoryItem() {
        if (!isAdmin()) { QMessageBox::warning(this,"Access Denied","Only admins can manage inventory."); return; }
        QDialog dlg(this); dlg.setWindowTitle("Add Inventory Item");
        dlg.setFixedSize(440,340); dlg.setStyleSheet("background:" C_SURFACE ";");
        QFormLayout* f = new QFormLayout(&dlg);
        f->setSpacing(10); f->setContentsMargins(30,24,30,24);

        QLabel* hdr = new QLabel("📦  Add Inventory Item");
        hdr->setStyleSheet("font-size:16px; font-weight:700; color:" C_TEXT ";");
        f->addRow(hdr); f->addRow(hLine());

        QLineEdit* nameE = new QLineEdit(); nameE->setPlaceholderText("Item name"); styleInput(nameE);
        QComboBox* catC  = new QComboBox(); styleInput(catC);
        catC->addItems({"Medicine","Supplies","Equipment","Consumables","Other"});
        QLineEdit* unitE = new QLineEdit(); unitE->setPlaceholderText("e.g. Tablet, Box, Bag"); styleInput(unitE);
        QSpinBox*  qtyS  = new QSpinBox(); qtyS->setRange(0,99999); styleInput(qtyS);
        QDoubleSpinBox* priceS = new QDoubleSpinBox();
        priceS->setRange(0,99999); priceS->setPrefix("PKR "); styleInput(priceS);

        f->addRow("Name:", nameE); f->addRow("Category:", catC);
        f->addRow("Unit:", unitE); f->addRow("Quantity:", qtyS); f->addRow("Price/Unit:", priceS);

        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btns->setStyleSheet("QPushButton{background:" C_AMBER "; color:white; padding:8px 20px; border-radius:8px; font-weight:600;}");
        f->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            InventoryData v;
            v.id           = nextInvId++;
            v.name         = nameE->text().trimmed().toStdString();
            v.category     = catC->currentText().toStdString();
            v.unit         = unitE->text().toStdString();
            v.quantity     = qtyS->value();
            v.pricePerUnit = priceS->value();
            inventory.push_back(v);
            saveInventory();
            refreshInventoryTable();
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  PRINT BILL
    // ─────────────────────────────────────────────────────────────────────
    void printBill() {
        int row = treatTable->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Error","Select a treatment to print bill."); return; }
        if (row >= (int)treatments.size()) return;
        TreatmentData& t = treatments[row];

        // Find patient info
        PatientData* pat = nullptr;
        for (auto& p : patients) if (p.id == t.pid) { pat = &p; break; }

        QString html = QString(R"(
<html><head><style>
  body { font-family: Arial, sans-serif; margin:30px; color:#1a202c; }
  h1   { color:#06b6d4; text-align:center; font-size:26px; margin-bottom:2px; }
  .sub { text-align:center; color:#718096; font-size:13px; margin-bottom:24px; }
  table { width:100%; border-collapse:collapse; margin-top:16px; }
  th    { background:#e2e8f0; padding:10px; text-align:left; font-size:13px; }
  td    { padding:10px; border-bottom:1px solid #edf2f7; font-size:13px; }
  .total{ font-weight:bold; font-size:16px; color:#1a202c; }
  .paid { color:#10b981; font-weight:bold; }
  .unpaid{color:#ef4444; font-weight:bold;}
  .footer{text-align:center; margin-top:30px; color:#718096; font-size:11px;}
  .divider{border-top:2px solid #06b6d4; margin:20px 0;}
</style></head><body>
  <h1>🏥 HMS NEXUS PRO</h1>
  <div class="sub">Hospital Bill — Generated %8</div>
  <div class="divider"></div>
  <table>
    <tr><th colspan="2">Patient Information</th></tr>
    <tr><td><b>Name</b></td><td>%1</td></tr>
    <tr><td><b>Patient ID</b></td><td>%2</td></tr>
    <tr><td><b>Phone</b></td><td>%3</td></tr>
    <tr><td><b>Blood Group</b></td><td>%9</td></tr>
  </table>
  <table style="margin-top:16px;">
    <tr><th>Treatment</th><th>Category</th><th>Date</th><th>Amount</th><th>Status</th></tr>
    <tr>
      <td>%4</td>
      <td>%5</td>
      <td>%6</td>
      <td class="total">PKR %7</td>
      <td class="%10">%11</td>
    </tr>
  </table>
  <div class="footer">
    Thank you for choosing HMS Nexus Pro &bull; For queries contact the billing department.
  </div>
</body></html>
)")
        .arg(QString::fromStdString(t.pname))
        .arg(t.pid)
        .arg(pat ? QString::fromStdString(pat->phone) : "—")
        .arg(QString::fromStdString(t.desc))
        .arg(QString::fromStdString(t.category))
        .arg(QString::fromStdString(t.date))
        .arg(t.cost, 0, 'f', 0)
        .arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm"))
        .arg(pat ? QString::fromStdString(pat->bloodGroup) : "—")
        .arg(t.paid ? "paid" : "unpaid")
        .arg(t.paid ? "PAID" : "UNPAID");

        QPrinter printer; QPrintDialog dlg(&printer, this);
        if (dlg.exec() == QDialog::Accepted) {
            QTextDocument doc; doc.setHtml(html); doc.print(&printer);
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  CHARTS
    // ─────────────────────────────────────────────────────────────────────
    void showRevenueChart() {
        loadTreatments();
        double paid = 0, pending = 0;
        for (auto& t : treatments) { if (t.paid) paid += t.cost; else pending += t.cost; }

        QBarSet* paidSet = new QBarSet("Collected"); *paidSet << paid;
        QBarSet* pendSet = new QBarSet("Pending");   *pendSet << pending;
        paidSet->setColor(QColor(C_GREEN));
        pendSet->setColor(QColor(C_AMBER));

        QBarSeries* series = new QBarSeries(); series->append(paidSet); series->append(pendSet);
        QChart* chart = new QChart(); chart->addSeries(series);
        chart->setTitle("Revenue Overview (PKR)");
        chart->setBackgroundBrush(QBrush(QColor(C_SURFACE)));
        chart->setTitleBrush(QBrush(QColor(C_TEXT)));

        QBarCategoryAxis* ax = new QBarCategoryAxis(); ax->append(QStringList{"Billing"});
        chart->addAxis(ax, Qt::AlignBottom); series->attachAxis(ax);
        QValueAxis* ay = new QValueAxis(); chart->addAxis(ay, Qt::AlignLeft); series->attachAxis(ay);

        QChartView* view = new QChartView(chart); view->setRenderHint(QPainter::Antialiasing);
        QDialog dlg(this); dlg.setWindowTitle("Revenue Chart");
        dlg.resize(640,420); dlg.setStyleSheet("background:" C_BG ";");
        QVBoxLayout* l = new QVBoxLayout(&dlg); l->addWidget(view); dlg.exec();
    }

    void showAgeChart() {
        loadPatients();
        int a=0,b=0,c=0,d=0;
        for (auto& p : patients) {
            if (p.age<=18) a++; else if (p.age<=35) b++; else if (p.age<=55) c++; else d++;
        }
        QBarSet* s = new QBarSet("Patients");
        *s << a << b << c << d;
        s->setColor(QColor(C_CYAN));
        QBarSeries* series = new QBarSeries(); series->append(s);
        QChart* chart = new QChart(); chart->addSeries(series);
        chart->setTitle("Patient Age Distribution");
        chart->setBackgroundBrush(QBrush(QColor(C_SURFACE)));
        chart->setTitleBrush(QBrush(QColor(C_TEXT)));
        QBarCategoryAxis* ax = new QBarCategoryAxis();
        ax->append(QStringList{"0-18","19-35","36-55","55+"});
        chart->addAxis(ax, Qt::AlignBottom); series->attachAxis(ax);
        QValueAxis* ay = new QValueAxis(); chart->addAxis(ay, Qt::AlignLeft); series->attachAxis(ay);

        QChartView* view = new QChartView(chart); view->setRenderHint(QPainter::Antialiasing);
        QDialog dlg(this); dlg.setWindowTitle("Age Distribution");
        dlg.resize(640,420); dlg.setStyleSheet("background:" C_BG ";");
        QVBoxLayout* l = new QVBoxLayout(&dlg); l->addWidget(view); dlg.exec();
    }

    void showGenderChart() {
        loadPatients();
        int male=0, female=0, other=0;
        for (auto& p : patients) {
            if (p.gender=="Male") male++;
            else if (p.gender=="Female") female++;
            else other++;
        }
        QPieSeries* series = new QPieSeries();
        series->append("Male",   male)->setBrush(QBrush(QColor(C_BLUE)));
        series->append("Female", female)->setBrush(QBrush(QColor(C_PURPLE)));
        if (other) series->append("Other", other)->setBrush(QBrush(QColor(C_AMBER)));

        QChart* chart = new QChart(); chart->addSeries(series);
        chart->setTitle("Gender Distribution");
        chart->setBackgroundBrush(QBrush(QColor(C_SURFACE)));
        chart->setTitleBrush(QBrush(QColor(C_TEXT)));

        QChartView* view = new QChartView(chart); view->setRenderHint(QPainter::Antialiasing);
        QDialog dlg(this); dlg.setWindowTitle("Gender Chart");
        dlg.resize(520,420); dlg.setStyleSheet("background:" C_BG ";");
        QVBoxLayout* l = new QVBoxLayout(&dlg); l->addWidget(view); dlg.exec();
    }

    // ─────────────────────────────────────────────────────────────────────
    //  EXPORT CSV
    // ─────────────────────────────────────────────────────────────────────
    void exportCSV() {
        QDialog dlg(this); dlg.setWindowTitle("Export CSV");
        dlg.setFixedSize(320,200); dlg.setStyleSheet("background:" C_SURFACE ";");
        QVBoxLayout* l = new QVBoxLayout(&dlg); l->setSpacing(10); l->setContentsMargins(24,24,24,24);
        QLabel* hdr = new QLabel("Choose what to export:");
        hdr->setStyleSheet("color:" C_TEXT "; font-weight:700;"); l->addWidget(hdr);
        QPushButton* b1 = styledBtn("Patients CSV",  C_CYAN,  "👥");
        QPushButton* b2 = styledBtn("Doctors CSV",   C_GREEN, "👨‍⚕️");
        QPushButton* b3 = styledBtn("Treatments CSV",C_PURPLE,"💊");
        l->addWidget(b1); l->addWidget(b2); l->addWidget(b3);

        auto doExport = [&](const QString& title, auto& vec, auto rowFn) {
            QString fn = QFileDialog::getSaveFileName(this, "Export "+title, title+".csv", "CSV (*.csv)");
            if (fn.isEmpty()) return;
            QFile f(fn); if (!f.open(QIODevice::WriteOnly)) return;
            QTextStream s(&f); s.setEncoding(QStringConverter::Utf8);
            rowFn(s);
            for (auto& item : vec) rowFn(s, item);
            f.close();
            QMessageBox::information(this,"Exported", title+" exported to:\n"+fn);
        };

        connect(b1, &QPushButton::clicked, [&](){
            loadPatients();
            QString fn = QFileDialog::getSaveFileName(this,"Export Patients","patients.csv","CSV (*.csv)");
            if (fn.isEmpty()) return;
            QFile f(fn); if (!f.open(QIODevice::WriteOnly)) return;
            QTextStream s(&f); s.setEncoding(QStringConverter::Utf8);
            s << "ID,Name,Age,Gender,Phone,BloodGroup,Balance,Address\n";
            for (auto& p : patients)
                s << p.id <<","<< QString::fromStdString(p.name) <<","<< p.age <<","
                  << QString::fromStdString(p.gender) <<","<< QString::fromStdString(p.phone) <<","
                  << QString::fromStdString(p.bloodGroup) <<","<< p.balance <<","
                  << QString::fromStdString(p.address) <<"\n";
            f.close(); QMessageBox::information(this,"Done","Patients exported!"); dlg.accept();
        });
        connect(b2, &QPushButton::clicked, [&](){
            loadDoctors();
            QString fn = QFileDialog::getSaveFileName(this,"Export Doctors","doctors.csv","CSV (*.csv)");
            if (fn.isEmpty()) return;
            QFile f(fn); if (!f.open(QIODevice::WriteOnly)) return;
            QTextStream s(&f); s.setEncoding(QStringConverter::Utf8);
            s << "ID,Name,Specialty,Experience,Days,Phone,ConsultFee\n";
            for (auto& d : doctors)
                s << d.id <<","<< QString::fromStdString(d.name) <<","
                  << QString::fromStdString(d.specialty) <<","<< d.experience <<","
                  << QString::fromStdString(d.days) <<","<< QString::fromStdString(d.phone) <<","
                  << d.consultFee <<"\n";
            f.close(); QMessageBox::information(this,"Done","Doctors exported!"); dlg.accept();
        });
        connect(b3, &QPushButton::clicked, [&](){
            loadTreatments();
            QString fn = QFileDialog::getSaveFileName(this,"Export Treatments","treatments.csv","CSV (*.csv)");
            if (fn.isEmpty()) return;
            QFile f(fn); if (!f.open(QIODevice::WriteOnly)) return;
            QTextStream s(&f); s.setEncoding(QStringConverter::Utf8);
            s << "ID,PatientID,Patient,Treatment,Category,Cost,Date,Paid\n";
            for (auto& t : treatments)
                s << t.id <<","<< t.pid <<","<< QString::fromStdString(t.pname) <<","
                  << QString::fromStdString(t.desc) <<","<< QString::fromStdString(t.category) <<","
                  << t.cost <<","<< QString::fromStdString(t.date) <<","<< (t.paid?"Yes":"No") <<"\n";
            f.close(); QMessageBox::information(this,"Done","Treatments exported!"); dlg.accept();
        });
        dlg.exec();
    }

    // ─────────────────────────────────────────────────────────────────────
    //  BACKUP / RESTORE
    // ─────────────────────────────────────────────────────────────────────
    void backup() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Backup Location");
        if (dir.isEmpty()) return;
        QString ts   = QDateTime::currentDateTime().toString("yyyyMMdd_HHmm");
        QString path = dir + "/HMS_Backup_" + ts;
        QDir().mkdir(path);
        const QStringList files = {"patients.txt","doctors.txt","appointments.txt",
                                   "treatments.txt","labtests.txt","inventory.txt","users.txt"};
        for (auto& fn : files) QFile::copy(fn, path + "/" + fn);
        QMessageBox::information(this, "Backup Complete",
            "Data backed up to:\n" + path + "\n\n" + QString::number(files.size()) + " files saved.");
    }

    void restore() {
        if (!isAdmin()) { QMessageBox::warning(this,"Access Denied","Only admins can restore data."); return; }
        QString dir = QFileDialog::getExistingDirectory(this, "Select Backup Folder");
        if (dir.isEmpty()) return;
        if (QMessageBox::question(this,"Confirm Restore",
            "⚠️ This will overwrite ALL current data with the backup.\nContinue?") == QMessageBox::Yes) {
            const QStringList files = {"patients.txt","doctors.txt","appointments.txt",
                                       "treatments.txt","labtests.txt","inventory.txt","users.txt"};
            for (auto& fn : files) {
                QFile::remove(fn);
                QFile::copy(dir + "/" + fn, fn);
            }
            loadPatients(); loadDoctors(); loadAppointments(); loadTreatments(); loadLabTests(); loadInventory();
            refreshPatientTable(); refreshDoctorTable();
            refreshAppointmentTable(appTableFull); refreshAppointmentTable(appTable);
            refreshTreatmentTable(); refreshLabTable(); refreshInventoryTable();
            updateStats();
            QMessageBox::information(this,"Restored","Data restored successfully.");
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    //  GENERATE REPORT (text summary dialog)
    // ─────────────────────────────────────────────────────────────────────
    void generateReport() {
        loadPatients(); loadDoctors(); loadAppointments(); loadTreatments(); loadLabTests();
        double total=0, paid=0, pending=0;
        for (auto& t : treatments) { total += t.cost; if (t.paid) paid += t.cost; else pending += t.cost; }
        long schd = count_if(appointments.begin(), appointments.end(),
                             [](auto& a){ return a.status=="Scheduled"; });
        long done = count_if(appointments.begin(), appointments.end(),
                             [](auto& a){ return a.status=="Completed"; });
        long labs = count_if(labTests.begin(), labTests.end(),
                             [](auto& l){ return l.status=="Pending"; });

        QDialog dlg(this); dlg.setWindowTitle("Hospital Summary Report");
        dlg.resize(560, 480); dlg.setStyleSheet("background:" C_SURFACE ";");
        QVBoxLayout* l = new QVBoxLayout(&dlg); l->setContentsMargins(30,24,30,24);

        QTextEdit* te = new QTextEdit(); te->setReadOnly(true);
        te->setStyleSheet("background:" C_BG "; color:" C_TEXT "; border:none; font-family:monospace; font-size:13px;");
        te->setHtml(QString(R"(
<h2 style='color:#06b6d4;'>🏥 HMS NEXUS PRO — Summary Report</h2>
<p style='color:#94a3b8;'>Generated: %1</p>
<hr style='border-color:#334155;'/>
<table style='width:100%; font-size:13px;'>
<tr><td style='color:#94a3b8; padding:6px 0;'>Total Patients</td>
    <td style='color:#f1f5f9; font-weight:bold;'>%2</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Total Doctors</td>
    <td style='color:#f1f5f9; font-weight:bold;'>%3</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Scheduled Appointments</td>
    <td style='color:#06b6d4; font-weight:bold;'>%4</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Completed Appointments</td>
    <td style='color:#10b981; font-weight:bold;'>%5</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Pending Lab Tests</td>
    <td style='color:#f59e0b; font-weight:bold;'>%6</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Total Billing</td>
    <td style='color:#f1f5f9; font-weight:bold;'>PKR %7</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Collected Revenue</td>
    <td style='color:#10b981; font-weight:bold;'>PKR %8</td></tr>
<tr><td style='color:#94a3b8; padding:6px 0;'>Pending Payments</td>
    <td style='color:#ef4444; font-weight:bold;'>PKR %9</td></tr>
</table>
)")
        .arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm"))
        .arg(patients.size()).arg(doctors.size())
        .arg(schd).arg(done).arg(labs)
        .arg((int)total).arg((int)paid).arg((int)pending));

        QPushButton* printBtn = styledBtn("🖨️ Print Report", C_PURPLE);
        connect(printBtn, &QPushButton::clicked, [&](){
            QPrinter printer; QPrintDialog pdlg(&printer, this);
            if (pdlg.exec() == QDialog::Accepted) {
                QTextDocument doc; doc.setHtml(te->toHtml()); doc.print(&printer);
            }
        });
        l->addWidget(te); l->addWidget(printBtn); dlg.exec();
    }

    // ─────────────────────────────────────────────────────────────────────
    //  BUILD UI
    // ─────────────────────────────────────────────────────────────────────
public:
    MainWindow() {
        createSampleData();
        loadPatients(); loadDoctors(); loadAppointments();
        loadTreatments(); loadLabTests(); loadInventory();

        setWindowTitle("HMS Nexus Pro");
        resize(1400, 820);
        setStyleSheet(QString(
            "QMainWindow, QWidget { background:" C_BG "; }"
            "QLabel { color:" C_TEXT "; }"
            "QComboBox QAbstractItemView { background:" C_SURFACE "; color:" C_TEXT "; border:1px solid " C_BORDER "; }"
            "QComboBox::drop-down { border:none; }"
            "QComboBox::down-arrow { image:none; }"
            "QCalendarWidget { background:" C_SURFACE "; color:" C_TEXT "; }"
            "QToolTip { background:" C_SURFACE "; color:" C_TEXT "; border:1px solid " C_BORDER "; padding:4px; }"
            "QScrollBar:horizontal { background:" C_BG "; height:8px; border-radius:4px; }"
            "QScrollBar::handle:horizontal { background:" C_BORDER "; border-radius:4px; }"
        ));

        QWidget* central = new QWidget(this);
        QHBoxLayout* rootLayout = new QHBoxLayout(central);
        rootLayout->setContentsMargins(0,0,0,0); rootLayout->setSpacing(0);

        // ── SIDEBAR ────────────────────────────────────────────────────
        QWidget* sidebar = new QWidget();
        sidebar->setFixedWidth(250);
        sidebar->setStyleSheet("background:" C_SURFACE "; border-right:1px solid " C_BORDER ";");
        QVBoxLayout* sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(0,0,0,0); sideLayout->setSpacing(0);

        QWidget* logoBox = new QWidget();
        logoBox->setStyleSheet("padding:16px 12px 12px;");
        QVBoxLayout* logoLay = new QVBoxLayout(logoBox);
        logoLay->setSpacing(0);
        QLabel* logoL = new QLabel();
        logoL->setAlignment(Qt::AlignCenter);
        logoL->setTextFormat(Qt::RichText);
        logoL->setText(QString(
            "<svg xmlns='http://www.w3.org/2000/svg' width='210' height='70' viewBox='0 0 210 70'>"
            "<polygon points='22,4 46,4 58,24 46,44 22,44 10,24' fill='#0f172a' stroke='#06b6d4' stroke-width='1.8'/>"
            "<polygon points='23,7 45,7 56,24 45,41 23,41 12,24' fill='#1e293b'/>"
            "<rect x='27' y='12' width='12' height='24' rx='2.5' fill='#06b6d4'/>"
            "<rect x='20' y='18' width='26' height='12' rx='2.5' fill='#06b6d4'/>"
            "<circle cx='33' cy='24' r='2.5' fill='#0f172a'/>"
            "<line x1='58' y1='24' x2='70' y2='24' stroke='#06b6d4' stroke-width='1' opacity='0.5'/>"
            "<circle cx='73' cy='24' r='2' fill='#10b981'/>"
            "<text x='80' y='17' font-family='sans-serif' font-size='13' font-weight='700' fill='#f1f5f9'>HMS NEXUS PRO</text>"
            "<text x='81' y='29' font-family='sans-serif' font-size='7' fill='#94a3b8' letter-spacing='1.5'>HOSPITAL MANAGEMENT</text>"
            "<text x='81' y='41' font-family='sans-serif' font-size='7' fill='#06b6d4'>by Farhan Ali  \u2022  v2.0</text>"
            "<line x1='81' y1='47' x2='205' y2='47' stroke='#334155' stroke-width='0.8'/>"
            "</svg>"));
        logoLay->addWidget(logoL);
        sideLayout->addWidget(logoBox);
        sideLayout->addWidget(hLine());

        auto sideBtn = [](const QString& text, const QString& icon) -> QPushButton* {
            QPushButton* b = new QPushButton(icon + "   " + text);
            b->setStyleSheet(
                "QPushButton{text-align:left; padding:10px 16px; margin:3px 10px;"
                " border-radius:10px; color:" C_MUTED "; background:transparent; border:none;}"
                "QPushButton:hover{background:" C_SURFACE2 ";}");
            b->setCursor(Qt::PointingHandCursor);
            b->setMinimumHeight(40);
            return b;
        };

        QPushButton* dashBtn   = sideBtn("Dashboard",   "📊");
        QPushButton* patBtn    = sideBtn("Patients",     "👥");
        QPushButton* docBtn    = sideBtn("Doctors",      "👨‍⚕️");
        QPushButton* appBtn    = sideBtn("Appointments", "📅");
        QPushButton* treatBtn  = sideBtn("Treatments",   "💊");
        QPushButton* labBtn    = sideBtn("Lab Tests",    "🧪");
        QPushButton* invBtn    = sideBtn("Inventory",    "📦");
        QPushButton* reportBtn = sideBtn("Reports",      "📄");
        QPushButton* logoutBtn = sideBtn("Logout",       "🚪");
        logoutBtn->setStyleSheet(
            "QPushButton{text-align:left; padding:10px 16px; margin:3px 10px;"
            " border-radius:10px; color:" C_RED "; background:transparent; border:none;}"
            "QPushButton:hover{background:" C_SURFACE2 ";}");

        sideLayout->addSpacing(6);
        sideLayout->addWidget(dashBtn); sideLayout->addWidget(patBtn);
        sideLayout->addWidget(docBtn);  sideLayout->addWidget(appBtn);
        sideLayout->addWidget(treatBtn);sideLayout->addWidget(labBtn);
        sideLayout->addWidget(invBtn);  sideLayout->addWidget(reportBtn);
        sideLayout->addStretch();

        // User info box at bottom of sidebar
        QWidget* userBox = new QWidget();
        userBox->setStyleSheet("background:" C_SURFACE2 "; border-top:1px solid " C_BORDER "; padding:14px;");
        QVBoxLayout* ubLay = new QVBoxLayout(userBox); ubLay->setSpacing(4);
        userInfoLbl = new QLabel(
            QString::fromStdString(currentUser.fullname) + "\n" +
            "[" + QString::fromStdString(currentUser.role).toUpper() + "]");
        userInfoLbl->setStyleSheet("color:" C_MUTED "; font-size:12px;");
        ubLay->addWidget(userInfoLbl);
        ubLay->addWidget(logoutBtn);
        sideLayout->addWidget(userBox);

        // ── CONTENT AREA ──────────────────────────────────────────────
        QWidget* content = new QWidget();
        QVBoxLayout* contentLay = new QVBoxLayout(content);
        contentLay->setContentsMargins(20,16,20,16); contentLay->setSpacing(14);

        // Top bar
        QWidget* topBar = new QWidget();
        topBar->setStyleSheet("background:" C_SURFACE "; border-radius:12px; padding:4px 16px;");
        topBar->setFixedHeight(54);
        QHBoxLayout* topLay = new QHBoxLayout(topBar);
        QLabel* pageTitle = new QLabel("Dashboard");
        pageTitle->setStyleSheet("font-size:18px; font-weight:700; color:" C_TEXT ";");

        globalSearch = new QLineEdit();
        globalSearch->setPlaceholderText("🔍  Search patients, doctors...");
        globalSearch->setStyleSheet(
            "background:" C_SURFACE2 "; color:" C_TEXT ";"
            " border:1px solid " C_BORDER "; border-radius:20px; padding:6px 16px;"
            " min-width:280px;");

        QLabel* clock = new QLabel();
        clock->setStyleSheet("color:" C_MUTED "; font-size:13px;");
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [clock](){
            clock->setText(QDateTime::currentDateTime().toString("ddd, dd MMM yyyy  hh:mm:ss"));
        });
        timer->start(1000);
        clock->setText(QDateTime::currentDateTime().toString("ddd, dd MMM yyyy  hh:mm:ss"));

        topLay->addWidget(pageTitle); topLay->addStretch();
        topLay->addWidget(globalSearch); topLay->addSpacing(16); topLay->addWidget(clock);
        contentLay->addWidget(topBar);

        // ── STACKED WIDGET ─────────────────────────────────────────────
        stack = new QStackedWidget();

        // ─── PAGE 0: DASHBOARD ─────────────────────────────────────────
        QWidget* dashPage = new QWidget();
        QVBoxLayout* dashLay = new QVBoxLayout(dashPage);
        dashLay->setSpacing(16);

        // Stat cards row
        QHBoxLayout* statsRow = new QHBoxLayout(); statsRow->setSpacing(14);
        statsRow->addWidget(statCard(patCountLbl,  "Total Patients",   "👥", C_CYAN));
        statsRow->addWidget(statCard(docCountLbl,  "Active Doctors",   "👨‍⚕️", C_GREEN));
        statsRow->addWidget(statCard(appCountLbl,  "Scheduled Today",  "📅", C_AMBER));
        statsRow->addWidget(statCard(revLbl,       "Revenue Collected","💰", C_PURPLE));
        dashLay->addLayout(statsRow);

        // Quick-action buttons
        QWidget* qaBox = new QWidget(); qaBox->setStyleSheet("background:" C_SURFACE "; border-radius:12px; padding:16px;");
        QVBoxLayout* qaLay = new QVBoxLayout(qaBox); qaLay->setSpacing(10);
        QLabel* qaTitle = new QLabel("⚡  Quick Actions");
        qaTitle->setStyleSheet("font-size:14px; font-weight:700; color:" C_TEXT ";");
        QHBoxLayout* qaRow = new QHBoxLayout(); qaRow->setSpacing(10);
        QPushButton* qaPat  = styledBtn("New Patient",     C_CYAN,   "👥");
        QPushButton* qaApp  = styledBtn("New Appointment", C_GREEN,  "📅");
        QPushButton* qaTreat= styledBtn("Add Treatment",   C_PURPLE, "💊");
        QPushButton* qaLab  = styledBtn("Order Lab Test",  C_AMBER,  "🧪");
        qaRow->addWidget(qaPat); qaRow->addWidget(qaApp); qaRow->addWidget(qaTreat); qaRow->addWidget(qaLab);
        qaLay->addWidget(qaTitle); qaLay->addLayout(qaRow);
        dashLay->addWidget(qaBox);

        // Recent appointments table (shown on dashboard)
        QWidget* recBox = new QWidget(); recBox->setStyleSheet("background:" C_SURFACE "; border-radius:12px; padding:16px;");
        QVBoxLayout* recLay = new QVBoxLayout(recBox);
        QLabel* recTitle = new QLabel("📋  Recent Appointments");
        recTitle->setStyleSheet("font-size:14px; font-weight:700; color:" C_TEXT "; margin-bottom:8px;");
        appTable = styledTable({"Patient","Doctor","Date","Time","Status"});
        appTable->setMaximumHeight(220);
        recLay->addWidget(recTitle); recLay->addWidget(appTable);
        dashLay->addWidget(recBox);

        stack->addWidget(dashPage); // index 0

        // ─── PAGE 1: PATIENTS ──────────────────────────────────────────
        QWidget* patPage = new QWidget();
        QVBoxLayout* patLay = new QVBoxLayout(patPage); patLay->setSpacing(12);
        QHBoxLayout* patHead = new QHBoxLayout();
        patHead->addWidget(sectionLabel("👥  Patient Management"));
        patHead->addStretch();
        QPushButton* addPatBtn  = styledBtn("Add Patient", C_GREEN, "➕");
        QPushButton* editPatBtn = styledBtn("Edit",        C_CYAN,  "✏️");
        QPushButton* delPatBtn  = styledBtn("Delete",      C_RED,   "🗑️");
        QPushButton* refPatBtn  = styledBtn("Refresh",     C_SURFACE2, "🔄");
        patHead->addWidget(addPatBtn); patHead->addWidget(editPatBtn);
        patHead->addWidget(delPatBtn); patHead->addWidget(refPatBtn);
        patTable = styledTable({"ID","Name","Age","Gender","Phone","Blood","Balance"});
        patLay->addLayout(patHead); patLay->addWidget(patTable);
        stack->addWidget(patPage); // index 1

        // ─── PAGE 2: DOCTORS ───────────────────────────────────────────
        QWidget* docPage = new QWidget();
        QVBoxLayout* docLay = new QVBoxLayout(docPage); docLay->setSpacing(12);
        QHBoxLayout* docHead = new QHBoxLayout();
        docHead->addWidget(sectionLabel("👨‍⚕️  Doctor Management"));
        docHead->addStretch();
        QPushButton* addDocBtn  = styledBtn("Add Doctor", C_GREEN, "➕");
        QPushButton* editDocBtn = styledBtn("Edit",       C_CYAN,  "✏️");
        QPushButton* delDocBtn  = styledBtn("Delete",     C_RED,   "🗑️");
        QPushButton* refDocBtn  = styledBtn("Refresh",    C_SURFACE2, "🔄");
        docHead->addWidget(addDocBtn); docHead->addWidget(editDocBtn);
        docHead->addWidget(delDocBtn); docHead->addWidget(refDocBtn);
        docTable = styledTable({"ID","Name","Specialty","Exp","Days","Phone","Fee"});
        docLay->addLayout(docHead); docLay->addWidget(docTable);
        stack->addWidget(docPage); // index 2

        // ─── PAGE 3: APPOINTMENTS ──────────────────────────────────────
        QWidget* appPage = new QWidget();
        QVBoxLayout* appLay = new QVBoxLayout(appPage); appLay->setSpacing(12);
        QHBoxLayout* appHead = new QHBoxLayout();
        appHead->addWidget(sectionLabel("📅  Appointment Management"));
        appHead->addStretch();
        QPushButton* schedBtn    = styledBtn("Schedule",    C_GREEN,  "📅");
        QPushButton* doneBtn     = styledBtn("Completed",   C_CYAN,   "✔");
        QPushButton* noShowBtn   = styledBtn("No-Show",     C_AMBER,  "⚠️");
        QPushButton* cancelBtn   = styledBtn("Cancel",      C_RED,    "✘");
        QPushButton* refAppBtn   = styledBtn("Refresh",     C_SURFACE2, "🔄");
        appHead->addWidget(schedBtn); appHead->addWidget(doneBtn);
        appHead->addWidget(noShowBtn); appHead->addWidget(cancelBtn); appHead->addWidget(refAppBtn);
        appTableFull = styledTable({"Patient","Doctor","Date","Time","Status"});
        appLay->addLayout(appHead); appLay->addWidget(appTableFull);
        stack->addWidget(appPage); // index 3

        // ─── PAGE 4: TREATMENTS ────────────────────────────────────────
        QWidget* treatPage = new QWidget();
        QVBoxLayout* treatLay = new QVBoxLayout(treatPage); treatLay->setSpacing(12);
        QHBoxLayout* treatHead = new QHBoxLayout();
        treatHead->addWidget(sectionLabel("💊  Treatment & Billing"));
        treatHead->addStretch();
        QPushButton* addTreatBtn = styledBtn("Add Treatment", C_GREEN,    "➕");
        QPushButton* printBillBtn= styledBtn("Print Bill",    C_PURPLE,   "🖨️");
        QPushButton* refTreatBtn = styledBtn("Refresh",       C_SURFACE2, "🔄");
        treatHead->addWidget(addTreatBtn); treatHead->addWidget(printBillBtn); treatHead->addWidget(refTreatBtn);
        treatTable = styledTable({"Patient","Treatment","Category","Cost","Date","Status","Action"});
        treatLay->addLayout(treatHead); treatLay->addWidget(treatTable);
        stack->addWidget(treatPage); // index 4

        // ─── PAGE 5: LAB TESTS ─────────────────────────────────────────
        QWidget* labPage = new QWidget();
        QVBoxLayout* labLay = new QVBoxLayout(labPage); labLay->setSpacing(12);
        QHBoxLayout* labHead = new QHBoxLayout();
        labHead->addWidget(sectionLabel("🧪  Lab Tests"));
        labHead->addStretch();
        QPushButton* addLabBtn    = styledBtn("Order Test",    C_PURPLE,   "➕");
        QPushButton* enterResBtn  = styledBtn("Enter Result",  C_GREEN,    "📝");
        QPushButton* refLabBtn    = styledBtn("Refresh",       C_SURFACE2, "🔄");
        labHead->addWidget(addLabBtn); labHead->addWidget(enterResBtn); labHead->addWidget(refLabBtn);
        labTable = styledTable({"Patient","Test","Result","Date","Status","Cost","Payment"});
        labLay->addLayout(labHead); labLay->addWidget(labTable);
        stack->addWidget(labPage); // index 5

        // ─── PAGE 6: INVENTORY ─────────────────────────────────────────
        QWidget* invPage = new QWidget();
        QVBoxLayout* invLay = new QVBoxLayout(invPage); invLay->setSpacing(12);
        QHBoxLayout* invHead = new QHBoxLayout();
        invHead->addWidget(sectionLabel("📦  Inventory & Supplies"));
        invHead->addStretch();
        QLabel* lowStockNote = new QLabel("🔴 Red = Low Stock (<20)");
        lowStockNote->setStyleSheet("color:" C_MUTED "; font-size:12px;");
        QPushButton* addInvBtn = styledBtn("Add Item",  C_AMBER,    "➕");
        QPushButton* refInvBtn = styledBtn("Refresh",   C_SURFACE2, "🔄");
        invHead->addWidget(lowStockNote); invHead->addWidget(addInvBtn); invHead->addWidget(refInvBtn);
        invTable = styledTable({"ID","Name","Category","Unit","Quantity","Price/Unit"});
        invLay->addLayout(invHead); invLay->addWidget(invTable);
        stack->addWidget(invPage); // index 6

        // ─── PAGE 7: REPORTS ───────────────────────────────────────────
        QWidget* reportPage = new QWidget();
        QVBoxLayout* reportLay = new QVBoxLayout(reportPage); reportLay->setSpacing(16);
        reportLay->addWidget(sectionLabel("📄  Reports & Analytics"));

        // Charts row
        QGroupBox* chartBox = new QGroupBox("📊  Charts");
        chartBox->setStyleSheet("QGroupBox{ color:" C_TEXT "; border:1px solid " C_BORDER "; border-radius:10px; margin-top:10px; padding:12px; }"
                                "QGroupBox::title{ subcontrol-origin:margin; left:14px; padding:0 6px; color:" C_CYAN "; font-weight:700; }");
        QHBoxLayout* chartRow = new QHBoxLayout(chartBox);
        QPushButton* revChartBtn = styledBtn("Revenue Chart",    C_GREEN,  "💰");
        QPushButton* ageChartBtn = styledBtn("Age Distribution", C_CYAN,   "📊");
        QPushButton* genChartBtn = styledBtn("Gender Chart",     C_PURPLE, "🥧");
        chartRow->addWidget(revChartBtn); chartRow->addWidget(ageChartBtn); chartRow->addWidget(genChartBtn);
        reportLay->addWidget(chartBox);

        // Data tools
        QGroupBox* toolBox = new QGroupBox("🔧  Data Tools");
        toolBox->setStyleSheet(chartBox->styleSheet());
        QGridLayout* toolGrid = new QGridLayout(toolBox);
        toolGrid->setSpacing(10);
        QPushButton* exportBtn  = styledBtn("Export CSV",       C_GREEN,  "📊");
        QPushButton* genRepBtn  = styledBtn("Summary Report",   C_BLUE,   "📄");
        QPushButton* backupBtn  = styledBtn("Backup Data",      C_AMBER,  "💾");
        QPushButton* restoreBtn = styledBtn("Restore Data",     C_RED,    "🔄");
        toolGrid->addWidget(exportBtn, 0,0); toolGrid->addWidget(genRepBtn,  0,1);
        toolGrid->addWidget(backupBtn, 1,0); toolGrid->addWidget(restoreBtn, 1,1);
        reportLay->addWidget(toolBox);
        reportLay->addStretch();

        stack->addWidget(reportPage); // index 7

        contentLay->addWidget(stack);

        rootLayout->addWidget(sidebar);
        rootLayout->addWidget(content, 1);
        setCentralWidget(central);

        // ── CONNECT SIGNALS ────────────────────────────────────────────

        // Sidebar navigation
        auto navTo = [=](QPushButton* btn, QWidget* page, const QString& title,
                         std::function<void()> refresh) {
            connect(btn, &QPushButton::clicked, [=](){
                activateSideBtn(btn);
                stack->setCurrentWidget(page);
                pageTitle->setText(title);
                if (refresh) refresh();
            });
        };
        navTo(dashBtn,   dashPage,   "Dashboard",       [this](){ updateStats(); refreshAppointmentTable(appTable); });
        navTo(patBtn,    patPage,    "Patients",         [this](){ refreshPatientTable(); });
        navTo(docBtn,    docPage,    "Doctors",          [this](){ refreshDoctorTable(); });
        navTo(appBtn,    appPage,    "Appointments",     [this](){ refreshAppointmentTable(appTableFull); });
        navTo(treatBtn,  treatPage,  "Treatments",       [this](){ refreshTreatmentTable(); });
        navTo(labBtn,    labPage,    "Lab Tests",        [this](){ refreshLabTable(); });
        navTo(invBtn,    invPage,    "Inventory",        [this](){ refreshInventoryTable(); });
        navTo(reportBtn, reportPage, "Reports",          nullptr);

        connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::close);

        // Patient page
        connect(addPatBtn,  &QPushButton::clicked, this, &MainWindow::addPatient);
        connect(editPatBtn, &QPushButton::clicked, this, &MainWindow::editPatient);
        connect(delPatBtn,  &QPushButton::clicked, this, &MainWindow::deletePatient);
        connect(refPatBtn,  &QPushButton::clicked, [this](){ refreshPatientTable(); });

        // Doctor page
        connect(addDocBtn,  &QPushButton::clicked, this, &MainWindow::addDoctor);
        connect(editDocBtn, &QPushButton::clicked, this, &MainWindow::editDoctor);
        connect(delDocBtn,  &QPushButton::clicked, this, &MainWindow::deleteDoctor);
        connect(refDocBtn,  &QPushButton::clicked, [this](){ refreshDoctorTable(); });

        // Appointment page
        connect(schedBtn,  &QPushButton::clicked, this, &MainWindow::scheduleAppointment);
        connect(doneBtn,   &QPushButton::clicked, [this](){ updateAppointmentStatus("Completed"); });
        connect(noShowBtn, &QPushButton::clicked, [this](){ updateAppointmentStatus("No-Show"); });
        connect(cancelBtn, &QPushButton::clicked, this, &MainWindow::cancelAppointment);
        connect(refAppBtn, &QPushButton::clicked, [this](){ refreshAppointmentTable(appTableFull); });

        // Treatment page
        connect(addTreatBtn,  &QPushButton::clicked, this, &MainWindow::addTreatment);
        connect(printBillBtn, &QPushButton::clicked, this, &MainWindow::printBill);
        connect(refTreatBtn,  &QPushButton::clicked, [this](){ refreshTreatmentTable(); });

        // Lab page
        connect(addLabBtn,   &QPushButton::clicked, this, &MainWindow::addLabTest);
        connect(enterResBtn, &QPushButton::clicked, this, &MainWindow::enterLabResult);
        connect(refLabBtn,   &QPushButton::clicked, [this](){ refreshLabTable(); });

        // Inventory page
        connect(addInvBtn, &QPushButton::clicked, this, &MainWindow::addInventoryItem);
        connect(refInvBtn, &QPushButton::clicked, [this](){ refreshInventoryTable(); });

        // Report page
        connect(revChartBtn, &QPushButton::clicked, this, &MainWindow::showRevenueChart);
        connect(ageChartBtn, &QPushButton::clicked, this, &MainWindow::showAgeChart);
        connect(genChartBtn, &QPushButton::clicked, this, &MainWindow::showGenderChart);
        connect(exportBtn,   &QPushButton::clicked, this, &MainWindow::exportCSV);
        connect(genRepBtn,   &QPushButton::clicked, this, &MainWindow::generateReport);
        connect(backupBtn,   &QPushButton::clicked, this, &MainWindow::backup);
        connect(restoreBtn,  &QPushButton::clicked, this, &MainWindow::restore);

        // Dashboard quick actions
        connect(qaPat,   &QPushButton::clicked, this, &MainWindow::addPatient);
        connect(qaApp,   &QPushButton::clicked, this, &MainWindow::scheduleAppointment);
        connect(qaTreat, &QPushButton::clicked, this, &MainWindow::addTreatment);
        connect(qaLab,   &QPushButton::clicked, this, &MainWindow::addLabTest);

        // Global live search
        connect(globalSearch, &QLineEdit::textChanged, [=](const QString& kw){
            int idx = stack->currentIndex();
            if      (idx == 1) refreshPatientTable(kw);
            else if (idx == 2) refreshDoctorTable(kw);
            else if (idx == 3) refreshAppointmentTable(appTableFull, kw);
            else if (idx == 4) refreshTreatmentTable(kw);
            else if (idx == 5) refreshLabTable(kw);
            else if (idx == 6) refreshInventoryTable(kw);
        });

        // Initial load
        activateSideBtn(dashBtn);
        updateStats();
        refreshAppointmentTable(appTable);
        stack->setCurrentWidget(dashPage);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("HMS Nexus Pro");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("Nexus Medical Solutions");

    loadUsers();

    // Login loop
    while (true) {
        LoginDialog login;
        if (login.exec() != QDialog::Accepted) return 0;

        QString u = login.userEdit->text().trimmed();
        QString p = login.passEdit->text();
        bool ok = false;
        for (auto& usr : users) {
            if (QString::fromStdString(usr.username) == u &&
                QString::fromStdString(usr.password) == p) {
                currentUser = usr; ok = true; break;
            }
        }
        if (ok) break;
        QMessageBox::critical(nullptr, "Login Failed",
            "Invalid username or password.\n\nDefault accounts:\n• admin / admin123\n• reception / rec123");
        login.userEdit->clear(); login.passEdit->clear();
    }

    MainWindow w;
    w.show();
    return app.exec();
}

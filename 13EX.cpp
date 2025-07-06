#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iomanip>
#include <cmath>
using namespace std;

/////////////////////////////////////////////////////////
// Abstract Base Class for All Frame Components
/////////////////////////////////////////////////////////

class FrameComponent {
public:
    virtual void inputDimensions() = 0;
    virtual void displayType() const = 0;
    virtual map<string, float> getRequiredSections() const = 0;
    virtual float calculateTotalPrice(const map<string, float>& rates) const = 0;
    virtual float getArea() const = 0;
    virtual ~FrameComponent() = default;
};
/////////////////////////////////////////////////////////
// 🪟 Three and Two (M section ) Panel Window Class (Type 1 and 2)
/////////////////////////////////////////////////////////

class FlexiblePanelWindow : public FrameComponent {
    float height = 0, width = 0;
    int collerType = 1;
    bool includeD29 = true;
    bool useMSeriesSections = false;

public:
    // Constructor lets you control behavior for any type
    FlexiblePanelWindow(bool includeD29 = true, bool useMSeries = false)
        : includeD29(includeD29), useMSeriesSections(useMSeries) {}

    void inputDimensions() override {
        cout << "Enter Collar Type (1 to 14): ";
        while (!(cin >> collerType) || collerType < 1 || collerType > 14) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "❌ Invalid input. Please enter collar type (1 to 14): ";
        }

        cout << "Enter height (in inches): ";
        while (!(cin >> height) || height <= 0) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "❌ Invalid input. Enter height again (in inches): ";
        }

        cout << "Enter width (in inches): ";
        while (!(cin >> width) || width <= 0) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "❌ Invalid input. Enter width again (in inches): ";
        }
    }

    void displayType() const override {
        cout << (includeD29 ? "Three Panel Window\n" : "Two Panel Window\n");
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f);
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float h = height, w = width;

        // Helper function to get correct section name
       auto sec = [&](const string& base) -> string {
        if (useMSeriesSections) {
        if (base == "DC30F") return "M30F";
        if (base == "DC26F") return "M26F";
        if (base == "DC30C") return "M30";
        if (base == "DC26C") return "M26";
    }
    return base;  // always return std::string
    };


        // Collar logic
        switch (collerType) {
            case 1:  sections[sec("DC30F")] = (h * 2) + w + 9; sections[sec("DC26F")] = w + 3;                                       break;
            case 2:  sections[sec("DC30C")] = (h * 2) + w;     sections[sec("DC26C")] = w;                                           break;
            case 3:  sections[sec("DC30C")] = w;               sections[sec("DC30F")] = (h * 2) + 6; sections[sec("DC26F")] = w + 3; break;
            case 4:  sections[sec("DC30C")] = h;               sections[sec("DC30F")] = (h + w) + 6; sections[sec("DC26F")] = w + 3; break;
            case 5:  sections[sec("DC30F")] = (h * 2) + w + 9; sections[sec("DC26C")] = w;                                           break;
            case 6:  sections[sec("DC30C")] = h;               sections[sec("DC30F")] = (h + w) + 6; sections[sec("DC26F")] = w + 3; break;
            case 7:  sections[sec("DC30C")] = h + w;           sections[sec("DC30F")] = h + 3;       sections[sec("DC26F")] = w + 3; break;
            case 8:  sections[sec("DC30C")] = h;               sections[sec("DC30F")] = (h + w) + 6; sections[sec("DC26C")] = w;     break;
            case 9:  sections[sec("DC30C")] = h * 2;           sections[sec("DC30F")] = w + 3;       sections[sec("DC26F")] = w + 3; break;
            case 10: sections[sec("DC30C")] = w;               sections[sec("DC30F")] = (h * 2) + 6; sections[sec("DC26C")] = w;     break;
            case 11: sections[sec("DC30C")] = h * 2;           sections[sec("DC30F")] = w + 3;       sections[sec("DC26C")] = w;     break;
            case 12: sections[sec("DC30C")] = w + h;           sections[sec("DC30F")] = h + 3;       sections[sec("DC26C")] = w;     break;
            case 13: sections[sec("DC30C")] = (h * 2) + w;     sections[sec("DC26F")] = w + 3;                                       break;
            case 14: sections[sec("DC30C")] = h + w;           sections[sec("DC30F")] = h + 3;       sections[sec("DC26F")] = w;     break;
            default: break; // Safe fallback
        }

        // Common members
        sections["M23"] = h * 2;
        sections["M28"] = h * 2;
        sections["M24"] = w * 2;

        if (includeD29)
            sections["D29"] = (h * 2) + w;

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float ft = inches / 12.0f;
            float rounded = roundToMarketFeet(inches);

            float price = rounded * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = "
                 << ft << " ft => rounded ft = " << rounded
                 << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
   float roundToMarketFeet(float inches) const {
    float ft = inches / 12.0f;
    float decimalPart = ft - static_cast<int>(ft);

    if (decimalPart == 0.0f) {
        return ft;  // Perfect foot, no rounding
    }

    float inch = decimalPart * 12;

    if (inch <= 6.0f)
        return static_cast<int>(ft) + 0.6f;
    else
        return static_cast<int>(ft) + 1.0f;
}

};

/////////////////////////////////////////////////////////
// 🪟 Three and Two Panel Window Class and 3 glass part (Type 1 and 2 in M section)
/////////////////////////////////////////////////////////

class ThreePanel_3glass : public FrameComponent {
    float height = 0, width = 0, netwidth = 0;
    int collerType = 1, net = 2;
    bool includeD29 = true; // 👈 You can toggle this based on window type
    string prefix30 = "DC30", prefix26 = "DC26"; // 👈 Flexible prefix system

public:
    ThreePanel_3glass(bool useD29 = true, string p30 = "DC30", string p26 = "DC26")
        : includeD29(useD29), prefix30(p30), prefix26(p26) {}

    void inputDimensions() override {
    if (includeD29) {
        cout << "Net Types:\n";
        cout << "1 - Single net (auto width)\n";
        cout << "2 - Double net (auto width)\n";
        cout << "3 - Custom single net\n";
        cout << "4 - Custom double net\n";
        cin >> net;

        if (net == 3 || net == 4) {
            do {
                cout << "Enter the width size of net (in inches): ";
                cin >> netwidth;
            } while (netwidth <= 0);
        }
    }

    do {
        cout << "Enter Coller Type (1 to 14): ";
        cin >> collerType;
    } while (collerType < 1 || collerType > 14);

    do {
        cout << "Enter height (in inches): ";
        cin >> height;
    } while (height <= 0);

    do {
        cout << "Enter width (in inches): ";
        cin >> width;
    } while (width <= 0);
}

    void displayType() const override {
        cout << "Three Panel and 3 Glass Part Window\n";
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f);
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float h = height, w = width, n = netwidth;

        // Build section keys based on coller type
        auto sec = [&](string base) -> string {
            return (collerType == 2 || collerType == 5 || collerType == 8 || collerType == 10 || collerType == 11 || collerType == 12)
                   ? prefix30 + "C"
                   : prefix30 + "F";
        };

        switch (collerType) {
            case 1:  sections[prefix30 + "F"] = (h * 2) + w + 9; sections[prefix26 + "F"] = w + 3;                                         break;
            case 2:  sections[prefix30 + "C"] = (h * 2) + w;     sections[prefix26 + "C"] = w;                                             break;
            case 3:  sections[prefix30 + "C"] = w;               sections[prefix30 + "F"] = (h * 2) + 6; sections[prefix26 + "F"] = w + 3; break;
            case 4:  sections[prefix30 + "C"] = h;               sections[prefix30 + "F"] = (h + w) + 6; sections[prefix26 + "F"] = w + 3; break;
            case 5:  sections[prefix30 + "F"] = (h * 2) + w + 9; sections[prefix26 + "C"] = w;                                             break;
            case 6:  sections[prefix30 + "C"] = h;               sections[prefix30 + "F"] = (h + w) + 6; sections[prefix26 + "F"] = w + 3; break;
            case 7:  sections[prefix30 + "C"] = h + w;           sections[prefix30 + "F"] = h + 3;       sections[prefix26 + "F"] = w + 3; break;
            case 8:  sections[prefix30 + "C"] = h;               sections[prefix30 + "F"] = (h + w) + 6; sections[prefix26 + "C"] = w; break;
            case 9:  sections[prefix30 + "C"] = h * 2;           sections[prefix30 + "F"] = w + 3;       sections[prefix26 + "F"] = w + 3; break;
            case 10: sections[prefix30 + "C"] = w;               sections[prefix30 + "F"] = (h * 2) + 6; sections[prefix26 + "C"] = w; break;
            case 11: sections[prefix30 + "C"] = h * 2;           sections[prefix30 + "F"] = w + 3;       sections[prefix26 + "C"] = w; break;
            case 12: sections[prefix30 + "C"] = w + h;           sections[prefix30 + "F"] = h + 3;       sections[prefix26 + "C"] = w; break;
            case 13: sections[prefix30 + "C"] = (h * 2) + w;     sections[prefix26 + "F"] = w + 3;                                     break;
            case 14: sections[prefix30 + "C"] = h + w;           sections[prefix30 + "F"] = h + 3;       sections[prefix26 + "F"] = w; break;
        }

        // Common sections
        sections["M23"] = h * 2;
        sections["M28"] = h * 4;
        sections["M24"] = w * 2;

        // Optional D29 section
        if (includeD29) {
            switch (net) {
                case 1: sections["D29"] = (h + (w / 3)) * 2; break;
                case 2: sections["D29"] = (h * 4) + w;       break;
                case 3: sections["D29"] = (h * 2) + (n * 2); break;
                case 4: sections["D29"] = (h * 4) + (n * 4); break;
            }
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float feet = inches / 12.0f;
            float roundedFeet = roundToMarketFeet(inches);

            float price = roundedFeet * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = " << feet << " ft => rounded ft = "
                 << roundedFeet << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inch = (ft - whole) * 12;

        if (inch <= 6.0f)
            return whole + 0.6f;
        else
            return whole + 1.0f;
    }
};

/////////////////////////////////////////////////////////
// 🪟 Fix Window With or Without Tee
/////////////////////////////////////////////////////////

class FixWindow : public FrameComponent {
    float height = 0, width = 0, tee = 0;
    int collerType = 1;
    bool includeTee = false;

public:
    void inputDimensions() override {
        char tOption;
        cout << "Include Tee? (y/n): ";
        cin >> tOption;
        includeTee = (tOption == 'y' || tOption == 'Y');

        do {
            cout << "Enter Coller Type (1 to 14): ";
            cin >> collerType;
        } while (collerType < 1 || collerType > 14);

        do {
            cout << "Enter height (in inches): ";
            cin >> height;
        } while (height <= 0);

        do {
            cout << "Enter width (in inches): ";
            cin >> width;
        } while (width <= 0);

        if (includeTee) {
            do {
                cout << "Enter Tee size (in inches): ";
                cin >> tee;
            } while (tee <= 0);
        }
    }

    void displayType() const override {
        cout << "Fixed Window";
        if (includeTee) cout << " with Tee";
        cout << "\n";
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f);
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float h = height, w = width, t = tee;

        // Apply formulas based on coller type
        switch (collerType) {
            case 1:  sections["D54F"] = (h + w) * 2 + 12;                        break;
            case 2:  sections["D54A"] = (h + w) * 2;                              break;
            case 3:  sections["D54F"] = (h * 2) + w + 9;  sections["D54A"] = w;    break;
            case 4:  sections["D54F"] = (w * 2) + h + 9;  sections["D54A"] = h;    break;
            case 5:  sections["D54F"] = (h * 2) + w + 9;  sections["D54A"] = w;    break;
            case 6:  sections["D54F"] = (w * 2) + h + 9;  sections["D54A"] = h;    break;
            case 7:  sections["D54F"] = h + w + 6;        sections["D54A"] = h + w; break;
            case 8:  sections["D54F"] = h + w + 6;        sections["D54A"] = h + w; break;
            case 9:  sections["D54F"] = (h * 2) + 6;      sections["D54A"] = w * 2; break;
            case 10: sections["D54F"] = (w * 2) + 6;      sections["D54A"] = h * 2; break;
            case 11: sections["D54F"] = w + 3;            sections["D54A"] = (h * 2) + w; break;
            case 12: sections["D54F"] = h * 3;            sections["D54A"] = (w * 2) + h; break;
            case 13: sections["D54F"] = w + 3;            sections["D54A"] = (h * 2) + w; break;
            case 14: sections["D54F"] = h + 3;            sections["D54A"] = (w * 2) + h; break;
        }

        if (includeTee) {
            sections["D52"] = t;
            sections["D41"] = ((h + w) * 2) + (t * 2);
        } else {
            sections["D41"] = (h + w) * 2;
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float feet = inches / 12.0f;
            float roundedFeet = roundToMarketFeet(inches);

            float price = 0;
            try {
                price = roundedFeet * rates.at(s.first);
            } catch (const std::out_of_range&) {
                cout << "⚠️ Rate missing for section: " << s.first << "\n";
                continue;
            }

            total += price;
            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = "
                 << feet << " ft => rounded = " << roundedFeet
                 << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inch = (ft - whole) * 12;
        return (inch <= 6.0f) ? (whole + 0.6f) : (whole + 1.0f);
    }
};

/////////////////////////////////////////////////////////
// 🪟 Random design Fix Window With or Without Tee
/////////////////////////////////////////////////////////

class RandomFixWindow : public FrameComponent {
    float length = 0, tee = 0;
    bool includeTee = false;

public:
    void inputDimensions() override {
        char tOption;
        cout << "Include Tee? (y/n): ";
        cin >> tOption;
        includeTee = (tOption == 'y' || tOption == 'Y');

        do {
            cout << "Enter total length (in inches): ";
            cin >> length;
            if (length <= 0) cout << "❌ Length must be greater than 0.\n";
        } while (length <= 0);

        if (includeTee) {
            do {
                cout << "Enter Tee size (in inches): ";
                cin >> tee;
                if (tee <= 0) cout << "❌ Tee size must be greater than 0.\n";
            } while (tee <= 0);
        }
    }

    void displayType() const override {
        cout << "Random Design Fixed Window";
        if (includeTee) cout << " (with Tee)";
        cout << "\n";
    }

    float getArea() const override {
        return 0.0f;  // Area doesn't apply for random fixed windows
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float l = length, t = tee;

        sections["D54"] = l;
        if (includeTee) {
            sections["D52"] = t;
            sections["D41"] = l + (t * 2);
        } else {
            sections["D41"] = l;
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float feet = inches / 12.0f;
            float roundedFeet = roundToMarketFeet(inches);

            float price = 0;
            try {
                price = roundedFeet * rates.at(s.first);
            } catch (const std::out_of_range&) {
                cout << "⚠️ Rate missing for section: " << s.first << "\n";
                continue;
            }

            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = "
                 << feet << " ft => rounded = " << roundedFeet
                 << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inch = (ft - whole) * 12;
        return (inch <= 6.0f) ? (whole + 0.6f) : (whole + 1.0f);
    }
};

/////////////////////////////////////////////////////////
// 🪟 Openable window with or without net
/////////////////////////////////////////////////////////

class OpenableWindow : public FrameComponent {
    float height = 0, width = 0;
    int collerType = 1;
    bool hasNet = false;

public:
    void inputDimensions() override {
        char netChoice;
        cout << "Do you want net in this Openable Window? (y/n): ";
        cin >> netChoice;
        hasNet = (netChoice == 'y' || netChoice == 'Y');

        do {
            cout << "Enter Coller Type (1 to 14): ";
            cin >> collerType;
        } while (collerType < 1 || collerType > 14);

        do {
            cout << "Enter height (in inches): ";
            cin >> height;
        } while (height <= 0);

        do {
            cout << "Enter width (in inches): ";
            cin >> width;
        } while (width <= 0);
    }

    void displayType() const override {
        cout << "Openable Window (" << (hasNet ? "with Net" : "without Net") << ")\n";
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f);
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float h = height, w = width;

        // Coller based section formula
        switch (collerType) {
            case 1:  sections["D54F"] = (h + w) * 2 + 12; break;
            case 2:  sections["D54A"] = (h + w) * 2; break;
            case 3:  sections["D54F"] = (h * 2) + w + 9; sections["D54A"] = w; break;
            case 4:  sections["D54F"] = (w * 2) + h + 9; sections["D54A"] = h; break;
            case 5:  sections["D54F"] = (h * 2) + w + 9; sections["D54A"] = w; break;
            case 6:  sections["D54F"] = (w * 2) + h + 9; sections["D54A"] = h; break;
            case 7:  sections["D54F"] = h + w + 6; sections["D54A"] = h + w; break;
            case 8:  sections["D54F"] = h + w + 6; sections["D54A"] = h + w; break;
            case 9:  sections["D54F"] = (h * 2) + 6; sections["D54A"] = w * 2; break;
            case 10: sections["D54F"] = (w * 2) + 6; sections["D54A"] = h * 2; break;
            case 11: sections["D54F"] = w + 3; sections["D54A"] = (h * 2) + w; break;
            case 12: sections["D54F"] = h * 3; sections["D54A"] = (w * 2) + h; break;
            case 13: sections["D54F"] = w + 3; sections["D54A"] = (h * 2) + w; break;
            case 14: sections["D54F"] = h + 3; sections["D54A"] = (w * 2) + h; break;
        }

        // Net logic
        sections["D50"] = (h + w) * 2;
        if (hasNet) {
            sections["D29"] = (h + w) * 2;
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            if (!rates.count(s.first)) {
                cerr << "❌ Error: Rate for section " << s.first << " not found!\n";
                continue;
            }

            float inches = s.second;
            float feet = inches / 12.0f;
            float roundedFeet = roundToMarketFeet(inches);
            float price = roundedFeet * rates.at(s.first);

            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " in = " << feet << " ft => rounded = "
                 << roundedFeet << " ft => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inch = (ft - whole) * 12;

        return (inch <= 6.0f) ? (whole + 0.6f) : (whole + 1.0f);
    }
};

/////////////////////////////////////////////////////////
// 🪟 Door/Double Door with Divider and Tee or Not
/////////////////////////////////////////////////////////

class DoorWindow : public FrameComponent {
    float height = 0, width = 0, tee = 0;
    int collerType = 1;
    bool includeD46 = false;
    bool includeTee = false;
    bool isDouble = false; // 👈 New flag

public:
    DoorWindow(bool doubleDoor = false) : isDouble(doubleDoor) {}

    void inputDimensions() override {
        char ans;
        cout << "Do you want to include bottom section D46? (y/n): ";
        cin >> ans;
        includeD46 = (ans == 'y' || ans == 'Y');

        cout << "Do you want to include a Tee (divider)? (y/n): ";
        cin >> ans;
        includeTee = (ans == 'y' || ans == 'Y');

        if (includeTee) {
            do {
                cout << "Enter Tee length (in inches): ";
                cin >> tee;
            } while (tee <= 0);
        }

        do {
            cout << "Enter collar type (1–8): ";
            cin >> collerType;
        } while (collerType < 1 || collerType > 8);

        do {
            cout << "Enter height (in inches): ";
            cin >> height;
        } while (height <= 0);

        do {
            cout << "Enter width (in inches): ";
            cin >> width;
        } while (width <= 0);
    }

    void displayType() const override {
        cout << (isDouble ? "🚪 Double Door\n" : "🚪 Single Door\n");
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f);
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float h = height, w = width, t = tee;

        // D54F and D54A
        switch (collerType) {
            case 1: sections["D54F"] = (h * 2) + w + 9;                        break;
            case 2: sections["D54A"] = (h * 2) + w;                            break;
            case 3: sections["D54F"] = h + w + 6;   sections["D54A"] = h;     break;
            case 4: sections["D54F"] = (h * 2) + 6; sections["D54A"] = w;     break;
            case 5: sections["D54F"] = h + w + 6;   sections["D54A"] = h;     break;
            case 6: sections["D54F"] = h + 3;       sections["D54A"] = h + w; break;
            case 7: sections["D54F"] = h + 3;       sections["D54A"] = h + w; break;
            case 8: sections["D54F"] = w + 3;       sections["D54A"] = h * 2; break;
        }

        // D50 and D46 logic updated for double door
        if (includeD46) {
            sections["D46"] = w;
            sections["D50"] = isDouble ? (h * 4) + w : (h * 2) + w;
        } else {
            sections["D50"] = isDouble ? (h * 4) + (w * 2) : (h + w) * 2;
        }

        // Optional Tee
        if (includeTee) {
            sections["D52"] = t;
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float feet = inches / 12.0f;
            float roundedFeet = roundToMarketFeet(inches);
            float price = roundedFeet * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = " << feet
                 << " ft => rounded ft = " << roundedFeet
                 << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inch = (ft - whole) * 12;
        return (inch <= 6.0f) ? whole + 0.6f : whole + 1.0f;
    }
};

/////////////////////////////////////////////////////////
// 🪟 Qadial Arch Window
/////////////////////////////////////////////////////////

class QadialArchWindow : public FrameComponent {
    float height = 0, width = 0, T = 0;
    int collerType = 1;
    bool includeTee = false;

public:
    void inputDimensions() override {
        do {
            cout << "Enter Coller Type (1 to 8): ";
            cin >> collerType;
        } while (collerType < 1 || collerType > 8);

        do {
            cout << "Enter height (in inches): ";
            cin >> height;
        } while (height <= 0);

        do {
            cout << "Enter width (in inches): ";
            cin >> width;
        } while (width <= 0);

        cout << "Do you want to include Tee/Divider (1 = Yes, 0 = No)? ";
        cin >> includeTee;
        if (includeTee) {
            do {
                cout << "Enter Tee (Divider) length in inches: ";
                cin >> T;
            } while (T <= 0);
        }
    }

    void displayType() const override {
        cout << "Qadial Arch (4-corner) Window\n";
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f); // Area in sq ft
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float h = height, w = width;

        switch (collerType) {
            case 1: sections["D50F"] = (h * 2) + w + 9; sections["D50A"] = w; break;
            case 2: sections["D50A"] = (h + w) * 2; break;
            case 3: sections["D50F"] = w + h + 6; sections["D50A"] = w + h; break;
            case 4: sections["D50F"] = (h * 2) + 6; sections["D50A"] = w * 2; break;
            case 5: sections["D50F"] = w + h + 6; sections["D50A"] = w; break;
            case 6: sections["D50F"] = w + 3; sections["D50A"] = (h * 3) + w; break;
            case 7: sections["D50F"] = h + 3; sections["D50A"] = (w * 2) + h; break;
            case 8: sections["D50F"] = h + 3; sections["D50A"] = (w * 2) + h; break;
        }

        // Handle optional Tee
        if (includeTee) {
            sections["D40"] = T;
            sections["D41"] = ((h + w) * 2) + (T * 2);
        } else {
            sections["D41"] = (h + w) * 2;
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float roundedFeet = roundToMarketFeet(inches);
            float price = roundedFeet * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = " << (inches / 12.0f)
                 << " ft => rounded = " << roundedFeet << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inchPart = (ft - whole) * 12;

        return inchPart <= 6.0f ? (whole + 0.6f) : (whole + 1.0f);
    }
};


/////////////////////////////////////////////////////////
// 🪟 Round Top Arch Window
/////////////////////////////////////////////////////////

class RoundArchWindow : public FrameComponent {
    float width = 0, arch = 0, T = 0, height = 0;
    bool hasColler = false;
    bool includeTee = false;

public:
    void inputDimensions() override {
        cout << "Does the window have a coller? (1 = Yes, 0 = No): ";
        cin >> hasColler;

        do {
            cout << "Enter Arch length (in inches): ";
            cin >> arch;
        } while (arch <= 0);

        do {
            cout << "Enter window width (in inches): ";
            cin >> width;
        } while (width <= 0);

        do {
            cout << "Enter approximate height for area calculation (used only for glass/labor): ";
            cin >> height;
        } while (height <= 0);

        cout << "Do you want to add a Tee/Divider? (1 = Yes, 0 = No): ";
        cin >> includeTee;

        if (includeTee) {
            do {
                cout << "Enter Tee (Divider) size (in inches): ";
                cin >> T;
            } while (T <= 0);
        }
    }

    void displayType() const override {
        cout << "Round Arch Window\n";
    }

    float getArea() const override {
        return (height / 12.0f) * (width / 12.0f);  // 👈 used only for square footage
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        float w = width;

        if (hasColler) {
            sections["D50F"] = arch + 12;
            sections["D50A"] = w;
        } else {
            sections["D50A"] = arch + w + 12;
        }

        if (includeTee) {
            sections["D40"] = T;
            sections["D41"] = arch + w + 12 + (T * 2);
        } else {
            sections["D41"] = arch + w + 12;
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float roundedFeet = roundToMarketFeet(inches);
            float price = roundedFeet * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = " << (inches / 12.0f)
                 << " ft => rounded = " << roundedFeet << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inchPart = (ft - whole) * 12;

        return inchPart <= 6.0f ? (whole + 0.6f) : (whole + 1.0f);
    }
};

/////////////////////////////////////////////////////////
// 🪟 Fix Coner Window
/////////////////////////////////////////////////////////

class FixCornerWindow : public FrameComponent {
    float h = 0, wl = 0, wr = 0, T = 0;
    bool coller = false;
    bool includeTee = false;

public:
    void inputDimensions() override {
        cout << "Does this corner window have a coller? (1 = Yes, 0 = No): ";
        cin >> coller;

        do {
            cout << "Enter Height (in inches): ";
            cin >> h;
        } while (h <= 0);

        do {
            cout << "Enter Left Width (in inches): ";
            cin >> wl;
        } while (wl <= 0);

        do {
            cout << "Enter Right Width (in inches): ";
            cin >> wr;
        } while (wr <= 0);

        cout << "Do you want to add a Tee/Divider? (1 = Yes, 0 = No): ";
        cin >> includeTee;

        if (includeTee) {
            do {
                cout << "Enter Tee size (in inches): ";
                cin >> T;
            } while (T <= 0);
        }
    }

    void displayType() const override {
        cout << "Fix Corner Window\n";
    }

    float getArea() const override {
        float totalWidth = wl + wr;
        return (h / 12.0f) * (totalWidth / 12.0f);  // Just for glass/labor estimation
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;

        if (coller)
            sections["D54F"] = (h * 2) + ((wl + wr) * 2) + 18;
        else
            sections["D54A"] = (h * 2) + ((wl + wr) * 2);

        if (includeTee) {
            sections["D40"] = T;
            sections["D41"] = (h * 2) + ((wl + wr) * 2) + (T * 2);
        } else {
            sections["D41"] = (h * 2) + ((wl + wr) * 2);
        }

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float roundedFeet = roundToMarketFeet(inches);
            float price = roundedFeet * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " inches = "
                 << (inches / 12.0f) << " ft => rounded = "
                 << roundedFeet << " => Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inchPart = (ft - whole) * 12;
        return (inchPart <= 6.0f) ? (whole + 0.6f) : (whole + 1.0f);
    }
};

/////////////////////////////////////////////////////////
// 🪟 Slide Corner Window
/////////////////////////////////////////////////////////

class SlideCornerWindow : public FrameComponent {
    float height = 0, wl = 0, wr = 0, d29Width = 0;
    int subtype = 1;
    bool includeD29 = true;
    bool useColler = true;
    string sec30, sec26;

public:
    SlideCornerWindow(bool d29, string s30, string s26)
        : includeD29(d29), sec30(s30), sec26(s26) {}

    void inputDimensions() override {
        cout << "\n--- Slide Corner Window ---\n";
        cout << "Select subtype:\n";
        cout << "1. Left Side Fix Corner\n";
        cout << "2. Right Side Fix Corner\n";
        cout << "3. Center Fix Corner\n";
        cout << "4. Center Fix (Far) Corner\n";
        cout << "5. Center Slide Corner\n";
        cin >> subtype;

        cout << "Do you want to use coller style? (1 = Yes, 0 = No): ";
        cin >> useColler;

        do {
            cout << "Enter Height (in inches): ";
            cin >> height;
        } while (height <= 0);

        do {
            cout << "Enter Width (Left side) (in inches): ";
            cin >> wl;
        } while (wl <= 0);

        do {
            cout << "Enter Width (Right side) (in inches): ";
            cin >> wr;
        } while (wr <= 0);

        if (subtype == 4 && includeD29) {
            do {
                cout << "Enter custom width for D29 (in inches): ";
                cin >> d29Width;
            } while (d29Width <= 0);
        }
    }

    void displayType() const override {
        cout << "Slide Corner Window - ";
        switch (subtype) {
            case 1: cout << "Left Side Fix\n"; break;
            case 2: cout << "Right Side Fix\n"; break;
            case 3: cout << "Center Fix\n"; break;
            case 4: cout << "Center Fix (Far)\n"; break;
            case 5: cout << "Center Slide\n"; break;
        }
    }

    float getArea() const override {
        return (height / 12.0f) * ((wl + wr) / 12.0f);
    }

    map<string, float> getRequiredSections() const override {
        map<string, float> sections;
        string suffix30 = useColler ? "F" : "A";
        string suffix26 = useColler ? "F" : "A";
        string sec30Full = sec30 + suffix30;
        string sec26Full = sec26 + suffix26;

        float c30Add = useColler ? 12.0f : 0.0f;
        float c26Add = useColler ? 6.0f : 0.0f;

        if (subtype == 1) {  // Left Side Fix
            sections[sec30Full] = ((height + wl + wr) * 2) + c30Add;
            sections[sec26Full] = wl + wr + c26Add;
            if (includeD29) sections["D29"] = (height * 2) + wr;
        }
        else if (subtype == 2) {  // Right Side Fix
            sections[sec30Full] = ((height + wl + wr) * 2) + c30Add;
            sections[sec26Full] = wl + wr + c26Add;
            if (includeD29) sections["D29"] = (height * 2) + wl;
        }
        else if (subtype == 3) {  // Center Fix
            sections[sec30Full] = (height * 2) + wl + wr + c30Add;
            sections[sec26Full] = wl + wr + c26Add;
            if (includeD29) sections["D29"] = (height * 4) + wl + wr;
        }
        else if (subtype == 4) {  // Center Fix (Far)
            sections[sec30Full] = (height * 2) + wl + wr + c30Add;
            sections[sec26Full] = wl + wr + c26Add;
            if (includeD29) sections["D29"] = (height * 4) + (d29Width * 4);
        }
        else if (subtype == 5) {  // Center Slide
            sections[sec30Full] = (height * 2) + wl + wr + c30Add;
            sections[sec26Full] = wl + wr + c26Add;
            if (includeD29) sections["D29"] = (height * 4) + wl + wr;
        }

        sections["M23"] = height * ((subtype == 5) ? 4 : 2);
        sections["M28"] = height * ((subtype == 1 || subtype == 2) ? 2 : 4);
        sections["M24"] = (wl + wr) * 2;

        return sections;
    }

    float calculateTotalPrice(const map<string, float>& rates) const override {
        auto sections = getRequiredSections();
        float total = 0;

        for (const auto& s : sections) {
            float inches = s.second;
            float feet = inches / 12.0f;
            float roundedFeet = roundToMarketFeet(inches);
            float price = roundedFeet * rates.at(s.first);
            total += price;

            cout << fixed << setprecision(2);
            cout << s.first << ": " << inches << " in = " << feet
                 << " ft → rounded = " << roundedFeet << " → Rs. " << price << "\n";
        }

        return total;
    }

private:
    float roundToMarketFeet(float inches) const {
        float ft = inches / 12.0f;
        int whole = static_cast<int>(floor(ft));
        float inch = (ft - whole) * 12;

        return (inch <= 6.0f) ? (whole + 0.6f) : (whole + 1.0f);
    }
};

/////////////////////////////////////////////////////////
// 🧠 Final Cost Summary Class
/////////////////////////////////////////////////////////

class FinalCostCalculator {
public:
    static void calculate(float aluminiumTotal, float totalSqFt, int windowCount) {
        float glassRate, laborRate, hardwareRate, discountPercent;

        cout << "\nEnter glass rate (Rs./sqft): ";
        cin >> glassRate;
        cout << "Enter labor rate (Rs./sqft): ";
        cin >> laborRate;
        cout << "Enter hardware cost per window: ";
        cin >> hardwareRate;
        cout << "Enter discount (%): ";
        cin >> discountPercent;

        float glass = glassRate * totalSqFt;
        float labor = laborRate * totalSqFt;
        float hardware = hardwareRate * windowCount;

        float discount = (discountPercent / 100.0f) * aluminiumTotal;
        float discountedAluminium = aluminiumTotal - discount;
        float net = discountedAluminium + glass + labor + hardware;

        cout << "\n--- Final Summary ---\n";
        cout << "Aluminium (before discount): Rs. " << aluminiumTotal << "\n";
        cout << "Discount on Aluminium (" << discountPercent << "%): Rs. " << discount << "\n";
        cout << "Aluminium (after discount): Rs. " << discountedAluminium << "\n";
        cout << "Glass: Rs. " << glass << "\n";
        cout << "Labor: Rs. " << labor << "\n";
        cout << "Hardware: Rs. " << hardware << "\n";
        cout << "Net Total: Rs. " << net << "\n";

    }
};

/////////////////////////////////////////////////////////
// 🏗️ Factory Function to Add More Types Later Easily
/////////////////////////////////////////////////////////

unique_ptr<FrameComponent> createComponent(int type) {
    if (type == 1) return make_unique<FlexiblePanelWindow>(true, false);  // Three Panel
    if (type == 2) return make_unique<FlexiblePanelWindow>(false, true);  // Two Panel
    if (type == 3) return make_unique<ThreePanel_3glass>(true, "DC30", "DC26");  // with D29
    if (type == 4) return make_unique<ThreePanel_3glass>(false, "M30", "M26");   // Without D29
    if (type == 5) return make_unique<FixWindow>();                           // Fix window 
    if (type == 6) return make_unique<RandomFixWindow>();                     // Random Fix
    if (type == 7) return make_unique<OpenableWindow>();                      // Openable
    if (type == 8) return make_unique<DoorWindow>(false);                     // Single Door
    if (type == 9) return make_unique<DoorWindow>(true);                      // Double Door ✅
    if (type == 10) return make_unique<QadialArchWindow>();                   // Qadial Arch
    if (type == 11) return make_unique<RoundArchWindow>();                    // Curve Arch
    if (type == 12) return make_unique<FixCornerWindow>();                    // Fix Corner
    if (type == 13) return make_unique<SlideCornerWindow>(true, "DC30", "DC26");  // Corner Window
    if (type == 14) return make_unique<SlideCornerWindow>(false, "M30", "M26");  // Corner M section
                                                                                
    return nullptr;
}

/////////////////////////////////////////////////////////
// 🧵 Main Application Logic
/////////////////////////////////////////////////////////

int main() {
    vector<unique_ptr<FrameComponent>> windows;
    float totalAluminium = 0, totalSqFt = 0;

    while (true) {
        int choice;
        cout << "\n=== Main Menu ===\n";
        cout << "1. Estimate Price\n";
        cout << "2. Length Derivation (Coming Soon)\n";
        cout << "3. Final Summary\n";
        cout << "0. Exit\n";
        cout << "Select Option: ";
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear(); cin.ignore(10000, '\n'); continue;
        }

        if (choice == 0) {
            break;

        } else if (choice == 1) {
            cout << "\n--- Window Types ---\n";
            cout << "1. Three Panel Window\n";
            cout << "2. Two Panel Window (M section)\n";
            cout << "3. Three Panel Window (3 glass part)\n";
            cout << "4. Two Panel Window (3 glass part & M section)\n";
            cout << "5. Fixed Window\n";
            cout << "6. Random Design Fixed Window\n";
            cout << "7. openable Window\n";
            cout << "8. Single Door\n";
            cout << "9. Double Door\n";
            cout << "10. Qadial Top Arch\n";
            cout << "11. Round Top Arch\n";
            cout << "12. Fix corner Window\n";
            cout << "13. Slide Corner Window\n";
            cout << "14. Slide Corner Window (M section)\n";

            // ✅ Add future window types here

            int winType, qty;
            cout << "Enter window type number: ";
            if (!(cin >> winType)) {
                cout << "Invalid input.\n"; cin.clear(); cin.ignore(10000, '\n'); continue;
            }

            auto component = createComponent(winType);
            if (!component) {
                cout << "Invalid window type selected.\n"; continue;
            }

            cout << "Enter quantity: ";
            if (!(cin >> qty) || qty <= 0) {
                cout << "Invalid quantity.\n"; cin.clear(); cin.ignore(10000, '\n'); continue;
            }

            map<string, float> rates;
            set<string> neededSections;
            vector<unique_ptr<FrameComponent>> batch;

            for (int i = 0; i < qty; ++i) {
                cout << "\n--- Enter details for Window " << (i + 1) << " ---\n";
                auto win = createComponent(winType);
                win->inputDimensions();
                auto sec = win->getRequiredSections();
                for (auto& s : sec) neededSections.insert(s.first);
                totalSqFt += win->getArea();
                batch.push_back(move(win));
            }

            for (const auto& secName : neededSections) {
                cout << "Enter rate for " << secName << " (Rs./ft): ";
                float rate;
                while (!(cin >> rate) || rate < 0) {
                    cout << "Invalid rate. Enter again: ";
                    cin.clear(); cin.ignore(10000, '\n');
                }
                rates[secName] = rate;
            }

            for (auto& win : batch) {
                win->displayType();
                float price = win->calculateTotalPrice(rates);
                totalAluminium += price;
                windows.push_back(move(win));
            }

            cout << "\n✅ Added " << qty << " window(s) successfully.\n";

        } else if (choice == 2) {
            cout << "🚧 Length Derivation Feature Coming Soon!\n";

        } else if (choice == 3) {
            if (windows.empty()) {
                cout << "⚠️ No windows added yet to calculate summary.\n";
            } else {
                FinalCostCalculator::calculate(totalAluminium, totalSqFt, windows.size());
            }
        } else {
            cout << "❌ Invalid option. Try again.\n";
        }
    }

    cout << "\n✅ Program Ended Successfully.\n";
    return 0;
}
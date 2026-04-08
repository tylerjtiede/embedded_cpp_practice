#include <iostream>
#include <string>
#include <memory>
#include <vector>

class Sensor {
private:
    float value;
    bool active;

public:
    Sensor(float initialValue) : value(initialValue), active(false) {
        std::cout << "Sensor initialized" << std::endl;
    }

    ~Sensor() {
        std::cout << "Sensor destroyed" << std::endl;
    }

    virtual void describe() const {
        std::cout << "Sensor with value: " << value << std::endl;
    }

    void activate() { active = true; }
    void deactivate() { active = false; }
    void setValue(float v) {value = v; }
    float getValue() const { return value; } // will not modify values
    bool isActive() const { return active; } // will not modify values
};


class TemperatureSensor : public Sensor {
private:
    std::string unit;  // "C" or "F"

public:
    TemperatureSensor(float initialValue, std::string u) : Sensor(initialValue), unit(u) {
        std::cout << "TemperatureSensor initialized" << std::endl;
    }

    ~TemperatureSensor() {
        std::cout << "TemperatureSensor destroyed" << std::endl;
    }

    void describe() const override {
        std::cout << "TemperatureSensor: " << getValue() << unit << std::endl;
    }

    std::string getUnit() const { return unit; }
};

class SensorArray {
private:
    std::vector<std::unique_ptr<Sensor>> sensors;

public:
    void addSensor(float initialValue) {
        sensors.push_back(std::make_unique<Sensor>(initialValue));
    }

    void printAll() const {
        for (const auto& s : sensors) {
            std::cout << s->getValue() << std::endl;
        }
    }
};

void printSensor(const Sensor& s) {
    std::cout << "value: " << s.getValue() << std::endl;
    std::cout << "active: " << s.isActive() << std::endl;
    s.describe();
}

int main() {
    SensorArray sa;  // rename to sa to avoid any confusion
    sa.addSensor(10.5);
    sa.addSensor(4.2);
    sa.addSensor(0.0);
    std::cout << "printing now" << std::endl;
    sa.printAll();
    std::cout << "done" << std::endl;
}
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>

// Simulate sensor readings (normally from GPIO)
struct SensorData {
    double temperature;      // °C
    double humidity;         // %
    double soil_moisture;    // % (0 = dry, 100 = wet)
    bool pump_status;        // on/off
    int servo_angle;         // degrees (0-180)
    std::string display_message;
};

// Read temperature sensor (simulated DHT22)
double readTemperature() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(18.0, 35.0); // 18-35°C range
    return dist(gen);
}

// Read humidity sensor (simulated DHT22)
double readHumidity() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(30.0, 80.0); // 30-80% range
    return dist(gen);
}

// Read soil moisture sensor (simulated)
double readSoilMoisture() {
    static double moisture = 45.0; // Start at 45%
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> drift(-2.0, 1.0); // Slowly decreases over time
    
    moisture += drift(gen);
    if (moisture < 0) moisture = 0;
    if (moisture > 100) moisture = 100;
    
    return moisture;
}

// Control water pump (simulated GPIO)
void controlPump(bool turnOn) {
    if (turnOn) {
        std::cerr << "[HARDWARE] Pump ON - Watering plant..." << std::endl;
    } else {
        std::cerr << "[HARDWARE] Pump OFF" << std::endl;
    }
}

// Control servo motor (simulated GPIO)
void controlServo(int angle) {
    std::cerr << "[HARDWARE] Servo moved to " << angle << " degrees" << std::endl;
}

// Update LCD display (simulated I2C)
void updateDisplay(const std::string& message) {
    std::cerr << "[DISPLAY] " << message << std::endl;
}

// Calculate irrigation needs
struct IrrigationDecision {
    bool should_water;
    int watering_duration; // seconds
    std::string reason;
    double comfort_score;  // 0-100
};

IrrigationDecision calculateIrrigation(const SensorData& sensors) {
    IrrigationDecision decision;
    
    // Algorithm: Water if soil moisture < 30% OR temperature > 30°C and moisture < 50%
    if (sensors.soil_moisture < 30.0) {
        decision.should_water = true;
        decision.watering_duration = 10; // 10 seconds
        decision.reason = "Soil too dry";
    } else if (sensors.temperature > 30.0 && sensors.soil_moisture < 50.0) {
        decision.should_water = true;
        decision.watering_duration = 5; // 5 seconds
        decision.reason = "Hot weather, preventive watering";
    } else {
        decision.should_water = false;
        decision.watering_duration = 0;
        decision.reason = "Plant is healthy";
    }
    
    // Calculate plant comfort score
    double temp_score = std::max(0.0, 100.0 - std::abs(sensors.temperature - 25.0) * 3.0);
    double moisture_score = std::min(100.0, sensors.soil_moisture * 2.0);
    double humidity_score = std::max(0.0, 100.0 - std::abs(sensors.humidity - 60.0) * 1.5);
    
    decision.comfort_score = (temp_score + moisture_score + humidity_score) / 3.0;
    
    return decision;
}

int main(int argc, char* argv[]) {
    try {
        // Read all sensors
        SensorData sensors;
        sensors.temperature = readTemperature();
        sensors.humidity = readHumidity();
        sensors.soil_moisture = readSoilMoisture();
        
        // Calculate what to do
        IrrigationDecision decision = calculateIrrigation(sensors);
        
        // Control hardware based on decision
        sensors.pump_status = decision.should_water;
        controlPump(sensors.pump_status);
        
        // Move servo to different positions based on moisture level
        if (sensors.soil_moisture < 20) {
            sensors.servo_angle = 0;    // Point to "DRY" indicator
        } else if (sensors.soil_moisture < 60) {
            sensors.servo_angle = 90;   // Point to "OK" indicator  
        } else {
            sensors.servo_angle = 180;  // Point to "WET" indicator
        }
        controlServo(sensors.servo_angle);
        
        // Update display
        std::string display_msg = "T:" + std::to_string((int)sensors.temperature) + 
                                 "C M:" + std::to_string((int)sensors.soil_moisture) + "%";
        sensors.display_message = display_msg;
        updateDisplay(display_msg);
        
        // If watering, simulate the watering process
        if (decision.should_water) {
            std::cerr << "[SYSTEM] Watering for " << decision.watering_duration << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate watering delay
            
            // Increase soil moisture after watering
            sensors.soil_moisture = std::min(100.0, sensors.soil_moisture + 25.0);
            controlPump(false); // Turn off pump
            sensors.pump_status = false;
        }
        
        // Output JSON for Node.js
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "{"
                  << "\"sensors\":{"
                  << "\"temperature\":" << sensors.temperature << ","
                  << "\"humidity\":" << sensors.humidity << ","
                  << "\"soil_moisture\":" << sensors.soil_moisture << ","
                  << "\"pump_status\":" << (sensors.pump_status ? "true" : "false") << ","
                  << "\"servo_angle\":" << sensors.servo_angle << ","
                  << "\"display_message\":\"" << sensors.display_message << "\""
                  << "},"
                  << "\"irrigation\":{"
                  << "\"should_water\":" << (decision.should_water ? "true" : "false") << ","
                  << "\"duration\":" << decision.watering_duration << ","
                  << "\"reason\":\"" << decision.reason << "\","
                  << "\"comfort_score\":" << decision.comfort_score
                  << "},"
                  << "\"timestamp\":\"" << std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count() << "\""
                  << "}" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
}
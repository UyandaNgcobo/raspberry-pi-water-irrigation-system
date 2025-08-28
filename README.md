# 🌱 Smart Water Irrigation System

A complete IoT irrigation system combining C++ hardware control, Node.js API, and web dashboard.

## 🏗️ Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Raspberry Pi  │    │    Node.js      │    │   Web Dashboard │
│                 │    │                 │    │                 │
│ • Temperature   │    │ • API Server    │    │ • Live Sensors  │
│ • Humidity      │◄──►│ • C++ Bridge    │◄──►│ • Charts        │
│ • Soil Moisture │    │ • Data Storage  │    │ • Manual Control│
│ • Water Pump    │    │ • CORS & Auth   │    │ • System Logs   │
│ • Servo Motor   │    │                 │    │                 │
│ • OLED Display  │    │                 │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 🚀 Quick Start

### 1. Clone & Setup
```bash
git clone <your-repo-url>
cd water-irrigation-system
```

### 2. Build C++ Backend
```bash
cd raspberry-pi
make
```

### 3. Start Node.js API
```bash
cd node-server
npm install
npm start
```

### 4. Open Dashboard
Navigate to `http://localhost:3000` in your browser

## 📁 Project Structure

```
raspberry-pi-water-irrigation-system/
├── raspberry-pi/           # C++ Hardware Control
│   ├── main.cpp           # Main program
│   ├── Makefile           # Build configuration  
│   └── build/             # Compiled executables
├── node-server/           # Node.js API
│   ├── server.js          # API server
│   └── package.json       # Dependencies
├── frontend/              # Web Dashboard
│   ├── index.html         # Main dashboard
│   ├── style.css          # Styling
│   └── script.js          # Frontend logic
└── README.md              # This file
```

## 🔧 Hardware Simulation

Since this is a demo without real hardware, the system simulates:

- **Temperature Sensor** (DHT22): Random values 18-35°C
- **Humidity Sensor** (DHT22): Random values 30-80%
- **Soil Moisture**: Gradually decreases, increases after watering
- **Water Pump**: Console logs when ON/OFF
- **Servo Motor**: Shows angle position (0-180°)
- **LCD Display**: Shows temp and moisture readings

## 🌐 API Endpoints

| Method | Endpoint       | Description                 |
|--------|----------------|-----------------------------|
| GET    | `/api/current` | Get current sensor readings |
| GET    | `/api/history` | Get historical data         |
| POST   | `/api/water`   | Manually trigger watering   |
| GET    | `/api/status`  | System status               |
| GET    | `/api/health`  | Health check                |

## 🎯 Features

### Smart Irrigation Logic
- Waters when soil moisture < 30%
- Preventive watering when temp > 30°C and moisture < 50%
- Calculates plant comfort score (0-100%)

### Web Dashboard
- 📊 Real-time sensor charts
- 🎛️ Manual pump control
- 📱 Responsive design
- 📝 System activity logs
- ⚡ Live updates every 5 seconds

### Hardware Controls
- 💧 Water pump automation
- 🔄 Servo moisture indicator
- 📺 LCD status display
- 🌡️ Multi-sensor monitoring

## 🛠️ Development

### Build Commands
```bash
# C++ compilation
cd raspberry-pi
make              # Build system
make test         # Build and test
make clean        # Clean build files
make debug        # Debug build

# Node.js development
cd node-server
npm run dev       # Start with auto-reload
```

### Adding New Sensors
1. Add sensor reading function in `main.cpp`
2. Update JSON output structure
3. Add API endpoint in `server.js`
4. Update frontend dashboard

## 🎓 School Project Demo

Perfect for demonstrating:
- **C++ Programming**: Hardware simulation, algorithms
- **Web APIs**: RESTful design, JSON data
- **Frontend Development**: Interactive dashboards
- **System Integration**: Multiple technologies working together
- **IoT Concepts**: Sensor data, automation, remote control

## 🔧 Troubleshooting

### C++ Compilation Issues
```bash
# Make sure g++ is installed
g++ --version

# Clean and rebuild
make clean && make
```

### Node.js Connection Issues  
```bash
# Check if C++ executable exists
ls raspberry-pi/build/

# Test C++ program directly
cd raspberry-pi && make test
```

### Frontend Not Loading
- Make sure Node.js server is running on port 3000
- Check browser console for errors
- Verify API endpoints respond: `curl http://localhost:3000/api/health`

## 📝 Future Enhancements

- 📱 Mobile app integration
- 🌐 Remote monitoring via cloud
- 📊 Data persistence (database)
- 🔔 Notification system
- 🌡️ Weather API integration
- 📈 Machine learning predictions

## 👨‍💻 Contributors

- Your Name - Initial work

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**Made with 🌱 for sustainable farming!**

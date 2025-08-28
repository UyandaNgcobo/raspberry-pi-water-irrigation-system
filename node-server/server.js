const express = require('express');
const { spawn } = require('child_process');
const path = require('path');
const cors = require('cors');

const app = express();
const PORT = 8080;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, '../frontend'))); // Serve frontend files

// Store historical data (in a real project, use a database)
let historicalData = [];
const MAX_HISTORY = 50; // Keep last 50 readings

// Function to call C++ irrigation system
function getIrrigationData() {
    return new Promise((resolve, reject) => {
        // Path to compiled C++ executable
        const cppPath = path.join(__dirname, '../raspberry-pi/build/irrigation_system');
        
        const cpp = spawn(cppPath);
        
        let output = '';
        let error = '';
        
        cpp.stdout.on('data', (data) => {
            output += data.toString();
        });
        
        cpp.stderr.on('data', (data) => {
            error += data.toString();
            // Log hardware simulation messages
            console.log('🔧 Hardware:', data.toString().trim());
        });
        
        cpp.on('close', (code) => {
            if (code !== 0) {
                reject(new Error(`Irrigation system failed: ${error}`));
                return;
            }
            
            try {
                const jsonData = JSON.parse(output.trim());
                
                // Add timestamp and store in history
                jsonData.timestamp = new Date().toISOString();
                historicalData.push(jsonData);
                
                // Keep only recent history
                if (historicalData.length > MAX_HISTORY) {
                    historicalData = historicalData.slice(-MAX_HISTORY);
                }
                
                resolve(jsonData);
            } catch (parseError) {
                reject(new Error(`Invalid data from irrigation system: ${parseError.message}`));
            }
        });
        
        cpp.on('error', (err) => {
            reject(new Error(`Failed to run irrigation system: ${err.message}`));
        });
    });
}

// API Routes

// Get current sensor status
app.get('/api/current', async (req, res) => {
    try {
        console.log('📊 Getting current irrigation data...');
        const data = await getIrrigationData();
        
        res.json({
            success: true,
            data: data
        });
    } catch (error) {
        console.error('❌ Error:', error.message);
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Get historical data
app.get('/api/history', (req, res) => {
    const limit = parseInt(req.query.limit) || 20;
    const recentData = historicalData.slice(-limit);
    
    res.json({
        success: true,
        count: recentData.length,
        data: recentData
    });
});

// Manual water control
app.post('/api/water', async (req, res) => {
    try {
        console.log('💧 Manual watering triggered...');
        
        // In a real system, this would trigger the pump directly
        // For demo, we'll just get current data and pretend we watered
        const data = await getIrrigationData();
        
        // Simulate manual watering effect
        if (data.sensors) {
            data.sensors.soil_moisture = Math.min(100, data.sensors.soil_moisture + 30);
            data.sensors.pump_status = true;
            data.irrigation.reason = "Manual watering triggered";
            
            console.log('🌱 Manual watering completed!');
        }
        
        res.json({
            success: true,
            message: 'Manual watering completed',
            data: data
        });
    } catch (error) {
        console.error('❌ Manual watering failed:', error.message);
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Get system status
app.get('/api/status', (req, res) => {
    res.json({
        success: true,
        system: {
            status: 'running',
            uptime: process.uptime(),
            memory: process.memoryUsage(),
            last_reading: historicalData.length > 0 ? historicalData[historicalData.length - 1].timestamp : null,
            total_readings: historicalData.length
        }
    });
});

// Health check
app.get('/api/health', (req, res) => {
    res.json({
        success: true,
        message: 'Water Irrigation System API is running',
        timestamp: new Date().toISOString()
    });
});

// Serve the main page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '../frontend/index.html'));
});

// Start the server
app.listen(PORT, () => {
    console.log('\n🌱 Water Irrigation System Started!');
    console.log(`🌐 Dashboard: http://localhost:${PORT}`);
    console.log(`📊 API: http://localhost:${PORT}/api/current`);
    console.log(`📈 History: http://localhost:${PORT}/api/history`);
    console.log(`💧 Manual Water: POST http://localhost:${PORT}/api/water`);
    console.log('\n📋 Make sure to compile the C++ program:');
    console.log('   cd raspberry-pi && make');
    console.log('\n🔧 Hardware simulation logs will appear below...\n');
    
    // Get initial reading to populate history
    setTimeout(() => {
        getIrrigationData().catch(err => {
            console.log('⚠️  Initial reading failed - make sure C++ is compiled!');
            console.log('   Run: cd raspberry-pi && make');
        });
    }, 1000);
});
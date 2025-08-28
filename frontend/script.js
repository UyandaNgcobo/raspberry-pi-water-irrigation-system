// Global variables
let chart = null;
let isConnected = false;
let updateInterval = null;
let historicalData = [];

// DOM elements
const elements = {
    temperature: document.getElementById('temperature'),
    humidity: document.getElementById('humidity'),
    soilMoisture: document.getElementById('soil-moisture'),
    comfortScore: document.getElementById('comfort-score'),
    tempStatus: document.getElementById('temp-status'),
    humidityStatus: document.getElementById('humidity-status'),
    soilStatus: document.getElementById('soil-status'),
    systemStatus: document.getElementById('system-status'),
    pumpIndicator: document.getElementById('pump-indicator'),
    pumpText: document.getElementById('pump-text'),
    pumpStatus: document.getElementById('pump-status'),
    servoPointer: document.getElementById('servo-pointer'),
    manualWaterBtn: document.getElementById('manual-water-btn'),
    connectionStatus: document.getElementById('connection-status'),
    lastUpdate: document.getElementById('last-update'),
    systemLog: document.getElementById('system-log')
};

// Initialize the application
document.addEventListener('DOMContentLoaded', function() {
    console.log('🌱 Smart Irrigation System - Frontend Loaded');
    
    // Initialize chart
    initializeChart();
    
    // Set up event listeners
    setupEventListeners();
    
    // Start periodic updates
    startPeriodicUpdates();
    
    // Get initial data
    fetchCurrentData();
    loadHistoricalData();
    
    addLogEntry('System started', 'success');
});

// Initialize the sensor chart
function initializeChart() {
    const ctx = document.getElementById('sensorChart').getContext('2d');
    
    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Temperature (°C)',
                    data: [],
                    borderColor: '#e53e3e',
                    backgroundColor: 'rgba(229, 62, 62, 0.1)',
                    tension: 0.4,
                    fill: false
                },
                {
                    label: 'Humidity (%)',
                    data: [],
                    borderColor: '#3182ce',
                    backgroundColor: 'rgba(49, 130, 206, 0.1)',
                    tension: 0.4,
                    fill: false
                },
                {
                    label: 'Soil Moisture (%)',
                    data: [],
                    borderColor: '#38a169',
                    backgroundColor: 'rgba(56, 161, 105, 0.1)',
                    tension: 0.4,
                    fill: true
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: {
                intersect: false,
                mode: 'index'
            },
            plugins: {
                legend: {
                    position: 'top',
                },
                title: {
                    display: true,
                    text: 'Real-time Sensor Readings'
                }
            },
            scales: {
                x: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Time'
                    }
                },
                y: {
                    display: true,
                    title: {
                        display: true,
                        text: 'Value'
                    },
                    suggestedMin: 0,
                    suggestedMax: 100
                }
            }
        }
    });
}

// Set up event listeners
function setupEventListeners() {
    // Manual water button
    elements.manualWaterBtn.addEventListener('click', handleManualWater);
    
    // Refresh button (if you add one)
    document.addEventListener('keydown', function(e) {
        if (e.key === 'r' || e.key === 'R') {
            fetchCurrentData();
        }
    });
}

// Start periodic updates
function startPeriodicUpdates() {
    // Update every 5 seconds
    updateInterval = setInterval(() => {
        fetchCurrentData();
    }, 5000);
    
    console.log('⏰ Started automatic updates every 5 seconds');
}

// Fetch current sensor data
async function fetchCurrentData() {
    try {
        const response = await fetch('/api/current');
        const result = await response.json();
        
        if (result.success) {
            updateUI(result.data);
            updateConnectionStatus(true);
            addLogEntry(`Data updated - Soil: ${result.data.sensors.soil_moisture.toFixed(1)}%`);
        } else {
            throw new Error(result.error || 'Unknown error');
        }
        
    } catch (error) {
        console.error('❌ Error fetching data:', error);
        updateConnectionStatus(false);
        addLogEntry(`Connection error: ${error.message}`, 'error');
    }
}

// Load historical data for chart
async function loadHistoricalData() {
    try {
        const response = await fetch('/api/history?limit=20');
        const result = await response.json();
        
        if (result.success && result.data.length > 0) {
            historicalData = result.data;
            updateChart();
            console.log(`📊 Loaded ${result.data.length} historical readings`);
        }
        
    } catch (error) {
        console.error('❌ Error loading history:', error);
        addLogEntry('Failed to load historical data', 'warning');
    }
}

// Update the UI with new sensor data
function updateUI(data) {
    const sensors = data.sensors;
    const irrigation = data.irrigation;
    
    // Update sensor values
    elements.temperature.textContent = `${sensors.temperature.toFixed(1)}°C`;
    elements.humidity.textContent = `${sensors.humidity.toFixed(1)}%`;
    elements.soilMoisture.textContent = `${sensors.soil_moisture.toFixed(1)}%`;
    elements.comfortScore.textContent = `${irrigation.comfort_score.toFixed(0)}%`;
    
    // Update sensor status
    elements.tempStatus.textContent = getTemperatureStatus(sensors.temperature);
    elements.humidityStatus.textContent = getHumidityStatus(sensors.humidity);
    elements.soilStatus.textContent = getSoilStatus(sensors.soil_moisture);
    elements.systemStatus.textContent = irrigation.reason;
    
    // Update pump status
    updatePumpStatus(sensors.pump_status);
    
    // Update servo position
    updateServoPosition(sensors.servo_angle);
    
    // Update last update time
    elements.lastUpdate.textContent = `Last update: ${new Date().toLocaleTimeString()}`;
    
    // Add data to chart if needed
    addDataToChart(data);
    
    // Log irrigation decisions
    if (irrigation.should_water) {
        addLogEntry(`🚨 Watering triggered: ${irrigation.reason}`, 'warning');
    }
}

// Update pump status display
function updatePumpStatus(isOn) {
    if (isOn) {
        elements.pumpIndicator.className = 'pump-indicator on';
        elements.pumpText.textContent = 'ON';
        elements.pumpStatus.style.color = '#48bb78';
    } else {
        elements.pumpIndicator.className = 'pump-indicator off';
        elements.pumpText.textContent = 'OFF';
        elements.pumpStatus.style.color = '#a0aec0';
    }
}

// Update servo position (0-180 degrees)
function updateServoPosition(angle) {
    // Convert servo angle to rotation (-90 to +90 degrees for visual)
    const rotation = (angle - 90); // 0° = -90°, 90° = 0°, 180° = +90°
    elements.servoPointer.style.transform = `translateX(-50%) rotate(${rotation}deg)`;
}

// Add data to chart
function addDataToChart(data) {
    const time = new Date().toLocaleTimeString();
    const sensors = data.sensors;
    
    // Add to chart
    chart.data.labels.push(time);
    chart.data.datasets[0].data.push(sensors.temperature);
    chart.data.datasets[1].data.push(sensors.humidity);
    chart.data.datasets[2].data.push(sensors.soil_moisture);
    
    // Keep only last 20 data points
    if (chart.data.labels.length > 20) {
        chart.data.labels.shift();
        chart.data.datasets.forEach(dataset => dataset.data.shift());
    }
    
    chart.update('none'); // Update without animation for performance
}

// Update chart with historical data
function updateChart() {
    if (!historicalData.length) return;
    
    chart.data.labels = [];
    chart.data.datasets[0].data = [];
    chart.data.datasets[1].data = [];
    chart.data.datasets[2].data = [];
    
    historicalData.forEach(entry => {
        const time = new Date(entry.timestamp).toLocaleTimeString();
        chart.data.labels.push(time);
        chart.data.datasets[0].data.push(entry.sensors.temperature);
        chart.data.datasets[1].data.push(entry.sensors.humidity);
        chart.data.datasets[2].data.push(entry.sensors.soil_moisture);
    });
    
    chart.update();
}

// Handle manual watering
async function handleManualWater() {
    const btn = elements.manualWaterBtn;
    
    // Disable button during request
    btn.disabled = true;
    btn.textContent = '💧 Watering...';
    
    try {
        const response = await fetch('/api/water', { method: 'POST' });
        const result = await response.json();
        
        if (result.success) {
            addLogEntry('💧 Manual watering completed', 'success');
            // Update UI with new data
            updateUI(result.data);
        } else {
            throw new Error(result.error || 'Watering failed');
        }
        
    } catch (error) {
        console.error('❌ Manual watering failed:', error);
        addLogEntry(`Manual watering failed: ${error.message}`, 'error');
    } finally {
        // Re-enable button
        setTimeout(() => {
            btn.disabled = false;
            btn.textContent = '💧 Water Plant';
        }, 2000);
    }
}

// Update connection status
function updateConnectionStatus(connected) {
    isConnected = connected;
    
    if (connected) {
        elements.connectionStatus.className = 'status connected';
        elements.connectionStatus.textContent = '● Connected';
    } else {
        elements.connectionStatus.className = 'status disconnected';
        elements.connectionStatus.textContent = '● Disconnected';
    }
}

// Add entry to system log
function addLogEntry(message, type = 'info') {
    const time = new Date().toLocaleTimeString();
    const entry = document.createElement('div');
    entry.className = `log-entry ${type}`;
    
    entry.innerHTML = `
        <span class="log-time">${time}</span>
        <span class="log-message">${message}</span>
    `;
    
    elements.systemLog.insertBefore(entry, elements.systemLog.firstChild);
    
    // Keep only last 50 entries
    while (elements.systemLog.children.length > 50) {
        elements.systemLog.removeChild(elements.systemLog.lastChild);
    }
}

// Status helper functions
function getTemperatureStatus(temp) {
    if (temp < 15) return 'Too Cold';
    if (temp < 20) return 'Cool';
    if (temp < 30) return 'Optimal';
    if (temp < 35) return 'Warm';
    return 'Too Hot';
}

function getHumidityStatus(humidity) {
    if (humidity < 30) return 'Too Dry';
    if (humidity < 50) return 'Low';
    if (humidity < 70) return 'Optimal';
    if (humidity < 80) return 'High';
    return 'Too Humid';
}

function getSoilStatus(moisture) {
    if (moisture < 20) return 'Very Dry - Needs Water';
    if (moisture < 40) return 'Dry';
    if (moisture < 60) return 'Good';
    if (moisture < 80) return 'Moist';
    return 'Very Wet';
}

// Handle page visibility changes
document.addEventListener('visibilitychange', function() {
    if (document.hidden) {
        // Page is hidden, slow down updates
        if (updateInterval) {
            clearInterval(updateInterval);
            updateInterval = setInterval(fetchCurrentData, 30000); // 30 seconds
        }
    } else {
        // Page is visible, resume normal updates
        if (updateInterval) {
            clearInterval(updateInterval);
            updateInterval = setInterval(fetchCurrentData, 5000); // 5 seconds
            fetchCurrentData(); // Immediate update
        }
    }
});

// Clean up on page unload
window.addEventListener('beforeunload', function() {
    if (updateInterval) {
        clearInterval(updateInterval);
    }
});
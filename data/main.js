// main.js - Main controller for ESP32-S2 Feather Control Panel

// Initialize everything when the window loads
window.onload = function () {
    console.log("Window loaded, initializing...");
    
    try {
      // Initialize all modules
      controlModule.initialize();
      scannerModule.initialize();
      sysInfoModule.initialize();
      
      // Start polling for connection status and sensor readings
      controlModule.updateSensorReadings(); // Initial reading
      console.log("Setting up polling...");
      setInterval(controlModule.updateConnectionStatus, 2000);
      setInterval(controlModule.updateSensorReadings, 2000);
      
      // Initialize tab navigation
      tabModule.initialize();
      
      // Check if system info elements exist and initialize system info if needed
      if (document.getElementById('refresh-sysinfo')) {
        console.log("System info elements found in DOM");
        // Only update system info if that panel is visible
        if (!document.getElementById('sysinfo-panel').classList.contains('hidden')) {
          sysInfoModule.updateSystemInfo();
          // Start auto-refresh if system info panel is already visible
          sysInfoModule.startSystemInfoAutoRefresh();
        }
      }
    } catch (e) {
      console.error("Error during initialization:", e);
    }
  };
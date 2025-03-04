// sysInfoModule.js - Modified for auto-refresh and improved display

const sysInfoModule = {
  // Variable to store the auto-refresh interval ID
  refreshInterval: null,
  
  initialize: function() {
    console.log("Initializing system info module...");
    
    // Hide the refresh button if it exists - we're auto-refreshing now
    const refreshButton = document.getElementById('refresh-sysinfo');
    if (refreshButton) {
      refreshButton.style.display = 'none';
    }
  },
  
  // Start auto-refresh of system info
  startSystemInfoAutoRefresh: function() {
    // Clear any existing interval first
    this.stopSystemInfoAutoRefresh();
    
    // Set new interval to update every 1000ms (1 second)
    this.refreshInterval = setInterval(function() {
      sysInfoModule.updateSystemInfo(false); // Pass false to indicate this is an auto-refresh (don't show loading state)
    }, 1000);
    
    console.log("System info auto-refresh started");
  },
  
  // Stop auto-refresh
  stopSystemInfoAutoRefresh: function() {
    if (this.refreshInterval !== null) {
      clearInterval(this.refreshInterval);
      this.refreshInterval = null;
      console.log("System info auto-refresh stopped");
    }
  },
  
  // Update system information
  updateSystemInfo: function(showLoading = true) {
    console.log("Updating system information...");
    
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/sysinfo", true);
    xhr.timeout = 10000; // 10 seconds timeout
    
    xhr.onreadystatechange = function () {
      if (xhr.readyState == 4) {
        if (xhr.status == 200) {
          console.log("System info received: " + xhr.responseText);
          try {
            const sysInfo = JSON.parse(xhr.responseText);
            sysInfoModule.displaySystemInfo(sysInfo);
          } catch (e) {
            console.error("Error parsing system info: " + e);
            // Only show alert for manual refreshes
            if (showLoading) {
              alert("Error fetching system information. Please try again.");
            }
          }
        } else {
          console.error("System info request failed with status: " + xhr.status);
        }
      }
    };
    
    xhr.ontimeout = function() {
      console.error("System info request timed out");
    };
    
    xhr.onerror = function() {
      console.error("Network error during system info request");
    };
    
    xhr.send();
  },
  
  // Display system information with improved unit handling
  displaySystemInfo: function(info) {
    console.log("Displaying system info:", info);
    
    // Network information
    if (info.network) {
      document.getElementById('wifi-mode').textContent = info.network.mode || "--";
      document.getElementById('ip-address').textContent = info.network.ip || "--";
      document.getElementById('mac-address').textContent = info.network.mac || "--";
      document.getElementById('wifi-ssid').textContent = info.network.ssid || "--";
      
      // Format RSSI with dBm and signal strength indicator
      const rssi = info.network.rssi;
      if (rssi !== undefined) {
        let signalStrength = "";
        if (rssi > -50) signalStrength = "Excellent";
        else if (rssi > -60) signalStrength = "Good";
        else if (rssi > -70) signalStrength = "Fair";
        else signalStrength = "Poor";
        
        document.getElementById('wifi-rssi').textContent = rssi + " dBm (" + signalStrength + ")";
      } else {
        document.getElementById('wifi-rssi').textContent = "--";
      }
    }
    
    // System resources
    if (info.resources) {
      document.getElementById('cpu-freq').textContent = (info.resources.cpuFreq || "--") + " MHz";
      
      // RAM usage
      const freeRam = info.resources.freeRam;
      const totalRam = info.resources.totalRam;
      if (freeRam !== undefined && totalRam !== undefined) {
        const usedRam = totalRam - freeRam;
        const usedRamPercent = ((usedRam / totalRam) * 100).toFixed(1);
        
        // Format with appropriate units
        document.getElementById('free-ram').textContent = freeRam.toFixed(2) + " KB (of " + totalRam.toFixed(2) + " KB)";
        document.getElementById('used-ram').textContent = usedRam.toFixed(2) + " KB (" + usedRamPercent + "%)";
      } else {
        document.getElementById('free-ram').textContent = "--";
        document.getElementById('used-ram').textContent = "--";
      }
      
      // Flash usage - now with proper units
      const flashSize = info.resources.flashSize;
      const flashSizeUnits = info.resources.flashSizeUnits || "MB";  // Default to MB if not specified
      
      if (flashSize !== undefined) {
        document.getElementById('flash-size').textContent = flashSize.toFixed(2) + " " + flashSizeUnits;
        
        // Handle flash usage with units
        const flashUsedPercent = info.resources.flashUsedPercent;
        const sketchSize = info.resources.sketchSize;
        const sketchSizeUnits = info.resources.sketchSizeUnits || "KB";
        
        if (sketchSize !== undefined && flashUsedPercent !== undefined) {
          document.getElementById('flash-usage').textContent = 
            sketchSize.toFixed(2) + " " + sketchSizeUnits + 
            " (" + flashUsedPercent.toFixed(1) + "%)";
        } else {
          document.getElementById('flash-usage').textContent = "--";
        }
      }
    }
    
    // Board information
    if (info.board) {
      document.getElementById('chip-model').textContent = info.board.chipModel || "--";
      document.getElementById('chip-revision').textContent = info.board.chipRevision || "--";
      document.getElementById('sdk-version').textContent = info.board.sdkVersion || "--";
      
      // Format uptime in a human-readable way
      const uptime = info.board.uptime;
      if (uptime !== undefined) {
        const days = Math.floor(uptime / 86400);
        const hours = Math.floor((uptime % 86400) / 3600);
        const minutes = Math.floor((uptime % 3600) / 60);
        const seconds = uptime % 60;
        
        let uptimeFormatted = "";
        if (days > 0) uptimeFormatted += days + " days, ";
        if (hours > 0 || days > 0) uptimeFormatted += hours + " hours, ";
        if (minutes > 0 || hours > 0 || days > 0) uptimeFormatted += minutes + " minutes, ";
        uptimeFormatted += seconds + " seconds";
        
        document.getElementById('uptime').textContent = uptimeFormatted;
      } else {
        document.getElementById('uptime').textContent = "--";
      }
    }
    
    // Battery information
    if (info.battery) {
      // Battery connected status
      const connected = info.battery.connected;
      document.getElementById('battery-connected').textContent = connected ? "Yes" : "No";
      
      // Battery percentage
      const percentage = info.battery.percentage;
      if (percentage !== undefined && connected) {
        document.getElementById('battery-percentage').textContent = percentage.toFixed(1) + "%";
      } else {
        document.getElementById('battery-percentage').textContent = connected ? "Calculating..." : "N/A";
      }
      
      // Battery voltage
      const voltage = info.battery.voltage;
      if (voltage !== undefined && connected) {
        document.getElementById('battery-voltage').textContent = voltage.toFixed(2) + " V";
      } else {
        document.getElementById('battery-voltage').textContent = connected ? "Calculating..." : "N/A";
      }
      
      // Power source
      const usbPowered = info.battery.usbPowered;
      document.getElementById('power-source').textContent = usbPowered ? "USB" : (connected ? "Battery" : "USB");
      
      // Charging status
      const charging = info.battery.charging;
      if (charging !== undefined && connected) {
        document.getElementById('charging-status').textContent = charging ? "Charging" : "Not Charging";
      } else {
        document.getElementById('charging-status').textContent = "N/A";
      }
    }
  }
};
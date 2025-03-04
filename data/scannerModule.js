// scannerModule.js - I2C Scanner functionality

const scannerModule = {
    initialize: function() {
      console.log("Initializing scanner module...");
      
      // Check if scanner elements exist
      if (document.getElementById('scan-button')) {
        console.log("Scanner elements found in DOM");
      } else {
        console.error("Scanner elements NOT found in DOM - check HTML structure");
      }
    },
    
    // Scan I2C bus function
    scanI2C: function() {
      console.log("I2C scan button clicked");
  
      // Cache DOM elements to ensure they exist
      const scanButton = document.getElementById('scan-button');
      const scanStatus = document.getElementById('scan-status');
      const devicesTable = document.getElementById('devices-table');
      const devicesList = document.getElementById('devices-list');
  
      if (!scanButton || !scanStatus) {
        console.error("Could not find scan-button or scan-status elements");
        return;
      }
  
      // Disable button and show scanning message
      scanButton.disabled = true;
      scanButton.innerHTML = "Scanning...";
      scanStatus.innerHTML = "Scanning I2C bus, please wait...";
  
      // Hide the table during scan
      if (devicesTable) {
        devicesTable.classList.add('hidden');
      }
  
      console.log("Sending scan request...");
  
      // Create a new XHR object for the scan request
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/scan", true);
      
      // Set timeout to handle potential network issues
      xhr.timeout = 15000; // 15 seconds timeout
      
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
          // Re-enable button regardless of result
          if (scanButton) {
            scanButton.disabled = false;
            scanButton.innerHTML = "Scan I2C Bus";
          }
          
          if (xhr.status == 200) {
            console.log("Scan results received: " + xhr.responseText);
            try {
              const results = JSON.parse(xhr.responseText);
              // Explicitly call a function to handle the display on a short timeout
              // This helps Safari process the DOM updates
              setTimeout(function() {
                scannerModule.displayScanResults(results);
              }, 50);
            } catch (e) {
              console.error("Error parsing scan results: " + e);
              if (scanStatus) {
                scanStatus.innerHTML = "Error parsing scan results.";
              }
            }
          } else {
            console.error("Scan request failed with status: " + xhr.status);
            if (scanStatus) {
              scanStatus.innerHTML = "Error performing scan. Server returned status " + xhr.status;
            }
          }
        }
      };
      
      // Handle timeout
      xhr.ontimeout = function() {
        console.error("Scan request timed out");
        if (scanButton) {
          scanButton.disabled = false;
          scanButton.innerHTML = "Scan I2C Bus";
        }
        if (scanStatus) {
          scanStatus.innerHTML = "Scan request timed out. Please try again.";
        }
      };
      
      // Handle network errors
      xhr.onerror = function() {
        console.error("Network error during scan request");
        if (scanButton) {
          scanButton.disabled = false;
          scanButton.innerHTML = "Scan I2C Bus";
        }
        if (scanStatus) {
          scanStatus.innerHTML = "Network error during scan. Please try again.";
        }
      };
      
      xhr.send();
    },
    
    // Display scan results function
    displayScanResults: function(results) {
      console.log("Displaying scan results:", results);
  
      // Re-fetch DOM elements to ensure they exist in current context
      const scanStatus = document.getElementById('scan-status');
      const devicesTable = document.getElementById('devices-table');
      const devicesList = document.getElementById('devices-list');
  
      if (!scanStatus || !devicesTable || !devicesList) {
        console.error("Could not find one or more elements for displaying results");
        return;
      }
  
      // Clear previous results - using a more compatible method
      while (devicesList.firstChild) {
        devicesList.removeChild(devicesList.firstChild);
      }
  
      if (results && results.scanComplete) {
        if (results.devices && results.devices.length > 0) {
          scanStatus.innerHTML = "Scan complete. Found " + results.devices.length +
            " device" + (results.devices.length > 1 ? "s" : "") + " on the I2C bus.";
  
          // Show table and add devices
          devicesTable.style.display = "table";
          devicesTable.classList.remove('hidden');
  
          results.devices.forEach(function(device) {
            console.log("Adding device to table:", device);
  
            const row = document.createElement('tr');
  
            const decimalCell = document.createElement('td');
            decimalCell.textContent = device.address;
  
            const hexCell = document.createElement('td');
            hexCell.textContent = device.hexAddress;
  
            row.appendChild(decimalCell);
            row.appendChild(hexCell);
            devicesList.appendChild(row);
          });
        } else {
          scanStatus.innerHTML = "Scan complete. No I2C devices found.";
          devicesTable.classList.add('hidden');
        }
      } else {
        scanStatus.innerHTML = "Error performing scan or invalid response.";
        devicesTable.classList.add('hidden');
      }
    }
  };
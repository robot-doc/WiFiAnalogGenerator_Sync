// controlModule.js - Control panel functionality

const controlModule = {
    initialize: function() {
      console.log("Initializing control module...");
      this.getInitialLEDState();
      this.getInitialDACValue();
    },
    
    // Get initial LED state
    getInitialLEDState: function() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/ledstate", true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          try {
            var state = xhr.responseText;
            console.log("Initial LED state: " + state);
            document.getElementById("led-status").innerHTML = "LED Status: " + (state == "1" ? "ON" : "OFF");
          } catch (e) {
            console.error("Error processing LED state:", e);
          }
        }
      };
      xhr.send();
    },
    
    // Get initial DAC value
    getInitialDACValue: function() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/dacstate", true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          try {
            var value = xhr.responseText;
            console.log("Initial DAC value: " + value);
            
            var dacSlider = document.getElementById("dacSlider");
            if (dacSlider) {
              dacSlider.value = value;
            }
            
            var dacValue = document.getElementById("dac-value");
            if (dacValue) {
              dacValue.innerHTML = "DAC Value: " + value;
            }
            
            var voltageValue = document.getElementById("voltage-value");
            if (voltageValue) {
              var voltage = (value / 255 * 3.3).toFixed(2);
              voltageValue.innerHTML = "Voltage: " + voltage + "V";
            }
          } catch (e) {
            console.error("Error processing DAC state:", e);
          }
        }
      };
      xhr.send();
    },
    
    // Toggle LED function
    toggleLED: function(state) {
      console.log("Toggling LED: " + state);
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/led?state=" + state, true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          document.getElementById("led-status").innerHTML = "LED Status: " + (state ? "ON" : "OFF");
          console.log("LED toggle response received: " + xhr.responseText);
        }
      };
      xhr.send();
    },
    
    // Update DAC function
    updateDAC: function(value) {
      // Calculate approximate voltage (0-255 maps to 0-3.3V)
      var voltage = (value / 255 * 3.3).toFixed(2);
      document.getElementById("dac-value").innerHTML = "DAC Value: " + value;
      document.getElementById("voltage-value").innerHTML = "Voltage: " + voltage + "V";
  
      console.log("Updating DAC to: " + value + " (voltage: " + voltage + "V)");
  
      // Send value to server immediately - no delay
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/dac?value=" + value, true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          console.log("DAC update response received: " + xhr.responseText);
        }
      };
      xhr.send();
    },
    
    // Update sensor readings
    updateSensorReadings: function() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/sensor", true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          try {
            console.log("Sensor data received: " + xhr.responseText);
            var sensorData = JSON.parse(xhr.responseText);
  
            if (sensorData.ready) {
              document.getElementById("temperature").innerHTML = sensorData.temperature.toFixed(1);
              document.getElementById("humidity").innerHTML = sensorData.humidity.toFixed(1);
            } else {
              document.getElementById("temperature").innerHTML = "--";
              document.getElementById("humidity").innerHTML = "--";
            }
          } catch (e) {
            console.error("Error processing sensor data:", e);
          }
        }
      };
      xhr.send();
    },
    
    // Update connection status
    updateConnectionStatus: function() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/clients", true);
      xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
          try {
            var count = parseInt(xhr.responseText);
            console.log("Connection status updated, client count: " + count);
            document.getElementById("client-count").innerHTML = count;
  
            // Update connection indicator
            var dot = document.getElementById("connection-dot");
            var text = document.getElementById("connection-text");
  
            if (count > 1) { // Count includes this client
              dot.className = "status-indicator status-green";
              text.innerHTML = "Other client(s) connected.";
            } else {
              dot.className = "status-indicator status-red";
              text.innerHTML = "No other clients connected.";
            }
          } catch (e) {
            console.error("Error processing connection data:", e);
          }
        }
      };
      xhr.send();
    }
  };
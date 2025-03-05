// ethernetModule.js - Ethernet controller functionality

const ethernetModule = {
    // Flag to track if user is editing the form
    isUserEditing: false,
    // Flag to track if user has made changes
    hasUserChanges: false,

    initialize: function () {
        console.log("Initializing ethernet module...");
        this.getEthernetStatus();

        // Set up event handlers if elements exist
        const refreshButton = document.getElementById('ethernet-refresh-button');
        const saveButton = document.getElementById('ethernet-save-button');

        // Get input elements
        const ipInput = document.getElementById('ethernet-ip');
        const subnetInput = document.getElementById('ethernet-subnet');
        const gatewayInput = document.getElementById('ethernet-gateway');
        const dnsInput = document.getElementById('ethernet-dns');

        // Add focus and blur event listeners to track when user is editing
        if (ipInput) {
            ipInput.addEventListener('focus', () => { this.isUserEditing = true; });
            ipInput.addEventListener('blur', () => {
                // Short delay to allow for potential click on other inputs
                setTimeout(() => {
                    // Only set to false if no input has focus
                    if (document.activeElement !== ipInput &&
                        document.activeElement !== subnetInput &&
                        document.activeElement !== gatewayInput &&
                        document.activeElement !== dnsInput) {
                        this.isUserEditing = false;
                    }
                }, 100);
            });
        }

        if (subnetInput) {
            subnetInput.addEventListener('focus', () => { this.isUserEditing = true; });
            subnetInput.addEventListener('blur', () => {
                setTimeout(() => {
                    if (document.activeElement !== ipInput &&
                        document.activeElement !== subnetInput &&
                        document.activeElement !== gatewayInput &&
                        document.activeElement !== dnsInput) {
                        this.isUserEditing = false;
                    }
                }, 100);
            });
        }

        if (gatewayInput) {
            gatewayInput.addEventListener('focus', () => { this.isUserEditing = true; });
            gatewayInput.addEventListener('blur', () => {
                setTimeout(() => {
                    if (document.activeElement !== ipInput &&
                        document.activeElement !== subnetInput &&
                        document.activeElement !== gatewayInput &&
                        document.activeElement !== dnsInput) {
                        this.isUserEditing = false;
                    }
                }, 100);
            });
        }

        if (dnsInput) {
            dnsInput.addEventListener('focus', () => { this.isUserEditing = true; });
            dnsInput.addEventListener('blur', () => {
                setTimeout(() => {
                    if (document.activeElement !== ipInput &&
                        document.activeElement !== subnetInput &&
                        document.activeElement !== gatewayInput &&
                        document.activeElement !== dnsInput) {
                        this.isUserEditing = false;
                    }
                }, 100);
            });
        }

        if (refreshButton) {
            refreshButton.addEventListener('click', this.getEthernetStatus.bind(this));
        } else {
            console.error("Ethernet refresh button NOT found in DOM - check HTML structure");
        }

        if (saveButton) {
            saveButton.addEventListener('click', this.saveEthernetConfig.bind(this));
        } else {
            console.error("Ethernet save button NOT found in DOM - check HTML structure");
        }

        // Add "change" event listeners to detect when user makes changes
        if (ipInput) {
            ipInput.addEventListener('change', () => { this.hasUserChanges = true; });
            // existing focus listener...
        }

        if (subnetInput) {
            subnetInput.addEventListener('change', () => { this.hasUserChanges = true; });
            // existing focus listener...
        }

        if (gatewayInput) {
            gatewayInput.addEventListener('change', () => { this.hasUserChanges = true; });
            // existing focus listener...
        }

        if (dnsInput) {
            dnsInput.addEventListener('change', () => { this.hasUserChanges = true; });
            // existing focus listener...
        }

        // Add a handler for the save button to clear the hasUserChanges flag
        if (saveButton) {
            saveButton.addEventListener('click', () => {
                this.saveEthernetConfig();
                this.hasUserChanges = false; // Clear the flag after saving
            });
        }
    },

    // Get current Ethernet status
    getEthernetStatus: function () {
        console.log("Getting Ethernet status...");

        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/api/ethernet/status", true);
        xhr.timeout = 5000; // 5 seconds timeout

        xhr.onreadystatechange = function () {
            if (xhr.readyState == 4) {
                if (xhr.status == 200) {
                    console.log("Ethernet status received: " + xhr.responseText);
                    try {
                        const status = JSON.parse(xhr.responseText);
                        ethernetModule.updateStatusDisplay(status);
                        ethernetModule.populateConfigForm(status);
                    } catch (e) {
                        console.error("Error parsing Ethernet status:", e);
                        ethernetModule.showMessage("Error processing Ethernet status data", false);
                    }
                } else {
                    console.error("Ethernet status request failed:", xhr.status);
                    ethernetModule.showMessage("Failed to fetch Ethernet status. Check if hardware is connected.", false);
                }
            }
        };

        xhr.ontimeout = function () {
            console.error("Ethernet status request timed out");
            ethernetModule.showMessage("Request timed out. Check hardware connection.", false);
        };

        xhr.onerror = function () {
            console.error("Network error during Ethernet status request");
            ethernetModule.showMessage("Network error. Check hardware connection.", false);
        };

        xhr.send();
    },

    // Update status display with current information
    updateStatusDisplay: function (data) {
        const statusIndicator = document.getElementById('ethernet-connection-status');
        const statusText = document.getElementById('ethernet-connection-text');

        if (!statusIndicator || !statusText) {
            console.error("Ethernet status display elements not found");
            return;
        }

        if (data.connected) {
            statusIndicator.className = 'status-indicator status-green';
            statusText.textContent = 'Connected';
        } else {
            statusIndicator.className = 'status-indicator status-red';
            statusText.textContent = 'Disconnected';
        }

        document.getElementById('ethernet-ip-address').textContent = data.ip || '-';
        document.getElementById('ethernet-subnet-mask').textContent = data.subnet || '-';
        document.getElementById('ethernet-gateway').textContent = data.gateway || '-';
        document.getElementById('ethernet-dns-server').textContent = data.dns || '-';
    },

    // Fill configuration form with current values
    populateConfigForm: function (data) {
        // If user is currently editing, don't change the values
        if (this.isUserEditing || this.hasUserChanges) {
            return;
        }

        const ipInput = document.getElementById('ethernet-ip');
        const subnetInput = document.getElementById('ethernet-subnet');
        const gatewayInput = document.getElementById('ethernet-gateway');
        const dnsInput = document.getElementById('ethernet-dns');

        if (!ipInput || !subnetInput || !gatewayInput || !dnsInput) {
            console.error("Ethernet form elements not found");
            return;
        }

        ipInput.value = data.ip || '';
        subnetInput.value = data.subnet || '';
        gatewayInput.value = data.gateway || '';
        dnsInput.value = data.dns || '';
    },

    // Save Ethernet configuration
    saveEthernetConfig: function () {
        console.log("Saving Ethernet configuration...");

        // Get form values
        const ipInput = document.getElementById('ethernet-ip');
        const subnetInput = document.getElementById('ethernet-subnet');
        const gatewayInput = document.getElementById('ethernet-gateway');
        const dnsInput = document.getElementById('ethernet-dns');

        if (!ipInput || !subnetInput || !gatewayInput || !dnsInput) {
            console.error("Ethernet form elements not found");
            return;
        }

        const ipAddress = ipInput.value;
        const subnetMask = subnetInput.value;
        const gateway = gatewayInput.value;
        const dnsServer = dnsInput.value;

        // Validate IP addresses using regex
        const ipPattern = /^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;

        if (!ipPattern.test(ipAddress)) {
            this.showMessage("Invalid IP address format", false);
            return;
        }

        if (!ipPattern.test(subnetMask)) {
            this.showMessage("Invalid subnet mask format", false);
            return;
        }

        if (!ipPattern.test(gateway)) {
            this.showMessage("Invalid gateway format", false);
            return;
        }

        if (!ipPattern.test(dnsServer)) {
            this.showMessage("Invalid DNS server format", false);
            return;
        }

        // Prepare configuration data
        const config = {
            ip: ipAddress,
            subnet: subnetMask,
            gateway: gateway,
            dns: dnsServer
        };

        // Send configuration to server
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/ethernet/config", true);
        xhr.setRequestHeader("Content-Type", "application/json");

        xhr.onreadystatechange = function () {
            if (xhr.readyState == 4) {
                if (xhr.status == 200) {
                    console.log("Configuration saved successfully:", xhr.responseText);
                    try {
                        const response = JSON.parse(xhr.responseText);
                        ethernetModule.showMessage(response.message || "Configuration saved successfully", true);
                    } catch (e) {
                        console.error("Error parsing save response:", e);
                        ethernetModule.showMessage("Configuration saved, but response had invalid format", true);
                    }
                } else {
                    console.error("Save configuration failed:", xhr.status, xhr.responseText);
                    try {
                        const response = JSON.parse(xhr.responseText);
                        ethernetModule.showMessage(response.error || "Failed to save configuration", false);
                    } catch (e) {
                        ethernetModule.showMessage("Failed to save configuration", false);
                    }
                }
            }
        };

        xhr.onerror = function () {
            console.error("Network error during configuration save");
            ethernetModule.showMessage("Network error while saving configuration", false);
        };

        xhr.send(JSON.stringify(config));
    },

    // Display status message
    showMessage: function (message, isSuccess) {
        const messageBox = document.getElementById('ethernet-message');

        if (!messageBox) {
            console.error("Ethernet message element not found");
            return;
        }

        // Set message and style
        messageBox.textContent = message;
        messageBox.className = isSuccess ? 'success-message' : 'error-message';
        messageBox.style.display = 'block';

        // Hide message after 5 seconds
        setTimeout(function () {
            messageBox.style.display = 'none';
        }, 5000);
    }
};
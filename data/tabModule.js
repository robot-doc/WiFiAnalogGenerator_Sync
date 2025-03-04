// tabModule.js - Tab navigation functionality

const tabModule = {
    initialize: function() {
      console.log("Initializing tab navigation...");
      const tabs = document.querySelectorAll('.tab');
  
      tabs.forEach(tab => {
        tab.addEventListener('click', function () {
          console.log("Tab clicked: " + this.getAttribute('data-tab'));
  
          // Remove active class from all tabs
          tabs.forEach(t => t.classList.remove('active'));
  
          // Add active class to clicked tab
          this.classList.add('active');
  
          // Hide all panels
          document.getElementById('controls-panel').classList.add('hidden');
          document.getElementById('scanner-panel').classList.add('hidden');
          document.getElementById('sysinfo-panel').classList.add('hidden');
  
          // Show the selected panel
          const panelId = this.getAttribute('data-tab') + '-panel';
          document.getElementById(panelId).classList.remove('hidden');
          console.log("Showing panel: " + panelId);
          
          // If system info panel is selected, update the info and start auto-refresh
          if (panelId === 'sysinfo-panel') {
            sysInfoModule.updateSystemInfo();
            sysInfoModule.startSystemInfoAutoRefresh();
          } else {
            sysInfoModule.stopSystemInfoAutoRefresh();
          }
        });
      });
      console.log("Tab navigation initialized");
    }
  };
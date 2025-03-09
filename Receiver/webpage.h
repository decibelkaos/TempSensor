#ifndef WEBPAGE_H
#define WEBPAGE_H

/*
  MIT License

  (See TempSensor-RCV.ino for full license text and credits.)
  
  This header serves a simple HTML/JavaScript interface so users can
  configure the device (OLED display settings and LED behavior),
  and monitor real-time sensor readings.
*/

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>OLED Display Config</title>
  <style>
    :root {
      --bg-color: #2c2c2c; --text-color: #e0e0e0; --accent-color: #3a3f44;
      --highlight-color: #5294e2; --card-bg: #1a1a1a; --button-bg: #3a3f44;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background-color: var(--bg-color); color: var(--text-color);
      font-family: Arial, sans-serif; padding: 20px;
    }
    .container { max-width: 800px; margin: 0 auto; }
    h2 {
      color: var(--highlight-color); margin-bottom: 15px; 
      border-bottom: 1px solid var(--accent-color); padding-bottom: 10px;
    }
    .data-display {
      background: var(--card-bg); padding: 20px; margin: 20px auto;
      border-radius: 10px; width: 90%; max-width: 600px;
      box-shadow: 0 4px 8px rgba(0,0,0,0.3); text-align: center;
      position: relative; overflow: hidden;
    }
    .data-value { margin: 10px 0; }
    .smallFont { font-family: "Courier New", monospace; font-size: 14px; }
    .largeFont { font-family: Arial, sans-serif; font-size: 28px; font-weight: bold; }
    .marquee {
      font-size: 18px; color: var(--highlight-color); white-space: nowrap;
      position: absolute; bottom: 10px; left: 0; width: 100%; overflow: hidden;
    }
    .marquee span {
      display: inline-block; padding-left: 100%; animation: scroll 10s linear infinite;
    }
    @keyframes scroll {
      0% { transform: translateX(0); }
      100% { transform: translateX(-100%); }
    }
    .options {
      background: var(--card-bg); padding: 15px; border-radius: 8px;
      margin: 20px auto; max-width: 600px;
    }
    .option-group { margin-bottom: 20px; }
    label { display: block; margin: 8px 0 4px; font-weight: bold; }
    input, select, textarea {
      width: 100%; padding: 10px; margin-bottom: 10px;
      border: 1px solid var(--accent-color); border-radius: 4px;
      background-color: var(--card-bg); color: var(--text-color);
    }
    input:focus, select:focus, textarea:focus {
      outline: none; border-color: var(--highlight-color);
    }
    .range-group { margin-bottom: 10px; }
    .range-group label { font-weight: normal; margin-bottom: 2px; }
    .range-group input[type="range"] { width: 100%; }
    .button {
      background: var(--button-bg); color: var(--text-color);
      padding: 10px; border: none; border-radius: 4px; cursor: pointer;
      transition: background-color 0.3s;
    }
    .button:hover { background-color: var(--highlight-color); }
    footer {
      margin-top: 20px; text-align: center; font-size: 12px; color: var(--text-color);
    }
  </style>
</head>
<body>
  <div class="container">
    <!-- Real-Time Data Display Section -->
    <h2>Real-Time Sensor Data</h2>
    <div class="data-display">
      <div id="topData" class="data-value smallFont"></div>
      <div id="middleData" class="data-value largeFont"></div>
      <div id="marqueeData" class="marquee"><span></span></div>
    </div>
    
    <!-- Display Configuration -->
    <h2>Display Configuration</h2>
    <div class="options">
      <div class="option-group">
        <label for="topPosition">Top Bar Value:</label>
        <select id="topPosition" class="auto-save">
          <option value="tempBoth">Temp C / Temp F</option>
          <option value="tempC">Temp C</option>
          <option value="tempF">Temp F</option>
          <option value="humidity">Humidity</option>
          <option value="none">None</option>
        </select>
      </div>
      <div class="option-group">
        <label for="middlePosition">Middle Value:</label>
        <select id="middlePosition" class="auto-save">
          <option value="tempC">Temp C</option>
          <option value="tempF">Temp F</option>
          <option value="humidity">Humidity</option>
          <option value="none">None</option>
        </select>
      </div>
      <div class="option-group">
        <label for="scrollingTextEnabled">Scrolling Text:</label>
        <select id="scrollingTextEnabled" class="auto-save">
          <option value="1">Show scrolling text</option>
          <option value="0">No scrolling text</option>
        </select>
      </div>
      <div class="option-group">
        <label for="scrollingText">Scrolling Text (max 256 chars):</label>
        <textarea id="scrollingText" class="auto-save" maxlength="256">
HUMIDITY AND TEMPERATURE DEFAULT</textarea>
      </div>
      <div class="option-group">
        <label for="updateInterval">Data Refresh Rate (ms):</label>
        <input type="number" id="updateInterval" value="20" step="10" min="10" max="500" class="auto-save">
        <small>1000 ms = 1 second</small>
      </div>
      <div class="option-group">
        <label for="marqueeEnabled">Marquee Dots:</label>
        <select id="marqueeEnabled" class="auto-save">
          <option value="1">Show dots</option>
          <option value="0">Hide dots</option>
        </select>
      </div>
    </div>
    
    <!-- LED Configuration -->
    <h2>LED Fun Settings</h2>
    <div class="options">
      <div class="option-group">
        <label for="ledEnabled">Turn on the LED:</label>
        <select id="ledEnabled" class="auto-save">
          <option value="1">Yes, light it up!</option>
          <option value="0">No, keep it off.</option>
        </select>
      </div>
      <div class="option-group">
        <label for="humidityThresholdLow">Low Humidity Threshold (%):</label>
        <input type="number" id="humidityThresholdLow" value="33.3" step="0.1" min="0" max="100" class="auto-save">
      </div>
      <div class="option-group">
        <label for="humidityThresholdHigh">High Humidity Threshold (%):</label>
        <input type="number" id="humidityThresholdHigh" value="66.6" step="0.1" min="0" max="100" class="auto-save">
      </div>
      <div class="option-group range-group">
        <label for="ledIntensity">LED Intensity: <span id="ledIntensityDisplay">100</span></label>
        <input type="range" id="ledIntensity" value="100" min="0" max="255" class="auto-save">
      </div>
      <div class="option-group range-group">
        <label for="ledAnimSpeed">LED Animation Speed (ms): <span id="ledAnimSpeedDisplay">3000</span></label>
        <input type="range" id="ledAnimSpeed" value="3000" min="100" max="10000" step="100" class="auto-save">
      </div>
      <div class="option-group">
        <label for="ledAnimationMode">LED Animation Mode:</label>
        <select id="ledAnimationMode" class="auto-save">
          <option value="static">Static (Follows Humidity)</option>
          <option value="fade">Fade</option>
          <option value="bounce">Bounce</option>
          <option value="wiggle">Wiggle</option>
        </select>
      </div>
      <div class="option-group">
        <label for="ledColorScheme">LED Color Scheme:</label>
        <select id="ledColorScheme" class="auto-save">
          <option value="Default">Default</option>
          <option value="Easter">Easter</option>
          <option value="Christmas">Christmas</option>
          <option value="Halloween">Halloween</option>
          <option value="Valentine">Valentine</option>
          <option value="Independence">Independence</option>
          <option value="NewYears">NewYears</option>
          <option value="StPatrick">StPatrick</option>
          <option value="Summer">Summer</option>
          <option value="Winter">Winter</option>
          <option value="Autumn">Autumn</option>
          <option value="Spring">Spring</option>
          <option value="Rainbow">Rainbow</option>
          <option value="Ocean">Ocean</option>
          <option value="Sunset">Sunset</option>
          <option value="Aurora">Aurora</option>
          <option value="Fireworks">Fireworks</option>
          <option value="Galaxy">Galaxy</option>
          <option value="Neon">Neon</option>
          <option value="Pastel">Pastel</option>
          <option value="Vintage">Vintage</option>
          <option value="Tropical">Tropical</option>
          <option value="Forest">Forest</option>
          <option value="Desert">Desert</option>
          <option value="Cyberpunk">Cyberpunk</option>
          <option value="Lava">Lava</option>
          <option value="Ice">Ice</option>
        </select>
      </div>
    </div>
    
    <footer>
      <p>Carl Brothers</p>
      <p>decibelkaos@gmail.com</p>
      <p>Not Seattle, Washington (But Close)</p>
    </footer>
  </div>
  <script>
    // We'll auto-update sensor data every 2s, 
    // and auto-send config changes to the device on field changes.
    
    const topData  = document.getElementById('topData');
    const middleData  = document.getElementById('middleData');
    const marqueeData = document.getElementById('marqueeData').querySelector("span");
    
    // Inputs
    const topPositionSelect           = document.getElementById('topPosition');
    const middlePositionSelect        = document.getElementById('middlePosition');
    const scrollingTextEnabledSelect  = document.getElementById('scrollingTextEnabled');
    const scrollingTextInput          = document.getElementById('scrollingText');
    const marqueeEnabledSelect        = document.getElementById('marqueeEnabled');
    const ledEnabledSelect            = document.getElementById('ledEnabled');
    const humidityThresholdLowInput   = document.getElementById('humidityThresholdLow');
    const humidityThresholdHighInput  = document.getElementById('humidityThresholdHigh');
    const ledIntensityInput           = document.getElementById('ledIntensity');
    const ledAnimSpeedInput           = document.getElementById('ledAnimSpeed');
    const ledAnimationModeSelect      = document.getElementById('ledAnimationMode');
    const ledColorSchemeSelect        = document.getElementById('ledColorScheme');
    
    let currentTemp     = 24.5;
    let currentHumidity = 45.2;
    let currentScrollText = scrollingTextInput.value;
    
    function updateSliderDisplay(slider, displayElemId) {
      const el = document.getElementById(displayElemId);
      if (el) el.textContent = slider.value;
    }
    
    function formatTopValue(type) {
      // For the top bar, we show decimal temps or humidity
      switch(type) {
        case "tempBoth":
          return currentTemp.toFixed(2) + " °C / " + ((currentTemp * 9/5)+32).toFixed(2) + " °F";
        case "tempC":
          return currentTemp.toFixed(2) + " °C";
        case "tempF":
          return ((currentTemp * 9/5)+32).toFixed(2) + " °F";
        case "humidity":
          return currentHumidity.toFixed(1) + "%";
        default:
          return "";
      }
    }
    
    function formatMiddleValue(type) {
      // For the middle, typically show whole number or humidity
      switch(type) {
        case "tempC":
          return parseInt(currentTemp) + " °C";
        case "tempF":
          return parseInt((currentTemp * 9/5)+32) + " °F";
        case "humidity":
          return currentHumidity.toFixed(1) + "%";
        default:
          return "";
      }
    }
    
    function updatePreview() {
      // Render data in the preview area
      topData.textContent    = formatTopValue(topPositionSelect.value);
      middleData.textContent = formatMiddleValue(middlePositionSelect.value);
      
      if(scrollingTextEnabledSelect.value === "0") {
        document.getElementById('marqueeData').style.display = "none";
      } else {
        document.getElementById('marqueeData').style.display = "block";
        marqueeData.textContent = currentScrollText;
      }
    }
    
    // Send config to the ESP
    function sendConfig() {
      const config = {
        scrollingEnabled:     scrollingTextEnabledSelect.value,
        scrollingText:        scrollingTextInput.value,
        updateInterval:       document.getElementById('updateInterval').value,
        topPosition:          topPositionSelect.value,
        middlePosition:       middlePositionSelect.value,
        marqueeEnabled:       marqueeEnabledSelect.value,
        ledEnabled:           ledEnabledSelect.value,
        humidityThresholdLow: humidityThresholdLowInput.value,
        humidityThresholdHigh:humidityThresholdHighInput.value,
        ledIntensity:         ledIntensityInput.value,
        ledAnimSpeed:         ledAnimSpeedInput.value,
        ledAnimationMode:     ledAnimationModeSelect.value,
        ledColorScheme:       ledColorSchemeSelect.value
      };
      fetch('/updateConfig', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(config)
      })
      .then(response => response.text())
      .then(data => console.log("Config updated:", data))
      .catch(error => console.error("Error updating config:", error));
    }
    
    // Auto-update on changes
    document.querySelectorAll('.auto-save').forEach(input => {
      input.addEventListener('change', function() {
        sendConfig();
        updatePreview();
      });
      input.addEventListener('input', function() {
        // Immediate feedback for text or sliders
        if (input.id === "scrollingText") {
          currentScrollText = input.value;
          updatePreview();
        }
        if (input.id === "ledIntensity") {
          updateSliderDisplay(this, "ledIntensityDisplay");
          sendConfig();
        }
        if (input.id === "ledAnimSpeed") {
          updateSliderDisplay(this, "ledAnimSpeedDisplay");
          sendConfig();
        }
      });
    });
    
    // Poll sensor data from ESP
    function fetchSensorData() {
      fetch('/getSensorData')
        .then(res => res.json())
        .then(data => {
          currentTemp     = data.temperature;
          currentHumidity = data.humidity;
          updatePreview();
        })
        .catch(err => console.error("Error fetching sensor data:", err));
    }
    setInterval(fetchSensorData, 2000);
    
    // On page load, fetch stored config from the device
    fetch('/getConfig')
      .then(res => res.json())
      .then(data => {
        scrollingTextEnabledSelect.value  = data.scrollingEnabled ? "1" : "0";
        scrollingTextInput.value          = data.scrollingText;
        document.getElementById('updateInterval').value = data.updateInterval;
        
        if(data.topPosition)    topPositionSelect.value    = data.topPosition;
        if(data.middlePosition) middlePositionSelect.value = data.middlePosition;
        
        marqueeEnabledSelect.value        = data.marqueeEnabled ? "1" : "0";
        ledEnabledSelect.value            = data.ledEnabled ? "1" : "0";
        humidityThresholdLowInput.value   = data.humidityThresholdLow;
        humidityThresholdHighInput.value  = data.humidityThresholdHigh;
        ledIntensityInput.value           = data.ledIntensity || 100;
        ledAnimSpeedInput.value           = data.ledAnimSpeed || 3000;
        ledAnimationModeSelect.value      = data.ledAnimationMode || "static";
        ledColorSchemeSelect.value        = data.ledColorScheme  || "Default";
        
        updateSliderDisplay(ledIntensityInput, "ledIntensityDisplay");
        updateSliderDisplay(ledAnimSpeedInput, "ledAnimSpeedDisplay");
        
        updatePreview();
      })
      .catch(err => console.error("Error loading config:", err));
    
    let scrollX = 128; // purely for demonstration
  </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGE_H

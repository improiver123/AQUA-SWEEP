/*
  ============================================================================
  AQUA-SWEEP - ESP32 Firmware (v2 - Final Wiring Configuration)
  ============================================================================
  Single-file Arduino sketch. Hosts its own Wi-Fi access point and serves a
  horizontal, landscape-style control dashboard (matching the "boat
  transmitter" reference layout) directly from the ESP32 - no phone app,
  no internet connection needed. Open a browser and connect.

  DESIGN NOTE ON COMMUNICATION METHOD:
    This uses plain HTTP polling (the browser fetches /data every ~150ms)
    rather than WebSockets. WebSockets would be marginally smoother, but
    require installing 2-3 additional libraries (ESPAsyncWebServer,
    AsyncTCP, an async WebSocket library) on top of the three you've
    already had to troubleshoot. 150ms polling is visually smooth for
    every element in this dashboard (radar sweep, gauges, telemetry) and
    keeps the build reliable with what you already have installed. If you
    want to upgrade to WebSockets later, that's a contained change to the
    networking section only - the rest of this code doesn't need to change.

  REQUIRED LIBRARIES :
    - ESP32Servo
    - OneWire
    - DallasTemperature
  (WiFi.h, WebServer.h, ESPmDNS.h are built into the ESP32 board package.)

  ============================================================================
  FINAL PIN MAP 
  ============================================================================
    Conveyor motor (L298N):   ENA -> GPIO25   IN1 -> GPIO26   IN2 -> GPIO27
    Propulsion motor (L298N): ENB -> GPIO14   IN3 -> GPIO33   IN4 -> GPIO32
    Tail / rudder servo:      GPIO18
    Surveillance scan servo:  GPIO13  (RESERVED - not yet installed, see
                                        USE_SCAN_SERVO flag below)
    HC-SR04:                  TRIG -> GPIO21   ECHO -> GPIO22
                               (ECHO through a voltage divider if you have
                                one - 5V signal into a 3.3V-only pin)
    Left IR sensor:           GPIO19
    Right IR sensor:          GPIO23
    DS18B20:                  GPIO4  (4.7k pull-up to 3.3V if available)
    Battery voltage sense:    RESERVED, not yet wired - see
                               USE_BATTERY_SENSE flag below

  Access the dashboard at:  http://192.168.4.1   or   http://boat.local
  ============================================================================
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------------------------------------------------------
// FEATURE FLAGS - flip these to 1 once the hardware is installed
// ---------------------------------------------------------------
#define USE_SCAN_SERVO     1   // scanner SG90 is now installed on GPIO13
#define USE_BATTERY_SENSE  0   // set to 1 once a voltage divider is wired
#define RUDDER_REVERSED    1   // rudder was mounted/moving inverted - flips direction in software
#define RUDDER_TRIM        0   // degrees, +/- to recenter if linkage isn't mechanically centered.
                                 // Sweeping more to one side than the other (like yours does)
                                 // means the linkage sits off-center at the 90-degree signal.
                                 // Try +10, then +15, -10, etc. and re-upload until the rudder
                                 // sits visually centered when the dashboard shows 90 degrees.
#define SCAN_REVERSED      1   // ultrasonic sensor is mounted upside-down on the scan servo,
                                 // which mirrors left/right as seen from outside the boat -
                                 // this flips the physical sweep direction without needing to
                                 // remount the sensor or change any of the steering/radar logic

const char* AP_SSID     = "AQUA-SWEEP";
const char* AP_PASSWORD = "trash1234";   // WPA2 needs 8+ characters

// ---------------------------------------------------------------
// PIN DEFINITIONS
// ---------------------------------------------------------------
#define CONV_ENA 25
#define CONV_IN1 26
#define CONV_IN2 27

#define PROP_ENB 14
#define PROP_IN3 33
#define PROP_IN4 32

#define RUDDER_SERVO_PIN 18
#define SCAN_SERVO_PIN   13

#define TRIG_PIN 21
#define ECHO_PIN 22

#define IR_LEFT_PIN  19
#define IR_RIGHT_PIN 23

#define DS18B20_PIN 4
#define BATTERY_ADC_PIN 36

#define PWM_FREQ 5000
#define PWM_RES  8

// ---------------------------------------------------------------
// GLOBAL OBJECTS
// ---------------------------------------------------------------
WebServer server(80);
Servo rudderServo;
#if USE_SCAN_SERVO
Servo scanServo;
#endif
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);

// ---------------------------------------------------------------
// STATE
// ---------------------------------------------------------------
enum Mode { MODE_MANUAL, MODE_SEMI_AUTO, MODE_FULLY_AUTO };
enum BoatState { STATE_IDLE, STATE_SEARCHING, STATE_TRACKING, STATE_COLLECTING, STATE_AVOIDING, STATE_STOPPED };
enum ConveyorMode { CONV_OFF, CONV_ON, CONV_AUTO };

Mode currentMode = MODE_MANUAL;
BoatState currentState = STATE_IDLE;
ConveyorMode conveyorMode = CONV_OFF;

int propulsionSpeedPercent = 0;
bool propulsionForward = true;
int conveyorSpeedPercent = 60;   // slider default
bool conveyorRunning = false;

int rudderAngle = 90;   // 90 = center, range 45-135
int scanAngle = 90;
bool autoScan = false;
int scanDir = 1;

float lastDistanceM = -1;
bool leftIRTriggered = false;
bool rightIRTriggered = false;
float waterTempC = -999;
float batteryVoltage = 0;
int batteryPercent = -1;   // -1 = unknown/not wired  // this part was kept into hault since because of some anomalies in wiring wires got heated up and melted 

bool manualForward = false, manualBackward = false, manualLeft = false, manualRight = false;
bool emergencyStopped = false;

unsigned long missionStartMillis = 0;
unsigned long lastTempReadMillis = 0;
unsigned long lastScanStepMillis = 0;
unsigned long collectingStartMillis = 0;

const int SCAN_MIN = 20, SCAN_MAX = 160, SCAN_STEP = 3, SCAN_INTERVAL_MS = 40;
const float DETECTION_RANGE_M = 0.9;
const float COLLECTION_ZONE_M = 0.20;
const unsigned long COLLECTION_TIMEOUT_MS = 6000;

String logBuffer[6];
int logCount = 0;
void pushLog(String msg) {
  for (int i = 5; i > 0; i--) logBuffer[i] = logBuffer[i-1];
  logBuffer[0] = msg;
  if (logCount < 6) logCount++;
}

// ---------------------------------------------------------------
// MOTOR CONTROL
// ---------------------------------------------------------------
void applyConveyor(int speedPercent) {
  speedPercent = constrain(speedPercent, 0, 100);
  conveyorRunning = speedPercent > 0;
  digitalWrite(CONV_IN1, speedPercent > 0 ? HIGH : LOW);
  digitalWrite(CONV_IN2, LOW);
  ledcWrite(CONV_ENA, map(speedPercent, 0, 100, 0, 255));
}

void setPropulsion(int speedPercent, bool forward) {
  speedPercent = constrain(speedPercent, 0, 100);
  propulsionSpeedPercent = speedPercent;
  propulsionForward = forward;
  int pwmVal = map(speedPercent, 0, 100, 0, 255);

  if (speedPercent == 0) {
    digitalWrite(PROP_IN3, LOW);
    digitalWrite(PROP_IN4, LOW);
  } else if (forward) {
    digitalWrite(PROP_IN3, HIGH);
    digitalWrite(PROP_IN4, LOW);
  } else {
    digitalWrite(PROP_IN3, LOW);
    digitalWrite(PROP_IN4, HIGH);
  }
  ledcWrite(PROP_ENB, pwmVal);
}

void stopPropulsion() { setPropulsion(0, true); }

void setRudder(int angle) {
  rudderAngle = constrain(angle, 45, 135);   // logical angle - what the UI/telemetry shows
#if RUDDER_REVERSED
  int physicalAngle = 180 - rudderAngle;     // mirror around center (90) to flip direction
#else
  int physicalAngle = rudderAngle;
#endif
  physicalAngle += RUDDER_TRIM;              // mechanical centering correction
  physicalAngle = constrain(physicalAngle, 0, 180);
  rudderServo.write(physicalAngle);
}
void centerRudder() { setRudder(90); }

void setScanAngle(int angle) {
#if USE_SCAN_SERVO
  scanAngle = constrain(angle, SCAN_MIN, SCAN_MAX);   // logical angle - true real-world side
  int physical = SCAN_REVERSED ? (SCAN_MIN + SCAN_MAX - scanAngle) : scanAngle;
  scanServo.write(physical);
#else
  scanAngle = 90;  // fixed forward - no servo installed yet
#endif
}

// ---------------------------------------------------------------
// SENSORS
// ---------------------------------------------------------------
float readUltrasonicM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1;
  float distanceCM = duration * 0.0343 / 2.0;
  return distanceCM / 100.0;
}

void readIRSensors() {
  leftIRTriggered  = (digitalRead(IR_LEFT_PIN) == LOW);
  rightIRTriggered = (digitalRead(IR_RIGHT_PIN) == LOW);
}

void readTemperatureIfDue() {
  if (millis() - lastTempReadMillis > 2000) {
    tempSensor.requestTemperatures();
    float t = tempSensor.getTempCByIndex(0);
    if (t != DEVICE_DISCONNECTED_C) waterTempC = t;
    lastTempReadMillis = millis();
  }
}

void readBattery() {
#if USE_BATTERY_SENSE
  int raw = analogRead(BATTERY_ADC_PIN);
  float vAdc = (raw / 4095.0) * 3.3;
  batteryVoltage = vAdc * (37.5 / 7.5);  // 30k+7.5k divider - to be adjusted if different
  batteryPercent = constrain(map((long)(batteryVoltage * 100), 1050, 1260, 0, 100), 0, 100);
#else
  batteryVoltage = 0;
  batteryPercent = -1;   // report as "N/A" on the dashboard
#endif
}

void stepAutoScan() {
#if USE_SCAN_SERVO
  if (!autoScan) return;
  if (millis() - lastScanStepMillis < SCAN_INTERVAL_MS) return;
  lastScanStepMillis = millis();
  scanAngle += scanDir * SCAN_STEP;
  if (scanAngle >= SCAN_MAX) { scanAngle = SCAN_MAX; scanDir = -1; }
  if (scanAngle <= SCAN_MIN) { scanAngle = SCAN_MIN; scanDir = 1; }
  int physical = SCAN_REVERSED ? (SCAN_MIN + SCAN_MAX - scanAngle) : scanAngle;
  scanServo.write(physical);
#endif
}

// ---------------------------------------------------------------
// SELF TEST
// ---------------------------------------------------------------
void runSelfTest() {
  Serial.println("=== AQUA-SWEEP Self-Test ===");

  Serial.print("Ultrasonic: ");
  float d = readUltrasonicM();
  Serial.println(d > 0 ? String(d) + " m OK" : "WARN - no echo");

  Serial.print("IR sensors: ");
  readIRSensors();
  Serial.println("L=" + String(leftIRTriggered) + " R=" + String(rightIRTriggered));

  Serial.print("Temperature: ");
  tempSensor.requestTemperatures();
  float t = tempSensor.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) {
    Serial.println("FAIL - DS18B20 not detected");
  } else {
    Serial.println(String(t) + " C OK");
    waterTempC = t;
  }

  Serial.print("Rudder servo: ");
  rudderServo.write(70); delay(250);
  rudderServo.write(110); delay(250);
  centerRudder(); delay(250);
  Serial.println("OK");

#if USE_SCAN_SERVO
  Serial.print("Scan servo: ");
  scanServo.write(60); delay(250);
  scanServo.write(120); delay(250);
  scanServo.write(90); delay(250);
  Serial.println("OK");
#else
  Serial.println("Scan servo: SKIPPED (USE_SCAN_SERVO = 0, not installed yet)");
#endif

  Serial.print("Motor quick-pulse: ");
  setPropulsion(30, true); delay(200); stopPropulsion();
  applyConveyor(40); delay(200); applyConveyor(0);
  Serial.println("OK (visually confirm both motors responded)");

  Serial.println("=== SELF-TEST COMPLETE ===");
  pushLog("System started - self-test complete");
}

// ---------------------------------------------------------------
// OBSTACLE AVOIDANCE (SEMI-AUTO and FULLY AUTO)
// ---------------------------------------------------------------
bool applyObstacleAvoidance() {
  readIRSensors();
  if (leftIRTriggered && rightIRTriggered) {
    if (currentState != STATE_AVOIDING) pushLog("Obstacle both sides - stopping");
    currentState = STATE_AVOIDING;
    stopPropulsion();
    centerRudder();
    return true;
  } else if (leftIRTriggered) {
    if (currentState != STATE_AVOIDING) pushLog("Obstacle detected (left) - steering right");
    currentState = STATE_AVOIDING;
    setRudder(130);
    setPropulsion(40, true);
    return true;
  } else if (rightIRTriggered) {
    if (currentState != STATE_AVOIDING) pushLog("Obstacle detected (right) - steering left");
    currentState = STATE_AVOIDING;
    setRudder(50);
    setPropulsion(40, true);
    return true;
  }
  return false;
}

// ---------------------------------------------------------------
// FULLY AUTO STATE 
// ---------------------------------------------------------------
void updateFullyAuto() {
  if (applyObstacleAvoidance()) return;

  switch (currentState) {
    case STATE_SEARCHING:
    case STATE_IDLE: {
      currentState = STATE_SEARCHING;
      stepAutoScan();
      float d = readUltrasonicM();
      if (d > 0) lastDistanceM = d;
      centerRudder();
      setPropulsion(70, true);
      applyConveyor(0);

      if (d > 0 && d < DETECTION_RANGE_M) {
        currentState = STATE_TRACKING;
        pushLog("Trash detected at " + String(d,2) + " m");
      }
      break;
    }
    case STATE_TRACKING: {
      int steer = map(scanAngle, SCAN_MIN, SCAN_MAX, 50, 130);
      setRudder(steer);
      setPropulsion(50, true);
      float d = readUltrasonicM();
      if (d > 0) lastDistanceM = d;

      if (d < 0 || d > DETECTION_RANGE_M * 1.4) {
        currentState = STATE_SEARCHING;
      } else if (d < COLLECTION_ZONE_M) {
        currentState = STATE_COLLECTING;
        collectingStartMillis = millis();
        applyConveyor(conveyorSpeedPercent > 0 ? conveyorSpeedPercent : 60);
        pushLog("Collection zone reached - conveyor ON");
      }
      break;
    }
    case STATE_COLLECTING: {
      setPropulsion(30, true);
      float d = readUltrasonicM();
      if (d > 0) lastDistanceM = d;
      bool timedOut = (millis() - collectingStartMillis) > COLLECTION_TIMEOUT_MS;
      bool gone = (d < 0 || d > COLLECTION_ZONE_M * 2.5);
      if (timedOut || gone) {
        applyConveyor(0);
        currentState = STATE_SEARCHING;
        pushLog("Collection complete - resuming patrol");
      }
      break;
    }
    case STATE_AVOIDING:
      currentState = STATE_SEARCHING;
      break;
    default:
      stopPropulsion();
      applyConveyor(0);
      break;
  }
}

// ---------------------------------------------------------------
// MANUAL / SEMI-AUTO CONTROL
// ---------------------------------------------------------------
void updateManualControl(bool semiAuto) {
  if (semiAuto && applyObstacleAvoidance()) return;

  float d = readUltrasonicM();
  if (d > 0) lastDistanceM = d;

  if (manualForward) setPropulsion(propulsionSpeedPercent > 0 ? propulsionSpeedPercent : 70, true);
  else if (manualBackward) setPropulsion(propulsionSpeedPercent > 0 ? propulsionSpeedPercent : 70, false);
  else stopPropulsion();

  if (manualLeft) setRudder(50);
  else if (manualRight) setRudder(130);
  else centerRudder();

  if (conveyorMode == CONV_ON) applyConveyor(conveyorSpeedPercent);
  else if (conveyorMode == CONV_OFF) applyConveyor(0);
  // CONV_AUTO under manual/semi-auto: activate only when something is close
  else if (conveyorMode == CONV_AUTO) applyConveyor(d > 0 && d < COLLECTION_ZONE_M ? conveyorSpeedPercent : 0);
}

// ---------------------------------------------------------------
// DASHBOARD HTML (horizontal / landscape layout)
// ---------------------------------------------------------------
const char DASHBOARD_HTML[] PROGMEM = R"HTMLPAGE(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>AQUA-SWEEP</title>
<style>
  * { box-sizing: border-box; }
  body {
    margin:0; font-family:'Segoe UI', Arial, sans-serif; background:#f3ecd8;
    color:#2b2b2b; padding:10px;
  }
  .grid {
    display:grid;
    grid-template-columns: 220px 1fr 220px;
    grid-template-rows: auto auto auto auto;
    gap:10px;
    grid-template-areas:
      "top top top"
      "left center right"
      "bottom bottom bottom"
      "telemetry telemetry telemetry";
    max-width:1200px; margin:0 auto;
  }
  @media (max-width:820px) {
    .grid { grid-template-columns: 1fr; grid-template-areas:
      "top" "center" "left" "right" "bottom" "telemetry"; }
  }
  .card {
    background:#fffaf0; border:1px solid #e3d8ba; border-radius:12px;
    padding:12px; box-shadow:0 2px 4px rgba(0,0,0,0.06);
  }
  .top { grid-area:top; display:flex; justify-content:space-between; align-items:center; flex-wrap:wrap; gap:10px; }
  .left { grid-area:left; }
  .center { grid-area:center; }
  .right { grid-area:right; }
  .bottom { grid-area:bottom; display:grid; grid-template-columns: 1fr 1.4fr 1fr; gap:10px; }
  @media (max-width:820px) { .bottom { grid-template-columns:1fr; } }
  .telemetry { grid-area:telemetry; display:flex; flex-wrap:wrap; gap:14px; font-size:12px; justify-content:space-between; }

  h1 { font-size:18px; margin:0; letter-spacing:1px; }
  .sub { font-size:11px; color:#8a7f5e; }
  .badge { padding:4px 10px; border-radius:14px; font-size:12px; font-weight:bold; }
  .badge.green { background:#d7efd7; color:#1e7a1e; }
  .badge.yellow { background:#fdf1cf; color:#8a6d00; }
  .badge.red { background:#fbdada; color:#a11; }
  .badge.grey { background:#e6e2d4; color:#666; }

  .distance-big { font-size:34px; font-weight:bold; text-align:center; }
  .label { font-size:11px; color:#8a7f5e; text-transform:uppercase; letter-spacing:0.5px; }

  .radarWrap { display:flex; flex-direction:column; align-items:center; }
  svg#radar { width:100%; max-width:340px; height:190px; }

  .irbox { text-align:center; padding:10px; border-radius:10px; }
  .irbox.clear { background:#e9f7e9; }
  .irbox.blocked { background:#fbe1e1; }

  input[type=range] { width:100%; }

  .padBtn {
    background:#2b3a52; color:white; border:none; border-radius:8px;
    padding:14px; font-size:13px; font-weight:bold; user-select:none;
  }
  .padBtn:active { background:#1a2740; }
  .dirGrid {
    display:grid; grid-template-columns:1fr 1fr 1fr; grid-template-rows:auto auto auto;
    gap:6px; text-align:center;
  }
  .dirGrid .f { grid-column:2; grid-row:1; }
  .dirGrid .l { grid-column:1; grid-row:2; }
  .dirGrid .c { grid-column:2; grid-row:2; background:#a11; }
  .dirGrid .r { grid-column:3; grid-row:2; }
  .dirGrid .b { grid-column:2; grid-row:3; }

  .modeBtn { background:#5a4b2c; }
  .modeBtn.active { background:#c98a1b; }

  .estop {
    background:#c62828; color:white; border:none; border-radius:14px;
    font-size:22px; font-weight:bold; width:100%; height:100%;
    min-height:90px; letter-spacing:1px;
  }
  .estop:active { background:#8e0000; }

  .sliderRow { display:flex; align-items:center; gap:8px; }
  .convBtnRow { display:flex; gap:6px; margin-top:6px; }
  .convBtnRow button { flex:1; padding:8px; border-radius:8px; border:none; background:#e6e2d4; font-size:11px; font-weight:bold; }
  .convBtnRow button.active { background:#c98a1b; color:white; }

  .logbox { font-size:11px; max-height:70px; overflow-y:auto; }
  .logbox div { padding:2px 0; border-bottom:1px dashed #e3d8ba; }

  .note { font-size:10px; color:#a08a4a; text-align:center; margin-top:4px; }
</style>
</head>
<body>
<div class="grid">

  <div class="card top">
    <div>
      <h1>AQUA-SWEEP</h1>
      <div class="sub">FLOATING TRASH COLLECTOR BOAT</div>
    </div>
    <div id="battBadge" class="badge grey">BATTERY: N/A</div>
    <div id="wifiBadge" class="badge green">WI-FI: CONNECTED</div>
    <div id="modeBadge" class="badge yellow">MODE: MANUAL</div>
  </div>

  <div class="card left">
    <div class="label">Left IR Sensor</div>
    <div id="leftIRBox" class="irbox clear">CLEAR</div>
    <div style="margin-top:14px;" class="label">Conveyor Speed</div>
    <div class="sliderRow">
      <input type="range" min="0" max="100" value="60" id="convSlider" oninput="conveyorSpeed(this.value)">
      <span id="convSpeedVal">60%</span>
    </div>
    <div class="label" style="margin-top:6px;">Conveyor Status: <b id="convStatus">OFF</b></div>
    <div class="convBtnRow">
      <button id="cv-OFF" onclick="conveyorMode('OFF')">OFF</button>
      <button id="cv-ON" onclick="conveyorMode('ON')">ON</button>
      <button id="cv-AUTO" onclick="conveyorMode('AUTO')">AUTO</button>
    </div>
  </div>

  <div class="card center">
    <div class="label" style="text-align:center;">Ultrasonic Surveillance (Front)</div>
    <div class="radarWrap">
      <svg id="radar" viewBox="0 0 340 190">
        <path d="M20,180 A160,160 0 0,1 320,180" fill="none" stroke="#dcd3ae" stroke-width="1"/>
        <path d="M55,180 A125,125 0 0,1 285,180" fill="none" stroke="#dcd3ae" stroke-width="1"/>
        <path d="M90,180 A90,90 0 0,1 250,180" fill="none" stroke="#dcd3ae" stroke-width="1"/>
        <path d="M125,180 A55,55 0 0,1 215,180" fill="none" stroke="#dcd3ae" stroke-width="1"/>
        <line id="sweepLine" x1="170" y1="180" x2="170" y2="20" stroke="#c98a1b" stroke-width="2"/>
        <circle id="hit" cx="170" cy="180" r="6" fill="#43a047"/>
      </svg>
      <div class="distance-big"><span id="distVal">--</span> m</div>
      <div id="distStatus" class="badge green">CLEAR</div>
      <div class="note" id="scanNote">Scanner servo not yet installed - showing fixed forward reading</div>
    </div>
    <div class="convBtnRow" style="margin-top:8px;">
      <button onclick="scanCmd('left')">&laquo; LEFT</button>
      <button onclick="scanCmd('center')">CENTER</button>
      <button onclick="scanCmd('right')">RIGHT &raquo;</button>
      <button id="autoscanBtn" onclick="toggleAutoscan()">AUTO SCAN</button>
    </div>
  </div>

  <div class="card right">
    <div class="label">Right IR Sensor</div>
    <div id="rightIRBox" class="irbox clear">CLEAR</div>
    <div style="margin-top:14px;" class="label">Water Temperature</div>
    <div class="distance-big"><span id="tempVal">--</span>&deg;C</div>
    <div id="tempStatus" class="badge green" style="display:block; text-align:center;">NORMAL</div>
    <div style="margin-top:14px;" class="label">Scanner Angle</div>
    <div style="text-align:center; font-size:20px; font-weight:bold;"><span id="scanAngleVal">90</span>&deg;</div>
  </div>

  <div class="card bottom">
    <div style="display:flex; align-items:stretch;">
      <button class="estop" ontouchstart="estop()" onclick="estop()">&#9888;<br>EMERGENCY<br>STOP</button>
    </div>

    <div>
      <div class="label" style="text-align:center;">Propulsion Speed</div>
      <div class="distance-big" style="text-align:center;"><span id="propSpeedVal">0</span>%</div>
      <div class="dirGrid" style="margin-top:8px;">
        <button class="padBtn f" ontouchstart="hold('fwd',1)" ontouchend="hold('fwd',0)"
          onmousedown="hold('fwd',1)" onmouseup="hold('fwd',0)">FORWARD</button>
        <button class="padBtn l" ontouchstart="hold('left',1)" ontouchend="hold('left',0)"
          onmousedown="hold('left',1)" onmouseup="hold('left',0)">LEFT</button>
        <button class="padBtn c" onclick="stopAll()">STOP</button>
        <button class="padBtn r" ontouchstart="hold('right',1)" ontouchend="hold('right',0)"
          onmousedown="hold('right',1)" onmouseup="hold('right',0)">RIGHT</button>
        <button class="padBtn b" ontouchstart="hold('back',1)" ontouchend="hold('back',0)"
          onmousedown="hold('back',1)" onmouseup="hold('back',0)">REVERSE</button>
      </div>
      <div class="label" style="text-align:center; margin-top:8px;">Rudder: <span id="rudderVal">90</span>&deg;</div>
    </div>

    <div>
      <div class="label" style="text-align:center;">Mode Select</div>
      <button class="padBtn modeBtn" id="mode-SEMI" style="width:100%; margin-top:6px;" onclick="setMode('SEMI')">SEMI-AUTO</button>
      <button class="padBtn modeBtn" id="mode-AUTO" style="width:100%; margin-top:6px;" onclick="setMode('AUTO')">FULLY AUTO</button>
      <button class="padBtn modeBtn" id="mode-MANUAL" style="width:100%; margin-top:6px;" onclick="setMode('MANUAL')">MANUAL</button>
      <div class="label" style="margin-top:10px;">Boat Status: <b id="boatStateVal">IDLE</b></div>
    </div>
  </div>

  <div class="card telemetry">
    <div><b>Speed:</b> <span id="t-speed">0</span>%</div>
    <div><b>Rudder:</b> <span id="t-rudder">90</span>&deg;</div>
    <div><b>Scanner:</b> <span id="t-scan">90</span>&deg;</div>
    <div><b>Temp:</b> <span id="t-temp">--</span>C</div>
    <div><b>Distance:</b> <span id="t-dist">--</span>m</div>
    <div><b>Left IR:</b> <span id="t-lir">CLEAR</span></div>
    <div><b>Right IR:</b> <span id="t-rir">CLEAR</span></div>
    <div><b>Conveyor:</b> <span id="t-conv">OFF</span></div>
    <div><b>Mode:</b> <span id="t-mode">MANUAL</span></div>
    <div style="flex-basis:100%;" class="logbox" id="logBox"></div>
  </div>

</div>

<script>
let convModeState = 'OFF';
let autoscanState = false;

function cmd(action, val) {
  fetch('/cmd?action=' + action + '&val=' + val);
}
function hold(action, val) { cmd(action, val); }
function stopAll() { cmd('fwd',0); cmd('back',0); cmd('left',0); cmd('right',0); }
function estop() { cmd('estop',1); }
function setMode(m) { cmd('mode', m); }
function conveyorSpeed(v) {
  document.getElementById('convSpeedVal').innerText = v + '%';
  cmd('conveyorSpeed', v);
}
function conveyorMode(m) {
  convModeState = m;
  cmd('conveyorMode', m);
  ['OFF','ON','AUTO'].forEach(x => document.getElementById('cv-'+x).classList.toggle('active', x===m));
}
function scanCmd(dir) { cmd('scan', dir); }
function toggleAutoscan() {
  autoscanState = !autoscanState;
  cmd('autoscan', autoscanState ? 1 : 0);
  document.getElementById('autoscanBtn').classList.toggle('active', autoscanState);
}

function badgeClass(kind) {
  if (kind === 'clear') return 'badge green';
  if (kind === 'warn') return 'badge yellow';
  return 'badge red';
}

function refresh() {
  fetch('/data').then(r => r.json()).then(d => {
    // top bar
    document.getElementById('modeBadge').innerText = 'MODE: ' + d.mode;
    document.getElementById('battBadge').innerText = d.batteryPct >= 0 ? ('BATTERY: ' + d.batteryPct + '%') : 'BATTERY: N/A';

    // left panel
    document.getElementById('leftIRBox').innerText = d.leftIR ? 'BLOCKED' : 'CLEAR';
    document.getElementById('leftIRBox').className = 'irbox ' + (d.leftIR ? 'blocked' : 'clear');
    document.getElementById('convStatus').innerText = d.conveyor ? 'ON (' + d.conveyorSpeed + '%)' : 'OFF';

    // right panel
    document.getElementById('rightIRBox').innerText = d.rightIR ? 'BLOCKED' : 'CLEAR';
    document.getElementById('rightIRBox').className = 'irbox ' + (d.rightIR ? 'blocked' : 'clear');
    document.getElementById('tempVal').innerText = d.temp.toFixed(1);
    let tStat = (d.temp < 15 || d.temp > 35) ? 'warn' : 'clear';
    document.getElementById('tempStatus').innerText = tStat==='clear' ? 'NORMAL' : 'CHECK';
    document.getElementById('tempStatus').className = badgeClass(tStat);
    document.getElementById('scanAngleVal').innerText = d.scanAngle;

    // center radar
    document.getElementById('distVal').innerText = d.distance >= 0 ? d.distance.toFixed(2) : '--';
    let zone = 'clear';
    if (d.distance >= 0 && d.distance < 0.20) zone = 'obstacle';
    else if (d.distance >= 0 && d.distance < 0.9) zone = 'trash';
    let ds = document.getElementById('distStatus');
    let hit = document.getElementById('hit');
    if (zone === 'obstacle') { ds.innerText='OBSTACLE'; ds.className='badge red'; hit.setAttribute('fill','#c62828'); }
    else if (zone === 'trash') { ds.innerText='TRASH DETECTED'; ds.className='badge yellow'; hit.setAttribute('fill','#c98a1b'); }
    else { ds.innerText='CLEAR'; ds.className='badge green'; hit.setAttribute('fill','#43a047'); }

    // sweep line rotation: map scanAngle(20-160) to svg angle
    let a = d.scanAngle;
    let rad = (180 - a) * Math.PI / 180;
    let cx=170, cy=180, len=155;
    let x2 = cx + len*Math.cos(rad);
    let y2 = cy - len*Math.sin(rad)*0.9;
    document.getElementById('sweepLine').setAttribute('x2', x2);
    document.getElementById('sweepLine').setAttribute('y2', y2);
    let hitDist = d.distance >= 0 ? Math.min(d.distance/2.0, 1) : 1;
    document.getElementById('hit').setAttribute('cx', cx + (x2-cx)*hitDist);
    document.getElementById('hit').setAttribute('cy', cy + (y2-cy)*hitDist);
    document.getElementById('scanNote').style.display = d.scannerInstalled ? 'none' : 'block';

    // bottom
    document.getElementById('propSpeedVal').innerText = d.speed;
    document.getElementById('rudderVal').innerText = d.rudder;
    document.getElementById('boatStateVal').innerText = d.state;
    ['SEMI','AUTO','MANUAL'].forEach(m => document.getElementById('mode-'+m).classList.toggle('active', d.mode===m));

    // telemetry strip
    document.getElementById('t-speed').innerText = d.speed;
    document.getElementById('t-rudder').innerText = d.rudder;
    document.getElementById('t-scan').innerText = d.scanAngle;
    document.getElementById('t-temp').innerText = d.temp.toFixed(1);
    document.getElementById('t-dist').innerText = d.distance >= 0 ? d.distance.toFixed(2) : '--';
    document.getElementById('t-lir').innerText = d.leftIR ? 'BLOCKED' : 'CLEAR';
    document.getElementById('t-rir').innerText = d.rightIR ? 'BLOCKED' : 'CLEAR';
    document.getElementById('t-conv').innerText = d.conveyor ? 'ON' : 'OFF';
    document.getElementById('t-mode').innerText = d.mode;

    // log
    let lb = document.getElementById('logBox');
    lb.innerHTML = d.log.map(l => '<div>' + l + '</div>').join('');
  });
}
setInterval(refresh, 150);
refresh();
</script>
</body>
</html>
)HTMLPAGE";

// ---------------------------------------------------------------
// WEB HANDLERS
// ---------------------------------------------------------------
void handleRoot() { server.send_P(200, "text/html", DASHBOARD_HTML); }

String modeName(Mode m) {
  if (m == MODE_FULLY_AUTO) return "AUTO";
  if (m == MODE_SEMI_AUTO) return "SEMI";
  return "MANUAL";
}
String stateName(BoatState s) {
  switch (s) {
    case STATE_SEARCHING: return "SEARCHING";
    case STATE_TRACKING: return "TRACKING";
    case STATE_COLLECTING: return "COLLECTING";
    case STATE_AVOIDING: return "AVOIDING";
    case STATE_STOPPED: return "STOPPED";
    default: return "IDLE";
  }
}

void handleData() {
  String json = "{";
  json += "\"mode\":\"" + modeName(currentMode) + "\",";
  json += "\"state\":\"" + stateName(currentState) + "\",";
  json += "\"speed\":" + String(propulsionSpeedPercent) + ",";
  json += "\"rudder\":" + String(rudderAngle) + ",";
  json += "\"scanAngle\":" + String(scanAngle) + ",";
  json += "\"scannerInstalled\":" + String(USE_SCAN_SERVO ? "true" : "false") + ",";
  json += "\"distance\":" + String(lastDistanceM, 3) + ",";
  json += "\"leftIR\":" + String(leftIRTriggered ? "true" : "false") + ",";
  json += "\"rightIR\":" + String(rightIRTriggered ? "true" : "false") + ",";
  json += "\"temp\":" + String(waterTempC, 1) + ",";
  json += "\"conveyor\":" + String(conveyorRunning ? "true" : "false") + ",";
  json += "\"conveyorSpeed\":" + String(conveyorSpeedPercent) + ",";
  json += "\"batteryPct\":" + String(batteryPercent) + ",";
  json += "\"log\":[";
  for (int i = 0; i < logCount; i++) {
    json += "\"" + logBuffer[i] + "\"";
    if (i < logCount - 1) json += ",";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void handleCmd() {
  String action = server.arg("action");
  String valStr = server.arg("val");
  int val = valStr.toInt();

  if (action == "mode") {
    if (valStr == "AUTO") { currentMode = MODE_FULLY_AUTO; autoScan = true; conveyorMode = CONV_AUTO; }
    else if (valStr == "SEMI") { currentMode = MODE_SEMI_AUTO; }
    else { currentMode = MODE_MANUAL; autoScan = false; }
    currentState = STATE_SEARCHING;
    emergencyStopped = false;
    pushLog("Mode changed to " + valStr);
  } else if (action == "fwd") {
    manualForward = val; manualBackward = false;
  } else if (action == "back") {
    manualBackward = val; manualForward = false;
  } else if (action == "left") {
    manualLeft = val; manualRight = false;
  } else if (action == "right") {
    manualRight = val; manualLeft = false;
  } else if (action == "conveyorMode") {
    if (valStr == "ON") conveyorMode = CONV_ON;
    else if (valStr == "AUTO") conveyorMode = CONV_AUTO;
    else conveyorMode = CONV_OFF;
  } else if (action == "conveyorSpeed") {
    conveyorSpeedPercent = constrain(val, 0, 100);
  } else if (action == "scan") {
    if (valStr == "left") setScanAngle(SCAN_MIN);
    else if (valStr == "right") setScanAngle(SCAN_MAX);
    else setScanAngle(90);
  } else if (action == "autoscan") {
    autoScan = (val == 1);
  } else if (action == "estop") {
    emergencyStopped = true;
    stopPropulsion();
    applyConveyor(0);
    centerRudder();
    currentMode = MODE_MANUAL;
    currentState = STATE_STOPPED;
    conveyorMode = CONV_OFF;
    autoScan = false;
    pushLog("EMERGENCY STOP triggered");
  }
  server.send(200, "text/plain", "OK");
}

// ---------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(300);

  // IMPORTANT: reserve dedicated PWM timers for the servos FIRST, before
  // any other LEDC/PWM usage (the motor speed control below). Without this,
  // the motor PWM setup can silently take over the timers the servo
  // library needs, leaving the servos powered and wired correctly but
  // receiving no valid signal - which is exactly the symptom you saw.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);

  rudderServo.setPeriodHertz(50);
  rudderServo.attach(RUDDER_SERVO_PIN, 500, 2400);
  centerRudder();

#if USE_SCAN_SERVO
  scanServo.setPeriodHertz(50);
  scanServo.attach(SCAN_SERVO_PIN, 500, 2400);
  scanServo.write(90);
#endif

  pinMode(CONV_IN1, OUTPUT); pinMode(CONV_IN2, OUTPUT);
  pinMode(PROP_IN3, OUTPUT); pinMode(PROP_IN4, OUTPUT);
  ledcAttach(CONV_ENA, PWM_FREQ, PWM_RES);
  ledcAttach(PROP_ENB, PWM_FREQ, PWM_RES);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_LEFT_PIN, INPUT);
  pinMode(IR_RIGHT_PIN, INPUT);

  tempSensor.begin();

  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println("Access Point started.");
  Serial.print("Connect your device to Wi-Fi: "); Serial.println(AP_SSID);
  Serial.print("Dashboard: http://"); Serial.println(WiFi.softAPIP());

  if (MDNS.begin("boat")) {
    Serial.println("mDNS active: http://boat.local");
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/cmd", handleCmd);
  server.begin();

  runSelfTest();
  missionStartMillis = millis();
  currentState = STATE_IDLE;
}

// ---------------------------------------------------------------
// MAIN LOOP
// ---------------------------------------------------------------
void loop() {
  server.handleClient();
  readTemperatureIfDue();
  readBattery();

  if (emergencyStopped) {
    delay(20);
    return;
  }

  switch (currentMode) {
    case MODE_FULLY_AUTO:
      updateFullyAuto();
      break;
    case MODE_SEMI_AUTO:
      updateManualControl(true);
      break;
    case MODE_MANUAL:
    default:
      updateManualControl(false);
      break;
  }

  delay(15);
}

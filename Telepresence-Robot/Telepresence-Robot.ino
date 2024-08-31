#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

bool followMode = false;
bool toggleFollowModeFlag = false;
const int obstacleDistance = 30;
unsigned long previousMillis = 0;
const long interval = 15;
int currentServoPos = 90;  
bool autonomousMode = false;
const char* webPassword = "novabot"; // Set desired for interface password
const char* webUsername = "novabot";  // Replace with your desired username

//const char *ngroksubdomain = "8cc2-154-160-3-117";  // Leave this empty


const char *ssid = "Galaxy A20s4798";    
const char *password = "sygl5773"; 


ESP8266WebServer server(80);

// Motor pins 
const int motor1Pin1 = D1;
const int motor1Pin2 = D2;
const int motor2Pin1 = D3;
const int motor2Pin2 = D4;
int servoSpeed = 15;

//servopin
const int servoPin = D0;
// Speed control pins
const int motor1SpeedPin = D5;
const int motor2SpeedPin = D6;

Servo myservo;



// Ultrasonic sensor pins
const int trigPin = D7;
const int echoPin = D8;



int speed = 0;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Ngrok URL

  // Start the server
  

  Serial.println("Server started");



  // Set motor pins as output
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
 pinMode(motor1SpeedPin, OUTPUT);
  pinMode(motor2SpeedPin, OUTPUT);
  

  // Set ultrasonic sensor pins as input/output
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

 

  // Attach servo to the servo pin
  myservo.attach(servoPin);

  // Start the web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/move/forward", handleMoveForward);
  server.on("/move/backward", handleMoveBackward);
  server.on("/move/left", handleMoveLeft);
  server.on("/move/right", handleMoveRight);

  server.on("/move/stop", handleStop);
  server.on("/servo/left", HTTP_GET, handleServoLeft);
  server.on("/servo/right", HTTP_GET, handleServoRight);
  server.on("/servo/middle", HTTP_GET, handleServoCenter);


/*   server.on("/led/toggle", handleToggleLED);
 */  server.on("/speed", handleSpeed);
  server.on("/toggleFollowMode", HTTP_GET, toggleFollowMode);
  server.on("/toggleAutonomousMode", HTTP_GET, toggleAutonomousMode);


  server.onNotFound(handleNotFound);
  server.begin(80); // Use the port number your server will listen on
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (toggleFollowModeFlag) {
    toggleFollowModeFlag = false;
    toggleFollowMode();
  }

  if (followMode) {
    followPerson();
  }
  if (autonomousMode) {
    autonomousNavigate();
  }
    


}



void checkObstacleAndStop() {
  if (isPersonDetected()) {
    stopRobot(); // Stop all motors if an obstacle is detected
  }
}

// ... (Rest of the code remains the same)
void handleRoot() {
  if (!server.authenticate(webUsername, webPassword)) {


    return server.requestAuthentication();
  }
 
  String html = "<html><head>";
  html += "<style>";
      html += ".middle { width: 140px; height: 140px;  background-color: white; border-radius:140px; margin-bottom:31px; }";

  html += "body, html {touch-action: manipulation;  font-family: Arial, sans-serif; position:relative;     background:linear-gradient(rgba(77, 75, 75, 0.925) ,rgba(39, 38, 38, 0.938) );}";
  html += "h1 { text-align: center; font-size: 29pt; color: white; }";
  html += ".name {  font-size: 60pt; color: white; font-weight:bolder;     font-family: 'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif; }";
  html += "@media (min-width: 868px) {";

  html += ".big-name {flex-direction:column; justify-content:center;display:flex; bottom:150;  position:absolute; z-index:-999; width:100%; }";
   html += "}";
  html += "@media (max-width: 1000px) {";
    html += ".big-name {flex-direction:column; justify-content:center;display:flex; bottom:410;  position:absolute; z-index:-999; width:100%; }";

   html += "}";

   html += ".sym {flex-direction:row; justify-content:space-evenly; align-items:center; display:flex;  }";
  html += ".tri { width: 950px; height: 950px; border-radius:950px; background-color:white; justify-content:center;display:flex; align-items:center;}";
    html += ".tri1 { width: 920px; height: 920px; border-radius:920px; background-color:black; border:2px solid white;  justify-content:center;display:flex; align-items:center;}";
        html += ".tri2 { width: 880px; height: 880px; border-radius:880px; background-color:grey; border:2px solid white; justify-content:center;display:flex; align-items:center;}";
        html += ".tri3 { width: 830px; height: 830px; border-radius:830px; background-color:white; border:2px solid white; justify-content:center;display:flex; align-items:center;}";
        html += ".tri4 { width: 780px; height: 780px; border-radius:780px; background-color:black; border:2px solid white; justify-content:center;display:flex; align-items:center;}";
        html += ".tri5 { width: 730px; height: 730px; border-radius:730px; background-color:white; border:2px solid white; justify-content:center;display:flex; align-items:center;}";
        html += ".tri6 { width: 640px; height: 640px; border-radius:640px; background-color:grey; border:2px solid white; justify-content:center;display:flex; align-items:center;}";
        html += ".tri7 { width: 500px; height: 500px; border-radius:500px; background-color:black; border:2px solid white; justify-content:center;display:flex; align-items:center;}";


  html += "* { padding:0; margin:0; box-sizing:border-box; }";


  html += ".navbar { background-color: #333; overflow: hidden; height:90px; display:flex; justify-content:space-between; align-items:center; }";
  html += ".navbar a { float: left; display: block; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; }";
  html += ".navbar a:hover { background-color: #ddd; color: black; }";

 
  html += ".led-button { width: 170px; height: 170px; border-radius:170px; background-color:rgb(231, 231, 24);font-size:30pt;color:white; }";
  html += ".follow-button { width: 170px; height: 170px; border-radius:170px; background-color: green;font-size:30pt;color:white; }";
  html += ".auto-button { width: 170px; height: 170px; border-radius:170px; background-color: black;font-size:30pt;color:white; }";

  html += ".stop { width: 170px; height: 170px; border-radius:170px;  background-color: red;font-size:30pt;color:white; }";
  html += ".forward-back {display: flex; flex-direction: column;  }";
  html += ".left-right {display: flex; width:40%; height:100%;  justify-content: center; }";
  html += ".main-container { touch-action: manipulation; width:100%;  display: flex; flex-direction: column; align-items: center; justify-content: space-between; position:absolute; bottom:0px;  }";
  html += ".button-container { touch-action: manipulation; display: flex;  justify-content:space-evenly;  width:100%; touch-action: manipulation; }";
  html += ".move-buttons { display: flex; flex-direction: row; justify-content:space-between; align-items:center; width:100%; }";
  html += ".speed-range { width: 100%; height: 20px; margin-bottom:21px; }";
  html += ".servo-buttons { display: flex; flex-direction: column;  align-items:center; justify-content:center; }";
  html += ".servo-button.up { width:0; height:0; border-left: 110px solid transparent; border-right: 110px solid transparent; border-bottom: 120px solid white; margin-bottom:28px;  }";
  html += ".servo-button.down { width:0; height:0; border-left: 110px solid transparent; border-right: 110px solid transparent; border-top: 120px solid white;  }";
  html += ".direction-button.up { width: 0; height: 0; border-left: 130px solid transparent; border-right: 130px solid transparent; border-bottom: 130px solid white; margin-bottom:31px;  }";
  html += ".direction-button.down { width: 0; height: 0; border-left: 130px solid transparent; border-right: 130px solid transparent; border-top: 130px solid white;  }";
  html += ".direction-button.left { width: 0; height: 0; border-left: 130px solid transparent; border-right: 130px solid transparent; border-top: 130px solid white; transform:rotate(90deg);  }";
  html += ".direction-button.right { width: 0; height: 0; border-left: 130px solid transparent; border-right: 130px solid transparent; border-top: 130px solid white; transform:rotate(-90deg); }";
  html += "</style>";
  html += "</head><body>";
  
  html += "<div class='navbar'>";
  html += "<h1>N.O.V.A</h1>";

  html += "<div>";
  html += "<a href='#home'>Home</a>";
  html += "<a href='#about'>About</a>";
  html += "<a href='#contact'>Contact</a>";
  html += "</div>";

  // Add more links as needed
  html += "</div>";

  html += "<div class='big-name'>";
  html += "<div class='sym'>";
  html += "<div class='tri' ><div class='tri1' ><div class='tri2' >  <div class='tri3' > <div class='tri4' > <div class='tri5' >   <div class='tri6' >   <div class='tri7' >    <span class='name'>N.O.V.A</span>      </div>       </div>       </div>         </div>         </div>        </div> </div> </div>";

html += "</div>";

  

  //html += "<span class='name'>N.O.V.A</span>";
  
  html += "</div>";
  //html += "<h1>N.O.V.A</h1>";
 


  html += "<div class='main-container'>";
  html += "<div class='button-container'>";
  html += "<button class='stop' onclick=\"move('stop')\">Stop</button>";
  html += "<button class='button follow-button'  onclick=\"toggleFollowMode()\">" + String(followMode ? " Follow " : " Follow ") + "</button>";
   html += "<button class='button auto-button' onclick=\"toggleAutonomousMode()\">" + String(autonomousMode ? " Auto" : " Auto") + "</button>";

  html += "<div class='servo-buttons'>";
 html += "<div class='servo-button up' onclick='moveServo(\"left\")'></div>";
  html += "<div class='middle' onclick='moveServo(\"middle\")'></div>";
  html += "<div class='servo-button down' onclick='moveServo(\"right\")'></div>";
 
  html += "</div>";
  html += "</div>";
  html += "<div class='move-buttons'>";
  html += "<div class='left-right'>";
  html += "<div class='direction-button left' ontouchstart=\"move('left')\" ontouchend=\"move('stop')\" onmousedown=\"move('left')\" onmouseup=\"move('stop')\"></div>";
  html += "</div>";
  html += "<div class='forward-back'>";
  html += "<div class='direction-button up' ontouchstart=\"move('forward')\" ontouchend=\"move('stop')\" onmousedown=\"move('forward')\" onmouseup=\"move('stop')\"></div>";
  html += "<div class='direction-button down' ontouchstart=\"move('backward')\" ontouchend=\"move('stop')\" onmousedown=\"move('backward')\" onmouseup=\"move('stop')\"></div>";
  html += "</div>";
  html += "<div class='left-right'>";
  html += "<div class='direction-button right' ontouchstart=\"move('right')\" ontouchend=\"move('stop')\" onmousedown=\"move('right')\" onmouseup=\"move('stop')\"></div>";
  html += "</div>";
  html += "</div>";
  html += "<input type='range' class='speed-range' id='speedRange' min='0' max='255' step='5' value='0' oninput=\"changeSpeed(this.value)\">";

  html += "</div>";
  html += "</div>";
  html += "<script>";
    html += "function changeServoSpeed(speed) { var xhr = new XMLHttpRequest(); xhr.open('GET', '/servo/speed?value=' + speed, true); xhr.send(); }";

   html += "var commandMap = {";
  html += "  'ArrowUp': '/move/forward',";
  html += "  'ArrowDown': '/move/backward',";
  html += "  'ArrowLeft': '/move/left',";
  html += "  'ArrowRight': '/move/right',";
 
  html += "  'a': '/toggleAutonomousMode',";  // 'a' key for toggling autonomous mode
  html += "  'f': '/toggleFollowMode',";      // 'f' key for toggling follow mode
  html += "};";
  html += "var keyIsPressed = {};"; // Object to track pressed keys
  html += "document.addEventListener('keydown', function(event) {";
  html += "  if (!keyIsPressed[event.key]) {";
  html += "    keyIsPressed[event.key] = true;";
  html += "    handleKeyPress(event.key);";
  html += "  }";
  html += "});";
  html += "document.addEventListener('keyup', function(event) {";
  html += "  keyIsPressed[event.key] = false;";
  html += "  handleKeyRelease(event.key);";
  html += "});";
  html += "function handleKeyPress(key) {";
  html += "  var command = commandMap[key];";
  html += "  if (command) {";
  html += "    fetch(command);";
  html += "  }";
  html += "}";
  html += "function handleKeyRelease(key) {";
  html += "  var stopCommand = '/move/stop';";
  html += "  if (commandMap[key]) {";
  html += "    fetch(stopCommand);";
  html += "  }";
  html += "}";
  html += "function moveServo(direction) {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/servo/' + direction, true);";
  html += "  xhr.send();";
  html += "}";
  html += "document.addEventListener('keydown', function(event) {";
  html += "  if (event.key === 'w') { moveServo('left'); }";
  html += "  else if (event.key === 's') { moveServo('right'); }";
  html += "  else if (event.key === 'q') { moveServo('middle'); }";
  html += "});";
  html += "function move(direction) {";
  html += "var xhttp = new XMLHttpRequest();";
  html += "xhttp.open('GET', '/move/' + direction, true);";
  html += "xhttp.send();";
  html += "}";
  html += "function toggleLED() {";
  html += "var xhttp = new XMLHttpRequest();";
  html += "xhttp.open('GET', '/led/toggle', true);";
  html += "xhttp.send();";
  html += "}";
  html += "function toggleFollowMode() {";
  html += "  fetch('/toggleFollowMode');";
  html += "}";
  html += "function toggleAutonomousMode() {";
  html += "  fetch('/toggleAutonomousMode');";
  html += "}";
  
  html += "function changeSpeed(speed) {";
  html += "var xhttp = new XMLHttpRequest();";
  html += "xhttp.open('GET', '/speed?value=' + speed, true);";
  html += "xhttp.send();";
  html += "}";
 
  html += "function updateDistance(distance) {";
  html += "  document.getElementById('distanceDisplay').innerText = 'Distance: ' + distance + ' cm';";
  html += "}";
  html += "function sendRequest(url) {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', url, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}




void stopRobot() {
  toggleMotors(false, false, false, false, 0);
}

void handleMoveForward() {
  toggleMotors(true, false, true, false, speed);
  server.send(200, "text/plain", "Moving forward");
}

void handleMoveBackward() {
  toggleMotors(false, true, false, true, speed);
  server.send(200, "text/plain", "Moving backward");
}

void handleMoveLeft() {
  toggleMotors(false, true, true, false, speed);
  server.send(200, "text/plain", "Moving left");
}

void handleMoveRight() {
  toggleMotors(true, false, false, true, speed);
  server.send(200, "text/plain", "Moving right");
}

void handleStop() {
  toggleMotors(false, false, false, false, 0);
  server.send(200, "text/plain", "Stopped");
}

void toggleMotors(bool m1a, bool m1b, bool m2a, bool m2b, int speed) {
  digitalWrite(motor1Pin1, m1a ? HIGH : LOW);
  digitalWrite(motor1Pin2, m1b ? HIGH : LOW);
  digitalWrite(motor2Pin1, m2a ? HIGH : LOW);
  digitalWrite(motor2Pin2, m2b ? HIGH : LOW);
  analogWrite(motor1SpeedPin, speed); // Set the speed using PWM
  analogWrite(motor2SpeedPin, speed); // Set the speed using PWM
}


void handleServoLeft() {
  moveServoSmoothly(0);  // Move to 0 degrees
  server.send(200, "text/plain", "Servo moved left");
}

void handleServoCenter() {
  moveServoSmoothly(90);  // Move to 90 degrees (center)
  server.send(200, "text/plain", "Servo centered");
}

void handleServoRight() {
  moveServoSmoothly(180);  // Move to 180 degrees
  server.send(200, "text/plain", "Servo moved right");
}



void handleSpeed() {
  String value = server.arg("value");
  int newSpeed = value.toInt();
  if (newSpeed >= 0 && newSpeed <= 255) {
    speed = newSpeed;
    analogWrite(motor1SpeedPin, speed); 
    analogWrite(motor2SpeedPin, speed); 
    server.send(200, "text/plain", "Speed changed");
  } else {
    server.send(400, "text/plain", "Invalid speed value");
  }
}



void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void toggleFollowMode() {
  followMode = !followMode;
  if (followMode) {
    handleStop();
  }
}

void moveServoSmoothly(int targetPos) {
  if (currentServoPos < targetPos) {
    for (int pos = currentServoPos; pos <= targetPos; pos++) {
      myservo.write(pos);
      delay(15); 
    }
  } else {
    for (int pos = currentServoPos; pos >= targetPos; pos--) {
      myservo.write(pos);
      delay(15); 
    }
  }
  currentServoPos = targetPos;
}



bool isPersonDetected() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  
  return (distance < obstacleDistance);
}




void followPerson() {
  if (!isPersonDetected()) {
    stopRobot();
    return;
  }

  int distance = getDistance();

  int motorSpeed = map(distance, 0, 100, 255, 0);
  motorSpeed = constrain(motorSpeed, 0, 255);

  int startingSpeed = 150;  

  // Determine the movement direction based on the distance
  bool moveForward = (distance > obstacleDistance);
  bool moveBackward = (distance <= obstacleDistance);

  // Gradually stop the robot if the person is too close
  if (!moveForward && !moveBackward) {
    stopRobot();
    followMode = false; // Stop following when too close
    return;
  }

  // Apply motor movement based on the distance
  if (moveBackward) {
    // Move forward with starting speed
    toggleMotors(true, false, true, false, startingSpeed);
  } else if (moveForward) {
    // Move backward with starting speed
    toggleMotors(false, true, false, true, startingSpeed);
  }

  delay(500); // Adjust the delay time as needed

  int personPosition = getPersonPosition(); 
  if (personPosition < 0) {
    toggleMotors(false, true, true, false, motorSpeed);
  } else if (personPosition > 0) {
    toggleMotors(true, false, false, true, motorSpeed);
  }

  delay(500); 

  stopRobot();

  server.send(200, "text/javascript", "updateDistance(" + String(distance) + ");");
}

int getPersonPosition() {
  return random(-1, 2);
  
}


void toggleAutonomousMode() {
  autonomousMode = !autonomousMode;
  if (autonomousMode) {
    autonomousNavigate();
  } else {
    stopRobot();
  }
}

void autonomousNavigate() {
  int distance = getDistance();

  if (distance <= obstacleDistance) {
    handleMoveBackward();
    delay(1000);
    stopRobot();

    if (random(2) == 0) {
      handleMoveLeft();
    } else {
      handleMoveRight();
    }

    delay(1000);
    stopRobot();
  } else {
    handleMoveForward();
  }
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  return distance;
}

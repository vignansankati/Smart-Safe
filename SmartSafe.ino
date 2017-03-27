// photon components variables
int greenLED = D0;
int burzer = D2;
int redLED = D3;
int blueLED = D1;
int whiteSwitch = A4;


bool status = false; //locker on or off


int touch = A0;
int reading = 0;

// default password for photon
String photonPassword = "qwerty";
String newPass = "";
// Password variables
int deviceWeight = 0;

bool passwordCorrect = false;
bool theftStatus = false;
bool switchOn = false;


void setup() {
    pinMode(greenLED,OUTPUT);
    pinMode(redLED,OUTPUT);
    pinMode(blueLED,OUTPUT);
    pinMode(whiteSwitch,INPUT_PULLUP);

    Particle.variable("photonPass",photonPassword);
    Particle.variable("newPass",newPass);

    Particle.function("unlock",unlock);
    Particle.function("changePass",changePass);

    Serial.begin(9600);
}

void loop() {
    lockerStatus();
    // touchSensing();
    if(status == true && passwordCorrect == false) {
        touchSensing();
    }
    else {
        // do nothing
    }
}


// Switching the locker on or off
void lockerStatus() {
    // If deivce is not theft and white switch is not switched on
    if(theftStatus == false && switchOn == false) {
        if(digitalRead(whiteSwitch)==LOW) {
            Serial.println("pressed");
            switchOn = true;
            // locker is off then it is switched on
            if(status == false) {
                deviceWeight = analogRead(A0);
                passwordCorrect = false;
                Serial.println("device weight "+deviceWeight);
                digitalWrite(greenLED,HIGH);
                status = true;
                touchSensing();
            }
            else {
                digitalWrite(greenLED,LOW);
                status = false;
            }
            delay(1000);
        }
    }

}

// Touch sensor reading, to check for the protected device status
void touchSensing() {
    reading = analogRead(A0);
    Serial.println(reading);
    verifyTheft(reading);
    delay(500);
}

// Verifying whether the device is stolen or not
void verifyTheft(int value) {
    if(value<deviceWeight-10 || value>deviceWeight+10) {
        Particle.publish("theft");
        for(int i=0;i<10;i++) {
            tone(burzer, 500,100);
            delay(500);
            noTone(burzer);
            theftStatus = true;
        }
    }
    else {
        theftStatus = false;
        noTone(burzer);
    }
}

// Unlocking the locker with the password entered
int unlock(String password) {
    Serial.println(password);
    Serial.println(photonPassword);
    if(password == photonPassword) {
        passwordCorrect = true;
        status = false;
        switchOn = false;
        digitalWrite(greenLED,LOW);
    }
    return 0;
}

// Changing the password of the locker
int changePass(String newPassword) {
    String oldPass = "";
    String newPass = "";
    int i=0;
    int count = 0;
    for (i=0;i<newPassword.length();i++) {
        if(newPassword[i] == '-') {
            Serial.println("i is "+i);
            count = i+1;
        }
    }
    Serial.println("count is ");
    Serial.println(count);
    // Grabing the old password entered in web page
    for(int j=0;j<count-1;j++) {
        oldPass += newPassword[j];
        Serial.println(oldPass);
    }
    Serial.println("old password is ");
    Serial.println(oldPass);

    // Grabing new password from webpage
    if(oldPass == photonPassword) {
        for (int k=count;k<newPassword.length();k++) {
            newPass += newPassword[k];
        }
        photonPassword = newPass;
        // Successful password change is denoted by Blue LED
        digitalWrite(blueLED,HIGH);
        delay(1000);
        digitalWrite(blueLED,LOW);
        Serial.println("new password is ");
        Serial.println(photonPassword);
    } else {
        // If passwords do not match then red LED is blinked
        digitalWrite(redLED,HIGH);
        delay(1000);
        digitalWrite(redLED,LOW);
    }
    return 0;
}

/*
	menu_2_serial.ino allows continuous checking of the button's status
	throughout the run
	
	Use Uno communicating with computer's serial port
	Connect a pushbutton between pin D2 and ground

*/


#define Button1 2 // Uno pin D2
#define LED 13 // use built-in LED on pin 13


int flashrate = 1; // initial flash rate, 1 Hz
int flashdelay = 1000/flashrate; // convert flash rate into a ms delay value
long oldtime;
byte buttonState;
byte buttonState2;

void setup(){
	Serial.begin(9600);
	pinMode(LED, OUTPUT); // turn LED pin to output
	pinMode(Button1, INPUT_PULLUP); // make button input, use internal pullup resistor
	delay(2000);
	Serial.println("Choose a flash rate at any time");
	Serial.print(flashrate);
	Serial.println("Hz");
	oldtime = millis(); // initial starting time mark
}


void loop() {
	// Read the button every time through the loop
	buttonState = digitalRead(Button1);
	if (buttonState == false) {
		delay(10);
		buttonState2 = digitalRead(Button1);
		if (buttonState == buttonState2){ // both reads were LOW
			if (flashrate < 64) 
			{
				flashrate = flashrate * 2; // double flashrate
			} 
			else if (flashrate >= 64) 
			{
				// Reset the flashrate back to 1
				flashrate = 1;
				// Flash rates above 64 become hard to see, so there's 
				// no point in flashing faster 
			}
		}
		
		Serial.print(flashrate);
		Serial.println("Hz");
		
		flashdelay = 1000 / flashrate; // calculate new flashdelay (ms)
		// delay(300);
	}
	
	// Each time through the loop, check the new time and see if
	// enough milliseconds have elapsed to toggle the LED
	if (millis() >= oldtime + flashdelay) {
		oldtime = millis(); // update oldtime to the new millis() value
		digitalWrite(LED, !(digitalRead(LED))); // toggle led
	}

}

// You will find that this version makes the button-super touchy, as it will
// read multiple button presses in the brief time you hold the button down for
// a single press, which will cause the flashrate and flashdelay values to jump
// much faster than you might hope. 
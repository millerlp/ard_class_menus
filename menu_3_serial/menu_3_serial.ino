/*
	menu_3_serial.ino move the button checking procedure to a function
	
	Use Uno communicating with computer serial monitor
	
	Connect pushbutton between D2 and ground
	
*/

#define Button1 2 // Uno pin D2
#define LED 13 // use built-in LED on pin 13


int flashrate = 1; // initial flash rate, 1 Hz
int flashdelay = 1000/flashrate; // convert flash rate into a ms delay value
long oldtime;
byte buttonState;
byte buttonState1;
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
	// Check the button each time through the loop
	// If Button1 is low (negated to true), then go to the
	// buttonCheck() function. If Button1 is high (true), we
	// don't waste time going to the buttonCheck function.
	if (!(digitalRead(Button1))) {
		flashdelay = buttonCheck();
	}
	// Each time through the loop, check the new time and see if
	// enough milliseconds have elapsed to toggle the LED
	if (millis() >= oldtime + flashdelay) {
		oldtime = millis(); // update oldtime to the new millis() value
		digitalWrite(LED, !(digitalRead(LED))); // toggle led
	}
}

//-------------------------------------------------
// Function buttonCheck()
int buttonCheck() {
	buttonState1 = digitalRead(Button1);
	delay(10); // crude debounce
	buttonState2 = digitalRead(Button1);
	if (buttonState1 == buttonState2){ // both reads were LOW
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
	// Update serial monitor with new rate
	Serial.print(flashrate);
	Serial.println("Hz");

	flashdelay = 1000 / flashrate; // calculate new flashdelay (ms)
	delay(300); // This delay makes the button usable instead of touchy
	return(flashdelay); // return the new value of flashdelay
}


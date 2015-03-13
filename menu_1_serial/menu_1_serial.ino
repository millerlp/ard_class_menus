/*
	Use an Uno, communicating with the computer serial monitor
	
	Connect a button between D2 and ground

*/

#define Button1 2 // Uno pin D2
#define LED 13 // use built-in LED on pin 13

int flashrate = 1; // initial flash rate, 1 Hz
int flashdelay = 0; // convert flash rate into a ms delay value

void setup(){
	Serial.begin(9600);
	pinMode(LED, OUTPUT); // turn LED pin to output
	pinMode(Button1, INPUT_PULLUP); // make button input, use internal pullup resistor
	
	delay(2000);
	
	Serial.println("Choose a flash rate");
	Serial.print(flashrate);
	Serial.println("Hz");
	
	flashdelay = flashRateFunc(); // call the flashRateFunc to choose a flash rate
}

void loop(){
	delay(flashdelay);
	digitalWrite(LED, !( digitalRead(LED) ) ); // toggle LED
}

//************************************************************
// flashRateFunc allows the user to press Button1 and choose a 
// new flash rate. The returned value is the delay in milliseconds
// needed to create the desired flash rate (in Hz). 
int flashRateFunc(void) {
	// the global variable flashrate was defined earlier. Here we will
	// change its value and return that to the main loop
	byte buttonValue1;
	byte buttonValue2;
	byte buttonState;
	
	long startTime = millis(); // Get start time for this loop
	buttonState = digitalRead(Button1); // get current state of button (probably HIGH)
	
	// Go into a loop that lasts at least 3 seconds to give the user time to punch button
	while (millis() <= startTime + 3000) // while millis is less than 3 sec from startTime
	{
		buttonValue1 = digitalRead(Button1);
		delay(10); // perform a crude debounce by checking button twice over 10ms
		buttonValue2 = digitalRead(Button1);
		if (buttonValue1 == buttonValue2) // make sure button is stable
		{ 
			if (buttonValue1 != buttonState) // make sure button has changed
			{
				if (buttonValue1 == LOW) // if button is pressed
				{
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
					
					Serial.print(flashrate);
					Serial.println("Hz");

					// Now update startTime to give user time to press button again
					startTime = millis();
				}
			}
			buttonState = buttonValue1; // updated buttonState so that only changes
			// in button status are registered (forcing button to be released between
			// registered presses). 
		}

	} // end of while loop
	
	Serial.print("Storing");
	delay(400);
	for (int i = 0; i <= 2; i++) {
		Serial.print(".");
		delay(350);
	}
	Serial.println();

	// Calculate the necessary delay for the user's flash rate
	// flashrate = 1 = 1000ms delay
	// flashrate = 100 = 10 ms delay
	flashdelay = 1000 / flashrate;
	return flashdelay;
	
} // end of flashRateFunc
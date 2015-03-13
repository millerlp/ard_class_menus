/*
	Use an Uno and Sparkfun Serial-enabled 4x20 character LCD display LCD-09568
	
	Connect D4 from Uno to RX pin of LCD display
	
	Connect pushbutton between D2 and ground
	
	
	Serial LCD positioning codes (send mySerial.write(254) first to initiate cursor move)
position 	1 		2 		3 		4 		5 		6 		7 		8 		9 		10 	11 	12 	13 	14 	15 	16
line 1 		128 	129 	130 	131 	132 	133 	134 	135 	136 	137 	138 	139 	140 	141 	142 	143
line 2 		192 	193 	194 	195 	196 	197 	198 	199 	200 	201 	202 	203 	204 	205 	206 	207


*/

#include <SoftwareSerial.h>

#define Button1 2 // Uno pin D2
#define LED 13 // use built-in LED on pin 13

SoftwareSerial mySerial(3,4); // pin 4 = TX, pin 3 = RX (unused)

int flashrate = 1; // initial flash rate, 1 Hz
int flashdelay = 0; // convert flash rate into a ms delay value
char buf[4]; // for ASCII conversion of flashrate

void setup(){
	pinMode(LED, OUTPUT); // turn LED pin to output
	pinMode(Button1, INPUT_PULLUP); // make button input, use internal pullup resistor

	mySerial.begin(9600);
	delay(600); // needed to let Serial LCD boot up
	mySerial.write(254); mySerial.write(0x01); // clear screen command
	
	delay(1000);
	mySerial.write(254);mySerial.write(128); // position cursor on 1st line
	mySerial.write("Choose a flash rate");
	mySerial.write(254);mySerial.write(192); // position cursor on 2nd line
	
	// The SerialLCD can only print ASCII representations of characters, rather than
	// numeric variables, so all numeric variables must first be converted to ASCII
	// equivalents. 
	// Use the Integer TO Ascii function itoa() to convert flashrate into ASCII
	// The value gets stored in the buffer 'buf', and the 3rd argument says to do a 
	// base10 conversion
	itoa(flashrate, buf, 10);
	mySerial.write(buf); // display ASCII flash rate stored in buf
	mySerial.write(254);mySerial.write(195); // position cursor
	mySerial.write("Hz");
	delay(1000);
	
	flashdelay = flashRateFunc(); // call the flashRateFunc to choose a flash rate
	
	// clear screen after returning with new flashrate
	mySerial.write(254); mySerial.write(0x01); // clear screen command
	mySerial.write(254);mySerial.write(128); // position cursor on 1st line
	// Use the itoa function once again to write out the flashrate value
	mySerial.write(itoa(flashrate,buf,10));
	mySerial.write("Hz");
}

void loop() {
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
					// Now update the displayed flashrate
					mySerial.write(254);mySerial.write(192);
					mySerial.write("   "); // write 3 blank spaces to clear number
					mySerial.write(254);mySerial.write(192);
					mySerial.write(itoa(flashrate,buf,10)); // display flash rate
					// Now update startTime to give user time to press button again
					startTime = millis();
				}
			}
			buttonState = buttonValue1; // updated buttonState so that only changes
			// in button status are registered (forcing button to be released between
			// registered presses). 
		}

	} // end of while loop
		// At this point the while loop has timed out
		mySerial.write(254); mySerial.write(148); // position at start of 3rd line
		mySerial.write("Storing");
		delay(400);
		for (int i = 0; i <= 2; i++) {
			mySerial.write(".");
			delay(350);
		}
	// Calculate the necessary delay for the user's flash rate
	// flashrate = 1 = 1000ms delay
	// flashrate = 100 = 10 ms delay
	flashdelay = 1000 / flashrate;
	return flashdelay;
	
} // end of flashRateFunc



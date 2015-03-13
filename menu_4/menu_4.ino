/*
	menu_4.ino implements a hardware interrupt on pin D2
	
	Use an Uno and Sparkfun Serial-enabled 4x20 character LCD display LCD-09568
	
	Connect D4 from Uno to RX pin of LCD display
	
	Connect pushbutton between D2 and ground
	
	
	Serial LCD positioning codes (send mySerial.write(254) first to initiate cursor move)
position 	1 		2 		3 		4 		5 		6 		7 		8 		9 		10 	11 	12 	13 	14 	15 	16
line 1 		128 	129 	130 	131 	132 	133 	134 	135 	136 	137 	138 	139 	140 	141 	142 	143
line 2 		192 	193 	194 	195 	196 	197 	198 	199 	200 	201 	202 	203 	204 	205 	206 	207
line 3 		148	149
line 4 		212	213
*/

#include <SoftwareSerial.h>

#define Button1 2 // Uno pin D2
#define LED 13 // use built-in LED on pin 13

SoftwareSerial mySerial(3,4); // pin 4 = TX, pin 3 = RX (unused)

int flashrate = 1; // initial flash rate, 1 Hz
int flashdelay = 1000/flashrate; // convert flash rate into a ms delay value
char buf[4]; // for ASCII conversion of flashrate
long oldtime;
boolean buttonFlag = false;
boolean buttonState1;
boolean buttonState2;

void setup(){
	pinMode(LED, OUTPUT); // turn LED pin to output
	pinMode(Button1, INPUT_PULLUP); // make button input, use internal pullup resistor

	mySerial.begin(9600);
	delay(600); // needed to let Serial LCD boot up
	mySerial.write(254); mySerial.write(0x01); // clear screen command
	delay(1000);
	mySerial.write(254);mySerial.write(128); // position cursor on 1st line
	mySerial.write("Choose a flash rate");
	mySerial.write(254);mySerial.write(192); // position cursor on 1st line
	mySerial.write("at any time");
	mySerial.write(254);mySerial.write(148); // position cursor on 3rd line
	
	// Use the Integer TO Ascii function itoa() to convert flashrate into ASCII
	// The value gets stored in the buffer 'buf', and the 3rd argument says to do a 
	// base10 conversion
	itoa(flashrate, buf, 10);
	mySerial.write(buf); // display ASCII flash rate stored in buf
	mySerial.write(254);mySerial.write(151); // move cursor over
	mySerial.write("Hz");
	oldtime = millis();
	
	// Create an interrupt to watch for. Use Arduino Uno's Interrupt 0
	// which is on pin D2, call the buttonCheck function when triggered,
	// and trigger the interrupt when D2 is pulled LOW.
	attachInterrupt(0, buttonInt, LOW);
}

void loop(){
	// Each time through the loop, check the new time and see if
	// enough milliseconds have elapsed to toggle the LED
	if (millis() >= oldtime + flashdelay) {
		oldtime = millis(); // update oldtime to the new millis() value
		digitalWrite(LED, !(digitalRead(LED)) ); // toggle led
	}
	
	// If the button was pressed to trigger the interrupt service routine
	// and set buttonFlag == true, then this section will execute the next
	// time through the main loop. 
	if (buttonFlag){
		flashdelay = flashadjust();
		buttonFlag = false; // reset buttonFlag
	}
}

// *******************************************
// The interrupt service routine. This will interrupt any
// long-running process briefly to execute its code, then 
// return to where ever the loop was previously at. 
void buttonInt(){
	detachInterrupt(0); // temporarily shut off the interrupt
	buttonState1 = digitalRead(Button1);
	delay(10); // crude debounce
	buttonState2 = digitalRead(Button1);
	if (buttonState1 == buttonState2){
		buttonFlag = true;
	}
	attachInterrupt(0, buttonInt, LOW); // restart the interrupt
}

//************************************************
// function flashadjust that adjusts the flashrate
// This only runs if the buttonFlag is true, so it only takes time
// away from the main loop if the button is actually pressed. 
int flashadjust() {
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
	mySerial.write(254);mySerial.write(148);
	mySerial.write("      "); // write 6 blank spaces to clear number
	mySerial.write(254);mySerial.write(148);
	mySerial.write(itoa(flashrate,buf,10)); // display flash rate
	mySerial.write(" Hz");
	flashdelay = 1000 / flashrate; // calculate new flashdelay (ms)
	return(flashdelay);
}
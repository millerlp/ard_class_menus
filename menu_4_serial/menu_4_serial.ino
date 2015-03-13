/*
	menu_4_serial.ino implements a hardware interrupt on pin D2
	
	Use Uno communicating with computer serial monitor
	
	Connect pushbutton between D2 and ground

*/

#define Button1 2 // Uno pin D2
#define LED 13 // use built-in LED on pin 13

int flashrate = 1; // initial flash rate, 1 Hz
int flashdelay = 1000/flashrate; // convert flash rate into a ms delay value
long oldtime;
boolean buttonFlag = false;
boolean buttonState1;
boolean buttonState2;

void setup(){
	pinMode(LED, OUTPUT); // turn LED pin to output
	pinMode(Button1, INPUT_PULLUP); // make button input, use internal pullup resistor
	Serial.begin(9600);
	delay(2000);
	Serial.println("Choose a flash rate at any time");
	Serial.print(flashrate);
	Serial.println("Hz");
	
	// Create an interrupt to watch for. Use Arduino Uno's Interrupt 0
	// which is on pin D2, call the buttonCheck function when triggered,
	// and trigger the interrupt when D2 is pulled LOW.
	// http://arduino.cc/en/Reference/AttachInterrupt 
	attachInterrupt(0, buttonInt, LOW);
	
	oldtime = millis(); // initial starting time mark
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
// The interrupt service routine. This is called when
// interrupt 0 is triggered.
// This will interrupt any long-running process briefly 
// to execute its code, then return to where ever the 
// loop was previously at. 
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
	Serial.print(flashrate);
	Serial.println("Hz");
	flashdelay = 1000 / flashrate; // calculate new flashdelay (ms)
	return(flashdelay);
}
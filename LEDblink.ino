#include "TM74HC595Display.h"

#define LED 13
#define A0 2
#define A1 8
#define B0 3
#define B1 11
#define SW 12

#define SH_CP 5	// SCLK
#define ST_CP 6	// RCLK
#define DS 7	// DIO

TM74HC595Display disp(SH_CP, ST_CP, DS);
unsigned char LED_0F[29];

volatile uint8_t frqH = 0;
volatile uint8_t frqL = 0;
float frq = 0;

String outpt = "";

String message = "";

union in_ref {
	byte asBytes[4];
	float asFloat;
} in_data;


void setup()
{
	/* add setup code here */
	
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(B0, INPUT);
	pinMode(B1, INPUT);
	pinMode(SW, INPUT);

	attachInterrupt(digitalPinToInterrupt(A0), frqHchange, LOW);
	attachInterrupt(digitalPinToInterrupt(B0), frqLchange, LOW);

	LED_0F[0] = 0xC0; //0
	LED_0F[1] = 0xF9; //1
	LED_0F[2] = 0xA4; //2
	LED_0F[3] = 0xB0; //3
	LED_0F[4] = 0x99; //4
	LED_0F[5] = 0x92; //5
	LED_0F[6] = 0x82; //6
	LED_0F[7] = 0xF8; //7
	LED_0F[8] = 0x80; //8
	LED_0F[9] = 0x90; //9
	LED_0F[10] = 0x88; //A
	LED_0F[11] = 0x83; //b
	LED_0F[12] = 0xC6; //C
	LED_0F[13] = 0xA1; //d
	LED_0F[14] = 0x86; //E
	LED_0F[15] = 0x8E; //F
	LED_0F[16] = 0xC2; //G
	LED_0F[17] = 0x89; //H
	LED_0F[18] = 0xF9; //I
	LED_0F[19] = 0xF1; //J
	LED_0F[20] = 0xC3; //L
	LED_0F[21] = 0xA9; //n
	LED_0F[22] = 0xC0; //O
	LED_0F[23] = 0x8C; //P
	LED_0F[24] = 0x98; //q
	LED_0F[25] = 0x92; //S
	LED_0F[26] = 0xC1; //U
	LED_0F[27] = 0x91; //Y
	LED_0F[28] = 0xFE; //hight -

	Serial.begin(38400);
}

void loop()
{
	/* add main program code here */

	
	/*
	delay(500);
	digitalWrite(LED, HIGH);
	delay(1000);
	digitalWrite(LED, LOW);

	disp.send(LED_0F[0], 0b0001);    //send digital "0" to 1st indicator
	delay(2000);
	disp.send(LED_0F[3], 0b0110);    //send digital "3" to 2nd and 3rd indicator
	delay(2000);
	disp.send(LED_0F[10], 0b1111);    //send simbol "A" to all indicators
	delay(2000);
	*/
	if (in_data.asFloat == 0)
	{
		disp.send(0x36, 0b1111);
	}
	else
	{
		//disp.digit2((frqH), 0b0100, 15);
		//disp.digit2(frqL, 0b0001, 15);
		//disp.send(0x7F, 0b0100);

		disp.digit2((int)(in_data.asFloat/100), 0b0100, 15);
		disp.digit2((int)in_data.asFloat, 0b0001, 15);
		disp.send(0x7F, 0b0100);
	}
	rotbtn();
	if (Serial.available())
	{
		if (Serial.read() == '$')
		{
			String tmp = Serial.readStringUntil(',');
			in_data.asFloat = tmp.toFloat();
				/*
			while (Serial.read() != ',')
			{

			
				for (int i = 0; i < 4; i++)
				{
					in_data.asBytes[i] = Serial.read();
					delay(1);

					digitalWrite(LED, HIGH);
					//delay(300);
					digitalWrite(LED, LOW);
					//delay(300);
				}

			}
			*/
		}
		
	}
	//printFreq();
}

void rotbtn()
{
	static bool pressed = !digitalRead(SW);

	// Handle button pressed
	if (!pressed && !digitalRead(SW))
	{
		pressed = true;
		digitalWrite(LED, HIGH);
		in_data.asFloat = 11824;
		//Serial.println("Button pressed.");
	}
	// Handle button released
	else if (pressed && digitalRead(SW))
	{
		pressed = false;
		digitalWrite(LED, LOW);
		//Serial.println("Button released.");
	}
	digitalWrite(13, pressed);
}

void frqHchange()
{
	static uint16_t last_interrupt_time = 0;
	uint16_t interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 0)
	{
		if (digitalRead(A0) != digitalRead(A1))
		{
			frqH++;
		}
		else
		{
			frqH--;
		}

		if (frqH > 136)
			frqH = 118;
		else if (frqH < 118)
			frqH = 136;
		printFreq();
	}
	last_interrupt_time = interrupt_time;
}

void frqLchange()
{
	static uint16_t last_interrupt_time = 0;
	uint16_t interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 0)
	{
		//delayMicroseconds(10);
		if (digitalRead(B0) == digitalRead(B1))
		{
			frqL++;
			if (frqL > 99)
				frqL = 0;
		}
		else
		{
			frqL--;
			if (frqL > 99)
				frqL = 99;
		}

		if (frqH > 136)
			frqH = 118;
		else if (frqH < 118)
			frqH = 136;
		printFreq();
	}
	last_interrupt_time = interrupt_time;
}

void printFreq()
{
	frq = (float)frqH + ((float)frqL / 100.0);
	//outpt = "Selected frequency: " + (String)frqH + '.' + (String)frqL;
	outpt = "Selected frequency: " + (String)frq;
	//Serial.println(outpt);
	int xoutput = (frqH*100) + frqL;
	Serial.print('$' + (String)xoutput + ',');
	delay(1);
}
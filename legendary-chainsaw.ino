#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2
#define NUM_LEDS 97

//Globals
Adafruit_NeoPixel strip = Adafruit_NeoPixel(97, PIN, NEO_GRB + NEO_KHZ800); //Params 1 - pixels; 2 - PIN; 3 - Flags (may be added(+) together) (NEO_KHZ800, NEO_KHZ400, NEO_GRB, NEO_RGB, NEO_RGBW)

class Rainbow {
private:
	uint8_t Pos;
	uint32_t Wheel(byte WheelPos) {
		WheelPos = 255 - WheelPos;
		if (WheelPos < 85) {
			return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
		}
		if (WheelPos < 170) {
			WheelPos -= 85;
			return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
		}
		WheelPos -= 170;
		return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	}

public:
	void Next(int Amount) {
		Pos++;
	}
	void SetPattern() {
		for (int i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel((i + Pos) & 255));
		}
	}
};

class Pong {
private:
	int Pos, InversedSpeed, Step, Size, Length;
	uint8_t R, G, B;
	bool Clockwise;
public:
	Pong(int invSpd, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, int startPos, int length ) {
		Size = size;
		InversedSpeed = invSpd;
		Clockwise = clockwise;
		if(!Clockwise)
			Pos = 95;
		R = r; G = g; B = b;
		Pos = startPos;
		Length = length;
	}
	void Next(int Amount) {
		Step += Amount;
		if (Step >= InversedSpeed) {
			Step %= InversedSpeed;
			if (Clockwise)
				Pos++;
			else
				Pos--;
		}
		if (Pos <= 0 || Pos >= Length)
			Clockwise = !Clockwise;
	}
	void BlendPattern() {
		for (int i = Pos - Size / 2; i <= Pos + Size / 2; i++)
		{
			if (i > 0 && i < Length)
				strip.setPixelColor(i - 1, R, G, B, 255);
		}
	}
};


Rainbow r = Rainbow();
Pong p = Pong(1,3, 255,255,255, true, 1, NUM_LEDS);
Pong q = Pong(1, 3, 255, 0, 0, false, 96, NUM_LEDS);

int Offset;

void setup() {
	strip.begin();
	strip.setBrightness(50);
	strip.show(); // Initialize all pixels to 'off'
}

void loop() {
	Offset++;
	r.SetPattern();
	p.BlendPattern();
	q.BlendPattern();
	r.Next(1);
	p.Next(1);
	q.Next(1);
	strip.show();
	delay(30);
}
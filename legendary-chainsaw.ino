#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2

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
			strip.setPixelColor(Pos, 255, 255, 255, 255);
			strip.setPixelColor(i, Wheel((i + Pos) & 255));
		}
	}
};
class Pong {
private:
	int Pos, InversedSpeed, Step, Size;
	bool Clockwise;
public:
	Pong(int invSpd, int size) {
		Size = size;
		InversedSpeed = invSpd;
		Clockwise = true;
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
		if (Pos <= 0 || Pos >= strip.numPixels())
			Clockwise = !Clockwise;
	}
	void BlendPattern() {
		for (int i = Pos - Size / 2; i <= Pos + Size / 2; i++)
		{
			if (i >= 0 && i < strip.numPixels())
				strip.setPixelColor(i - 1, 255, 255, 255, 255);
		}
	}
};


Rainbow r = Rainbow();
Pong p = Pong(1,3);
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
	r.Next(1);
	p.Next(1);
	strip.show();
	delay(40);
}
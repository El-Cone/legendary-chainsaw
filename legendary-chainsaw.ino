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

class Blip {
protected:
	int Size, Length;
	uint8_t WaitSteps;
	uint16_t Pos;
	float Speed, Acceleration;
	uint8_t R, G, B;
	bool Clockwise;
public:
	Blip() {}
	Blip(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos, int length, uint8_t waitSteps) {
		Size = size; R = r; G = g; B = b; Length = length; Speed = speed; 
		Acceleration = acceleration + 1; Clockwise = clockwise; Pos = startPos;
	}
	void Next(int Amount) {
		if (Clockwise)
			Pos += Speed * Amount;
		else
			Pos -= Speed * Amount;
		Speed *= Acceleration;

	}
};

class Pong :public Blip {
public:
	Pong(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos, int length, uint8_t waitSteps) {
		Blip::Blip(speed, acceleration, size, r, g, b, clockwise, startPos, length, waitSteps);
	}
	void Next(int Amount) {
		Blip::Next(Amount);
		if (Pos <= 0 || Pos >= Length + WaitSteps)
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
Pong p = Pong(1, 0, 3, 255, 255, 255, true, 1, NUM_LEDS, 0);
Pong q = Pong(1, 0, 3, 255, 0, 0, false, 96, NUM_LEDS, 0);

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
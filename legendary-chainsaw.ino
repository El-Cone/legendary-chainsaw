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
	void Next(int Amount = 1) {
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
	int Size;
	uint16_t Pos;
	float Speed, Acceleration;
	uint8_t R, G, B;
	bool Clockwise;
public:
	Blip() {}
	Blip(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos) {
		Size = size; R = r; G = g; B = b; Speed = speed;
		Acceleration = acceleration + 1; Clockwise = clockwise; Pos = startPos;
	}
	void Next(int Amount = 1) {
		if (Clockwise)
			Pos += Speed * Amount;
		else
			Pos -= Speed * Amount;
		Speed *= Acceleration;
	}
	void BlendPattern() {
		int posA = Pos;
		int posB = Pos + Clockwise ? Size : -Size;
		for (uint16_t i = Clockwise ? Pos : Pos - Size; i < Clockwise ? Pos + Size : Pos; i++)
			if (i <= NUM_LEDS)
				strip.setPixelColor(i, R, G, B, 255);
	}
};

class Pong : public Blip {
private:
	int Length;
public:
	Pong(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos, int length) :
		Blip(speed, acceleration, size, r, g, b, clockwise, startPos)
	{
		Length = length;
	}
	void Next(int Amount = 1) {
		Blip::Next(Amount);
		if (Pos <= 0 || Pos >= Length)
			Clockwise = !Clockwise;
	}
};

class Walker : public Blip {
private:
	uint8_t WaitSteps, WaitCount;
	uint16_t StartPos, EndPos;
	bool Recurring, Dead;
public:
	Walker(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos, uint16_t endPos, uint8_t waitSteps, bool recurring) :
		Blip(speed, acceleration, size, r, g, b, clockwise, startPos)
	{
		WaitSteps = waitSteps; Recurring = recurring; StartPos = startPos; EndPos = endPos;
	}
	void Next(int Amount = 1) {
		if (Dead)
		{
			if (Recurring && WaitSteps++ >= WaitCount)
				Revive();
			else
				return;
		}
		Blip::Next(Amount);
		if ((Clockwise && (Pos < StartPos || Pos > EndPos)) || (!Clockwise && (Pos > StartPos || Pos < EndPos)))
			Dead = true;
	}
	void BlendPattern() {
		if (!Dead)
			Blip::BlendPattern();

	}
	void Revive() {
		if (!(Recurring && Dead))
			return;
		Pos = StartPos;
		Dead = false;
	}
};

Rainbow r = Rainbow();
Pong p = Pong(1, 0, 3, 255, 255, 255, true, 1, NUM_LEDS);
Pong q = Pong(1, 0, 3, 255, 0, 0, false, 96, NUM_LEDS);
Walker w = Walker(1, 0.5, 3, 0, 255, 0, true, 1, 96, 200, true);

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
	w.BlendPattern();
	r.Next();
	p.Next();
	q.Next();
	w.Next();
	strip.show();
	delay(30);
}
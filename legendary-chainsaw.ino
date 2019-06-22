#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//********************************************************
//============  Geschreven door ElCone
//============  In samenwerking met Tom 2
//============  Versie 0.2019-06-20.4
//*******************************************************/

#define PIN 2
#define PN2 4
//#define NUM_LEDS 680 //261 
#define NUM_LEDS_L 256
#define NUM_LEDS_R 256
#define NUM_LEDS NUM_LEDS_L + NUM_LEDS_R
#define BRIGHTNESS 200

//********************************************************
//============  Structs
//*******************************************************/

struct RGB {
	byte R, G, B;
};

//********************************************************
//============  Globals
//*******************************************************/

class Strip {
	Adafruit_NeoPixel stripL = Adafruit_NeoPixel(NUM_LEDS_L, PIN, NEO_GRB + NEO_KHZ800); //Params 1 - pixels; 2 - PIN; 3 - Flags (may be added(+) together) (NEO_KHZ800, NEO_KHZ400, NEO_GRB, NEO_RGB, NEO_RGBW)
	Adafruit_NeoPixel stripR = Adafruit_NeoPixel(NUM_LEDS_R, PN2, NEO_GRB + NEO_KHZ800);
public:
	void begin() {
		stripL.begin();
		stripR.begin();
	}
	uint16_t numPixels() {
		return stripL.numPixels() + stripR.numPixels();
	}
	void setBrightness() {
		setBrightness(BRIGHTNESS);
	}
	void setBrightness(uint8_t brightness) {
		stripL.setBrightness(brightness);
		stripR.setBrightness(brightness);
	}
	void setPixelColor(uint16_t n, RGB color) {
		return setPixelColor(n, color.R, color.G, color.B);
	}
	void setPixelColor(uint16_t n, RGB color, uint8_t w) {
		return setPixelColor(n, color.R, color.G, color.B, w);
	}
	void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
		if (n > NUM_LEDS_L)
		{
			n -= NUM_LEDS_L;
			n = NUM_LEDS_R - n;
			stripR.setPixelColor(n, r, g, b);
		}
		else
			stripL.setPixelColor(n, r, g, b);
	}
	void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
		if (n > NUM_LEDS_L)
		{
			n -= NUM_LEDS_L;
			n = NUM_LEDS_R - n;
			stripR.setPixelColor(n, r, g, b, w);
		}
		else
			stripL.setPixelColor(n, r, g, b, w);
	}
	void show() {
		stripL.show();
		stripR.show();
	}
};

Strip LedStrip = Strip();

//Adafruit_NeoPixel stripR = Adafruit_NeoPixel(NUM_LEDS, PINTwee, NEO_RGB + NEO_KHZ800); 

//********************************************************
//============  Entities
//*******************************************************/

class OneColor {
public:
	RGB Color;
	byte *R = &Color.R;
	byte *G = &Color.G;
	byte *B = &Color.B;
	byte Brightness;
	OneColor() {}
	OneColor(RGB color) {
		Color = color;
		Brightness = BRIGHTNESS;
	}
	OneColor(RGB color, uint8_t brightness) {
		Color = color;
		Brightness = brightness;
	}
	void White(bool day = true) {
		Color = { 255, day ? 255 : 200, day ? 127 : 50 };
	}
	void WhiteK(uint8_t temp) {
		const int rangeSize = 32;
		uint8_t range = temp / rangeSize;
		temp %= rangeSize;
		Color.R = 255;
		Color.G = 255;
		Color.B = 255;
		Brightness = 127;
		switch (range)
		{
		case 0:
			Color.G = 96 + temp * 2;
			Color.B = 2 * temp;
			break;
		case 1:
			Color.G = 160 + temp / 2;
			Color.B = 64 + temp;
			break;
		case 2:
			Color.G = 176 + temp;
			Color.B = 96 + temp;
			break;
		case 3:
			Color.G = 208 + temp / 2;
			Color.B = 160 + temp;
			break;
		case 4:
			Color.G = 224 + temp;
			Color.B = 192 + temp;
			break;
		case 5:
			Color.B = 224 + temp;
			break;
		case 6:
			Color.R = 255 - temp * 2;
			Color.G = 255 - temp;
			break;
		case 7:
			Color.R = 192 - temp * 4;
			Color.G = 224 - temp * 2;
			break;
		}
		//White(false);
	}
	void SetPattern() {
		for (int i = 0; i <= NUM_LEDS; i++)
		{
			LedStrip.setPixelColor(i, Color.R, Color.G, Color.B, Brightness);
		}
	}
};

class ColorLoop {
private:
	RGB *Colors; //Array size = Size
	byte Pos, Steps, Current, Size;
	float dR, dG, dB;

	void CalcDeltas(byte idx1, byte idx2)
	{
		dR = (Colors[idx2].R - Colors[idx1].R) / Steps;
		dG = (Colors[idx2].G - Colors[idx1].G) / Steps;
		dB = (Colors[idx2].B - Colors[idx1].B) / Steps;
	}
public:
	ColorLoop() {};		

	ColorLoop(RGB colors[], byte size, byte steps) {
		Colors = colors;
		Size = size;
		Steps = steps;
		CalcDeltas(0, 1);
	}

	void Next() {
		Pos++;
		if (Pos == Steps)
		{
			Pos = 0;
			Current++;
			Current %= Size;
			CalcDeltas(Current, (Current + 1) % Size);
		}
	}

	void SetPattern() {

		byte r = Colors[Current].R + dR * Pos;
		byte g = Colors[Current].G + dG * Pos;
		byte b = Colors[Current].B + dB * Pos;
		for (int i = 0; i <= NUM_LEDS; i++)
		{
			LedStrip.setPixelColor(i, r, g, b, BRIGHTNESS);
		}
		LedStrip.setPixelColor(Pos, 255, 255, 255, 255);
	}
};

class ColorBounce {
public:
	OneColor C1;
	OneColor C2;
	int Pos;
	int Length = 32;

	ColorBounce() {};
	ColorBounce(RGB color1, RGB color2)
	{
		C1 = OneColor(color1);
		C2 = OneColor(color2);
		//C1.R = color1.R; C1.G = color1.G; C1.B = color1.B;
		//C2.R = color2.R; C2.G = color2.G; C2.B = color2.B;
	}

	void Next() {
		Pos++;
		Pos %= (Length * 2);
	}

	void SetPattern() {
		int rRng = *C1.R - *C2.R;
		int gRng = *C1.G - *C2.G;
		int bRng = *C1.B - *C2.B;
		float rStep = (*C1.R - *C2.R) / Length;
		float gStep = (*C1.G - *C2.G) / Length;
		float bStep = (*C1.B - *C2.B) / Length;

		for (int i = 0; i < NUM_LEDS; i++) {
			uint8_t r, g, b;
			int pos = Pos % Length;
			if (Pos < 32) {
				r = *C1.R - rRng + rStep * pos;
				g = *C1.G - gRng + gStep * pos;
				b = *C1.B - bRng + bStep * pos;
			}
			else {
				r = *C2.R + rRng - rStep * pos;
				g = *C2.G + gRng - gStep * pos;
				b = *C2.B + bRng - bStep * pos;
			}
			LedStrip.setPixelColor(i, r, g, b, BRIGHTNESS);
		}
		//stripL.setPixelColor(Pos, 255, 255, 255, 255);  //Debug
	}
};

class Rainbow {
private:
	uint8_t Pos;
	//uint32_t Wheel(byte WheelPos) {
	//	WheelPos = 255 - WheelPos;
	//	if (WheelPos < 85) {
	//		return LedStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	//	}
	//	if (WheelPos < 170) {
	//		WheelPos -= 85;
	//		return LedStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	//	}
	//	WheelPos -= 170;
	//	return LedStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	//}
	RGB Wheel(byte WheelPos) {
		WheelPos = 255 - WheelPos;
		if (WheelPos < 85) {
			return RGB{ 255 - WheelPos * 3, 0, WheelPos * 3 };
		}
		if (WheelPos < 170) {
			WheelPos -= 85;
			return RGB{ 0, WheelPos * 3, 255 - WheelPos * 3 };
		}
		WheelPos -= 170;
		return RGB{ WheelPos * 3, 255 - WheelPos * 3, 0 };
	}

public:
	void Next(int Amount = 1) {
		Pos++;
	}
	void SetPattern() {
		for (int i = 0; i < LedStrip.numPixels(); i++) {
			LedStrip.setPixelColor(i, Wheel((i + Pos) & 255));
			//strip.setPixelColor(i, Wheel((i + Pos) & 255, stripR));
		}
	}
};

class Fire {
private:
	uint8_t Pos;

	void Wheel(int index, byte WheelPos) {
		/*
		WheelPos;
		if (WheelPos < 128) {
		  int g = 127 - WheelPos == 255 ? 0 : WheelPos;
		  return strip.Color(192, g, 0);
		}
		return strip.Color(192, WheelPos == 255 ? 0 : 0+WheelPos,0);
		*/
		//WheelPos = 255 - WheelPos;
		WheelPos %= 255;
		if (WheelPos < 85) {
			LedStrip.setPixelColor(index,
				255,
				170 - WheelPos * 2,
				0,
				Brightness);
		}
		if (WheelPos < 170) {
			WheelPos -= 85;
			LedStrip.setPixelColor(index,
				255 - WheelPos * 2,
				0,
				0,
				Brightness);
		}
		WheelPos -= 170;
		LedStrip.setPixelColor(index,
			85 + WheelPos * 2,
			WheelPos * 2,
			0,
			Brightness);
	}

public:
	uint8_t Brightness = 255;
	void Next(int Amount = 1) {
		Pos++;
	}
	void SetPattern() {
		for (int i = 0; i < LedStrip.numPixels(); i++) {
			Wheel(i + Pos % 255, i);
			//strip.setPixelColor(i, Wheel((i + Pos) & 255, stripR));
		}
		LedStrip.show();
	}
};

//
//class Blip {
//protected:
//  int Size;
//  uint16_t Pos;
//  float Speed, Acceleration;
//  uint8_t R, G, B;
//  bool Clockwise;
//public:
//  Blip() {}
//  Blip(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos) {
//    Size = size; R = r; G = g; B = b; Speed = speed;
//    Acceleration = acceleration + 1; Clockwise = clockwise; Pos = startPos;
//  }
//  void Next(int Amount = 1) {
//    if (Clockwise)
//      Pos += Speed * Amount;
//    else
//      Pos -= Speed * Amount;
//    Speed *= Acceleration;
//  }
//  void BlendPattern(Adafruit_NeoPixel strip) {
//    int posA = Pos;
//    int posB = Pos + Clockwise ? Size : -Size;
//    for (uint16_t i = Clockwise ? Pos : Pos - Size; i < Clockwise ? Pos + Size : Pos; i++)
//      if (i <= NUM_LEDS)
//        strip.setPixelColor(i, R, G, B, 255);
//  }
//};
//
//class Pong : public Blip {
//private:
//  int Length;
//public:
//  Pong(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos, int length) :
//    Blip(speed, acceleration, size, r, g, b, clockwise, startPos)
//  {
//    Length = length;
//  }
//  void Next(int Amount = 1) {
//    Blip::Next(Amount);
//    if (Pos <= 0 || Pos >= Length)
//      Clockwise = !Clockwise;
//  }
//};
//
//class Walker : public Blip {
//private:
//  uint8_t WaitSteps, WaitCount;
//  uint16_t StartPos, EndPos;
//  bool Recurring, Dead;
//public:
//  Walker(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos, uint16_t endPos, uint8_t waitSteps, bool recurring) :
//    Blip(speed, acceleration, size, r, g, b, clockwise, startPos)
//  {
//    WaitSteps = waitSteps; Recurring = recurring; StartPos = startPos; EndPos = endPos;
//  }
//  void Next(int Amount = 1) {
//    if (Dead)
//    {
//      if (Recurring && WaitSteps++ >= WaitCount)
//        Revive();
//      else
//        return;
//    }
//    Blip::Next(Amount);
//    if ((Clockwise && (Pos < StartPos || Pos > EndPos)) || (!Clockwise && (Pos > StartPos || Pos < EndPos)))
//      Dead = true;
//  }
//  void BlendPattern() {
//    if (!Dead)
//      Blip::BlendPattern();
//
//  }
//  void Revive() {
//    if (!(Recurring && Dead))
//      return;
//    Pos = StartPos;
//    Dead = false;
//  }
//};


class Siren {
private:
	uint8_t Duration, Pos;
public:
	Siren() { Duration = 30; }

	void Next(int amount = 1) {
		Pos++;
		Pos %= Duration;
	}

	void SetPattern()
	{
		int section;
		int sectSize = NUM_LEDS / 4;
		bool halfway = Pos * 2 > Duration;

		for (int i = 0; i < NUM_LEDS; i++)
			LedStrip.setPixelColor(i, 0, 0, 0);
		if (Pos % 2 == 1)
			return;
		if (Pos <= Duration / 2)
		{
			for (int i = 0; i < NUM_LEDS / 2; i++)
				LedStrip.setPixelColor(i, 255, 0, 0);
		}
		else
		{
			for (int i = NUM_LEDS / 2; i < NUM_LEDS; i++)
				LedStrip.setPixelColor(i, 0, 0, 255);
		}

	}

};

class Sparkle {
public:
	uint8_t R, G, B, Brightness;
	int X, Y;
	bool Previous;
	Sparkle() {};
	Sparkle(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
		R = r;
		G = g;
		B = b;
		Brightness = brightness;
	}
	void BlendPattern() {
		Previous = !Previous;
		if (Previous)
			X = random(0, NUM_LEDS);
		else
			Y = random(0, NUM_LEDS);
		Redraw();

	}
	void Redraw() {
		LedStrip.setPixelColor(Previous ? X : Y, R, G, B, Brightness);
		LedStrip.setPixelColor(Previous ? Y : X, R / 2, G / 2, B / 2, Brightness);
	}
};

class Bouncer {
public:
	int Pos, Length;
	int Direction = 1;
	uint8_t R, G, B, Brightness;
	void Next() {
		if (Pos == 0 || Pos == NUM_LEDS)
			Direction *= -1;
		Pos += Direction;
	}
	void BlendPattern() {
		for (int i = Pos; i < Pos + Length; i++)
			if (i <= NUM_LEDS)
				LedStrip.setPixelColor(i, R, G, B, Brightness);
	}

};

//********************************************************
//============  Assemblies
//*******************************************************/

class Prog00 {
public:
	//Pong p = Pong(1, 0, 3, 255, 255, 255, true, 1, NUM_LEDS);
	//Pong q = Pong(1, 0, 3, 255, 0, 0, false, 96, NUM_LEDS);
	//Walker w = Walker(1, 0.5, 3, 0, 255, 0, true, 1, 96, 200, true);
	//Blip(float speed, float acceleration, int size, uint8_t r, uint8_t g, uint8_t b, bool clockwise, uint16_t startPos) 
	//Blip b = Blip(1,0,3,255,0,0,true, 40);
	void Looper() {
		//b.BlendPattern();
	//b.Next();
  //  p.BlendPattern();
	//q.BlendPattern();
	//w.BlendPattern();
	//r.Next();
  //  p.Next();
	//q.Next();
	//w.Next();
		delay(30);
	}
}; //Original

class Prog01 {
public:
	Fire f = Fire();
	Sparkle s = Sparkle(0, 0, 255, 255);
	Sparkle s2 = Sparkle(255, 0, 0, 255);
	Sparkle s3 = Sparkle(255, 255, 255, 255);
	Sparkle s4 = Sparkle(255, 127, 0, 255);

	void Setup() {
		f.SetPattern();
		f.Brightness = 255;
	}

	void Looper() {
		f.Next();
		s.BlendPattern();
		s2.BlendPattern();
		s3.BlendPattern();
		s4.BlendPattern();

		delay(40);
	}

}; //Fire

class Prog02 {
public:
	Rainbow r = Rainbow();
	Bouncer b = Bouncer();

	void Setup() {
		b.R = 255;
		b.G = 255;
		b.B = 255;
		b.Brightness = 255;
	}
	void Looper() {
		r.SetPattern();
		r.Next();
		b.Next();
		b.BlendPattern();

		delay(1); //40
	}
}; //Rainbow

void SleepSecs(int secs)
{
	delay(1000 * secs);
}

class Prog03 {
public:
	Siren s = Siren();
	void Looper() {
		s.Next();
		s.SetPattern();
		delay(20);
	}
};

class Prog04 {  //Clau
public:
	ColorBounce Cb;
	void Setup() {
		Cb = ColorBounce(
			RGB{ 111, 155, 0 },  //color1
			RGB{ 0, 33, 111 }   //color2
		);
	}
	void Looper() {
		Cb.Next();
		Cb.SetPattern();
		delay(75);
	}
}; //Clau

class Prog05 {
public:
	OneColor OC = OneColor();
	void Looper() {
		for (int i = 0; i <= 255; i++)
		{
			OC.WhiteK(255 - i);
			OC.SetPattern();
			LedStrip.show();
			//stripL.show();
			delay(250);
		}
	}
}; //WhiteDemo

class Prog06 {
public:
	Rainbow RB = Rainbow();
	void Setup() {};
	void Looper() {
		RB.Next();
		RB.SetPattern();
		delay(40);
	}
}; //RainbowDemo

class Prog07 {
public:
	ColorLoop CL;
	static const int size = 6;
	RGB colors[size] = {
		{ 255, 0, 0 },
		{ 0, 255, 0 },
		{ 0, 0, 255 },
		{ 255, 255, 0 },
		{ 0, 255, 255 },
		{ 255, 0, 255}
	};
	void Setup() {

		CL = ColorLoop(colors, size, 32);
	};
	void Looper() {
		CL.Next();
		CL.SetPattern();
		delay(100);
	}
};

//********************************************************
//============  Operation
//*******************************************************/

OneColor oC = OneColor(RGB{ 95, 27, 88 }, BRIGHTNESS);

Prog00 p0 = Prog00();
Prog01 p1 = Prog01(); //Fire
Prog02 p2 = Prog02(); //Rainbow
Prog03 p3 = Prog03(); //Siren
Prog04 p4 = Prog04(); //Clau
Prog05 p5 = Prog05(); //WhiteDemo
Prog06 p6 = Prog06(); //RainbowDemo
Prog07 p7 = Prog07(); //ColorLoop

void setupStrip()
{
	LedStrip.begin();
	LedStrip.setBrightness(BRIGHTNESS);
	//oC.WhiteK(0); //False = nacht
	oC.SetPattern();
	p1.Setup();
	p7.Setup();
	LedStrip.show(); // Initialize all pixels to 'off'

}

void setup() {
	//setupStrip(stripR);
	setupStrip();
}

void loop() {
	p2.Looper();
	LedStrip.show();
}
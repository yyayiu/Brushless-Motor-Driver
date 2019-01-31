# Profiling Note
By Rex Cheng

## General
- Ticks frequency: 84000000 (half of main frequency)
- Ticks prescaler: 83
- Ticks period: 2
- Result: 1 ticks = 2 us

- Optimization level: *O1*
- Not optimized for time

***

## Floating Point calculator (FPU)

Using the following code:

	s32 starting_ticks = get_full_ticks();
	
	volatile float result[200] = {0.0f};
	for (u16 i=0;i<200;i++){
		result[i] = (float)rand() / (float)RAND_MAX;
	}
	
	for (u16 i=1;i<199;i++){
		result[i] = ((result[i]*65.37f*result[i-1] - result[i-2])+14.589f)/562.3458f;
	}

	for (u16 i=198;i>1;i--){
		result[i] = result[i]/result[i+1]/result[i-1];
	}
	
	s32 end_ticks = get_full_ticks();

Then find end_ticks - starting_ticks.
### With single point precision hardware *disabled*
- ticks diff = *808*

### With single point precision hardware *enabled*
- ticks diff = *101*

No new random seeds are generated.
Conclusion: FPU helps.

***

## Comparing square root funciton in different libraries (with FPU)

Using the following code:
	
	#define SAMPLES 100
	
	volatile s32 result[SAMPLES] = {0};
	s32 original[SAMPLES] = {0};
	
	for (u16 i=0;i<SAMPLES;i++){
		float32_t tmp = (float32_t)rand() /(float32_t)RAND_MAX *1000000.0f;
		original[i] = (s32)roundf(tmp);
	}
	
	s32 starting_ticks = get_full_ticks();
	
	for (u16 i=0;i<SAMPLES;i++){
		//result[i] = Sqrt(original[i]);
		result[i] = s32_sqrt(original[i]);
	}
	
	s32 end_ticks = get_full_ticks();
	
	double total_error = 0;
	#define scaling 1024.0
	for (u16 i=0;i<SAMPLES;i++){
		total_error += fabs((result[i] - sqrt(original[i])*scaling)/scaling*100.0/original[i])*1000;
	}

sqrt() function of math.h is considered to be the most accurate result.
All functions are declared inline.

### With approx_math Sqrt()
- Scaled by 1024
- Ticks used = 29
- Average error = 15.313709% (x1000)

### With quick_math s32_sqrt()
- A warp for __sqrtf(), scaled by 1024
- Ticks used = 16
- Average error = 0.034407% (x1000)

### With quick_math s32_sqrt2()
- A warp for sqrtf(), scaled by 1024
- Ticks used = 30
- Average error = 0.034407% (x1000)

Discussion: I made quite a lot of mistakes when I'm trying to test these.. The most important one is that roundf() is slow.. like REALLY SLOW.. It brings s32_sqrt from 16 ticks up to >40 ticks... OMFG. Just +0.5f and type cast that bitch..

Conclusion: s32_sqrt() rules.

***

## Comparing sine/cosine funciton in different libraries (with FPU)

Code used:
	volatile s32 result[360] = {0};

	s32 starting_ticks = get_full_ticks();
	u8 end_counter = 0;
	for (u16 i=0;i<360;i++){
		u16 testing_angle = i*100 + end_counter;
		//result[i] = cosf(testing_angle*PI/180.0f/100.0f)*10000;
		//result[i] = dsp_cosf(testing_angle*PI/180.0f/100.0f)*10000;
		//result[i] = int_cos(testing_angle/10);
		result[i] = dsp_cos(testing_angle);
		end_counter = (end_counter+1)%100;
	}
	
	s32 end_ticks = get_full_ticks();
	
	#define scale 32768.0
	end_counter = 0;
	double total_error = 0;
	for (u16 i=0;i<36;i++){
		float error[10];
		//tft_clear();
		//tft_println("%d %d", i, 0);
		for (u16 j=0;j<5;j++){
			u16 testing_angle = i*1000 + j*100 + end_counter;
			error[j] = (result[i*10+j] - cos(testing_angle*PI/180.0/100.0)*scale)/scale*1000.0;
			total_error += fabs(error[j]);
			end_counter = (end_counter+1)%100;
			//tft_println("%f", error[j]);
		}
		//tft_update();
		//while(!button_pressed(BUTTON_1));
		//while(button_pressed(BUTTON_1));
		//tft_clear();
		//tft_println("%d %d", i, 1);
		for (u16 j=5;j<10;j++){
			u16 testing_angle = i*1000 + j*100 + end_counter;
			error[j] = (result[i*10+j] - cos(testing_angle*PI/180.0/100.0)*scale)/scale*1000.0;
			total_error += fabs(error[j]);
			end_counter = (end_counter+1)%100;
			//tft_println("%f", error[j]);
		}
		//tft_update();
		//while(!button_pressed(BUTTON_1));
		//while(button_pressed(BUTTON_1));
	}

sin()/cos() function of math.h is considered to be the most accurate result.
Error is scaled by 1000.

### With math.h sin()/cos()
- Scaled by 10000
- Conversion from angle to radian involved
- Ticks used = 210/204
- Average error = 0.051233/0.051009

### With approx_math int_sin()/int_cos()
- Ticks used = 148/155
- Average error = 0.506137/0.513112

### With dsp_math dsp_sin()/dsp_cos()
- Ticks used = 149/149
- Average error = 0.070083/0.071107

### With dsp_math dsp_sinf()/dsp_cosf()
- Scaled by 10000
- Conversion from angle to radian involved
- Ticks used = 190/190
- Average error = 0.051233/0.051009

Discussion: sine and cosine functions from the DSP library are quite fast and accurate, but scaling would be a little bit more troublesome (it is using a fixed-point representation, approx. scaled by 32768, but it can never represent 1..., only -1)
Note: 
- dsp_cos() max error 0.22
- dsp_sin() max error 0.22

Conclusion: Will use dsp_sin() and dsp_cos(). But need good ways for other trigon. functions too, they are not in dsp library.

## tan() function

Code is similar to that of sin/cos.
Profiling is more difficult as angle approaches multiple of 90 deg.
Error scaled by 1000.

### approx_math's int_tan()
- Input angle is not scaled
- Output is only scaled by 100
- Intuitively very inaccurate
- Ticks used = 40
- Error ~20 (not at extreme angle)

### trigon_math's app_tan()
- Input angle scaled by 100
- Output scaled by 16384
- Ticks used = 62
- Error ~0.05 (not at extreme angle)

Conclusion: Will use new app_tan(). Be careful not to put angle ~multiple of 90 degree.

## atan() function

Not going to test its error, as the original one is very inaccurate anyway, and this one heavily built on the tan() function so its error would also depends on it.

The function uses binary search on the tan table and linear interpolation.

For 100 samples running time ~186 ticks.

Then I tested the atanf() with the same samples, it takes only 79 ticks...scaling included..... and it should be a lot more accurate....
Hmm...

I guess the binary search is still too slow and FPU is too great...

Why am I wasting my time to make my own atan...

Conclusion: I will make the app_atan() a wrapper for atanf(). Or just use floating point somehow.
Note: The same goes for atan2f(). They are probably fast enough, FML
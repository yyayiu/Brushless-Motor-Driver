#include "timer.h"

static volatile TimerActionStruct actions[TIMER_SIZE] = {0}; //binary heap
static volatile uint16_t size = 0;

static bool running = false;

void timer_init(){
	RCC_APB1PeriphClockCmd(TIMER_RCC, ENABLE);
	
	//Setup timer
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
  TIM_TimeBaseStructure.TIM_Prescaler = TIMER_PRESCALER;
  TIM_TimeBaseStructure.TIM_Period = 1;
  TIM_TimeBaseInit(TIMER_TIM, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIMER_TIM, TIM_IT_Update, ENABLE);
	
	//Setup interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//Disable preload as to disable shadow register, ARR update would require an update event otherwise
	TIM_ARRPreloadConfig(TIMER_TIM, DISABLE);
	TIM_Cmd(TIMER_TIM, DISABLE);
	running = false;
}

/**
* Register a event call that will happen after some time.
* @param action: The function to be called
* @param ms: The time to be waited
*/
void do_after(TimerAction action, uint32_t ms){
	do_after_for(action, ms, 1, 1);
}

/**
* Register a event call that will happen after some time.
* @param action: The function to be called
* @param ms: The time to be waited (0 ~ 2^32/QUANTUM_MULTIPLER)
* @param reload: The time ms to be reloaded when repeated
* @param repeat: How many times this function will be repeated
*/
void do_after_for(TimerAction action, uint32_t ms, uint32_t reload, uint16_t repeat){
	
	uint32_t curr_ticks = get_ticks();
	uint32_t trig_time = curr_ticks + ms;
	
	while(repeat > 0){
		if (size >= TIMER_SIZE){
			//Error
			while(1);
		}
		
		//Dig a hole
		int hole = size;
		while (hole != 0) {
			//Bubble up
			if (actions[(hole - 1) / 2].trig_time > trig_time) {
				actions[hole] = actions[(hole - 1)/2];
				hole = (hole - 1) / 2;
			}else {
				break;
			}
		}
		
		//Place in hole
		actions[hole].action = action;
		actions[hole].trig_time = trig_time;
		
		trig_time += reload;
		repeat--;
		size++;
	}
	
	if (running == false){
		TIM_SetCounter(TIMER_TIM, 0);
		if (ms*QUANTUM_MULTIPLER >= 65535){
			TIM_SetAutoreload(TIMER_TIM, 65535);
		}else{
			TIM_SetAutoreload(TIMER_TIM, ms*QUANTUM_MULTIPLER);
		}
		TIM_Cmd(TIMER_TIM, ENABLE);

		running = true;
		
	}else if (TIMER_TIM->ARR - TIM_GetCounter(TIMER_TIM) > ms*QUANTUM_MULTIPLER){
		TIM_SetAutoreload(TIMER_TIM, TIM_GetCounter(TIMER_TIM) + ms*QUANTUM_MULTIPLER);
	}
}

//Timer interrupt handler
void TIMER_IRQ_HANDLER(void){
	if (TIM_GetITStatus(TIMER_TIM, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIMER_TIM, TIM_IT_Update);

		uint32_t curr_ticks = get_ticks();
		//Keep popping
		while (size !=0 && actions[0].trig_time <= curr_ticks){
			//Execute min. function
			actions[0].action();
			//actions[0].action = 0; //Not required
			size--;
			
			//Dig a hole
			uint16_t hole = 0;
			
			//Bubble down
			while (hole < size){
				int child = hole * 2 + 1;
				if (child < size){
					if (child + 1 < size){
						//Find the smaller child
						if (actions[child].trig_time > actions[child + 1].trig_time){
							child++;
						}
					}
					if (actions[child].trig_time < actions[size].trig_time){
						actions[hole] = actions[child];
					}else{
						break;
					}
				}else{
					break;
				}
				hole = child;
			}
			
			actions[hole] = actions[size];
		}
		
		if (size == 0){
			//If no function remains
			TIM_Cmd(TIMER_TIM, DISABLE);
			running = false;
		}else{
			//else setup a new timer
			uint32_t min_quantum = (actions[0].trig_time - curr_ticks)*QUANTUM_MULTIPLER;
			
			if (min_quantum >= 65535){
				//If the target period > 16bit
				min_quantum = 65535;
			}
			TIM_SetCounter(TIMER_TIM, 0);
			TIM_SetAutoreload(TIMER_TIM, min_quantum);
		}
	}
}

// Return the current size of timer array
uint16_t get_timer_size(){
	return size;
}

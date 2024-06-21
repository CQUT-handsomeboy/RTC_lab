/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tftlcd.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RTC_TimeTypeDef sTime = {0};
RTC_AlarmTypeDef sAlarm = {0};
RTC_DateTypeDef sDate = {0};

int hours,minutes,seconds;

char temp_str[64];

int year,month,weekDay,date;

int alarmHours = 0,alarmMinutes = 0,alarmSeconds = 0;

typedef enum
{
	NORMAL = 0,
	SET_ALARM_HOUR,
	SET_ALARM_MIN,
	SET_ALARM_SEC,
	SET_TIME_HOUR,
	SET_TIME_MIN,
	SET_TIME_SEC,
	SET_TIME_YEAR,
	SET_TIME_MONTH,
	SET_TIME_DAY,
	SET_TIME_WEEKDAY,
	ALARM // ����ģʽ
}	MODE;

MODE mode = NORMAL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct MyAlarm_s
{
	int trigger_flag; //�Ƿ���������
	int hours;
	int minutes;
	int seconds;
} myAlarm;

int fputc(int ch, FILE *f)
{
	uint8_t temp[1] = {ch};
	HAL_UART_Transmit(
        &huart2 // �Զ��崮�ں�
    , temp, 1, 2); 
	return ch;
}

void display(int my_hours,int my_minutes,int my_seconds,int my_year,int my_month,int my_date,int my_weekDay);
void display1(int my_hours,int my_minutes,int my_seconds);
void display2(void); // ��ʾ����
void display_current_time(void);

void display2(void)
{
	sprintf(temp_str,"Alarm Set:%02d:%02d:%02d",myAlarm.hours,myAlarm.minutes,myAlarm.seconds);
	TFT_ShowLine(10,148,24,temp_str);
}

// A���� ������
void setAlarmToA(int seconds)
{
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = seconds;
  sAlarm.AlarmTime.SubSeconds = 23;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
                              |RTC_ALARMMASK_MINUTES;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;  // ����ν
  sAlarm.Alarm = RTC_ALARM_A;
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

// B���� ʱ��������
void setAlarmToB(int hours,int minutes,int seconds)
{
  sAlarm.AlarmTime.Hours = hours;
  sAlarm.AlarmTime.Minutes = minutes;
  sAlarm.AlarmTime.Seconds = seconds;
  sAlarm.AlarmTime.SubSeconds = 23;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1; // ����ν
  sAlarm.Alarm = RTC_ALARM_B;
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

int keyState = 0;
int pressedKey;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_TIM_Base_Start_IT(&htim1);
}

void onKeyUP()
{
	// �л�ģʽ
	printf("onKeyUP\r\n");
	
	if(mode == SET_TIME_WEEKDAY)
	{
		mode = SET_ALARM_HOUR;
	} else {
		mode ++;
	}
	
	TFT_Clear(0x00);
	
	switch(mode)
	{
		case NORMAL:
			TFT_ShowLine(10,70,24,"NORMAL");
			break;
		case SET_TIME_HOUR:
			TFT_ShowLine(10,70,24,"SET_TIME_HOUR");
			break;
		case SET_TIME_MIN:
			TFT_ShowLine(10,70,24,"SET_TIME_MIN");
			break;
		case SET_TIME_SEC:
			TFT_ShowLine(10,70,24,"SET_TIME_SEC");
			break;
		case SET_TIME_YEAR:
			TFT_ShowLine(10,70,24,"SET_TIME_YEAR");
			break;
		case SET_TIME_MONTH:
			TFT_ShowLine(10,70,24,"SET_TIME_MONTH");
			break;
		case SET_TIME_DAY:
			TFT_ShowLine(10,70,24,"SET_TIME_DAY");
			break;
		case SET_TIME_WEEKDAY:
			TFT_ShowLine(10,70,24,"SET_TIME_WEEKDAY");
			break;
		case SET_ALARM_HOUR:
			TFT_ShowLine(10,70,24,"SET_ALARM_HOUR");
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
		case SET_ALARM_MIN:
			TFT_ShowLine(10,70,24,"SET_ALARM_MIN");
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
		case SET_ALARM_SEC:
			TFT_ShowLine(10,70,24,"SET_ALARM_SEC");
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
	}
	display(hours,minutes,seconds,year,month,date,weekDay);

	printf("mode:%d\r\n",mode);
}

void getTime(void);

void onKey0()
{
	// OK
	switch(mode)
	{
		case SET_ALARM_HOUR:
		case SET_ALARM_MIN:
		case SET_ALARM_SEC:
			// �������ӵ�
			setAlarmToB(alarmHours,alarmMinutes,alarmSeconds);
			
			myAlarm.hours = alarmHours;
			myAlarm.minutes = alarmMinutes;
			myAlarm.seconds = alarmSeconds;
			myAlarm.trigger_flag = 1;
			break;
		case SET_TIME_HOUR:
		case SET_TIME_MIN:
		case SET_TIME_SEC:
		case SET_TIME_YEAR:
		case SET_TIME_MONTH:
		case SET_TIME_DAY:
		case SET_TIME_WEEKDAY:
			// ����ʱ�����ڵ�
			sDate.Date = date;
			sDate.Month = month;
			sDate.WeekDay = weekDay;
			sDate.Year = year;
			
			sTime.Hours = hours;
			sTime.Minutes = minutes;
			sTime.Seconds = seconds;
			
			HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
			break;
		case ALARM:
			// ����ȷ�Ϲر�
			myAlarm.trigger_flag = 0; // �ر�����
			break;
	}
	mode = NORMAL; // ̫����
	TFT_Clear(0x00);
	getTime();
	setAlarmToA(seconds + 1);
}

void onKey1()
{
	// -
	switch(mode)
	{
		case SET_TIME_HOUR:
			if(hours == 0)
			{
				hours = 23;
			} else {
				hours--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_MIN:
			if(minutes == 0)
			{
				minutes = 59;
			} else {
				minutes--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_SEC:
			if(seconds == 0)
			{
				seconds = 59;
			} else {
				seconds--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_YEAR:
			if(year == 0)
			{
				year = 99;
			} else {
				year--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_MONTH:
			if(month == 1)
			{
				month = 12;
			} else {
				month--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_DAY:
			if(date == 1)
			{
				date = 31;
			} else {
				date--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_WEEKDAY:
			if(weekDay == 1)
			{
				weekDay = 7;
			} else {
				weekDay--;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_ALARM_HOUR:
			if(alarmHours == 0)
			{
				alarmHours = 23;
			} else {
				alarmHours--;
			}
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
		case SET_ALARM_MIN:
			if(alarmMinutes == 0)
			{
				alarmMinutes = 59;
			} else {
				alarmMinutes--;
			}
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
		case SET_ALARM_SEC:
			if(alarmSeconds == 0)
			{
				alarmSeconds = 59;
			} else {
				alarmSeconds--;
			}
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
	}
}

void onKey2()
{
	// +
	switch(mode)
	{
		case SET_TIME_HOUR:
			if(hours == 23)
			{
				hours = 0;
			} else {
				hours++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_MIN:
			if(minutes == 59)
			{
				minutes = 0;
			} else {
				minutes++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_SEC:
			if(seconds == 59)
			{
				seconds = 0;
			} else {
				seconds++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_YEAR:
			if(year == 99)
			{
				year = 0;
			} else {
				year++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_MONTH:
			if(month == 12)
			{
				month = 1;
			} else {
				month++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_DAY:
			if(date == 31)
			{
				date = 1;
			} else {
				date++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_TIME_WEEKDAY:
			if(weekDay == 7)
			{
				weekDay = 1;
			} else {
				weekDay++;
			}
			display(hours,minutes,seconds,year,month,date,weekDay);
			break;
		case SET_ALARM_HOUR:
			if(alarmHours == 23)
			{
				alarmHours = 0;
			} else {
				alarmHours++;
			}
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
		case SET_ALARM_MIN:
			if(alarmMinutes == 59)
			{
				alarmMinutes = 1;
			} else {
				alarmMinutes++;
			}
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
		case SET_ALARM_SEC:
			if(alarmSeconds == 59)
			{
				alarmSeconds = 0;
			} else {
				alarmSeconds++;
			}
			display1(alarmHours,alarmMinutes,alarmSeconds);
			break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim1)
	{
	switch(keyState)
	{
		case 0:
			// δ����
			if(!HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin))
			{
				pressedKey = 0;
				keyState = 1;
				onKey0();
			} else if(!HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))
			{
				pressedKey = 1;
				keyState = 1;
				onKey1();
			} else if(!HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin))
			{
				pressedKey = 2;
				keyState = 1;
				onKey2();
				
			} else if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin))
			{
				pressedKey = -1;
				keyState = 1;
				onKeyUP();
			}
			break;
		case 1:
			// ����
		
		switch(pressedKey)
		{
			case 0:
				if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin))
				{
					keyState = 0;
				}
				break;
			case 1:
				if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin))
				{
					keyState = 0;
				}
				break;
			case 2:
				if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin))
				{
					keyState = 0;
				}
				break;
			case -1:
				if (!HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin))
				{
					keyState = 0;
				}
				break;
		}
		break;
	}
}
	else if (htim == &htim2)
	{
		// ������
		if(mode == ALARM)
		{
			HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin);
		} else {
			HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,0);
			HAL_TIM_Base_Stop_IT(&htim2);
		}
	}
}
void getTime(void)
{
	HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
	
	year = sDate.Year;
	month = sDate.Month;
	weekDay = sDate.WeekDay;
	date = sDate.Date;
	
	hours = sTime.Hours;
	minutes = sTime.Minutes;
	seconds = sTime.Seconds;
}

void display(int my_hours,int my_minutes,int my_seconds,int my_year,int my_month,int my_date,int my_weekDay)
{
	sprintf(temp_str,"%02d:%02d:%02d",my_hours,my_minutes,my_seconds);
	TFT_ShowLine(10,10,24,temp_str);
	sprintf(temp_str,"20%02d-%02d-%02d weekDay:%d",my_year,my_month,my_date,my_weekDay);
	TFT_ShowLine(10,34,24,temp_str);
}

void display_current_time(void)
{
	getTime();
	display(hours,minutes,seconds,year,month,date,weekDay);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *nhrtc)
{
	switch(mode)
	{
		case NORMAL:
		case ALARM:
		case SET_ALARM_HOUR:
		case SET_ALARM_MIN:
		case SET_ALARM_SEC:
			
		if(myAlarm.trigger_flag)
		{
			display2();
		}
		display_current_time();
		
		break;
	}
	setAlarmToA(seconds == 59 ? 0 :seconds+ 1  );
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *nhrtc)
{
	printf("B alarm!\r\n");
	mode = ALARM;
	HAL_TIM_Base_Start_IT(&htim2);
}

void display1(int my_hours,int my_minutes,int my_seconds)
{
	sprintf(temp_str,"%02d:%02d:%02d",my_hours,my_minutes,my_seconds);
	TFT_ShowLine(10,124,24,temp_str);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_FSMC_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	myAlarm.trigger_flag = 0;

#if 1
	HAL_RTC_Init(&hrtc);
	TFT_Init();
	TFT_Clear(0x0000);
	
	getTime();
	setAlarmToA(seconds + 1);
#endif


	while(1)
	{
		
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

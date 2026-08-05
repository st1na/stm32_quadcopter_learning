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
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "init.h"
#include "usbd_cdc_if.h"
#include "log.h"
#include "receiver.h"
#include "motor.h"
#include "pwm.h"
#include "gyro_acc.h"
#include "mpu_6000.h"
#include "barometer.h"
#include "bmp280.h"
#include "imu.h"
#include "pid.h"
#include "control.h"
#include "state_machine.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//INIT TASK Management
volatile uint32_t __FLAGS = 0;

/* PID controllers for attitude stabilization */
pid_t rollPID;
pid_t pitchPID;
pid_t yawPID;

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();

  /* USER CODE BEGIN 2 */


  //RC Init
  RCData_t* rcData;
  imuAngles_t imuAnglesFiltered = {0};
  //Gyro Accelerometer Device
  gyroAccDev_t gyroAccDev;
  gyroAccData_t rawScaledGyroAcc;
  gyroAccDev.gyroHardware = GYRO_ACC_MPU6500;
  //Set sensor type
  gyroAccOps_t mpu6000Ops = {
      .Init = Mpu6000Init,
      .ReadRawGyroAccData = Mpu6000ReadRawGyroAccData,
	  .ReadGyroAcc = Mpu6000ReadGyroAcc,
	  .CalibrateGyroAcc = Mpu6000CalibrateGyroAcc
  };
  //Set sensor protocol
  devInterface_t mpu6000DevInterface;
  sensorInterface_t mpu6000SensorInterface = {
  	.InitProtocol = SPIInitProtocol,
  	.ReadRegister = SPIReadRegister,
  	.WriteRegister = SPIWriteRegister,
	.BusBusy = SPIBusy,
	.devInterface = &mpu6000DevInterface
  };
  //Initialize the sensor
  gyroAccDev.ops = mpu6000Ops;
  gyroAccDev.interface = mpu6000SensorInterface;

//  gyroAccDev.rawData.gyro[X] = 0; gyroAccDev.rawData.gyro[Y] = 0; gyroAccDev.rawData.gyro[Z] = 0;
//  gyroAccDev.rawData.acc[X] = 0; gyroAccDev.rawData.acc[Y] = 0; gyroAccDev.rawData.acc[Z] = 0;

  baroDev_t baroDev;
  baroDev.gyroHardware = BARO_BMP280;
  //Set sensor type
  baroOps_t bmp280Ops = {
      .Init = Bmp280Init,
      .ReadAltitude= Bmp280ReadAltitude
  };
  //Set sensor protocol
  devInterface_t bmp280DevInterface;
  sensorInterface_t bmp280SensorInterface = {
  	.InitProtocol = SPIInitProtocol,
  	.ReadRegister = SPIReadRegister,
  	.WriteRegister = SPIWriteRegister,
	.BusBusy = SPIBusy,
	.devInterface = &bmp280DevInterface
  };
  //Initialize the sensor
  baroDev.ops = bmp280Ops;
  baroDev.interface = bmp280SensorInterface;

  //Motor initialization
  motorDevice_t *motorDevice_FR;
  motorDevice_t *motorDevice_FL;
  motorDevice_t *motorDevice_RL;
  motorDevice_t *motorDevice_RR;

  LogMessage(LOG_LEVEL_INFO, "MOTOR INITIALIZATION \r\n");
  motorDevice_FR = InitMotor(&htim3, TIM_CHANNEL_4);
  motorDevice_FL = InitMotor(&htim2, TIM_CHANNEL_3);
  motorDevice_RR = InitMotor(&htim3, TIM_CHANNEL_3);
  motorDevice_RL = InitMotor(&htim2, TIM_CHANNEL_4);
  motorDevice_t *motors[4] = {motorDevice_FR, motorDevice_FL, motorDevice_RL, motorDevice_RR};
  armState_e armState = DISARMED;
  smCtx_t smCtx = {0};


  LogEnable();
  LogMessage(LOG_LEVEL_INFO, "LOG ENABLED\r\n");
  LogMessage(LOG_LEVEL_INFO, "SYSTEM INITIALIZATION START\r\n");

  LogMessage(LOG_LEVEL_INFO, "RC INITIALIZATION \r\n");
  rcData = RCInit(IBUS);


  LogMessage(LOG_LEVEL_INFO, "GYRO ACCELEROMETER INITIALIZATION \r\n");
  gyroAccDev.ops.Init(&gyroAccDev);

  LogMessage(LOG_LEVEL_INFO, "PID CONTROLLER INITIALIZATION \r\n");
  PidInit(&rollPID,  1.5f, 0.05f, 0.4f, -500.0f, 500.0f);  // Tune these gains
  PidInit(&pitchPID, 1.5f, 0.05f, 0.4f, -500.0f, 500.0f);
  PidInit(&yawPID,   1.0f, 0.01f, 0.2f, -500.0f, 500.0f);

  LogMessage(LOG_LEVEL_INFO, "IMU SAMPLE TIMER INTERRUPT INITIALIZATION \r\n");
   HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1); // Initialize timer1 ch1 interrupt for IMU filter fixed sampling rate


  LogMessage(LOG_LEVEL_INFO, "BAROMETER INITIALIZATION \r\n");
  //baroDev.ops.Init(&baroDev);
  //TODO: Work on barometer code
  //TODO: Compass code
  LogMessage(LOG_LEVEL_INFO, "ARM STATE INITIALIZATION \r\n");
  smCtx.rcData    = rcData;
  smCtx.motors[0] = motorDevice_FR;
  smCtx.motors[1] = motorDevice_FL;
  smCtx.motors[2] = motorDevice_RL;
  smCtx.motors[3] = motorDevice_RR;
  smCtx.angles    = &imuAnglesFiltered;
  smCtx.rollPID   = &rollPID;
  smCtx.pitchPID  = &pitchPID;
  smCtx.yawPID    = &yawPID;

  LogMessage(LOG_LEVEL_INFO, "SYSTEM INITIALIZATION END\r\n");

  MX_IWDG_Init(); //Init watch dog after system initialization
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  rawScaledGyroAcc = gyroAccDev.ops.ReadGyroAcc(&gyroAccDev);


	  if(__FLAGS & FLAG_TIMER_IMU) {
		  imuAnglesFiltered = IMUCalculateAngles(rawScaledGyroAcc, MADGWICK);
		  __FLAGS &= ~FLAG_TIMER_IMU;
		  HAL_GPIO_TogglePin(DEBUG_PORT, DEBUG_PIN);
	  }

	  LogMessage(LOG_LEVEL_DEBUG, "PITCH:%.2f ROLL:%.2f YAW:%.2f\r\n",
					imuAnglesFiltered.rpyDeg[PITCH], imuAnglesFiltered.rpyDeg[ROLL], imuAnglesFiltered.rpyDeg[YAW]);

	  //baroDev.ops.ReadAltitude(&baroDev);

	  RCRead(rcData);

    LogMessage(LOG_LEVEL_DEBUG, "RC Input: THROTTLE=%d ROLL=%d PITCH=%d YAW=%d AUX1=%d AUX2=%d\r\n",
               rcData->rcInput[RC_THROTTLE],
               rcData->rcInput[RC_ROLL],
               rcData->rcInput[RC_PITCH],
               rcData->rcInput[RC_YAW],
               rcData->rcInput[RC_AUX1],
               rcData->rcInput[RC_AUX2]);

	  /* Arm state machine */
	  switch (armState) {
		  case DISARMED:       armState = HandleDisarmed(&smCtx);      break;
		  case ARMED:          armState = HandleArmed(&smCtx);         break;
		  case CALIBRATING:    armState = HandleCalibrating(&smCtx);   break;
		  case FAILSAFE:       armState = HandleFailsafe(&smCtx);      break;
		  case CRASH_DETECTED: armState = HandleCrashDetected(&smCtx); break;
		  default:             armState = DISARMED;                    break;
	  }

     HAL_IWDG_Refresh(&hiwdg);
  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
#ifdef USE_FULL_ASSERT
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

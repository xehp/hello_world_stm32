/**
  ******************************************************************************
  * @file    stm32l4xx_hal_gfxmmu.h
  * @author  MCD Application Team
  * @brief   Header file of GFXMMU HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_HAL_GFXMMU_H
#define __STM32L4xx_HAL_GFXMMU_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../../STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_def.h"

#if defined(GFXMMU)

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup GFXMMU
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** @defgroup GFXMMU_Exported_Types GFXMMU Exported Types
  * @{
  */

/** 
  * @brief  HAL GFXMMU states definition
  */
typedef enum
{
  HAL_GFXMMU_STATE_RESET = 0x00U, /*!< GFXMMU not initialized */
  HAL_GFXMMU_STATE_READY = 0x01U, /*!< GFXMMU initialized and ready for use */
}HAL_GFXMMU_StateTypeDef;

/** 
  * @brief  GFXMMU buffers structure definition
  */
typedef struct
{
  uint32_t Buf0Address; /*!< Physical address of buffer 0. */
  uint32_t Buf1Address; /*!< Physical address of buffer 1. */
  uint32_t Buf2Address; /*!< Physical address of buffer 2. */
  uint32_t Buf3Address; /*!< Physical address of buffer 3. */
}GFXMMU_BuffersTypeDef;

/** 
  * @brief  GFXMMU interrupts structure definition
  */
typedef struct
{
  FunctionalState Activation;     /*!< Interrupts enable/disable */
  uint32_t        UsedInterrupts; /*!< Interrupts used.
                                       This parameter can be a values combination of @ref GFXMMU_Interrupts.
                                       @note: Usefull only when interrupts are enabled. */
}GFXMMU_InterruptsTypeDef;

/** 
  * @brief  GFXMMU init structure definition
  */
typedef struct
{
  uint32_t                    BlocksPerLine; /*!< Number of blocks of 16 bytes per line.
                                                  This parameter can be a value of @ref GFXMMU_BlocksPerLine. */
  uint32_t                    DefaultValue;  /*!< Value returned when virtual memory location not physically mapped. */
  GFXMMU_BuffersTypeDef       Buffers;       /*!< Physical buffers addresses. */
  GFXMMU_InterruptsTypeDef    Interrupts;    /*!< Interrupts parameters. */
}GFXMMU_InitTypeDef;

/** 
  * @brief  GFXMMU handle structure definition
  */
typedef struct
{
  GFXMMU_TypeDef          *Instance; /*!< GFXMMU instance */
  GFXMMU_InitTypeDef      Init;      /*!< GFXMMU init parameters */
  HAL_GFXMMU_StateTypeDef State;     /*!< GFXMMU state */
  __IO uint32_t           ErrorCode; /*!< GFXMMU error code */
}GFXMMU_HandleTypeDef;

/** 
  * @brief  GFXMMU LUT line structure definition
  */
typedef struct
{
  uint32_t LineNumber;        /*!< LUT line number.
                                   This parameter must be a number between Min_Data = 0 and Max_Data = 1023. */
  uint32_t LineStatus;        /*!< LUT line enable/disable.
                                   This parameter can be a value of @ref GFXMMU_LutLineStatus. */
  uint32_t FirstVisibleBlock; /*!< First visible block on this line.
                                   This parameter must be a number between Min_Data = 0 and Max_Data = 255. */
  uint32_t LastVisibleBlock;  /*!< Last visible block on this line.
                                   This parameter must be a number between Min_Data = 0 and Max_Data = 255. */
  int32_t  LineOffset;        /*!< Offset of block 0 of the current line in physical buffer.
                                   This parameter must be a number between Min_Data = -4080 and Max_Data = 4190208.
                                   @note: Line offset has to be computed with the following formula:
                                          LineOffset = [(Blocks already used) - (1st visible block)]*BlockSize. */
}GFXMMU_LutLineTypeDef;

/**
  * @}
  */ 
/* End of exported types -----------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup GFXMMU_Exported_Constants GFXMMU Exported Constants
  * @{
  */

/** @defgroup GFXMMU_BlocksPerLine GFXMMU blocks per line
  * @{
  */
#define GFXMMU_256BLOCKS 0x00000000U     /*!< 256 blocks of 16 bytes per line */
#define GFXMMU_192BLOCKS GFXMMU_CR_192BM /*!< 192 blocks of 16 bytes per line */
/**
  * @}
  */

/** @defgroup GFXMMU_Interrupts GFXMMU interrupts
  * @{
  */
#define GFXMMU_AHB_MASTER_ERROR_IT GFXMMU_CR_AMEIE /*!< AHB master error interrupt */
#define GFXMMU_BUFFER0_OVERFLOW_IT GFXMMU_CR_B0OIE /*!< Buffer 0 overflow interrupt */
#define GFXMMU_BUFFER1_OVERFLOW_IT GFXMMU_CR_B1OIE /*!< Buffer 1 overflow interrupt */
#define GFXMMU_BUFFER2_OVERFLOW_IT GFXMMU_CR_B2OIE /*!< Buffer 2 overflow interrupt */
#define GFXMMU_BUFFER3_OVERFLOW_IT GFXMMU_CR_B3OIE /*!< Buffer 3 overflow interrupt */
/**
  * @}
  */

/** @defgroup GFXMMU_Error_Code GFXMMU Error Code
  * @{
  */
#define GFXMMU_ERROR_NONE             0x00000000U    /*!< No error */
#define GFXMMU_ERROR_BUFFER0_OVERFLOW GFXMMU_SR_B0OF /*!< Buffer 0 overflow */
#define GFXMMU_ERROR_BUFFER1_OVERFLOW GFXMMU_SR_B1OF /*!< Buffer 1 overflow */
#define GFXMMU_ERROR_BUFFER2_OVERFLOW GFXMMU_SR_B2OF /*!< Buffer 2 overflow */
#define GFXMMU_ERROR_BUFFER3_OVERFLOW GFXMMU_SR_B3OF /*!< Buffer 3 overflow */
#define GFXMMU_ERROR_AHB_MASTER       GFXMMU_SR_AMEF /*!< AHB master error */
/**
  * @}
  */

/** @defgroup GFXMMU_LutLineStatus GFXMMU LUT line status
  * @{
  */
#define GFXMMU_LUT_LINE_DISABLE 0x00000000U     /*!< LUT line disabled */
#define GFXMMU_LUT_LINE_ENABLE  GFXMMU_LUTxL_EN /*!< LUT line enabled */
/**
  * @}
  */

/**
  * @}
  */ 
/* End of exported constants -------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/** @defgroup GFXMMU_Exported_Macros GFXMMU Exported Macros
 * @{
 */

/** @brief  Reset GFXMMU handle state.
  * @param  __HANDLE__ GFXMMU handle.
  * @retval None
  */
#define __HAL_GFXMMU_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_GFXMMU_STATE_RESET)

/**
  * @}
  */
/* End of exported macros ----------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup GFXMMU_Exported_Functions GFXMMU Exported Functions
  * @{
  */

/** @addtogroup GFXMMU_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization and de-initialization functions *****************************/
HAL_StatusTypeDef HAL_GFXMMU_Init(GFXMMU_HandleTypeDef *hgfxmmu);
HAL_StatusTypeDef HAL_GFXMMU_DeInit(GFXMMU_HandleTypeDef *hgfxmmu);
void HAL_GFXMMU_MspInit(GFXMMU_HandleTypeDef *hgfxmmu);
void HAL_GFXMMU_MspDeInit(GFXMMU_HandleTypeDef *hgfxmmu);
/**
  * @}
  */

/** @addtogroup GFXMMU_Exported_Functions_Group2 Operations functions
  * @{
  */
/* Operation functions ********************************************************/
HAL_StatusTypeDef HAL_GFXMMU_ConfigLut(GFXMMU_HandleTypeDef *hgfxmmu,
                                       uint32_t FirstLine,
                                       uint32_t LinesNumber,
                                       uint32_t Address);

HAL_StatusTypeDef HAL_GFXMMU_DisableLutLines(GFXMMU_HandleTypeDef *hgfxmmu,
                                             uint32_t FirstLine,
                                             uint32_t LinesNumber);

HAL_StatusTypeDef HAL_GFXMMU_ConfigLutLine(GFXMMU_HandleTypeDef *hgfxmmu, GFXMMU_LutLineTypeDef *lutLine);

HAL_StatusTypeDef HAL_GFXMMU_ModifyBuffers(GFXMMU_HandleTypeDef *hgfxmmu, GFXMMU_BuffersTypeDef *Buffers);

void HAL_GFXMMU_IRQHandler(GFXMMU_HandleTypeDef *hgfxmmu);

void HAL_GFXMMU_ErrorCallback(GFXMMU_HandleTypeDef *hgfxmmu);
/**
  * @}
  */

/** @defgroup GFXMMU_Exported_Functions_Group3 State functions
  * @{
  */
/* State function *************************************************************/
HAL_GFXMMU_StateTypeDef HAL_GFXMMU_GetState(GFXMMU_HandleTypeDef *hgfxmmu);

uint32_t HAL_GFXMMU_GetError(GFXMMU_HandleTypeDef *hgfxmmu);
/**
  * @}
  */

/**
  * @}
  */
/* End of exported functions -------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/** @defgroup GFXMMU_Private_Macros GFXMMU Private Macros
* @{
*/
#define IS_GFXMMU_BLOCKS_PER_LINE(VALUE) (((VALUE) == GFXMMU_256BLOCKS) || \
                                          ((VALUE) == GFXMMU_192BLOCKS))

#define IS_GFXMMU_BUFFER_ADDRESS(VALUE) (((VALUE) & 0xFU) == 0U)

#define IS_GFXMMU_INTERRUPTS(VALUE) (((VALUE) & 0x1FU) != 0U)

#define IS_GFXMMU_LUT_LINE(VALUE) ((VALUE) < 1024U)

#define IS_GFXMMU_LUT_LINES_NUMBER(VALUE) (((VALUE) > 0U) && ((VALUE) <= 1024U))

#define IS_GFXMMU_LUT_LINE_STATUS(VALUE) (((VALUE) == GFXMMU_LUT_LINE_DISABLE) || \
                                          ((VALUE) == GFXMMU_LUT_LINE_ENABLE))

#define IS_GFXMMU_LUT_BLOCK(VALUE) ((VALUE) < 256U)

#define IS_GFXMMU_LUT_LINE_OFFSET(VALUE) (((VALUE) >= -4080) && ((VALUE) <= 4190208))
/**
  * @}
  */ 
/* End of private macros -----------------------------------------------------*/

/**
  * @}
  */ 

/**
  * @}
  */
#endif /* GFXMMU */
#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_HAL_GFXMMU_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

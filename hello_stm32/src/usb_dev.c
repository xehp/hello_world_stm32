#if 0

#include "cfg.h"
#include "sys.h"
#include "usb_dev.h"

#if (defined STM32L151xB)
#include "stm32l1xx_ll_usb.h"

//static uint8_t usb_rx_buffer[1024];

void __attribute__ ((interrupt, used)) USB_HP_IRQHandler(void)
{
	// TODO
}

void __attribute__ ((interrupt, used)) USB_LP_IRQHandler(void)
{
	// TODO
}

void __attribute__ ((interrupt, used)) USB_FS_WKUP_IRQHandler(void)
{
	// TODO
}


// NOTE! This code is not working yet...
void usb_init(void)
{
	//uint32_t tmp = 0;

	/*
	[RM0038 Rev 16] 6.2.4 PLL
	If the USB or SDIO interface is used in the application, the PLL VCO clock (defined by
	the PLL multiplication factor) must be programmed to output a 96 MHz frequency. This
	is required to provide a 48 MHz clock to the USB or SDIO (SDIOCLK or USBCLK =
	PLLVCO/2).

	This must be done in "system_stm32l1xx.c" so check that one out.
	*/


	/*
	[RM0038 Rev 16] 24.4.2 System and power-on reset
	Upon system and power-on reset, the first operation the application software should perform
	is to provide all required clock signals to the USB peripheral and subsequently de-assert its
	reset signal so to be able to access its registers. The whole initialization sequence is
	hereafter described.
	*/

  	RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN_Msk;
  	RCC->APB1ENR |= RCC_APB1ENR_USBEN_Msk;


  	// Clear pending interrupts
  	USB->ISTR = 0U;
  	sysSleepMs(10);

  	// Put USB device in reset and power it on
	// Reset state for USB_CNTR is 3 so its already in reset but off.
	// So what we do is turn it on nut keep it in reset.
  	// Bit 1 PDWN: Power down
  	// Bit 0 FRES: Force USB Reset
  	USB->CNTR = (uint16_t)USB_CNTR_FRES;
  	sysSleepMs(100);


  	// Set PA11 & PA12 to alternate USB function? Hmm USB is "Additional functions" not "Alternate functions".
  	// Perhaps this happens automatically when USB is turned on?

    /*
    Alternate function register.
    Set it to USART2. Remember to set the port mode to alternate mode also.
    */
#if 0
    const uint32_t afrIndex = 11 >> 3;
    const uint32_t afrOffset = (11 & 0x7) * 4;
    tmp = GPIOA->AFR[afrIndex];
    tmp &= ~(15U << afrOffset);
    tmp |= GPIO_AF_USB << afrOffset;
    GPIOA->AFR[afrIndex] = tmp;

    const uint32_t afrIndex = 12 >> 3;
    const uint32_t afrOffset = (12 & 0x7) * 4;
    tmp = GPIO_AF_USB->AFR[afrIndex];
    tmp &= ~(15U << afrOffset);
    tmp |= GPIO_AF8_USB << afrOffset;
    GPIOA->AFR[afrIndex] = tmp;
#endif

    /* [1]
    Configure the IO Speed
    00: Low speed (reset state for most IO pins)
    01: Medium speed
    10: High speed
    11: Very high speed (reset state for PA11, PA12 so perhaps not needed)
    */
    GPIOA->OSPEEDR |= (3U << (11 * 2)) | (3U << (12 * 2));

  	/*
	[RM0038 Rev 16] 24.4.2 System and power-on reset
  	An internal pull-up resistor is connected to Data+ (DP) line and controlled by software using
  	the USB_PU bit in the SYSCFG_PMC register of the SYSCFG module (refer to Section 8:
  	System configuration controller (SYSCFG) and routing interface (RI). When the USB_PU bit
  	is reset, no pull-up is connected to the DP line and the device cannot be detected on the
  	USB bus (if no external pull-up is connected). When the USB_PU bit is set, the internal pull-
  	up is connected and the device can be detected on the USB bus.
	*/
  	// [RM0038 Rev 16] 8.5.2 SYSCFG peripheral mode configuration register (SYSCFG_PMC)
  	// USB_PU bit in the SYSCFG_PMC register of the SYSCFG, this makes this device detectable to USB Host.
  	SYSCFG->PMC |= SYSCFG_PMC_USB_PU_Msk;
  	sysSleepMs(10);
  	// Or shall we do this further down after starting clocks (as a test).

  	/*
	[RM0038 Rev 16] 24.4.2 System and power-on reset
  	As a first step application software needs to activate register macrocell clock and de-assert
  	macrocell specific reset signal using related control bits provided by device clock
  	management logic.
  	*/
  	// Do not know for sure what a "macrocell clock" is. Will try this:

  	// Bit 23 USBRST: USB reset active
  	RCC->APB1RSTR |= RCC_APB1RSTR_USBRST_Msk;
  	sysSleepMs(10);

  	// Bit 23 USBEN: USB clock enable
  	RCC->APB1ENR |= RCC_APB1ENR_USBEN_Msk;
  	sysSleepMs(10);

  	// Perhaps this is not needed? It will not hurt for now. Try remove only when everything works.
  	RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN;
  	sysSleepMs(10);

  	// Bit 23 USBRST: USB reset deactive
  	RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;
  	sysSleepMs(10);


	/*
	[RM0038 Rev 16] 24.4.2 System and power-on reset
	After that, the analog part of the device related to the USB transceiver must be switched on
	using the PDWN bit in CNTR register, which requires a special handling. This bit is intended
	to switch on the internal voltage references that supply the port transceiver. This circuit has
	a defined startup time (t STARTUP specified in the datasheet) during which the behavior of the
	USB transceiver is not defined. It is thus necessary to wait this time, after setting the PDWN
	bit in the CNTR register, before removing the reset condition on the USB part (by clearing
	the FRES bit in the CNTR register). Clearing the ISTR register then removes any spurious
	pending interrupt before any other macrocell operation is enabled.
	*/
  	USB->CNTR &= ~USB_CNTR_PDWN_Msk;
  	sysSleepMs(100);


  	// [RM0038 Rev 16] 8.5.2 SYSCFG peripheral mode configuration register (SYSCFG_PMC)
  	// USB_PU bit in the SYSCFG_PMC register of the SYSCFG, this makes this device detectable to USB Host.
  	SYSCFG->PMC |= SYSCFG_PMC_USB_PU_Msk;
  	sysSleepMs(10);


  	// "de-assert its reset signal", so release USB device from reset
  	USB->CNTR &= ~(uint16_t)USB_CNTR_FRES;;
  	sysSleepMs(10);

  	// Clear pending interrupts
  	USB->ISTR = 0U;
  	sysSleepMs(10);


	/*
	[RM0038 Rev 16] 24.4.2 System and power-on reset
	At system reset, the microcontroller must initialize all required registers and the packet
	buffer description table, to make the USB peripheral able to properly generate interrupts and
	data transfers. All registers not specific to any endpoint must be initialized according to the
	needs of application software (choice of enabled interrupts, chosen address of packet
	buffers, etc.). Then the process continues as for the USB reset case (see further
	paragraph).
	*/
  	// TODO




  	// "(EF) bit of the USB_DADDR"
  	USB->DADDR |= USB_DADDR_EF_Msk;
  	sysSleepMs(10);


  	// TODO
  	// "As a first step application software needs to activate register macrocell clock and de-assert
  	// macrocell specific reset signal using related control bits provided by device clock
  	// management logic."
  	// Will assume that this "device clock" is about the "USB clock enable" because I do not know what "device clock" would be otherwise.


  	// USB_DevInit Initializes the USB controller registers for device mode



  	// USB->CNTR = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_ERRM | USB_CNTR_SUSPM | USB_CNTR_WKUPM;

  	// Also needed?
  	// USB->CNTR |= USB_CNTR_SOFM;



  	USB->BTABLE = BTABLE_ADDRESS;
  	sysSleepMs(10);



  	// TODO None of the above say anything about virtual com port CDC so have we set it up for that?


  	// For much more advanced use of USB, read more here:
  	// https://github.com/dmitrystu/libusb_stm32/tree/master/src
}

#endif
#endif

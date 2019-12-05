#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "xstatus.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"

#define EMIO_0 78	// 0~77 are number of MIO
#define EMIO_1 79	// 78 for EMIO[0] and 79 for EMIO[1] and so on.
					// Reference to ug1085 page.770

XGpioPs EMIO;	// a XGpioPs struct named EMIO
XScuGic INTC;	// a XScuGic struct named INTC

// Interrupt handler function
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	printf("interrupt occured\n");
}

int main()
{
	int status;
	int x;

	XGpioPs_Config *GPIOConfig;
	// Look up gpiops' configuration from memory,
	// XPAR_XGPIOPS_0_DEVICE_ID in xparameters.h
	GPIOConfig = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	// Initialize EMIO which is XGpioPs
	status = XGpioPs_CfgInitialize(&EMIO, GPIOConfig, GPIOConfig->BaseAddr);
	// Check successfully initialize or not
	if(status != XST_SUCCESS) return XST_FAILURE;
	// Set direction of gpio input
	XGpioPs_SetDirectionPin(&EMIO, EMIO_0, 0);
	// Set direction of gpio output
	XGpioPs_SetDirectionPin(&EMIO, EMIO_1, 1);
	// Enable output
	XGpioPs_SetOutputEnablePin(&EMIO, EMIO_1, 1);
	// Set output voltage low
	XGpioPs_WritePin(&EMIO, EMIO_1, 0);


	XScuGic_Config *INTCConfig;
	// Look up scugic's configuration from memory
	INTCConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	//Initialize INTC which is XScuGic
	status = XScuGic_CfgInitialize(&INTC, INTCConfig, INTCConfig->CpuBaseAddress);
	// Check successfully initialize or not
	if(status != XST_SUCCESS) return XST_FAILURE;
	// Connect Interrupt Controller, Interrupt ID, Handler and Input Parameter of handler
	// XPAR_XGPIOPS_0_INTR in xparameters_ps.h
	status = XScuGic_Connect(&INTC, XPAR_XGPIOPS_0_INTR, (Xil_ExceptionHandler)XGpioPs_IntrHandler, (void*)&EMIO);
	// Check successfully connect or not
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Interrupt Controller enable the GPIO interrupt which ID is XPAR_XGPIOPS_0_INTR
	// XPAR_XGPIOPS_0_INTR in xparameters_ps.h
	XScuGic_Enable(&INTC, XPAR_XGPIOPS_0_INTR);


	// Exception initialize
	Xil_ExceptionInit();
	// Register handler if exception ID "XIL_EXCEPTION_ID_INT" occur,
	// handle by XScuGic_InterruptHandler and its input parameter is &INTC
	// XIL_EXCEPTION_ID_INT in xil_exception.h
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &INTC);
	// Enable xil_exception
	Xil_ExceptionEnable();

	// Set the trigger type of a specific pin to a rising edge
	// XGPIOPS_IRQ_TYPE_EDGE_RISING in xgpiops.h
	XGpioPs_SetIntrTypePin(&EMIO, EMIO_0, XGPIOPS_IRQ_TYPE_EDGE_RISING);
	// Relate the interrupt handler function to a specific pin
	XGpioPs_SetCallbackHandler(&EMIO, (void *)&EMIO, IntrHandler);
	// Enable pin interrupt
	XGpioPs_IntrEnablePin(&EMIO, EMIO_0);
	printf("all run");
	while(1)
	{
		// Convenience to change the output voltage
		scanf("%d", &x);
		XGpioPs_WritePin(&EMIO, EMIO_1, x&0x01);
	}
    return 0;
}

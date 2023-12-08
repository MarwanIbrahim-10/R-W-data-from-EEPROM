/*
 * main.c
 *
 * Created: 12/8/2023 9:32:24 PM
 *  Author: Marwan & Ahmed
 */ 

#define F_CPU 16000000UL  // Define the CPU frequency for delay calculations

#include <avr/io.h>       // Include AVR input/output header for IO port access
#include <avr/eeprom.h>   // Include AVR EEPROM header for EEPROM functionality

#define BAUD 9600         // Define baud rate for UART
#define BRC ((F_CPU/16/BAUD) - 1)  // Calculate baud rate register value

// Initialize UART for serial communication
void UART_init() {
	UBRR0H = (BRC >> 8);         // Set upper part of the baud rate
	UBRR0L = BRC;                // Set lower part of the baud rate

	UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // Enable transmitter and receiver
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Set frame: 8data, 1 stop bit
}

// Transmit a single character over UART
void UART_transmit(char data) {
	while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer, sends the data
}

// Send a string over UART
void UART_Send_String(char* str) {
	int i = 0;
	while (str[i]) {
		UART_transmit(str[i]); // Transmit each character of the string
		i++;
	}
}

// Write a byte to a specific EEPROM address
void EEPROM_Write(uint16_t uiAddress, uint8_t ucData) {
	while(EECR & (1<<EEPE)); // Wait for completion of previous write
	EEAR = uiAddress;        // Set up address register
	EEDR = ucData;           // Set up data register
	EECR |= (1<<EEMPE);      /// Enable EEPROM master write enable
	EECR |= (1<<EEPE);       // Start EEPROM write
}

// Read a byte from a specific EEPROM address
uint8_t EEPROM_Read(uint16_t uiAddress) {
	while(EECR & (1<<EEPE)); // Wait for completion of previous write
	EEAR = uiAddress;        // Set up address register
	EECR |= (1<<EERE);       // Start EEPROM read
	return EEDR;             // Return data from data register
}

// Write a string to EEPROM starting from a specific address
void EEPROM_Write_String(uint16_t uiAddress, const char* string) {
	// Write each character to EEPROM
	for (uint16_t i = 0; string[i] != '\0'; ++i) {
		EEPROM_Write(uiAddress + i, string[i]); // Write each character
	}
}

// Read a string from EEPROM starting from a specific address
void EEPROM_Read_String(uint16_t uiAddress, char* buffer, uint16_t length) {
	for (uint16_t i = 0; i < length; ++i) {
		buffer[i] = EEPROM_Read(uiAddress + i); // Read each character
	}
	buffer[length] = '\0'; // Null-terminate the string 
}

int main(void) {
	UART_init(); // Initialize UART communication
	
	char data_to_write[] = "Hello World"; // Data to write to EEPROM
	char read_data[50]; // Buffer to store read string
	
	 // Write to EEPROM
	 EEPROM_Write_String(0, data_to_write);

	 // Read from EEPROM
	 EEPROM_Read_String(0, read_data, sizeof(data_to_write));

	 // Send read_data over UART
	 UART_Send_String(read_data);
	 
	while (1) {
	}
	return 0;
}

/*
 ====================================================================================
 Name        : ADXL345.c
 Author      : JSC
 Website	 : https://www.jscblog.com/post/bbb-adxl345-3-axis-accelerometer
 Description : This project is to communicate with the ADXL345 3-axis Accelerometer
 	 	 	   sensor using the BeagleBone Black.
 ====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

#define ADXL345_SLAVE_ADDR		0x53			// Slave Address
#define I2C_FILE_PATH			"/dev/i2c-2"	// Files path for the i2c-2 directory

// Registers taken from the ADXL345 Datasheet
#define ADXL345_REG_BW_RATE		0x2C
#define ADXL345_REG_POWERCTL	0x2D
#define ADXL345_REG_DATA_FORMAT	0x31
#define ADXL345_REG_DATAX0		0x32
#define ADXL345_REG_DATAX1		0x33
#define ADXL345_REG_DATAY0		0x34
#define ADXL345_REG_DATAY1		0x35
#define ADXL345_REG_DATAZ0		0x36
#define ADXL345_REG_DATAZ1		0x37

int file;

int adxl354_init()
{
	// Two element buffer to hold address and data
	char writeBuffer[2];

	// Open file
	if ((file=open(I2C_FILE_PATH, O_RDWR)) < 0)
	{
		perror("Failed to open the bus. \n");
		return -1;
	}

	// Connect to slave
	if ((ioctl(file, I2C_SLAVE, ADXL345_SLAVE_ADDR)) < 0)
	{
		perror("Failed to connect to the sensor\n");
		return -1;
	}

	// Configure the BW Rate to 3200 and turn low power off
	writeBuffer[0] = ADXL345_REG_BW_RATE;
	writeBuffer[1] = 0x0A;
	if (write(file, writeBuffer, 2) != 2)
	{
		perror("Failed to write to the register\n");
		return -1;
	}

	// Configure the Data Format register to set full res
	writeBuffer[0] = ADXL345_REG_DATA_FORMAT;
	writeBuffer[1] = 0x08;
	if (write(file, writeBuffer, 2) != 2)
	{
		perror("Failed to write to the register\n");
		return -1;
	}

	// Configure the Power Control register to turn on measurment mode
	writeBuffer[0] = ADXL345_REG_POWERCTL;
	writeBuffer[1] = 0x08;
	if (write(file, writeBuffer, 2) != 2)
	{
		perror("Failed to write to the register\n");
		return -1;
	}

	sleep(1);
	close(file);

	return 0;
}

int adxl345_read()
{
	short acc_x, acc_y, acc_z;

	// Open file
	if ((file=open(I2C_FILE_PATH, O_RDWR)) < 0)
	{
		perror("\n");
		return -1;
	}

	// Connect to slave
	if ((ioctl(file, I2C_SLAVE, ADXL345_SLAVE_ADDR)) < 0)
	{
		perror("Failed to connect to the sensor\n");
		return -1;
	}

	// Set the buffer pointer to the start of the data registers (X0)
	char buffer_pointer[1] = {0x32};
	if (write(file, buffer_pointer, 1) != 1)
	{
		perror("Failed to write to the register\n");
		return -1;
	}

	// Read the six data registers
	char acc_buffer[6];
	if (read(file, acc_buffer, 6) != 6)
	{
		perror("Failed to read from the buffer\n");
		return -1;
	}

	close(file);

	/* The data is 10 bits wide, meaning the high address contains the upper two bits,
	   therefore it's shifted 8 bits to the left (to bit 10 and 9 position). The lower address
	   is OR with the first 8 bits of the 16 bit register.*/
	acc_x = ((short)acc_buffer[1] << 8) | (short)acc_buffer[0];
	acc_y = ((short)acc_buffer[3] << 8) | (short)acc_buffer[2];
	acc_z = ((short)acc_buffer[5] << 8) | (short)acc_buffer[4];

	printf("Acc => X:%d Y:%d Z:%d\n", acc_x,acc_y,acc_z);

	return 0;
}

int main(void)
{
	adxl354_init();
	usleep(1000);

	while(1)
	{
		adxl345_read();
		sleep(1);
	}
	return 0;
}

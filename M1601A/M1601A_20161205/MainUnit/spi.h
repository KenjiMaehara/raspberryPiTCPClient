/*
 * spi.h
 *
 * 
 * 
 */ 
#ifndef SPI_H_
#define SPI_H_


void spi_init(void);
u8 spi_write_single(u8 byte);
u8 spi_read_single(void);
u8 spi_write(u8* data, u16 len);
void spi_read(u8* data, u16 len);
void spi_select(void);
void spi_deselect(void);
u16 spi_read_somi(void);

#define WAIT_EORX() while (! (SPIF.STATUS & SPI_IF_bm)){} 


#endif /* SPI_H_ */
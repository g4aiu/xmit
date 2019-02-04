void rtty_txstring (char * string);
void rtty_txbyte (char c);
void rtty_txbit (int bit);
uint16_t gps_CRC16_checksum (char *string);
uint16_t _crc_xmodem_update (uint16_t crc, uint8_t data);

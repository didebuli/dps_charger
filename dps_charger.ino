

/*

cf  BANK0:PORTB, RB2 ; turn psu off!
bcf BANK0:PORTC, RC5 ; power off


BANK0:PORTA, RA4 ; adjust I2c address based on address inputs (PA4,PA5,PE3)






*/



   // ! = Known   ? = Guessed
   // T = Tested, A = From asm, D = From doc

bool ignore_registers[0xFF+1];
uint8_t inspect_register;

uint8_t test_cmd  = 0x01;
uint16_t test_data = 0x0000;

void setup_ignore_registers(){

  ignore_registers[0x00] = false; 
  ignore_registers[0x02] = false; // [?D] Flags. Power Good?
  ignore_registers[0x04] = false; // [?T] Flags? 0x00 0x00 -> 0x00 0x16 when pulling power
  ignore_registers[0x06] = false; // [?T] Under voltage alarm flag? b.3 = pot3 overflow?
  ignore_registers[0x08] = true;  // [!T] grid voltage
  ignore_registers[0x0A] = true;  // [!T] grid amperage
  ignore_registers[0x0C] = true;  // [!T] grid wattage
  ignore_registers[0x0E] = true;  // [!T] out voltage

  ignore_registers[0x10] = true;  // [!T] out amperage
  ignore_registers[0x12] = true;  // [!T] out wattage
  ignore_registers[0x14] = false;
  ignore_registers[0x16] = false;
  ignore_registers[0x18] = false;
  ignore_registers[0x1A] = true;  // [!T] intake temp
  ignore_registers[0x1C] = true;  // [!T] internal temp
  ignore_registers[0x1E] = true;  // [!T] fan speed actual


  ignore_registers[0x20] = false;
  ignore_registers[0x22] = false;
  ignore_registers[0x24] = false;
  ignore_registers[0x26] = false;
  ignore_registers[0x28] = false;
  ignore_registers[0x2A] = false;  
  ignore_registers[0x2C] = true;  // [!T] Ws in 32bits
  ignore_registers[0x2E] = false;  // [?T] Very slow counter. Resets when unplugging

  
  ignore_registers[0x30] = true;  // [!T] Uptime s
  ignore_registers[0x32] = true;  // [?D] Peak watts in 
  ignore_registers[0x34] = true;  // [?D] Min amps in
  ignore_registers[0x36] = true;  // [?D] Peak amps out
  ignore_registers[0x38] = false; 
  ignore_registers[0x3A] = false; // [?D] Cool flags1

    // 0x3A:5 seems to be "Power out is on" flag
/*0x3A
00000000 00000001
Register 0x3A changed from 0 to 1 after reg 0x3A bit 0
00000000 00000010
Register 0x3A changed from 1 to 2 after reg 0x3A bit 1
00000000 00000100
Register 0x3A changed from 2 to 4 after reg 0x3A bit 2
00000000 00001000
Register 0x3A changed from 4 to 8 after reg 0x3A bit 3
00000000 00010000
Register 0x3A changed from 8 to 16 after reg 0x3A bit 4
00000000 00100000
Register 0x3A changed from 16 to 0 after reg 
0x3C
00000000 00000001
Register 0x02 changed from 775 to 839 after reg 0x3C bit 0
Register 0x3A changed from 0 to 64 after reg 0x3C bit 0
Register 0x3C changed from 0 to 1 after reg 0x3C bit 0
00000000 00000010
Register 0x3C changed from 1 to 2 after reg 0x3C bit 1
00000000 00000100
Register 0x3C changed from 2 to 4 after reg 0x3C bit 2
00000000 00001000
Register 0x3C changed from 4 to 8 after reg 0x3C bit 3
00000000 00010000
Register 0x3C changed from 8 to 16 after reg 0x3C bit 4
00000000 00100000
Register 0x3C changed from 16 to 32 after reg 0x3C bit 5
00000000 01000000
Register 0x3C changed from 32 to 64 after reg 0x3C bit 6
00000000 10000000
Register 0x3C changed from 64 to 128 after reg 0x3C bit 7
00000001 00000000
Register 0x3C changed from 128 to 256 after reg 0x3C bit 8
00000010 00000000
Register 0x3C changed from 256 to 512 after reg 0x3C bit 9
00000100 00000000
Register 0x3C changed from 512 to 1024 after reg 0x3C bit 10
00001000 00000000
Register 0x3C changed from 1024 to 2048 after reg 0x3C bit 11
00010000 00000000
Register 0x3C changed from 2048 to 4096 after reg 0x3C bit 12
00100000 00000000
Register 0x3C changed from 4096 to 8192 after reg 0x3C bit 13
01000000 00000000
Register 0x3C changed from 8192 to 16384 after reg 0x3C bit 14
10000000 00000000
Register 0x3C changed from 16384 to 32768 after reg 0x3C bit 15
0x3D
00000000 00000001
Register 0x02 changed from 839 to 775 after reg 0x3D bit 0
*/

  // 0x3A: 0x00 0x1F 00000000 00011111 31 batt power + grid power                   
  //  was: 0x00 0x3F 00000000 00111111 63 batt power

  // Last 6b is settable 0x3A: 0x00 0x3F 00000000 00111111 63

  ignore_registers[0x3C] = false; // [?D] Cool flags2
  /* All bits are settable 0x3C: 0xFF 0xFF 11111111 11111111 65535 -> Fan speeds up

    0x02: 0x03 0x07 00000011 00000111 775                   was: 0x03 0x47 00000011 01000111 839
    0x3A: 0x00 0x5F 00000000 01011111 95                    was: 0x00 0x1F 00000000 00011111 31
    0x3C: 0xFF 0xFF 11111111 11111111 65535                 was: 0x00 0x00 00000000 00000000 0
  
  */



      /* Adjusting to undervoltage? ->



0x02: 0x03 0x06 00000011 00000110 774                   was: 0x03 0x07 00000011 00000111 775
0x04: 0x00 0x09 00000000 00001001 9                     was: 0x00 0x00 00000000 00000000 0
0x06: 0x00 0x20 00000000 00100000 32                    was: 0x00 0x00 00000000 00000000 0
0x3A: 0x00 0x3F 00000000 00111111 63                    was: 0x00 0x1F 00000000 00011111 31




      */


  ignore_registers[0x3E] = false;  

  ignore_registers[0x40] = true;  // [!T] Fan speed commanded. Possibly min speed commanded
  ignore_registers[0x42] = false; // [?A] 
  ignore_registers[0x44] = false; // [?A] Voltage thresh 1
  /*0x44: 00000000 00000001
Register 0x44 changed from 2560 to 1 after reg 0x44 bit 0
0x44: 00000000 00000010
Register 0x44 changed from 1 to 2 after reg 0x44 bit 1
0x44: 00000000 00000100
Register 0x44 changed from 2 to 4 after reg 0x44 bit 2
0x44: 00000000 00001000
Register 0x44 changed from 4 to 8 after reg 0x44 bit 3
0x44: 00000000 00010000
Register 0x44 changed from 8 to 16 after reg 0x44 bit 4
0x44: 00000000 00100000
Register 0x44 changed from 16 to 32 after reg 0x44 bit 5
0x44: 00000000 01000000
Register 0x44 changed from 32 to 64 after reg 0x44 bit 6
0x44: 00000000 10000000
Register 0x44 changed from 64 to 128 after reg 0x44 bit 7
0x44: 00000001 00000000
Register 0x44 changed from 128 to 256 after reg 0x44 bit 8
0x44: 00000010 00000000
Register 0x44 changed from 256 to 512 after reg 0x44 bit 9
0x44: 00000100 00000000
Register 0x44 changed from 512 to 1024 after reg 0x44 bit 10
0x44: 00001000 00000000
Register 0x44 changed from 1024 to 2048 after reg 0x44 bit 11
0x44: 00010000 00000000
Register 0x44 changed from 2048 to 4096 after reg 0x44 bit 12
0x44: 00100000 00000000
Register 0x06 changed from 0 to 2 after reg 0x44 bit 13
Register 0x44 changed from 4096 to 8192 after reg 0x44 bit 13
0x44: 01000000 00000000
Register 0x44 changed from 8192 to 8384 after reg 0x44 bit 14
*/
  ignore_registers[0x46] = false; // [?A] Voltage thresh 2
  
  /*0x46: 01000000 00000000
Register 0x46 changed from 8640 to 16384 after reg 0x46 bit 14
0x46: 10000000 00000000
Register 0x46 changed from 16384 to 32768 after reg 0x46 bit 15
*/
  ignore_registers[0x48] = false; // [?A]
  /*0x48: 00000000 00000001
Register 0x48 changed from 2816 to 1 after reg 0x48 bit 0
0x48: 00000000 00000010
Register 0x48 changed from 1 to 2 after reg 0x48 bit 1
0x48: 00000000 00000100
Register 0x48 changed from 2 to 4 after reg 0x48 bit 2
0x48: 00000000 00001000
Register 0x48 changed from 4 to 8 after reg 0x48 bit 3
0x48: 00000000 00010000
Register 0x48 changed from 8 to 16 after reg 0x48 bit 4
0x48: 00000000 00100000
Register 0x48 changed from 16 to 32 after reg 0x48 bit 5
0x48: 00000000 01000000
Register 0x48 changed from 32 to 64 after reg 0x48 bit 6
0x48: 00000000 10000000
Register 0x48 changed from 64 to 128 after reg 0x48 bit 7
0x48: 00000001 00000000
Register 0x48 changed from 128 to 256 after reg 0x48 bit 8
0x48: 00000010 00000000
Register 0x48 changed from 256 to 512 after reg 0x48 bit 9
0x48: 00000100 00000000
Register 0x48 changed from 512 to 1024 after reg 0x48 bit 10
0x48: 00001000 00000000
Register 0x48 changed from 1024 to 2048 after reg 0x48 bit 11
0x48: 00010000 00000000
Register 0x48 changed from 2048 to 3072 after reg 0x48 bit 12
*/
  ignore_registers[0x4A] = false; // [?A]
  
  /*0x4A: 00010000 00000000
Register 0x4A changed from 3328 to 4096 after reg 0x4A bit 12
0x4A: 00100000 00000000
Register 0x06 changed from 2 to 6 after reg 0x4A bit 13
Register 0x4A changed from 4096 to 8192 after reg 0x4A bit 13
0x4A: 01000000 00000000
Register 0x4A changed from 8192 to 16384 after reg 0x4A bit 14
0x4A: 10000000 00000000
Register 0x4A changed from 16384 to 32768 after reg 0x4A bit 15
*/
  ignore_registers[0x4C] = false; // [?A]
  ignore_registers[0x4E] = false; // [?A]

/*

0x06: 0x00 0x06 00000000 00000110 6                     was: 0x00 0x02 00000000 00000010 2
0x3A: 0x00 0x0A 00000000 00001010 10                    was: 0x00 0x00 00000000 00000000 0
0x44: 0x20 0xC0 00100000 11000000 8384                  was: 0x0A 0x00 00001010 00000000 2560
0x46: 0xAA 0xAA 10101010 10101010 43690                 was: 0x21 0xC0 00100001 11000000 8640
0x48: 0x0C 0x00 00001100 00000000 3072                  was: 0x0B 0x00 00001011 00000000 2816
0x4A: 0xAA 0xAA 10101010 10101010 43690                 was: 0x0D 0x00 00001101 00000000 3328
0x50: 0xAA 0xAA 10101010 10101010 43690                 was: 0x0C 0x80 00001100 10000000 3200
0x52: 0xAA 0xAA 10101010 10101010 43690                 was: 0x12 0xC0 00010010 11000000 4800


Testing command: 0x53: 0xAA 0xAA
Executed



0x54: 0xAA 0xAA 10101010 10101010 43690                 was: 0x00 0x00 00000000 00000000 0
0x56: 0xAA 0xAA 10101010 10101010 43690                 was: 0x00 0x00 00000000 00000000 0


Testing command: 0x6C: 0xAA 0xAA
Executed
*/












  ignore_registers[0x50] = false; // [?D] Maybe under voltage thresh
  /*
Register 0x06 changed from 6 to 22 after reg 0x50 bit 0
Register 0x50 changed from 3200 to 1 after reg 0x50 bit 0
0x50: 00000000 00000010
Register 0x50 changed from 1 to 2 after reg 0x50 bit 1
0x50: 00000000 00000100
Register 0x50 changed from 2 to 4 after reg 0x50 bit 2
0x50: 00000000 00001000
Register 0x50 changed from 4 to 8 after reg 0x50 bit 3
0x50: 00000000 00010000
Register 0x50 changed from 8 to 16 after reg 0x50 bit 4
0x50: 00000000 00100000
Register 0x50 changed from 16 to 32 after reg 0x50 bit 5
0x50: 00000000 01000000
Register 0x50 changed from 32 to 64 after reg 0x50 bit 6
0x50: 00000000 10000000
Register 0x50 changed from 64 to 128 after reg 0x50 bit 7
0x50: 00000001 00000000
Register 0x50 changed from 128 to 256 after reg 0x50 bit 8
0x50: 00000010 00000000
Register 0x50 changed from 256 to 512 after reg 0x50 bit 9
0x50: 00000100 00000000
Register 0x50 changed from 512 to 1024 after reg 0x50 bit 10
0x50: 00001000 00000000
Register 0x50 changed from 1024 to 2048 after reg 0x50 bit 11
0x50: 00010000 00000000
Register 0x06 changed from 22 to 6 after reg 0x50 bit 12
Register 0x50 changed from 2048 to 4096 after reg 0x50 bit 12
0x50: 00100000 00000000
Register 0x50 changed from 4096 to 8192 after reg 0x50 bit 13
0x50: 01000000 00000000
Register 0x06 changed from 6 to 22 after reg 0x50 bit 14
Register 0x50 changed from 8192 to 16384 after reg 0x50 bit 14
0x50: 10000000 00000000
Register 0x50 changed from 16384 to 32768 after reg 0x50 bit 15
*/



  ignore_registers[0x52] = false; // [?D] Maybe over voltage thresh

/*
Register 0x06 changed from 22 to 54 after reg 0x52 bit 0
Register 0x52 changed from 4800 to 1 after reg 0x52 bit 0
0x52: 00000000 00000010
Register 0x52 changed from 1 to 2 after reg 0x52 bit 1
0x52: 00000000 00000100
Register 0x52 changed from 2 to 4 after reg 0x52 bit 2
0x52: 00000000 00001000
Register 0x52 changed from 4 to 8 after reg 0x52 bit 3
0x52: 00000000 00010000
Register 0x52 changed from 8 to 16 after reg 0x52 bit 4
0x52: 00000000 00100000
Register 0x52 changed from 16 to 32 after reg 0x52 bit 5
0x52: 00000000 01000000
Register 0x52 changed from 32 to 64 after reg 0x52 bit 6
0x52: 00000000 10000000
Register 0x52 changed from 64 to 128 after reg 0x52 bit 7
0x52: 00000001 00000000
Register 0x52 changed from 128 to 256 after reg 0x52 bit 8
0x52: 00000010 00000000
Register 0x52 changed from 256 to 512 after reg 0x52 bit 9
0x52: 00000100 00000000
Register 0x52 changed from 512 to 1024 after reg 0x52 bit 10
0x52: 00001000 00000000
Register 0x52 changed from 1024 to 2048 after reg 0x52 bit 11
0x52: 00010000 00000000
Register 0x52 changed from 2048 to 4096 after reg 0x52 bit 12
0x52: 00100000 00000000
Register 0x06 changed from 54 to 22 after reg 0x52 bit 13
Register 0x52 changed from 4096 to 8192 after reg 0x52 bit 13
0x52: 01000000 00000000
Register 0x06 changed from 22 to 54 after reg 0x52 bit 14
Register 0x52 changed from 8192 to 16384 after reg 0x52 bit 14
0x52: 10000000 00000000
Register 0x52 changed from 16384 to 32768 after reg 0x52 bit 15
*/

  ignore_registers[0x54] = false; // [?] 
/*0x54: 00000000 00000001
Register 0x54 changed from 0 to 1 after reg 0x54 bit 0
0x54: 00000000 00000010
Register 0x54 changed from 1 to 2 after reg 0x54 bit 1
0x54: 00000000 00000100
Register 0x54 changed from 2 to 4 after reg 0x54 bit 2
0x54: 00000000 00001000
Register 0x54 changed from 4 to 8 after reg 0x54 bit 3
0x54: 00000000 00010000
Register 0x54 changed from 8 to 16 after reg 0x54 bit 4
0x54: 00000000 00100000
Register 0x54 changed from 16 to 32 after reg 0x54 bit 5
0x54: 00000000 01000000
Register 0x54 changed from 32 to 64 after reg 0x54 bit 6
0x54: 00000000 10000000
Register 0x54 changed from 64 to 128 after reg 0x54 bit 7
0x54: 00000001 00000000
Register 0x54 changed from 128 to 256 after reg 0x54 bit 8
Register 0x56 changed from 32768 to 256 after reg 0x54 bit 8
0x54: 00000010 00000000
Register 0x54 changed from 256 to 512 after reg 0x54 bit 9
Register 0x56 changed from 256 to 512 after reg 0x54 bit 9
0x54: 00000100 00000000
Register 0x54 changed from 512 to 1024 after reg 0x54 bit 10
Register 0x56 changed from 512 to 1024 after reg 0x54 bit 10
0x54: 00001000 00000000
Register 0x54 changed from 1024 to 2048 after reg 0x54 bit 11
Register 0x56 changed from 1024 to 2048 after reg 0x54 bit 11
0x54: 00010000 00000000
Register 0x54 changed from 2048 to 4096 after reg 0x54 bit 12
Register 0x56 changed from 2048 to 4096 after reg 0x54 bit 12
0x54: 00100000 00000000
Register 0x54 changed from 4096 to 8192 after reg 0x54 bit 13
Register 0x56 changed from 4096 to 8192 after reg 0x54 bit 13
0x54: 01000000 00000000
Register 0x54 changed from 8192 to 16384 after reg 0x54 bit 14
Register 0x56 changed from 8192 to 16384 after reg 0x54 bit 14
0x54: 10000000 00000000
Register 0x54 changed from 16384 to 32768 after reg 0x54 bit 15
Register 0x56 changed from 16384 to 32768 after reg 0x54 bit 15


*/
  ignore_registers[0x56] = false; // [?] 
  /*
0x56: 00000000 00000001
Register 0x56 changed from 32768 to 1 after reg 0x56 bit 0
0x56: 00000000 00000010
Register 0x56 changed from 1 to 2 after reg 0x56 bit 1
0x56: 00000000 00000100
Register 0x56 changed from 2 to 4 after reg 0x56 bit 2
0x56: 00000000 00001000
Register 0x56 changed from 4 to 8 after reg 0x56 bit 3
0x56: 00000000 00010000
Register 0x56 changed from 8 to 16 after reg 0x56 bit 4
0x56: 00000000 00100000
Register 0x56 changed from 16 to 1312 after reg 0x56 bit 5
0x56: 00000000 01000000
Register 0x56 changed from 1312 to 64 after reg 0x56 bit 6
0x56: 00000000 10000000
Register 0x56 changed from 64 to 128 after reg 0x56 bit 7
0x56: 00000001 00000000
Register 0x56 changed from 128 to 32768 after reg 0x56 bit 8*/

  ignore_registers[0x58] = false; // [?] 
  ignore_registers[0x5A] = false; // [?] 
  ignore_registers[0x5C] = false; // [?] 
  ignore_registers[0x5E] = false; // [?] 

  ignore_registers[0x60] = false; // [?] 
  ignore_registers[0x62] = false; // [?] 
  ignore_registers[0x64] = false; // [?] 
  ignore_registers[0x66] = false; // [?] 
  ignore_registers[0x68] = false; // [?] 
  ignore_registers[0x6A] = false; // [?] 
  ignore_registers[0x6C] = false; // [?] 
  ignore_registers[0x6E] = false; // [?] 

  ignore_registers[0x70] = false; // [?] 
  ignore_registers[0x72] = false; // [?] 
  ignore_registers[0x74] = false; // [?] 
  ignore_registers[0x76] = false; // [?] 
  ignore_registers[0x78] = false; // [?] 
  ignore_registers[0x7A] = false; // [?] 
  ignore_registers[0x7C] = false; // [?] 
  ignore_registers[0x7E] = false; // [?] 

  ignore_registers[0x80] = false; // [?] 
  ignore_registers[0x82] = false; // [?] 
  ignore_registers[0x84] = false; // [?] 
  ignore_registers[0x86] = false; // [?] 
  ignore_registers[0x88] = false; // [?] 
  ignore_registers[0x8A] = false; // [?] 
  ignore_registers[0x8C] = false; // [?] 
  ignore_registers[0x8E] = false; // [?] 

  ignore_registers[0x90] = false; // [?] 
  ignore_registers[0x92] = false; // [?] 
  ignore_registers[0x94] = false; // [?] 
  ignore_registers[0x96] = false; // [?] 
  ignore_registers[0x98] = false; // [?] 
  ignore_registers[0x9A] = false; // [?] 
  ignore_registers[0x9C] = false; // [?] 
  ignore_registers[0x9E] = false; // [?] 

  ignore_registers[0xA0] = false; // [?] 
  ignore_registers[0xA2] = false; // [?] 
  ignore_registers[0xA4] = false; // [?] 
  ignore_registers[0xA6] = false; // [?] 
  ignore_registers[0xA8] = false; // [?] 
  ignore_registers[0xAA] = false; // [?] 
  ignore_registers[0xAC] = false; // [?] 
  ignore_registers[0xAE] = false; // [?] 

  ignore_registers[0xB0] = false; // [?] 
  ignore_registers[0xB2] = false; // [?] 
  ignore_registers[0xB4] = false; // [?] 
  ignore_registers[0xB6] = false; // [?] 
  ignore_registers[0xB8] = false; // [?] 
  ignore_registers[0xBA] = false; // [?] 
  ignore_registers[0xBC] = false; // [?] 
  ignore_registers[0xBE] = false; // [?] 

  ignore_registers[0xC0] = false; // [?] 
  ignore_registers[0xC2] = false; // [?] 
  ignore_registers[0xC4] = false; // [?] 
  ignore_registers[0xC6] = false; // [?] 
  ignore_registers[0xC8] = false; // [?] 
  ignore_registers[0xCA] = false; // [?] 
  ignore_registers[0xCC] = false; // [?] 
  ignore_registers[0xCE] = false; // [?] 

  ignore_registers[0xD0] = false; // [?] 
  ignore_registers[0xD2] = false; // [?] 
  ignore_registers[0xD4] = false; // [?] 
  ignore_registers[0xD6] = false; // [?] 
  ignore_registers[0xD8] = false; // [?] 
  ignore_registers[0xDA] = false; // [?] 
  ignore_registers[0xDC] = false; // [?] 
  ignore_registers[0xDE] = false; // [?] 

  ignore_registers[0xE0] = false; // [?] 
  ignore_registers[0xE2] = false; // [?] 
  ignore_registers[0xE4] = false; // [?] 
  ignore_registers[0xE6] = false; // [?] 
  ignore_registers[0xE8] = false; // [?] 
  ignore_registers[0xEA] = false; // [?] 
  ignore_registers[0xEC] = false; // [?] 
  ignore_registers[0xEE] = false; // [?] 

  ignore_registers[0xF0] = false; // [?] 
  ignore_registers[0xF2] = false; // [?] 
  ignore_registers[0xF4] = false; // [?] 
  ignore_registers[0xF6] = false; // [?] 
  ignore_registers[0xF8] = false; // [?] 
  ignore_registers[0xFA] = false; // [?] 
  ignore_registers[0xFC] = false; // [?] 
  ignore_registers[0xFE] = false; // [?] 


  
/*

|HP PROLIANT SERVER PS                                 |
| Input: 228.13V  0.16A    | Output: 12.73V  7.83A     |
| Fan: 3360.00/s | Intake: 30.00C  | Internal  42.22C  |
________________________________________________________


0x1A: 0x00 0x18 00000000 00011000 24                    was: 0x00 0x18 00000000 00011000 24
0x84: 0x1B 0xD9 00011011 11011001 7129                  was: 0x1B 0xCD 00011011 11001101 7117
0x86: 0x00 0xC7 00000000 11000111 199                   was: 0x00 0xC8 00000000 11001000 200
0x87: 0x0C 0xBB 00001100 10111011 3259                  was: 0x0C 0xBC 00001100 10111100 3260
0x88: 0x03 0xE6 00000011 11100110 998                   was: 0x03 0xE9 00000011 11101001 1001
0x89: 0x00 0xC6 00000000 11000110 198                   was: 0x00 0xC7 00000000 11000111 199
0x8E: 0x0D 0x80 00001101 10000000 3456                  was: 0x0D 0xC0 00001101 11000000 3520
0x8F: 0x0D 0x20 00001101 00100000 3360                  was: 0x0D 0x98 00001101 10011000 3480
0x96: 0xF3 0xAA 11110011 10101010 62378                 was: 0xEE 0xFB 11101110 11111011 61179
0x98: 0x2E 0x72 00101110 01110010 11890                 was: 0x2E 0x6C 00101110 01101100 11884
0x9A: 0x00 0x18 00000000 00011000 24                    was: 0x00 0x18 00000000 00011000 24









Pulling AC power :
  0x01: 0x00 0x04 00000000 00000100 4                     was: 0x03 0x07 00000011 00000111 775


  
0x02: 0x00 0x00 00000000 00000000 0                     was: 0x00 0x10 00000000 00010000 16
0x1A: 0x00 0x18 00000000 00011000 24                    was: 0x00 0x12 00000000 00010010 18



Reconnecting AC power: 
  0x01: 0x01 0x06 00000001 00000110 262                   was: 0x00 0x04 00000000 00000100 4
  0x01: 0x03 0x07 00000011 00000111 775                   was: 0x01 0x06 00000001 00000110 262


  0x02: 0x00 0x00 00000000 00000000 0                     was: 0x00 0x10 00000000 00010000 16

*/

}


#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "Wire.h"

#include "./stassid.h"

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

char psu_spn[11];
char psu_date[9];
char psu_name[27];
//char psu_mfu[5];
char psu_ct[15];

WiFiServer TelnetServer(23);
WiFiClient Telnet;

uint8_t psu_mcu_addr = 0xFF;
uint8_t psu_mem_addr = 0xFF;


 double psu_grid_voltage = 0;
 double psu_grid_amperage = 0;
 double psu_grid_wattage = 0;
 double psu_out_voltage = 0;
 double psu_out_amperage = 0;
 double psu_out_wattage = 0;
 double psu_intake_temp = 0;
 double psu_internal_temp = 0;
 double psu_fan_speed_actual = 0;
 double psu_fan_speed_desired = 0;
 /*double psu_cool_flags_1 = 0;
 double psu_cool_flags_2 = 0;
 double psu_voltage_threshold_1 = 0;
 double psu_voltage_threshold_2 = 0;*/



/****************************************************** U T I L I T Y */

void print_byte(uint8_t b){
  Telnet.print("0x");
  if (b < 16)
    Telnet.print("0");
  Telnet.print(b, HEX);
}

    
void print_bits(uint8_t b){    
  for(uint8_t mask = 0x80; mask; mask >>= 1){
   if (mask & b)
       Telnet.print('1');
   else
       Telnet.print('0');
  }
}

void print_data(){
    Telnet.println("________________________________________________________");
    Telnet.print("|");
    Telnet.print(psu_name);
    Telnet.println("                            |");
    Telnet.print("| Input: ");
    Telnet.print(psu_grid_voltage);
    Telnet.print("V  ");
    Telnet.print(psu_grid_amperage);
    
    Telnet.print("A    | Output: ");
    Telnet.print(psu_out_voltage);
    Telnet.print("V  ");
    Telnet.print(psu_out_amperage);
    Telnet.println("A     |");

    Telnet.print("| Fan: ");
    Telnet.print(psu_fan_speed_actual);
    Telnet.print("/s | Intake: ");
    Telnet.print(psu_intake_temp);
    Telnet.print("C  | Internal  ");
    Telnet.print(psu_internal_temp);
    Telnet.println("C  |");



    Telnet.println("________________________________________________________\n\n");
}

/****************************************************** i 2 C - f u n c t i o n s */


void scan_for_device(uint8_t from, uint8_t to, uint8_t& ret) {
  byte error, address;
  
  
  for (address = from ; address <= to; address++ )  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0){
      /*Telnet.print("Device found at ");
      print_byte(address);
      Telnet.println();*/
      ret = address;
      return;
    } 
  }

  if (ret == 0xFF) {
    Telnet.print("No device found between ");
    print_byte(from);
    Telnet.print(" and ");
    print_byte(to);
    Telnet.println(".");
  }
  
}


/****************************************************** H O U S E K E E P I N G */

void handleTelnet() {
    if (TelnetServer.hasClient()) {
      if (!Telnet || !Telnet.connected()) {
        if (Telnet) Telnet.stop();
        Telnet = TelnetServer.available();
      } else {
        TelnetServer.available().stop();
      }
    }
}

void reconnect_wifi() {  
    WiFi.mode(WIFI_STA);  
    WiFi.begin(ssid, password);  
    while (WiFi.status() != WL_CONNECTED) {  
        delay(500);  
    }  
}  


/****************************************************** P S U  E E P R O M */

byte read_eeprom_byte(long addr)
{
  Wire.beginTransmission(psu_mem_addr);

  Wire.write((uint8_t)(addr));
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)psu_mem_addr, (uint8_t)1);

  byte rdata = 0xFF;
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void read_eeprom_string(uint8_t addr, uint8_t len, char* s)
{
  Wire.beginTransmission(psu_mem_addr);

  Wire.write((uint8_t)(addr));
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)psu_mem_addr, (uint8_t)len);


  uint8_t i=0;
  while (Wire.available() && (i <= len)){
    s[i] = Wire.read();
    i++;
  }
  s[i] = '\0';
}

void read_eeprom(){

  read_eeprom_string(0x12, 10, psu_spn);
  read_eeprom_string(0x1D, 8, psu_date);
  read_eeprom_string(0x32, 26, psu_name);
  read_eeprom_string(0x5B, 14, psu_ct);
  
  Telnet.print("Found: ");
  Telnet.println(psu_name);

}

void read_entire_eeprom(){

  uint8_t a = 0;
  for (a = 0 ; a<0xFF; a++){
    uint8_t b = read_eeprom_byte(a);
    print_byte(a);
    Telnet.print(": ");
    print_byte(b);
    Telnet.print(" ");
    Telnet.println((char)b);
  }
}




/****************************************************** P S U  M C U */


bool checksum(uint8_t* msg, uint8_t msg_len){

  uint8_t cs = 0;
  for (uint8_t i=0; i<msg_len; ++i){
    //print_byte(msg[i]); 
    //Telnet.print(" ");
    cs += msg[i];
  }
  cs = (( 0xFF - cs) + 1) & 0xFF;
  //Telnet.print("  cs: ");
  //print_byte(cs);
  //Telnet.println();
  if (cs) { 
    Telnet.print("Wrong checksum: ");
    Telnet.println(cs);
  }
  // cs == 0 > A-OK
  // (cs == 0) == true > A-OK
  //
  return (cs==0);

}

bool read_psu_mcu(uint8_t reg, uint8_t len, uint8_t* msg){
    //reg = reg << 1;
    uint8_t cs = reg + (psu_mcu_addr<<1);
    uint8_t reg_cs = ((0xff-cs)+1) & 0xff;
    Wire.beginTransmission(psu_mcu_addr);
    Wire.write((uint8_t)(reg));
    Wire.write((uint8_t)(reg_cs));
    Wire.endTransmission(true);
    delay(1);

    Wire.requestFrom((uint8_t)psu_mcu_addr, (uint8_t)len);
    /*print_byte(reg);
    Telnet.print(": ");*/
    
    uint8_t i = 0;
    //uint8_t msg[len];
    while (Wire.available() && (i < len)){ 
      msg[i] = Wire.read(); 
      /*print_byte((uint8_t)msg[i]);
      Telnet.print(" ");*/
      i++;
    }
    //Telnet.println();
    if (i!=len){
      Telnet.print("Expected to read ");
      Telnet.print(len);
      Telnet.print(" bytes, but got ");
      Telnet.print(i);
      Telnet.println(" bytes.");
    }
    
    return checksum(msg, len);

}

bool read_psu_mcu_u16(uint8_t reg, uint16_t& ret){
  uint8_t msg[3];
  if (!read_psu_mcu(reg, 3, msg)){
    return false;
  }
  ret = (msg[1] << 8) + msg[0];
  return true;
}

bool read_psu_mcu_f16(uint8_t reg, double scale, double& ret){
  uint16_t u16;
  if (!read_psu_mcu_u16(reg, u16)){
    return false;
  }
  ret = scale * u16;
  return true;
}

bool read_psu_mcu_flags16(uint8_t reg, double scale, uint16_t& ret){
  if (!read_psu_mcu_u16(reg, ret)){
    return false;
  }
  return true;
}



bool read_psu_grid_voltage(double& ret){
  return (read_psu_mcu_f16(0x08, 0.032, ret));
}

bool read_psu_grid_amperage(double& ret){
  return (read_psu_mcu_f16(0x0A, 1/128., ret));
}

bool read_psu_grid_wattage(double& ret){
  return (read_psu_mcu_f16(0x0C, 2., ret));
}

bool read_psu_out_voltage(double& ret){
  return (read_psu_mcu_f16(0x0E, 1./256, ret));
}

bool read_psu_out_amperage(double& ret){
  return (read_psu_mcu_f16(0x10, 1./128, ret));
}

bool read_psu_out_wattage(double& ret){
  return (read_psu_mcu_f16(0x1A, 2., ret));
}

bool read_psu_intake_temp(double& ret){
  bool r = (read_psu_mcu_f16(0x18, 1./32., ret));
  //ret = (ret - 32.) * 5./9.;
  return r;
}

bool read_psu_internal_temp(double& ret){
  bool r = (read_psu_mcu_f16(0x1C, 1./32., ret));
  //ret = (ret - 32.) * 5./9.;
  return r;
}

bool read_psu_fan_speed_actual(double& ret){
  return (read_psu_mcu_f16(0x1E, 1., ret));
}

bool read_psu_fan_speed_desired(double& ret){
  return (read_psu_mcu_f16(0x40, 1., ret));
}

bool read_psu_cool_flags_1(uint16_t& ret){
  return (read_psu_mcu_u16(0x3A, ret));
}

bool read_psu_cool_flags_2(uint16_t& ret){
  return (read_psu_mcu_u16(0x3A, ret));
}

bool read_psu_voltage_threshold_1(double& ret){
  return (read_psu_mcu_f16(0x44, 32., ret));
}

bool read_psu_voltage_threshold_2(double& ret){
  return (read_psu_mcu_f16(0x46>>1, 32., ret));
}


bool read_psu_data(){
  return ((read_psu_grid_voltage(psu_grid_voltage)) &&
  (read_psu_grid_amperage(psu_grid_amperage)) &&
  (read_psu_grid_wattage(psu_grid_wattage)) &&
  (read_psu_out_voltage(psu_out_voltage)) &&
  (read_psu_out_amperage(psu_out_amperage)) &&
  (read_psu_out_wattage(psu_out_wattage)) &&
  (read_psu_intake_temp(psu_intake_temp)) &&
  (read_psu_internal_temp(psu_internal_temp)) &&
  (read_psu_fan_speed_actual(psu_fan_speed_actual)) &&
  (read_psu_fan_speed_desired(psu_fan_speed_desired)));
}

uint16_t registers[255];
uint16_t registers_old[255];
uint16_t registers_last_change[255];
uint8_t registers_last_change_age[255];


void read_psu_mcu_registers_init(){
  uint16_t u16 = 0;
  for (uint8_t i=0; i<255;++i){
    read_psu_mcu_u16(i, u16);
    registers[i] = u16;
    registers_old[i] = u16;
    registers_last_change[i] = u16;
    registers_last_change_age[i]=0xFF;
    ignore_registers[i] = false;
  }
}

void read_psu_mcu_changing_registers(){

  uint16_t u16 = 0;
  for (uint8_t i=0; i<255;++i){
    read_psu_mcu_u16(i, u16);
    registers_old[i] = registers[i];
    registers[i] = u16;
  }

  Telnet.println("\033c");
  read_psu_data();
  print_data();
  for (uint8_t i=0; i<255;++i){
    if (ignore_registers[i]) continue;

    if (registers[i] != registers_old[i]) {
      registers_last_change_age[i] = 0;
      registers_last_change[i] = registers_old[i];
    }
    else {
      if (registers_last_change_age[i]!=0xFF){
        registers_last_change_age[i]++;
      }
    }
    if (/*(i==test_cmd) || */(registers_last_change_age[i] < 25)){
      print_byte(i);
      Telnet.print(": ");
      
      print_byte((uint8_t)(registers[i]>>8));
      Telnet.print(" ");
      print_byte(registers[i] & 0xFF);
      Telnet.print(" ");
      
      print_bits((uint8_t)(registers[i]>>8));
      Telnet.print(" ");
      print_bits(registers[i] & 0xFF);

      Telnet.print(" ");
      Telnet.print(registers[i]);
      
      Telnet.print("             \twas: ");

      print_byte((uint8_t)(registers_last_change[i]>>8));
      Telnet.print(" ");
      print_byte(registers_last_change[i] & 0xFF);
      Telnet.print(" ");
      
      print_bits((uint8_t)(registers_last_change[i]>>8));
      Telnet.print(" ");
      print_bits(registers_last_change[i] & 0xFF);

      Telnet.print(" ");
      Telnet.print(registers_last_change[i]);
      
      Telnet.print('\n');
    }
  }
  Telnet.println();
  
  if (test_cmd != 0) {
    Telnet.print("\nTesting command: ");
    print_byte((uint8_t)(test_cmd));
    Telnet.print(": ");
    print_byte((uint8_t)(test_data >> 8));
    Telnet.print(" ");
    print_byte((uint8_t)(test_data & 0xFF));
  }


}


void fuzz_for_voltage(){
  uint8_t reg;
  uint8_t bit;

  double volt;
  double old_volt;

  read_psu_out_voltage(volt);
  old_volt=volt;
  for (reg = 0; reg < 0xFF; reg++){
    print_byte(reg);
    Telnet.println();
    yield();
    ArduinoOTA.handle();
    handleTelnet();

    for (bit=0; bit<16;bit++){
      write_psu_mcu_u16(reg, (uint16_t)(1<<bit));
      
      read_psu_out_voltage(volt);
      if (abs(old_volt - volt) > 0.02){
          Telnet.println("Voltage changed from ");
          Telnet.print(old_volt);
          Telnet.print("v to ");
          Telnet.print(volt);
          Telnet.println("v after reg ");
          print_byte(reg);
          Telnet.print(" bit ");
          Telnet.println(bit);
      }
      old_volt=volt;
      force_psu_fan(3200);
    }
  }  
}

void read_all_psu_registers(){
  uint16_t u16;
  for (uint8_t i=0; i<255;++i){
    if (ignore_registers[i]) continue;
    
    read_psu_mcu_u16(i, u16);
    registers[i] = u16;

    if (registers[i] != registers_old[i]) {
      registers_last_change_age[i] = 0;
      registers_last_change[i] = registers_old[i];
    }
    else {
      if (registers_last_change_age[i]!=0xFF){
        registers_last_change_age[i]++;
      }
    }
    registers_old[i] = registers[i];
  }
}

void fuzz(){
  uint8_t reg;
  uint8_t bit;


  uint16_t u16 = 0;
  uint16_t mask;
  for (uint8_t i=0; i<255;++i){
    read_psu_mcu_u16(i, u16);
    registers[i] = u16;
    registers_old[i] = u16;
    registers_last_change[i] = u16;
    registers_last_change_age[i]=0xFF;
  }

  for (reg = 0xFE; reg <= 0xFF; reg++){
    for (bit=0; bit<16;bit++){
      print_byte(reg);
      Telnet.print(": ");
      mask = (uint16_t)(1<<bit);
      print_bits(mask >> 8);
      Telnet.print(" ");
      print_bits(mask & 0xFF);
      Telnet.println();
      write_psu_mcu_u16(reg, (uint16_t)(1<<bit));
      read_all_psu_registers();
      yield();
      ArduinoOTA.handle();
      handleTelnet();
      for (uint8_t i=0; i<255;++i){
        if (ignore_registers[i]) continue;
        


        if (registers_last_change_age[i] == 0){
          Telnet.print("Register ");
          print_byte(i);
          Telnet.print(" changed from ");
          Telnet.print(registers_last_change[i]);
          Telnet.print(" to ");
          Telnet.print(registers[i]);
          Telnet.print(" after reg ");
          print_byte(reg);
          Telnet.print(" bit ");
          Telnet.println(bit);  
        }
      }
      force_psu_fan(3200);
    }
  }  
}


void write_psu_mcu_u8(uint8_t reg, uint8_t val){

  uint8_t cs = (psu_mcu_addr<<1) + reg + val;
  uint8_t reg_cs = ((0xff-cs)+1) & 0xff;
/*
  print_byte(reg); Telnet.print(" ");
  print_byte(val_lsb); Telnet.print(" ");
  print_byte(val_msb); Telnet.print(" ");
  //print_byte(cs); Telnet.println(" ");
  print_byte(reg_cs); Telnet.println(" ");
  
*/
  Wire.beginTransmission(psu_mcu_addr);
  Wire.write((uint8_t)(reg));
  Wire.write((uint8_t)(val));
  Wire.write((uint8_t)(reg_cs));
  Wire.endTransmission();

}

void write_psu_mcu_u16(uint8_t reg, uint16_t val){

  uint8_t val_lsb = val & 0xff;
  uint8_t val_msb = val >> 8;

  uint8_t cs = (psu_mcu_addr<<1) + reg + val_lsb + val_msb;
  uint8_t reg_cs =((0xff-cs)+1) & 0xff;
/*
  print_byte(reg); Telnet.print(" ");
  print_byte(val_lsb); Telnet.print(" ");
  print_byte(val_msb); Telnet.print(" ");
  //print_byte(cs); Telnet.println(" ");
  print_byte(reg_cs); Telnet.println(" ");
  
*/
  Wire.beginTransmission(psu_mcu_addr);
  Wire.write((uint8_t)(reg));
  Wire.write((uint8_t)(val_lsb));
  Wire.write((uint8_t)(val_msb));
  Wire.write((uint8_t)(reg_cs));
  Wire.endTransmission();

}

void force_psu_fan(uint16_t rpm){
  write_psu_mcu_u16(0x40, rpm);
}

void write_psu_mcu_f16(uint8_t reg, double val, double scale){
  uint16_t v = (uint16_t)(val / scale);
  write_psu_mcu_u16(reg, v);
}
/****************************************************** S E T U P */

void setup() {

  

  Serial.begin(115200);
  Serial.println("Start");

  reconnect_wifi();
 
  ArduinoOTA.begin(); 
 
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
  Telnet.println("Ready");
  Telnet.println("IP address: ");
  Telnet.println(WiFi.localIP());

  Wire.begin(D1, D2);  
  while (psu_mem_addr == 0xFF){
    Telnet.println("Scanning for EEPROM.");
    scan_for_device(0x50, 0x57, psu_mem_addr);
    handleTelnet();
    ArduinoOTA.handle();
    yield();
  }
  read_eeprom();

  while (psu_mcu_addr == 0xFF){
    Telnet.println("Scanning for MCU.");
    scan_for_device(0x58, 0x5F, psu_mcu_addr);
    handleTelnet();
    ArduinoOTA.handle();
    yield();
  }
  handleTelnet();
  read_psu_mcu_registers_init();
  setup_ignore_registers();
  force_psu_fan(3200);
}




/****************************************************** M A I N  L O O P */

uint8_t bootdelay = 60;
void loop() {
  if (WiFi.status() != WL_CONNECTED) {  
      reconnect_wifi();
  }  
  ArduinoOTA.handle();
  handleTelnet();
  Telnet.println(".");
  write_psu_mcu_f16(0x44, 13.8, 1/256);
  write_psu_mcu_f16(0x46, 13.8, 1/256);
  write_psu_mcu_f16(0x48, 13.8, 1/256);
  write_psu_mcu_f16(0x4A, 13.8, 1/256);
  write_psu_mcu_f16(0x4C, 13.8, 1/256);
  write_psu_mcu_f16(0x4E, 13.8, 1/256);
  write_psu_mcu_f16(0x50, 13.8, 1/256);
  write_psu_mcu_f16(0x52, 13.8, 1/256);
  write_psu_mcu_f16(0x54, 13.8, 1/256);
  write_psu_mcu_f16(0x56, 13.8, 1/256);



  //write_psu_mcu_f16(0x44, 13.8, 1/256);
  
  //fuzz(); 
  //read_psu_mcu_changing_registers();
/*
  if (test_cmd != 0){
    if (bootdelay > 0) {
      Telnet.print("\nExecuting in: ");
      Telnet.println(bootdelay);
      bootdelay--;
    } else if (bootdelay == 0){
      Telnet.println("\nExecuted");
      if ((test_cmd != 0x40) && (test_cmd != 0x3c)){
        write_psu_mcu_u16(test_cmd, test_data);
      }
      test_cmd++;
      bootdelay = 0;    
    }
  } else {
    read_psu_mcu_changing_registers();
  }
*/
/*
  double psu_out_volt;
  double psu_out_amp;
  if (read_psu_out_voltage(psu_out_volt) && read_psu_out_amperage(psu_out_amp)){

    Telnet.print(psu_out_volt);
    Telnet.print("V ");
    Telnet.print(psu_out_amp);
    Telnet.println("A");
  } else {
    Telnet.println("Error reading out voltage and current");
  }
*/
/*
  double psu_intake_temp;
  double psu_internal_temp;
  if (read_psu_intake_temp(psu_intake_temp) && read_psu_internal_temp(psu_internal_temp)){

    Telnet.print(psu_intake_temp);
    Telnet.print("C ");
    Telnet.print(psu_internal_temp);
    Telnet.println("C");
  } else {
    Telnet.println("Error reading temperatures");
  }

  */
  
  /*Telnet.print("MCU ");
  print_byte(psu_mcu_addr);
  Telnet.println();
*/

/*
  double actual_fan_speed;
  double desired_fan_speed;
  if (read_psu_fan_speed_actual(actual_fan_speed) && read_psu_fan_speed_desired(desired_fan_speed)){
    Telnet.print("Fan speed: ");
    Telnet.print(fan_speed);
    Telnet.print(" (commanded)  ");
    Telnet.print(actual_fan_speed);
    Telnet.print(" (actual) ");
    Telnet.print(desired_fan_speed);
    Telnet.println(" (desired)");
  } else {
    Telnet.println("Error reading fan speed");
  }
  
*/
/*
  if (psu_mcu_addr != 0xFF){
    force_psu_fan(fan_speed);
    if (fan_speed > 1000){
      fan_speed -= 10;
    } else { 
      fan_speed = 5000;
    }
    delay(500);
    double actual_fan_speed;
    double desired_fan_speed;
    if (read_psu_fan_speed_actual(actual_fan_speed) && read_psu_fan_speed_desired(desired_fan_speed)){
      Telnet.print("Fan speed: ");
      Telnet.print(fan_speed);
      Telnet.print(" (commanded)  ");
      Telnet.print(actual_fan_speed);
      Telnet.print(" (actual) ");
      Telnet.print(desired_fan_speed);
      Telnet.println(" (desired)");
    } else {
      Telnet.println("Error reading fan speed");
    }
    double involt=0;

    if (read_psu_grid_voltage(involt)){
      Telnet.print("In voltage ");
      Telnet.print(involt);
      Telnet.println("v");
    } else {
      Telnet.println("Error reading voltage");
    }

    delay(500);
  }
*/
}
  
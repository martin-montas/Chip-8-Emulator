
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cstdlib>


#include <random>
#include <stdlib.h>
#include <stdint.h>
#include "chip-8.h"
#include <time.h>

// third party library
#include <SDL2/SDL.h>


unsigned char chip8_fontset[80] =
{
   0xF0, 0x90, 0x90, 0x90, 0xF0, //0
   0x20, 0x60, 0x20, 0x20, 0x70, //1
   0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
   0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
   0x90, 0x90, 0xF0, 0x10, 0x10, //4
   0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
   0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
   0xF0, 0x10, 0x20, 0x40, 0x40, //7
   0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
   0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
   0xF0, 0x90, 0xF0, 0x90, 0x90, //A
   0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
   0xF0, 0x80, 0x80, 0x80, 0xF0, //C
   0xE0, 0x90, 0x90, 0x90, 0xE0, //D
   0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
   0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};
Chip_8::Chip_8() {}

//--------------------------------------------------
///  initiates the main variables declared in
///  the chip 8 header file
///
//--------------------------------------------------
void Chip_8::initiate_SDL()
{
   pc      = 0x200;    // Set program counter to 0x200
   instruction  = 0;        // Reset op code
   I     = 0;          // Reset I
   sp      = 0;        // Reset stack pointer

   // Clear the display
   for (int i = 0; i < 2048; ++i) {
      vram[i] = 0;
   }

   // Clear the stack, keypad, and V registers
   for (int i = 0; i < 16; ++i) {
      stack[i]    = 0;
      key[i]      = 0;
      v[i]        = 0;
   }

   // Clear memory
   for (int i = 0; i < 4096; ++i) {
      memory[i] = 0;
   }

   // Load font set into memory
   for (int i = 0; i < 80; ++i) {
      memory[i] = chip8_fontset[i];
   }

   // Reset timers
   delay_timer = 0;
   sound_timer = 0;

   // Seed rng
   srand (time(NULL));
}

//--------------------------------------------------
///   loads the rom file in the the memory
///   variables in the  chip_8 class
///
//--------------------------------------------------
bool Chip_8::load(const char *file_path)
{

   // Initialise
   initiate_SDL();

   printf("Loading ROM: %s\n", file_path);

   // Open ROM file
   FILE* rom = fopen(file_path, "rb");
   if (rom == NULL) {
      std::cerr << "Failed to open ROM" << std::endl;
      return false;
   }

   // Get file size
   fseek(rom, 0, SEEK_END);
   long rom_size = ftell(rom);
   rewind(rom);

   // Allocate memory to store rom
   char* rom_buffer = (char*) malloc(sizeof(char) * rom_size);
   if (rom_buffer == NULL) {
      std::cerr << "Failed to allocate memory for ROM" << std::endl;
      return false;
   }

   // Copy ROM into buffer
   size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, rom);
   if (result != rom_size) {
      std::cerr << "Failed to read ROM" << std::endl;
      return false;
   }

   // Copy buffer to memory
   if ((4096-512) > rom_size){
      for (int i = 0; i < rom_size; ++i) {
         memory[i + 512] = (uint8_t)rom_buffer[i];   // Load into memory starting
                                                     // at 0x200 (=512)
      }
   }
   else {
      std::cerr << "ROM too large to fit in memory" << std::endl;
      return false;
   }

   // Clean up
   fclose(rom);
   free(rom_buffer);

   return true;

}


//-------------------------------------------------
//      fettches and execute the instructions
//      from the memory array
///
//-------------------------------------------------
void Chip_8::fetch_instructions() {

   instruction = memory[pc] << 8 | memory[pc + 1];

   /*
    *
    * TODO work on this opcodes
    Unknown op code: F007

    Unknown op code: 3000

    Unknown op code: 121A
    *
    *
    */

   switch (instruction & 0xF000) {


      case 0x0000: 
         switch (instruction & 0x000F) { 

            // resturns from the stack
            case 0x000e:  
               printf("\nUnknown op code: %.4X\n", instruction);
               sp--;
               pc = stack[sp];
               pc+=2;
               break;


               // clears the screen
            case 0x0000: 

               for (int i = 0; i < 2048; ++i) {
                  vram[i] = 0;
               }
               draw_flag = true;
               pc+=2;
               break;

            default:
               printf("\nUnknown op code: %.4X\n", instruction);
               exit(3);
         }
         break;

      case 0x1000:  
         printf("\nUnknown op code: %.4X\n", instruction);
         pc = instruction & 0x0FFF;
         break;



      case 0x2000:  // 2NNN (call addr)
         printf("\nUnknown op code: %.4X\n", instruction);
         stack[sp] = pc;
         sp++;
         pc = instruction & 0x0FFF;
         break;

      case 0x3000:  // 3xkk (byte)


         printf("\nUnknown op code: %.4X\n", instruction);
         if(v[(instruction & 0x0F00) >> 8] == (instruction & 0x00FF)) {
            pc+= 4;

         }
         else{
            pc+=2;
         }
         break;

      case 0x4000:  // 4xkk (byte)

         printf("\nUnknown op code: %.4X\n", instruction);
         if (v[(instruction & 0x0F00) >> 8] != (instruction & 0x00FF))
            pc += 4;
         else
            pc += 2;
         break;

      case 0x5000:  // 5xy0 (byte)

         printf("\nUnknown op code: %.4X\n", instruction);
         if( v[(instruction & 0x0F00) >> 8]== v[(instruction & 0x00F0)>> 4]) {
            pc+= 4;

         }
         else{
            pc+=2;
         }
         break;

      case 0x6000:
         printf("\nUnknown op code: %.4X\n", instruction);
         v[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;
         pc += 2;
         break;

      case 0x7000: 

         printf("\nUnknown op code: %.4X\n", instruction);
         v[(instruction & 0x0F00) >> 8] += instruction & 0x00FF;
         pc += 2;
         break;


      case 0x8000: 
         switch (instruction & 0x000F) {

            case  0x0000: // 8xy0 (store)

               printf("\nUnknown op code: %.4X\n", instruction);
               v[(instruction & 0x0F00)>> 8] =  v[(instruction & 0x00F0)>> 4];
               pc+=2;
               break;
            case  0x0001:  // 8xy1 (OR)
               printf("\nUnknown op code: %.4X\n", instruction);
               v[(instruction & 0x0F00) >> 8] |= v[(instruction & 0x00F0) >> 4];
               pc += 2;
               break;
            case  0x0002:  // 8xy2 (AND)
               printf("\nUnknown op code: %.4X\n", instruction);
               v[(instruction & 0x0F00) >> 8] &= v[(instruction & 0x00F0) >> 4];
               pc += 2;
               break;

            case  0x0003:  // 8xy3 (XOR)

               printf("\nUnknown op code: %.4X\n", instruction);
               v[(instruction & 0x0F00) >> 8] ^= v[(instruction & 0x00F0) >> 4];
               pc += 2;
               break;


            case  0x0004:  // 8xy4 (ADD)
                           //
               printf("\nUnknown op code: %.4X\n", instruction);
               v[(instruction & 0x0F00) >> 8] += v[(instruction & 0x00F0) >> 4];
               if(v[(instruction & 0x00F0) >> 4] > (0xFF - v[(instruction & 0x0F00) >> 8]))
                  v[0xF] = 1; //carry
               else
                  v[0xF] = 0;
               pc += 2;
               break;

            case  0x0005:  

               printf("\nUnknown op code: %.4X\n", instruction);
               if(v[(instruction & 0x00F0) >> 4] > v[(instruction & 0x0F00) >> 8])
                  v[0xF] = 0; // there is a borrow
               else
                  v[0xF] = 1;

               v[(instruction & 0x0F00) >> 8] -= v[(instruction & 0x00F0) >> 4];
               pc += 2;
               break;

            case  0x0006: // 8xy6 - SHR Vx {, Vy}
               printf("\nUnknown op code: %.4X\n", instruction);
               v[0xF] = v[(instruction & 0x0F00)>> 8 ] & 0x1;
               v[(instruction & 0x0F00)>> 8 ] >>= 1;
               pc+=2;
               break;
            case  0x0007: // 8xy7 - SUBN Vx, Vy
               printf("\nUnknown op code: %.4X\n", instruction);

               if(v[(instruction & 0x0F00) >> 8] > v[(instruction & 0x00F0) >> 4])	// VY-VX
                  v[0xF] = 0; // there is a borrow
               else
                  v[0xF] = 1;
               v[(instruction & 0x0F00) >> 8] = v[(instruction & 0x00F0) >> 4] - v[(instruction & 0x0F00) >> 8];
               pc += 2;
               break;
            case  0x000E:  // 8xyE - SHL Vx {, Vy}
               printf("\nUnknown op code: %.4X\n", instruction);
               v[0xF] = v[(instruction & 0x0F00) >> 8] >> 7;
               v[(instruction & 0x0F00) >> 8] <<= 1;
               pc += 2;
               break; 
            default:
               printf("\nUnknown op code: %.4X\n", instruction);
               exit(3);
         }
         break;

      case 0x9000:
         printf("\nUnknown op code: %.4X\n", instruction);
         if (v[(instruction & 0x0F00) >> 8] != v[(instruction & 0x00F0) >> 4])
           pc += 4;
         else
            pc += 2;
         break;

      case 0xa000:
         printf("\nUnknown op code: %.4X\n", instruction);
         I = instruction & 0x0FFF;
         pc += 2;
         break;

      case 0xb000:
         printf("\nUnknown op code: %.4X\n", instruction);
         pc = v[0] + (instruction & 0x0FFF);
         break;
      case 0xc000:

         printf("\nUnknown op code: %.4X\n", instruction);
         v[(instruction & 0x0F00) >> 8] =  (rand()%(0xFF + 1)) & (instruction & 0x00FF);
         pc+=2;
         break;

      case 0xd000:  {

                       printf("\nUnknown op code: %.4X\n", instruction);
                       unsigned short x = v[(instruction & 0x0F00) >> 8];
                       unsigned short y = v[(instruction & 0x00F0) >> 4];
                       unsigned short height = instruction & 0x000F;
                       unsigned short pixel;
                       v[0xF] = 0;
                       for (int yline = 0; yline < height; yline++)
                       {
                          pixel = memory[I + yline];
                          for(int xline = 0; xline < 8; xline++)
                          {
                             if((pixel & (0x80 >> xline)) != 0)
                             {
                                if(vram[(x + xline + ((y + yline) * 64))] == 1)
                                {
                                   v[0xF] = 1;
                                }
                                vram[x + xline + ((y + yline) * 64)] ^= 1;
                             }
                          }
                       }
                       draw_flag = true;
                       pc += 2;
                    }
                    break;

      case 0xe000: 
                    switch(instruction & 0x000FF) {

                      case 0x009E: 
                        printf("\nUnknown op code: %.4X\n", instruction);
                        if (key[v[(instruction & 0x0F00) >> 8]] != 0) 
                             pc +=  4;
                          else 
                             pc += 2;
                          break;
                       case 0x00A1:

                          printf("\nUnknown op code: %.4X\n", instruction);
                          if (key[v[(instruction & 0x0F00) >> 8]] == 0) 
                            pc +=  4;
                          else 
                             pc += 2;
                          break;

                       default:
                          printf("\nUnknown op code: %.4X\n", instruction);
                          exit(3);
                    }
                    break;

      case 0xF000 : {

                       switch(instruction & 0x00FF) {

                          case 0x0007 :
                             //printf("\nUnknown op code: %.4X\n", instruction);
                             v[(instruction &0x0F00) >> 8] = delay_timer;
                             pc+=2;
                             break;

                          case 0x000A: {
                                         printf("\nUnknown op code: %.4X\n", instruction);
                                         bool key_pressed = false;
                                         for(int i = 0; i < 16; ++i)
                                          {
                                             if(key[i] != 0)
                                             {
                                                v[(instruction & 0x0F00) >> 8] = i;
                                                key_pressed = true;
                                             }

                                          }
                                          if(!key_pressed)
                                             return;

                                          pc += 2;
                                       }
                                       break;
                          case 0x0015:
                                       printf("\nUnknown op code: %.4X\n", instruction);
                                       delay_timer = v[(instruction & 0x0F00) >> 8];
                                       pc+=2;
                                       break;
                          case 0x0018:
                                       printf("\nUnknown op code: %.4X\n", instruction);
                                       sound_timer =  v[(instruction & 0x0F00) >> 8];
                                       pc+=2;
                                       break;
                          case 0x001E:
                                       printf("\nUnknown op code: %.4X\n", instruction);

                                       // VF is set to 1 when range overflow (I+VX>0xFFF), and 0
                                       // when there isn't.
                                       //
                                       if(I + v[(instruction & 0x0F00) >> 8] > 0xFFF)
                                          v[0xF] = 1;
                                       else
                                          v[0xF] = 0;

                                       I += v[(instruction & 0x0F00) >> 8];
                                       pc += 2;
                                       break;

                          case 0x0029:
                                       printf("\nUnknown op code: %.4X\n", instruction);
                                       I = v[(instruction & 0x0F00) >> 8] * 0x5;
                                       pc += 2;
                                       break;
                          case 0x0033:
                                       printf("\nUnknown op code: %.4X\n", instruction);
                                       memory[I]     = v[(instruction & 0x0F00) >> 8] / 100;
                                       memory[I + 1] = (v[(instruction & 0x0F00) >> 8] / 10) % 10;
                                        memory[I + 2] = v[(instruction & 0x0F00) >> 8] % 10;
                                        pc += 2;
                                        break;


                          case 0x0055:
                                        printf("\nUnknown op code: %.4X\n", instruction);
                                        for (int i = 0; i <= ((instruction & 0x0F00) >> 8); ++i)
                                          memory[I + i] = v[i];

                                       // On the original interpreter, when the
                                       // operation is done, I = I + X + 1.
                                       I += ((instruction & 0x0F00) >> 8) + 1;

                                       pc += 2;
                                       break;
                          case 0x0065:
                                       printf("\nUnknown op code: %.4X\n", instruction);
                                       for (int i = 0; i <= ((instruction & 0x0F00) >> 8); ++i)
                                          v[i] = memory[I + i];
                                       // On the original interpreter,
                                       // when the operation is done, I = I + X + 1.
                                       I += ((instruction & 0x0F00) >> 8) + 1;
                                       pc += 2;
                                       break;

                          default:
                              printf("Unknown opcode [0xF000]: 0x%X\n", instruction);
                       }

                            break;
           default:
           printf ("Unknown opcode [0xF000]: 0x%X\n", instruction);

        }
   }
   if (delay_timer > 0)
      --delay_timer;
   if(sound_timer> 0)
      if(sound_timer == 1)
         --sound_timer;

}




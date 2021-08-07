#ifndef __MCAL_CPU_H__
#define __MCAL_CPU_H__

/**
 * @brief Exectute no-op instruction
 */
void MCAL_NOP();
#define MCAL_NOP() __asm__ __volatile__("nop")

/**
 * @brief Lock arbiter priority settings
 * 
 * Interrupts have to be disabled before executing this
 */
void MCAL_CPU_ARBITER_LOCK();
#define MCAL_CPU_ARBITER_LOCK()    \
      do{ \
           asm("BANKSEL PRLOCK");\
           asm("MOVLW   0x55");\
           asm("MOVWF   (PRLOCK & 0xFF)");\
           asm("MOVLW   0xAA");\
           asm("MOVWF   (PRLOCK & 0xFF)");\
           asm("BSF     (PRLOCK & 0xFF),0");\
        }while(0)

/**
 * @brief Unlock arbiter priority settings
 * 
 * Interrupts have to be disabled before executing this
 */
void MCAL_CPU_ARBITER_UNLOCK();
#define MCAL_CPU_ARBITER_UNLOCK()  \
      do{ \
           asm("BANKSEL PRLOCK");\
           asm("MOVLW   0x55");\
           asm("MOVWF   (PRLOCK & 0xFF)");\
           asm("MOVLW   0xAA");\
           asm("MOVWF   (PRLOCK & 0xFF)");\
           asm("BCF     (PRLOCK & 0xFF),0");\
        }while(0)

#ifndef MCAL_ISR_BASE
#define MCAL_ISR_BASE 8
#endif // MCAL_ISR_BASE

// This is required for vs code intellisense to not scream at ISRs
#if defined(__INTELLISENSE__)
#define MCAL_ISR(irqname) void irqname ## _ISR()
#else
#define MCAL_ISR(irqname) void __interrupt(irq(irqname),base(MCAL_ISR_BASE)) irqname ## _ISR()
#endif

#endif // __MCAL_CPU_H__
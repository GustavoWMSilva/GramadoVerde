#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void ledsOff();
void blink();

volatile uint8_t val = 0x1;
volatile uint8_t j = 0;
volatile int piscando_continua = 0;

ISR(PCINT0_vect)
{

    if (PINB & (1 << PB0))
    {
        piscando_continua = !piscando_continua;
    }
    PORTB &= ~(1 << PB0);
}

int main()
{

    DDRB &= ~(1 << PB0); // entrada no B0
    DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB3) |
            (1 << PB4) | (1 << PB5); // seta PB1 a PB5 como saídas, ligando as saídas
    PORTB &= ~(1 << PB0);

    PCICR |= (1 << PCIE0);   // habilita vetor de interrupção
    PCMSK0 |= (1 << PCINT0); // habilita interrupção para PB0

    sei();

    while (1)
    {
        if (piscando_continua && j < 0x6)
        {

            blink();
            j++;

            if (!piscando_continua || j == 0x6)
            {
                piscando_continua = 0;
                ledsOff();
            }
        }
    }
}

void ledsOff()
{
    uint8_t k = 0;
    while (val && !piscando_continua) // desliga os LEDs usando uma variável
    {

        while (k < 0x64 && !piscando_continua)
        {
            _delay_ms(50); // 5s quebrar o delay e ajeitar o botão
            k++;
        }
        k = 0;
        if (!piscando_continua)
        {
            PORTB &= ~val;
            val = val >> 1; // desliga o anterior
            j--;            // controla o numero de leds ativos
        }
        else
            val = val << 1;
    }
}

void blink()
{
    uint8_t i = 0;
    uint8_t res = val;

    while ((i < 0x64) && piscando_continua)
    { // Liga os LEDs usando uma variável
        i++;
        PORTB ^= res;
        res = res << 1; // Liga o próximo
        if (!res)
        {
            res = val;
        }
        _delay_ms(30); // 3000/100
    }

    if (piscando_continua)
    {
        PORTB |= val;   // Liga
        val = val << 1; // Atualiza
    }

    res = val;
    while (res)
    {
        PORTB &= ~res;
        res = res << 1; // desliga o próximo
    }
}
#include "mbed.h"
#include <hairio.h>
#include <kertoimet.h>
#include <IIR_kertoimet.h>
#define TS 0.000125   //Ts = i/FS = 1/8000 => 0.000125 

Serial pc (USBTX,USBRX);
DigitalOut led1(LED1);
Ticker aDDaKello;
AnalogIn AnalogToDigital(A0);
AnalogOut DigitalToAnalog(A2);

int indeksi = 0;
float viivastysElementti[pituus];
float muisti[riveja][sarakkeita / 2];
float FIR_suodin (float);
float IIR_suodin (float);

void aDDaKello_isr(void)
{
    indeksi = (indeksi + 1)%79998;
    // DigitalToAnalog = suodatettava_signaali[indeksi];
    // DigitalToAnalog = FIR_suodin(suodatettava_signaali[indeksi]);
    DigitalToAnalog = IIR_suodin(suodatettava_signaali[indeksi]);
}

int main()
{

    aDDaKello.attach(&aDDaKello_isr, TS);
    while (1)
    {
        // Täällä odotellaan vain, että ticker antaa keskeytyksen.
    }

}

float IIR_suodin (float j)
{
    // Viittaukset edellisiin lähtöihin vrt. FIR, jossa edellisiin datapisteisiin
    float suodatuksen_tulos = j;

    for (int i = 0; i < riveja; i++)
    {
        /*****************************************************************
        * riveja = suodatinLohko lkm. ja i = lohkoindeksi
        * a = takaisinkyt.kerroin
           => eli käytännössä 2 (3) viimeistä saraketta headerista (4, 5)
        * suoraankyt.kertoimet = b
            => eli 3 ensimmäistä saraketta headerista (0, 1, 2)
            
        1. m2 = m1
        2. m1 = m0
        3. m0 = x(n) - a(1) * m1 - a(2) * m2
        4. y(n) = b(0) * m0 + b1 * m1 + b(2) * m2
        *****************************************************************/
        muisti[i][2] = muisti[i][1];

        muisti[i][1] = muisti[i][0];

        muisti[i][0] = suodatuksen_tulos - suodatinLohko[i][4] * muisti[i][1] - suodatinLohko[i][5] * muisti[i][2];

        suodatuksen_tulos = suodatinLohko[i][0] * muisti[i][0] + suodatinLohko[i][1] * muisti[i][1] + suodatinLohko[i][2] * muisti[i][2];
    }
    return suodatuksen_tulos;
}

float FIR_suodin (float j)
{
    float suodatuksen_tulos = 0.0;

    for (int i = pituus - 1; i > 0; i--)
    {

        viivastysElementti[i] = viivastysElementti[i-1]; // 1. kierroksella viivastysElementtin indeksi 31 - 1;
    }

    viivastysElementti[0] = j;

    for (int i = 0; i < pituus; i++)
    {
        suodatuksen_tulos += viivastysElementti[i] * kertoimet[i];
    }
    return suodatuksen_tulos;
}

#include "FileGroup.h"
#include "Module.h"
#include "FastFFT.h"
#include "math.h"

#define BufferCount     256

static int32_t OriBuffer[BufferCount];
static int32_t ImagBuffer[BufferCount];
static float WindowFunction[BufferCount];
float Strength[BufferCount / 2];

static bool FFT_Ready = false;

void FFT_AddData(int32_t data)
{
    static uint16_t cnt = 0;
    
    if(!FFT_Ready)
    {
        OriBuffer[cnt] = data;
        cnt++;
        if(cnt >= __Sizeof(OriBuffer))
        {
            cnt = 0;
            FFT_Ready = true;
        }
    }
}

static void WindowFunction_Init()
{
    for(int i = 0; i < BufferCount; i++)
    {
        WindowFunction[i] = 1 * (0.42f - 0.5f * cos(2.0f * PI * i / (BufferCount - 1)) + 0.08f * cos(4.0f * PI * i / (BufferCount - 1)));
    }
}

const float *FFT_GetStrength()
{
    return Strength;
}

void FFT_Process()
{
    __ExecuteOnce(WindowFunction_Init());
    
    if(!FFT_Ready)
        return;
    
    int64_t avg = 0;
    for(int i = 0; i < BufferCount; i++)
    {
        avg += OriBuffer[i];
    }
    avg /= BufferCount;

    for(int i = 0; i < BufferCount; i++)
    {
        OriBuffer[i] = (OriBuffer[i] - avg) * WindowFunction[i];
    }

    fast_fft_256_res_2p8_analyse(OriBuffer, ImagBuffer);

    for(int i = 0 ; i < BufferCount / 2; i++)
    {
        Strength[i] = sqrtf((float)(sq(OriBuffer[i]) + sq(ImagBuffer[i]))) * !!i;
    }
    
    FFT_Ready = false;
}

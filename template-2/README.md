# STM32F446RE Clock and FPU Quick Notes

## Clock Configuration

### PLL Settings

```c
#define PLL_M 4
#define PLL_N 180
#define PLL_P 0   // PLLP = 2
#define PLL_Q 8
```

```text
STM32F446RE HSE clock (using ST link source): HSE = 8 MHz
```

### Clock Calculation

```text
PLL Input = HSE / PLL_M
          = 8 / 4
          = 2 MHz

VCO = PLL Input × PLL_N
    = 2 × 180
    = 360 MHz

SYSCLK = VCO / PLLP
       = 360 / 2
       = 180 MHz
```

### Bus Clocks

| Clock        | Value   |
| ------------ | ------- |
| SYSCLK       | 180 MHz |
| HCLK (AHB)   | 180 MHz |
| APB1 (PCLK1) | 45 MHz  |
| APB2 (PCLK2) | 90 MHz  |

---

# FPU (Floating Point Unit)

The Cortex-M4 contains a hardware FPU that speeds up `float` calculations.

Example:

```c
float a = 1.5f;
float b = 2.5f;
float c = a * b;
```

---

## Enable FPU in Code

Call before using floating-point operations:

```c
int main(void)
{
    fpu_enable();

    SystemClock_Config();

    while(1)
    {
    }
}
```

---

## Enable FPU in STM32CubeIDE

Open:

```text
Project
 └─ Properties
     └─ C/C++ Build
         └─ Settings
             └─ MCU Settings
```

Set:

```text
Floating-point hardware : FPv4-SP-D16
Floating-point ABI      : Hard
```

Compiler flags:

```text
-mfpu=fpv4-sp-d16
-mfloat-abi=hard
```


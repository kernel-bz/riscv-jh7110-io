# 커널연구회 RISC-V 입출력 보드 실습용 소스

소스 경로 구조:

|-- README.md
|-- app
|   |-- gpio (GPIO 실습 소스)
|   |   |-- gpio
|   |   |-- gpio-test
|   |   |-- gpio-test.c
|   |   `-- gpio.c
|   |-- hello
|   |   |-- hello
|   |   `-- hello.c
|   |-- i2c (I2C 실습 소스)
|   |   |-- i2c
|   |   `-- i2c.c
|   |-- pwm (PWM 실습 소스)
|   |   |-- pwm
|   |   |-- pwm-test
|   |   |-- pwm-test.c
|   |   `-- pwm.c
|   `-- uart (UART 실습 소스)
|       |-- build.sh
|       |-- recv
|       |-- recv.c
|       |-- send
|       |-- send.c
|       |-- serial.c
|       `-- serial.h
|-- dtb
|   `-- overlay (DTB 추가 소스)
|       |-- run.sh
|       |-- vf2-overlay-i2c1.dtbo
|       `-- vf2-overlay-uart3.dtbo
`-- modules (드라이버 모듈)
    |-- user-gpio-driver.ko
    |-- user-i2c-smartmotor.ko
    `-- user-i2c-smartpower.ko


보드 기능 요약:

    • RISC-V 64비트 StarFive JH7110  40핀맵 호환
    • Serial(UART0, ttyS0) to USB 포트 탑재 (디버그 콘솔)
    • Serial(UART3, ttyS3) 포트 지원
    • I2C0, I2C1 포트 지원 (센서 연결)
    • SPI 포트 지원 (센서 연결)
    • PWM0, PWM1, PWM2, PWM3 포트 출력 (모터 연결)
    • GPIO 포트 및 LED 출력 지원
    • Switch1, Switch2 외부 인터럽트 처리 지원
    • 5V 외부 전원 입력 RCA 잭
    • 쿨링팬 연결 지원
    • 40핀 입출력핀 확장 (적층) 지원 (기능 확장)
    • 실습 소스 제공(https://github.com/kernel-bz/riscv-jh7110-io.git)

좀더 자세한 내용들은 커널연구회(www.kernel.bz) 홈페이지 참조 바랍니다.

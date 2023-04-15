#include <Arduino.h>
#include "pioasm/bidir_dshot.pio.h"

#define outpin 3
#define pwmpin (outpin + 1)


PIO pio;
uint sm;
char str[128];
void setup() {
	set_sys_clock_khz(132000, true);
    gpio_init(25);
	gpio_set_dir(25, true);
	gpio_put(25, HIGH);
	Serial.begin(115200);
	delay(2000);
	gpio_put(25, LOW);
	printf("prepared blinking LED and clock freq\n");

    gpio_init(outpin);
	gpio_set_dir(outpin, GPIO_OUT);
	gpio_pull_up(outpin);
    gpio_init(pwmpin);
	gpio_set_dir(pwmpin, GPIO_OUT);
	delay(2000);
	gpio_put(25, HIGH);
	printf("prepared other GPIOs\n");

	//PWM
	gpio_set_function(pwmpin, GPIO_FUNC_PWM);
	uint8_t sliceNum = pwm_gpio_to_slice_num(pwmpin);
	pwm_set_wrap(sliceNum, 100);
	pwm_set_clkdiv_int_frac(sliceNum, 132, 0);
	pwm_set_gpio_level(pwmpin, 20);
    pwm_set_enabled(sliceNum, true);
	printf("prepared PWM\n");

	// PIO
	pio = pio0;
	uint offset = pio_add_program(pio, &bidir_dshot_program);
	sm = pio_claim_unused_sm(pio, true);
	pio_sm_config c = bidir_dshot_program_get_default_config(offset);
	printf("prepared PIO program\n");

	pio_gpio_init(pio, outpin);
	sm_config_set_set_pins(&c, outpin, 1);
	sm_config_set_out_pins(&c, outpin, 1);
	sm_config_set_in_pins(&c, outpin);
	sm_config_set_jmp_pin(&c, outpin);
	printf("prepared PIO pins\n");

	sm_config_set_out_shift(&c, false, false, 32);
	sm_config_set_in_shift(&c, false, false, 32);
	printf("prepared shift registers\n");

	pio_sm_set_consecutive_pindirs(pio, sm, outpin, 1, true);
	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);
	pio_sm_set_clkdiv_int_frac(pio, sm, bidir_dshot_DSHOT1200_CLKDIV_INT, bidir_dshot_DSHOT1200_CLKDIV_FRAC);
	pio_sm_put(pio, sm, 15);
	printf("enabled pio\n");
	delay(8000);
}

uint16_t appendChecksum(uint16_t data){
	int csum = data;
	csum ^= data >> 4;
	csum ^= data >> 8;
	csum = ~csum;
	csum &= 0xF;
	return (data << 4) | csum;
}

#define iv 0xFFFFFFFF
uint32_t packet0, packet1, dec, cscor;
int calcRPM(uint32_t data){

	//perform 21 bit -> 20 bit decoding
	packet0 = data;
	data = (data >> 1) ^ data;
	packet1 = data;

	static const uint32_t decode[32] = {
        iv, iv, iv, iv, iv, iv, iv, iv, iv, 9, 10, 11, iv, 13, 14, 15,
        iv, iv, 2, 3, iv, 5, 6, 7, iv, 0, 8, 1, iv, 4, 12, iv };
	//perform GCR decoding
	uint32_t decodedValue = decode[data & 0x1f];
    decodedValue |= decode[(data >> 5) & 0x1f] << 4;
    decodedValue |= decode[(data >> 10) & 0x1f] << 8;
    decodedValue |= decode[(data >> 15) & 0x1f] << 12;
	dec = decodedValue;

	//checksum
	uint32_t csum = decodedValue;
    csum = csum ^ (csum >> 8);
    csum = csum ^ (csum >> 4);
	if ((csum & 0xf) != 0xf || decodedValue > 0xffff) {
		cscor = 0;
		return -5000;
	} else {
		cscor = 1;
		data = decodedValue >> 4;
        if (data == 0x0fff) {
            return 0;
        }
        data = (data & 0x000001ff) << ((data >> 9) & 0x7);
        if (!data) {
            return 1500;
        }
        data = (1000000 * 60 + data * 50) / data;
		#define MOTOR_POLES 14
		return data / (MOTOR_POLES / 2);
	}
}

uint16_t data;
uint32_t packet;
int num = 0;
int updown = 0;

int32_t p, i, d, lastRPM;
float kP = .2, kI = 0.0005, kD = .2;
uint8_t counter;

uint16_t calcThrottle(uint32_t rpm){
	#define TARGET_RPM 2500

	d = lastRPM - rpm;
	p = TARGET_RPM - rpm;
	i += TARGET_RPM - rpm;

	lastRPM = rpm;
	int ap = kP*p, ai = kI*i, ad = kD*d;
	if (!counter++) printf("%8d %8d %8d %8d %8d\n", ap, ai, ad, i, rpm);


	uint16_t throttle = ap + ai + ad;
	if (throttle > 1000) throttle = 1000;
	return throttle;
}

uint16_t throttle;
void loop() {
	// put your main code here, to run repeatedly:
	uint32_t rpm = calcRPM(pio_sm_get_blocking(pio, sm));
		// printf("%4d, %6d\n", throttle, rpm);
	throttle = calcThrottle(rpm);
	pio_sm_put(pio, sm, appendChecksum((throttle + 48) << 1));
}
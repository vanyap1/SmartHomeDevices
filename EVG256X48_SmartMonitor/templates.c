
/*
 * templates.c
 *
 * Created: 15.10.2023 21:38:37
 *  Author: Vanya
 */ 

//led_val = rf_isReady();
//led1[g_part] = led_val;
//led1[b_part] = 255-led_val;
//led2[g_part] = led_val;
//led2[b_part] = 255-led_val;
//led3[g_part] = led_val;
//led3[b_part] = 255-led_val;
//
//led4[g_part] = led_val;
//led4[b_part] = 255-led_val;
//
//led5[g_part] = led_val;
//led5[b_part] = 255-led_val;


//if(I2C_write_batch(panel_addr,  led1, sizeof(led1))){
//gpio_set_pin_level(LED_R, false);
//}else{
//gpio_set_pin_level(LED_R, true);
//}
//I2C_write_batch(panel_addr, led2, sizeof(led2));
//I2C_write_batch(panel_addr, led3, sizeof(led3));
//I2C_write_batch(panel_addr, led4, sizeof(led4));
//I2C_write_batch(panel_addr, led5, sizeof(led5));
//I2C_write_batch(panel_addr, ext_led, sizeof(ext_led));



//
//


//while (1) {
	//WDT->CLEAR.bit.CLEAR = 0xA5;
	//while(WDT->STATUS.bit.SYNCBUSY);
	//gpio_toggle_pin_level(LED_G);
	//
	//
	////
	////delay_ms(25);
	////I2C_write_batch(addr_expander, port_expander2, sizeof(port_expander2));
	////I2C_write_batch(addr_expander, port_expander_on, sizeof(port_expander_on));
	//
	//uint8_t read_state = I2C_read_batch(addr_expander, port_state, sizeof(port_state));
	//if((port_state[0] >> 1) & 1){
		//gpio_set_pin_level(LED_R, true);
		//I2C_write_batch(addr_expander, port_expander_on, sizeof(port_expander_on));
		//}else{
		//gpio_set_pin_level(LED_R, false);
		//I2C_write_batch(addr_expander, port_expander_off, sizeof(port_expander_off));
	//}
	////delay_ms(20);
	////addr_expander++;
	//
	//
//}





//i2c Expander
	
	//#define  input_reg		0x00
	//#define  output_reg		0x02
	//#define  polarity_reg	0x04
	//#define  config_reg		0x06
	//
	//uint8_t port_expander_off[3] = {output_reg,0x00, 0x00};
	//uint8_t port_expander_on[3] = {output_reg,0x01, 0x00};
	//uint8_t port_config[3] = {config_reg, 0x02, 0x00};
	//uint8_t port_polarity[3] = {polarity_reg, 0x00, 0x00};
	//
	//uint8_t port_state[2];
	//uint8_t addr_expander = 0x20;
	//
	//
	//I2C_write_batch(addr_expander, port_config, sizeof(port_config));
	//I2C_write_batch(addr_expander, port_polarity, sizeof(port_polarity));







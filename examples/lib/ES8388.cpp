#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ES8388.h"
#include "driver/i2c.h"

// Initialize codec
uint8_t ES8388::init(void)
{
  initI2C();
  
  writeReg(ES8388_CONTROL2,80); // 0101 0000: chip control settings
  writeReg(ES8388_CHIPPOWER,0); // 0000 0000: chip power management
  writeReg(ES8388_MASTERMODE,0); // 0000 0000: slave serial port mode
  writeReg(ES8388_CONTROL1,18); // 0001 0010: chip control settings 
  writeReg(ES8388_DACCONTROL2,2); // 0000 0010: DACFsRatio (Master mode DAC MCLK to sampling frequency ratio) is set to 256 
  writeReg(ES8388_DACCONTROL16,0); // 0000 0000: no shelving filter (all coefs to 0)
  writeReg(ES8388_DACCONTROL17,144); // 1001 0000: left DAC to left mixer enabled + LIN signal to left mixer gain is 0dB
  writeReg(ES8388_DACCONTROL20,144); // 1001 0000: same config as above but for right mixer
  writeReg(ES8388_DACCONTROL23,0); // 0000 0000: VREF to default
  writeReg(ES8388_DACCONTROL5,0); // 0000 0000: Digital volume control attenuates the signal in 0.5 dB incremental from 0dB (right)
  writeReg(ES8388_DACCONTROL4,0); // 0000 0000: same as above but for left
  writeReg(ES8388_ADCCONTROL1,187); // 1011 1011: mic preamps gains
  writeReg(ES8388_ADCCONTROL2,0); // 0000 0000: input selection: default
  writeReg(ES8388_ADCCONTROL3,2); // 0000 0010: signal routing of ADC: default
  writeReg(ES8388_ADCCONTROL5,2); // 0000 0010: ADC MCLK at 256 fo ADC
  writeReg(ES8388_ADCCONTROL8,0); // 0000 0000: Digital volume control attenuates the signal in 0.5 dB incremental at 0dB for ADC L
  writeReg(ES8388_ADCCONTROL9,0); // 0000 0000: Digital volume control attenuates the signal in 0.5 dB incremental at 0dB for ADC R
  writeReg(ES8388_ADCCONTROL10,12); // 0000 1100: 16 bits for ADC
  writeReg(ES8388_ADCCONTROL4,12); // 0000 1100: settings i2s config
  writeReg(ES8388_DACCONTROL1,24); // 0001 1000: already set again...
  writeReg(ES8388_DACCONTROL24,23); // 0001 0111: DAC volume (L)
  writeReg(ES8388_DACCONTROL25,23); // 0001 0111: DAC volume (R)
  writeReg(ES8388_DACCONTROL26,0); // 0000 0000: LOUT volume set to 0
  writeReg(ES8388_DACCONTROL27,0); // 0000 0000: LOUT volume set to 0
  writeReg(ES8388_DACCONTROL21,128); // 1000 0000: DACLRC and ADCLRC are same
  writeReg(ES8388_ADCPOWER,0); // 0000 0000: Deactivating ADC
  writeReg(ES8388_DACPOWER,60); // 0011 1100: enabling output
  writeReg(ES8388_DACCONTROL3,0); // 0000 0000: muting analog out

  return 0;
}

// Initialize i2c for the ESP32
void ES8388::initI2C(void)
{
  i2c_port_t i2c_master_port = (i2c_port_t) I2C_MASTER_NUM;
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = (gpio_num_t) I2C_MASTER_SDA_IO;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num = (gpio_num_t) I2C_MASTER_SCL_IO;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
  i2c_param_config(i2c_master_port, &conf);
  i2c_driver_install(i2c_master_port, conf.mode,
                     I2C_MASTER_RX_BUF_DISABLE,
                     I2C_MASTER_TX_BUF_DISABLE, 0);
}

// Register write
// reg: Register Address
// val: Register value
uint8_t ES8388::writeReg(uint8_t reg,uint8_t val)
{    
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, ES8388_ADDR, 1);
  i2c_master_write(cmd, &reg, sizeof(reg), ACK_CHECK_EN);
  i2c_master_write(cmd, &val, sizeof(val), ACK_CHECK_EN);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin((i2c_port_t) I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
    
  return 0;
}

// Returns the value in reg
// reg: Register Address
uint8_t ES8388::readReg(uint8_t reg)
{
  uint8_t p_data;
  i2c_cmd_handle_t cmd;
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, ES8388_ADDR, ACK_CHECK_EN);
  i2c_master_write(cmd, &reg, sizeof(reg), ACK_CHECK_EN);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin((i2c_port_t) I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, ES8388_ADDR | 0x01, ACK_CHECK_EN);
  
  int datalen = 1;
  uint8_t *r_data;
  r_data = &p_data;
  for (int i = 0; i < datalen - 1; i++) {
    i2c_master_read_byte(cmd, &r_data[i], (i2c_ack_type_t) 0);
  }
  i2c_master_read_byte(cmd, &r_data[datalen - 1], (i2c_ack_type_t) 1);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin((i2c_port_t) I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  
  return p_data;
}

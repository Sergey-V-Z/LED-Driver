#include "flash_spi.h"

bool flash :: Init(SPI_HandleTypeDef *hspi, uint32_t StartAddr,  pins_spi_t ChipSelect, pins_spi_t WriteProtect, pins_spi_t Hold){
  
  flash :: WriteProtect = WriteProtect;
  flash :: ChipSelect = ChipSelect;
  flash :: Hold = Hold;
  flash :: hspi = hspi;
  StartAddres = StartAddr;
  uint8_t TxBuff[4] = {0x05};
  uint8_t RxBuff[4] = {0x05};
  
  uint8_t *pAddr = (uint8_t*)&StartAddres;
  for(int i = 0; i < 3; ++i)
    {
      cmdRead[i+1] = *(pAddr+i);
      cmdWrite[i+1] = *(pAddr+i);
    }
  
  HAL_GPIO_WritePin(WriteProtect.GPIO_Port, WriteProtect.GPIO_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(Hold.GPIO_Port, Hold.GPIO_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(hspi, TxBuff, 1, 10); // 
  HAL_SPI_Receive(hspi, RxBuff, 1, 10);
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  return true;
}

void flash :: Read(settings_t *data){
  HAL_StatusTypeDef StatusSPI;
  uint32_t sizeData = sizeof(*data);
  uint8_t *ptrData = (uint8_t*)data;
  
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  
  StatusSPI = HAL_SPI_Transmit(hspi, cmdRead, 4, 10); // cmd fead + addres
  
  StatusSPI = HAL_SPI_Receive(hspi, ptrData, sizeData, 10);
  //StatusSPI2 = StatusSPI2;
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  lastStatusSPI = StatusSPI;
  
}

void flash :: Write(settings_t data){
  HAL_StatusTypeDef StatusSPI;
  uint32_t sizeData = sizeof(data);
  uint8_t *ptrData = (uint8_t*)&data;
  uint8_t wren[1] = {0x06};
  uint8_t sr[1] = {0x05};
  uint8_t eraseSector[1] = {0x20};
  uint8_t rd[5] = {0};
  
  // write eneable 
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(hspi, wren, 1, 10); //
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  // read status register
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  StatusSPI = HAL_SPI_Transmit(hspi, sr, 1, 10); // 
  StatusSPI = HAL_SPI_Receive(hspi, rd, 2, 10);
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  // erase
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(hspi, eraseSector, 1, 10); //
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  // read status register
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  StatusSPI = HAL_SPI_Transmit(hspi, sr, 1, 10); // 
  StatusSPI = HAL_SPI_Receive(hspi, rd, 2, 10);
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  // write eneable 
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(hspi, wren, 1, 10); //
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_RESET);
  StatusSPI = HAL_SPI_Transmit(hspi, cmdWrite, 4, 10); // cmd write + addres
  StatusSPI = HAL_SPI_Transmit(hspi, ptrData, sizeData, 10);
  HAL_GPIO_WritePin(ChipSelect.GPIO_Port, ChipSelect.GPIO_Pin, GPIO_PIN_SET);
  
  lastStatusSPI = StatusSPI;
}

flash :: flash(){
  
}


#include "xbc_mb.h"
#include "can_xbc_mb.h"
#include "spi_xbc_mb.h"
#include "bt_xbc_mb.h"

static XBC_PRIORITY xbc_priority = XBC_SPI_FIRST;

//Listeners
static uint32_t pressedTime[XBC_BTN_COUNT] = {0};
static OnClickListener clickListeners[XBC_BTN_COUNT] = {0};
static OnReleaseListener releaseListeners[XBC_BTN_COUNT] = {0};
static HoldListenerStruct holdListeners[XBC_BTN_COUNT] = {0};

/**
  * @brief Initialize xbox contollers
  * @param priority: If multiple controllers are connected, which controller is used
  * @retval None.
  */
void xbc_mb_init(XBC_PRIORITY priority) {
  xbc_priority = priority;
  spi_xbc_mb_init();
	bt_xbc_mb_init();
  can_xbc_mb_init();
}

/**
  * @brief Get xbc connection state
  * @retval Connection state. @ref XBC_CONNECTION
  */
XBC_CONNECTION xbc_get_connection(void) {
  switch (xbc_priority) {
		case XBC_SPI_FIRST:
			if (spi_xbc_get_connection() == SPI_XBC_ALL_CONNECTED) {
        return XBC_SPI_CONNECTED;
      } else if (bt_xbc_get_connection() == BT_XBC_ALL_CONNECTED) {
        return XBC_BT_CONNECTED;
      } else if (can_xbc_get_connection() == CAN_XBC_ALL_CONNECTED) {
        return XBC_CAN_CONNECTED;
      }
    break;
		
    case XBC_CAN_FIRST:
      if (can_xbc_get_connection() == CAN_XBC_ALL_CONNECTED) {
        return XBC_CAN_CONNECTED;
      } else if (spi_xbc_get_connection() == SPI_XBC_ALL_CONNECTED) {
        return XBC_SPI_CONNECTED;
      }  else if (bt_xbc_get_connection() == BT_XBC_ALL_CONNECTED) {
        return XBC_BT_CONNECTED;
      }
    break;
			
		case XBC_BT_FIRST:
			if (bt_xbc_get_connection() == BT_XBC_ALL_CONNECTED) {
        return XBC_BT_CONNECTED;
      } else if (spi_xbc_get_connection() == SPI_XBC_ALL_CONNECTED) {
        return XBC_SPI_CONNECTED;
      } else if (can_xbc_get_connection() == CAN_XBC_ALL_CONNECTED) {
        return XBC_CAN_CONNECTED;
      }
		break;
  }
  return XBC_DISCONNECTED;
}

/**
  * @brief Get the current XBC priority.
  * @retval The current XBC priority. @ref XBC_PRIORITY.
  */
XBC_PRIORITY xbc_get_priority(void) {
  return xbc_priority;
}

/**
  * @brief Get XBC digital. (Used with bitwise operators)
  * @retval xbc_digital (bitwise).
  */
u32 xbc_get_digital(void) {
  switch (xbc_get_connection()) {
    case XBC_CAN_CONNECTED:
      return can_xbc_get_digital();

    case XBC_SPI_CONNECTED:
      return spi_xbc_get_digital();
		
		case XBC_BT_CONNECTED:
      return bt_xbc_get_digital();

    default:
      return 0;
  }
}

/**
  * @brief Get the value of a specific button.
  * @param buttonId: Button to be checked
  * @retval 1 if pressed, 0 if not pressed
  */
u8 xbc_get_button(u8 buttonId) {
  switch (xbc_get_connection()) {
    case XBC_CAN_CONNECTED:
      return can_xbc_get_button(buttonId);

    case XBC_SPI_CONNECTED:
      return spi_xbc_get_button(buttonId);
		
		case XBC_BT_CONNECTED:
      return bt_xbc_get_button(buttonId);

    default:
      return 0;
  }
}

/**
  * @brief Get XBC joystick value (with deadzone correction).
  * @param j: joystick ID. @ref XBC_JOY
  * @retval The joystick value. (0 - 255 for 1-byte joystick, -XBC_JOY_SCALE to XBC_JOY_SCALE for 2-byte joystick)
  */
s16 xbc_get_joy(XBC_JOY j) {
  switch (xbc_get_connection()) {
    case XBC_CAN_CONNECTED:
      return can_xbc_get_joy(j);

    case XBC_SPI_CONNECTED:
      return spi_xbc_get_joy(j);
		
		case XBC_BT_CONNECTED:
      return bt_xbc_get_joy(j);

    default:
      return 0;
  }
}

/**
  * @brief Get the raw XBC joystick value (NO deadzone correction).
  * @param j: joystick ID. @ref XBC_JOY
  * @retval The joystick value. (0 - 255 for 1-byte joystick, -32768 to 32 for 2-byte joystick)
  */
s16 xbc_get_joy_raw(XBC_JOY j) {
  switch (xbc_get_connection()) {
    case XBC_CAN_CONNECTED:
      return can_xbc_get_joy_raw(j);

    case XBC_SPI_CONNECTED:
      return spi_xbc_get_joy_raw(j);
		
		case XBC_BT_CONNECTED:
      return bt_xbc_get_joy_raw(j);

    default:
      return 0;
  }
}

/**
  * @brief Update states of button. Call regularly at 20-100Hz.
  */
void xbc_button_update(void) {
	for (uint8_t i=0; i<XBC_BTN_COUNT; i++){
		if (xbc_get_button(i)){
			//If the button is pressed
			
			if (pressedTime[i]==0 && clickListeners[i] !=0 ){
				//Capture the instant when the button is pressed
				clickListeners[i]();
			}
			
			if (holdListeners[i].holdListener != 0){
				if (pressedTime[i] > holdListeners[i].holdThreshold && (pressedTime[i]%holdListeners[i].trigFrequency==0)){
					//Trigger the hold listener
					holdListeners[i].holdListener();
				}
			}
			
			pressedTime[i]++;
			
		}else{
			//If the button is not pressed
			if (pressedTime[i] > 0 && releaseListeners[i] != 0){
				//Capture the instant when the button is released
				releaseListeners[i]();
			}
			pressedTime[i] = 0;
		}
	}
}

/**
  * @brief Set the click listener of a button.
  * @param button: button ID.
  * @param listener: pointer to listener function.
  */
void xbc_reg_OnClickListener(XBCButtonID button, OnClickListener listener) {
	clickListeners[button] = listener;
}

/**
  * @brief Set the release listener of a button.
  * @param button: button ID.
  * @param listener: pointer to listener function.
  */
void xbc_reg_OnReleaseListener(XBCButtonID button, OnClickListener listener) {
  releaseListeners[button] = listener;
}

//Called every "frequency" after "threshold" have been reached. Actual time depends on frequency of @button_update()
void xbc_reg_OnHoldListener(u8 button_id, uint16_t threshold, uint16_t frequency, OnHoldListener listener){
	holdListeners[button_id].holdListener = listener;
	holdListeners[button_id].holdThreshold = threshold;
	holdListeners[button_id].trigFrequency = frequency;
}

/**
  * @brief Clear the click listener of a button.
  * @param button: button ID.
  */
void xbc_drop_OnClickListener(XBCButtonID button) {
  clickListeners[button] = 0;
}

/**
  * @brief Clear the release listener of a button.
  * @param button: button ID.
  */
void xbc_drop_OnReleaseListener(XBCButtonID button) {
  releaseListeners[button] = 0;
}

void xbc_drop_OnHoldListener(u8 button_id){
	holdListeners[button_id].holdListener = 0;
}

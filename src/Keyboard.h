#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum KEYS_ENUM
{
		KEY_0 = 0,
		KEY_1 = 1,
		KEY_2 = 2,
		KEY_3 = 3,
		KEY_4 = 4,
		KEY_5 = 5,
		KEY_6 = 6,
		KEY_7 = 7,
		KEY_8 = 8,
		KEY_9 = 9,
		KEY_PLUS,
		KEY_MINUS,
		KEY_VERB,
		KEY_NOUN,
		KEY_CLEAR,
		KEY_PROCEED,
		KEY_ENTER,
		KEY_RELEASE,
		KEY_RESET,
		KEY_NONE,
		NUM_KEYS

}KeysEnum;


extern void KeyboardSetup (void);
extern void KeyboardTask(void);
extern KeysEnum GetPressedKey(void);
#endif

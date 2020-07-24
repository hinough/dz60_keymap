#include QMK_KEYBOARD_H
#include <print.h>

//Typedef for tapstate
typedef struct {
    bool is_press_action;
    int state;
} tap;

//De forskjellige tapstates
enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD = 2,
    DOUBLE_TAP = 3,
    DOUBLE_HOLD = 4,
    TRIPLE_TAP = 5,
    TRIPLE_HOLD = 6
};

//De forskjellige mulige tap dance triggers
enum {
    ALT_OSL1 = 0,           //Brukes ved Alt TD
    SHT_OSL1 = 1            //Brukes ved Shift TD
};

//Custom keycode for makro
enum custom_keycodes {
    WINSNIP = 51711     //WINSNIP -> WIN + SHIFT + S
};

//Definisjoner på funksjoner
int cur_dance (qk_tap_dance_state_t *state);
void alt_finished (qk_tap_dance_state_t *state, void *user_data);
void alt_reset (qk_tap_dance_state_t *state, void *user_data);

//Keymaps, 1 per layer
//[ID] = LAYOUT_60_iso(keymap)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    //Default, denne er det som er aktivt uten noen FN knapper holdt nede
    [0] = LAYOUT_60_iso(
    KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,
    KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT,
    KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, TD(SHT_OSL1),
    KC_LCTL, KC_LGUI, TD(ALT_OSL1),                            KC_SPC,                             KC_RALT, MO(1),   WINSNIP,   KC_RCTL),

    //FN1, denne aktiveres ved MO(1) (Se over til høyre for KC_RALT eller ved TD(****))
    [1] = LAYOUT_60_iso(
    KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,
    RGB_HUI, RGB_M_P, RGB_M_B, RGB_M_R, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_UP,   XXXXXXX,
    RGB_HUD, KC_VOLD, KC_VOLU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX,
    RGB_SAI, KC_MPLY, KC_MPRV, KC_MNXT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, TD(SHT_OSL1),
    RGB_SAD, XXXXXXX, TD(ALT_OSL1),                            RGB_TOG,                            XXXXXXX, XXXXXXX, RGB_VAD, RGB_VAI)
};

//Denne sjekker om det er single/double/triple tap og evt om det er hold etterpå
int cur_dance (qk_tap_dance_state_t *state) {
    if(state->count == 1) {
        if(state->pressed) return SINGLE_HOLD;  //Trykk og hold knapp
        else return SINGLE_TAP;                 //Tap en knapp 1 gang
    }
    else if(state->count == 2) {
        if(state->pressed) return DOUBLE_HOLD;  //Tap 1 gang deretter trykk og hold 
        else return DOUBLE_TAP;                 //Tap en knapp 2 ganger
    }
    else if(state->count == 3) {
        if(state->pressed) return TRIPLE_HOLD;  //Tap 2 ganger deretter trykk og hold
        else return TRIPLE_TAP;                 //Tap en knapp 3 ganger
    }
    else return 8;                              //Returnerer 8? hvis ingen code, tipper 8 er "ingenting" eller vanlig operation
}

//TAPSTATE for ALT knappen, single/double/triple tap/hold
static tap alttap_state = {
    .is_press_action = true,
    .state = 0
};

//TAPSTATE for SHIFT knappen, single/double/triple tap/hold
static tap sfttap_state = {
    .is_press_action = true,
    .state = 0
};

//Etter at tap er ferdig, leser hva status er og reagerer deretter
void alt_finished(qk_tap_dance_state_t *state, void *user_data) {
    alttap_state.state = cur_dance(state);
    switch (alttap_state.state) {
        case SINGLE_TAP: set_oneshot_layer(1, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break;
        case SINGLE_HOLD: register_code(KC_LALT); break;
        case DOUBLE_TAP: set_oneshot_layer(1, ONESHOT_START); set_oneshot_layer(1, ONESHOT_PRESSED); break;
        case DOUBLE_HOLD: register_code(KC_LALT); layer_on(1); break;
    }
}

//Etter at tap er ferdig, leser hva status er og reagerer deretter
void shift_finished(qk_tap_dance_state_t  *state, void *user_data) {
    sfttap_state.state = cur_dance(state);
    switch (sfttap_state.state) {
        case SINGLE_TAP: set_oneshot_layer(1, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break;
        case SINGLE_HOLD: register_code(KC_RSHIFT); break;
        case DOUBLE_TAP: set_oneshot_layer(1, ONESHOT_START); set_oneshot_layer(1, ONESHOT_PRESSED); break;
        case DOUBLE_HOLD: register_code(KC_RSHIFT); layer_on(1); break;
    }
}

//Etter actions for tapcode er ferdig, basically unregister
void alt_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (alttap_state.state) {
        case SINGLE_TAP: break;
        case SINGLE_HOLD: unregister_code(KC_LALT); break;
        case DOUBLE_TAP: break;
        case DOUBLE_HOLD: layer_off(1); unregister_code(KC_LALT); break;
    }
    alttap_state.state = 0;
}

//Etter actions for tapcode er ferdig, basically unregister
void shift_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (sfttap_state.state) {
        case SINGLE_TAP: break;
        case SINGLE_HOLD: unregister_code(KC_RSHIFT); break;
        case DOUBLE_TAP: break;
        case DOUBLE_HOLD: layer_off(1); unregister_code(KC_RSHIFT); break;
    }
    sfttap_state.state = 0;
}

//Liste over forskjellige oneshotlayersetups
qk_tap_dance_action_t tap_dance_actions[] = {
    [ALT_OSL1]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,alt_finished, alt_reset),
    [SHT_OSL1]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL,shift_finished, shift_reset)
};

//"Macro" med cancel på oneshot layer ved TRNS eller NO press (Knapper uten funksjoner)
bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_TRNS:
        case KC_NO:
            /* Always cancel one-shot layer when another key gets pressed */
            if (record->event.pressed && is_oneshot_layer_active())
                clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
            return true;
        case RESET:
            /* Don't allow reset from oneshot layer state */
            if (record->event.pressed && is_oneshot_layer_active()){
                clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
                return false;
            }	
            return true;
        default:
            return true;
    }
    return true;
}

//Makroer
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    dprintf("\nMACRO: %u pressed\n", keycode);
    switch(keycode) {
        case WINSNIP:                               //Dersom WINSNIP er tapped
            dprint("\nMACRO: Case WINSNIP entered\n");
            if(record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_LSFT("s"))); //Trykker WIN + SHIFT + S
                dprint("\nMACRO: Record Event Pressed\n");
            }
        break;
    }
    dprint("\nMACRO: Returning\n");
    return true;
}

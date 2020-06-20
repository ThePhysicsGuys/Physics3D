#include "core.h"

#include "keyboard.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace Engine {

std::string Key::getName() const {
	return name;
}

int Key::getCode() const {
	return code;
}

// Key <-> Key
bool Key::operator==(const Key& other) const {
	return other.code == code;
}

bool Key::operator!=(const Key& other) const {
	return other.code != code;
}

bool Key::operator>=(const Key& other) const {
	return other.code >= code;
}

bool Key::operator>(const Key& other) const {
	return other.code > code;
}

bool Key::operator<=(const Key& other) const {
	return other.code <= code;
}

bool Key::operator<(const Key& other) const {
	return other.code < code;
}

// Key <-> int
bool Key::operator==(int other) const {
	return other == code;
}

bool Key::operator!=(int other) const {
	return other != code;
}

bool Key::operator>=(int other) const {
	return other >= code;
}

bool Key::operator>(int other) const {
	return other > code;
}

bool Key::operator<=(int other) const {
	return other <= code;
}

bool Key::operator<(int other) const {
	return other < code;
}

// Key <-> string
bool Key::operator==(const std::string& other) const {
	return other == name;
}

bool Key::operator!=(const std::string& other) const {
	return other != name;
}

namespace Keyboard {

	const Key KEY_UNKNOWN         = { "none"           , GLFW_KEY_UNKNOWN       };
	const Key KEY_SPACE           = { "space"          , GLFW_KEY_SPACE         };
	const Key KEY_APOSTROPHE      = { "apostrophe"     , GLFW_KEY_APOSTROPHE    };
	const Key KEY_COMMA           = { "comma"          , GLFW_KEY_COMMA         };
	const Key KEY_MINUS           = { "minus"          , GLFW_KEY_MINUS         };
	const Key KEY_PERIOD          = { "period"         , GLFW_KEY_PERIOD        };
	const Key KEY_SLASH           = { "slash"          , GLFW_KEY_SLASH         };
	const Key KEY_NUMBER_0        = { "number_0"       , GLFW_KEY_0             };
	const Key KEY_NUMBER_1        = { "number_1"       , GLFW_KEY_1             };
	const Key KEY_NUMBER_2        = { "number_2"       , GLFW_KEY_2             };
	const Key KEY_NUMBER_3        = { "number_3"       , GLFW_KEY_3             };
	const Key KEY_NUMBER_4        = { "number_4"       , GLFW_KEY_4             };
	const Key KEY_NUMBER_5        = { "number_5"       , GLFW_KEY_5             };
	const Key KEY_NUMBER_6        = { "number_6"       , GLFW_KEY_6             };
	const Key KEY_NUMBER_7        = { "number_7"       , GLFW_KEY_7             };
	const Key KEY_NUMBER_8        = { "number_8"       , GLFW_KEY_8             };
	const Key KEY_NUMBER_9        = { "number_9"       , GLFW_KEY_9             };
	const Key KEY_SEMICOLON       = { "semicolon"      , GLFW_KEY_SEMICOLON     };
	const Key KEY_EQUAL           = { "equal"          , GLFW_KEY_EQUAL         };
	const Key KEY_A               = { "a"              , GLFW_KEY_A             };
	const Key KEY_B               = { "b"              , GLFW_KEY_B             };
	const Key KEY_C               = { "c"              , GLFW_KEY_C             };
	const Key KEY_D               = { "d"              , GLFW_KEY_D             };
	const Key KEY_E               = { "e"              , GLFW_KEY_E             };
	const Key KEY_F               = { "f"              , GLFW_KEY_F             };
	const Key KEY_G               = { "g"              , GLFW_KEY_G             };
	const Key KEY_H               = { "h"              , GLFW_KEY_H             };
	const Key KEY_I               = { "i"              , GLFW_KEY_I             };
	const Key KEY_J               = { "j"              , GLFW_KEY_J             };
	const Key KEY_K               = { "k"              , GLFW_KEY_K             };
	const Key KEY_L               = { "l"              , GLFW_KEY_L             };
	const Key KEY_M               = { "m"              , GLFW_KEY_M             };
	const Key KEY_N               = { "n"              , GLFW_KEY_N             };
	const Key KEY_O               = { "o"              , GLFW_KEY_O             };
	const Key KEY_P               = { "p"              , GLFW_KEY_P             };
	const Key KEY_Q               = { "q"              , GLFW_KEY_Q             };
	const Key KEY_R               = { "r"              , GLFW_KEY_R             };
	const Key KEY_S               = { "s"              , GLFW_KEY_S             };
	const Key KEY_T               = { "t"              , GLFW_KEY_T             };
	const Key KEY_U               = { "u"              , GLFW_KEY_U             };
	const Key KEY_V               = { "v"              , GLFW_KEY_V             };
	const Key KEY_W               = { "w"              , GLFW_KEY_W             };
	const Key KEY_X               = { "x"              , GLFW_KEY_X             };
	const Key KEY_Y               = { "y"              , GLFW_KEY_Y             };
	const Key KEY_Z               = { "z"              , GLFW_KEY_Z             };
	const Key KEY_LEFT_BRACKET    = { "left_bracket"   , GLFW_KEY_LEFT_BRACKET  };
	const Key KEY_BACKSLASH       = { "backslash"      , GLFW_KEY_BACKSLASH     };
	const Key KEY_RIGHT_BRACKET   = { "right_bracket"  , GLFW_KEY_RIGHT_BRACKET };
	const Key KEY_GRAVE_ACCENT    = { "grave_accent"   , GLFW_KEY_GRAVE_ACCENT  };
	const Key KEY_WORLD_1         = { "world_1"        , GLFW_KEY_WORLD_1       };
	const Key KEY_WORLD_2         = { "world_2"        , GLFW_KEY_WORLD_2       };
	const Key KEY_ESCAPE          = { "escape"         , GLFW_KEY_ESCAPE        };
	const Key KEY_ENTER           = { "enter"          , GLFW_KEY_ENTER         };
	const Key KEY_TAB             = { "tab"            , GLFW_KEY_TAB           };
	const Key KEY_BACKSPACE       = { "backspace"      , GLFW_KEY_BACKSPACE     };
	const Key KEY_INSERT          = { "insert"         , GLFW_KEY_INSERT        };
	const Key KEY_DELETE          = { "delete"         , GLFW_KEY_DELETE        };
	const Key KEY_RIGHT           = { "right"          , GLFW_KEY_RIGHT         };
	const Key KEY_LEFT            = { "left"           , GLFW_KEY_LEFT          };
	const Key KEY_DOWN            = { "down"           , GLFW_KEY_DOWN          };
	const Key KEY_UP              = { "up"             , GLFW_KEY_UP            };
	const Key KEY_PAGE_UP         = { "page_up"        , GLFW_KEY_PAGE_UP       };
	const Key KEY_PAGE_DOWN       = { "page_down"      , GLFW_KEY_PAGE_DOWN     };
	const Key KEY_HOME            = { "home"           , GLFW_KEY_HOME          };
	const Key KEY_END             = { "end"            , GLFW_KEY_END           };
	const Key KEY_CAPS_LOCK       = { "caps_lock"      , GLFW_KEY_CAPS_LOCK     };
	const Key KEY_SCROLL_LOCK     = { "scroll_lock"    , GLFW_KEY_SCROLL_LOCK   };
	const Key KEY_NUM_LOCK        = { "num_lock"       , GLFW_KEY_NUM_LOCK      };
	const Key KEY_PRINT_SCREEN    = { "print_screen"   , GLFW_KEY_PRINT_SCREEN  };
	const Key KEY_PAUSE           = { "pause"          , GLFW_KEY_PAUSE         };
	const Key KEY_F1              = { "f1"             , GLFW_KEY_F1            };
	const Key KEY_F2              = { "f2"             , GLFW_KEY_F2            };
	const Key KEY_F3              = { "f3"             , GLFW_KEY_F3            };
	const Key KEY_F4              = { "f4"             , GLFW_KEY_F4            };
	const Key KEY_F5              = { "f5"             , GLFW_KEY_F5            };
	const Key KEY_F6              = { "f6"             , GLFW_KEY_F6            };
	const Key KEY_F7              = { "f7"             , GLFW_KEY_F7            };
	const Key KEY_F8              = { "f8"             , GLFW_KEY_F8            };
	const Key KEY_F9              = { "f9"             , GLFW_KEY_F9            };
	const Key KEY_F10             = { "f10"            , GLFW_KEY_F10           };
	const Key KEY_F11             = { "f11"            , GLFW_KEY_F11           };
	const Key KEY_F12             = { "f12"            , GLFW_KEY_F12           };
	const Key KEY_F13             = { "f13"            , GLFW_KEY_F13           };
	const Key KEY_F14             = { "f14"            , GLFW_KEY_F14           };
	const Key KEY_F15             = { "f15"            , GLFW_KEY_F15           };
	const Key KEY_F16             = { "f16"            , GLFW_KEY_F16           };
	const Key KEY_F17             = { "f17"            , GLFW_KEY_F17           };
	const Key KEY_F18             = { "f18"            , GLFW_KEY_F18           };
	const Key KEY_F19             = { "f19"            , GLFW_KEY_F19           };
	const Key KEY_F20             = { "f20"            , GLFW_KEY_F20           };
	const Key KEY_F21             = { "f21"            , GLFW_KEY_F21           };
	const Key KEY_F22             = { "f22"            , GLFW_KEY_F22           };
	const Key KEY_F23             = { "f23"            , GLFW_KEY_F23           };
	const Key KEY_F24             = { "f24"            , GLFW_KEY_F24           };
	const Key KEY_F25             = { "f25"            , GLFW_KEY_F25           };
	const Key KEY_NUMPAD_0        = { "numpad_0"       , GLFW_KEY_KP_0          };
	const Key KEY_NUMPAD_1        = { "numpad_1"       , GLFW_KEY_KP_1          };
	const Key KEY_NUMPAD_2        = { "numpad_2"       , GLFW_KEY_KP_2          };
	const Key KEY_NUMPAD_3        = { "numpad_3"       , GLFW_KEY_KP_3          };
	const Key KEY_NUMPAD_4        = { "numpad_4"       , GLFW_KEY_KP_4          };
	const Key KEY_NUMPAD_5        = { "numpad_5"       , GLFW_KEY_KP_5          };
	const Key KEY_NUMPAD_6        = { "numpad_6"       , GLFW_KEY_KP_6          };
	const Key KEY_NUMPAD_7        = { "numpad_7"       , GLFW_KEY_KP_7          };
	const Key KEY_NUMPAD_8        = { "numpad_8"       , GLFW_KEY_KP_8          };
	const Key KEY_NUMPAD_9        = { "numpad_9"       , GLFW_KEY_KP_9          };
	const Key KEY_NUMPAD_DECIMAL  = { "numpad_decimal" , GLFW_KEY_KP_DECIMAL    };
	const Key KEY_NUMPAD_DIVIDE   = { "numpad_divide"  , GLFW_KEY_KP_DIVIDE     };
	const Key KEY_NUMPAD_MULTIPLY = { "numpad_multiply", GLFW_KEY_KP_MULTIPLY   };
	const Key KEY_NUMPAD_SUBTRACT = { "numpad_subtract", GLFW_KEY_KP_SUBTRACT   };
	const Key KEY_NUMPAD_ADD      = { "numpad_add"     , GLFW_KEY_KP_ADD        };
	const Key KEY_NUMPAD_ENTER    = { "numpad_enter"   , GLFW_KEY_KP_ENTER      };
	const Key KEY_NUMPAD_EQUAL    = { "numpad_equal"   , GLFW_KEY_KP_EQUAL      };
	const Key KEY_LEFT_SHIFT      = { "left_shift"     , GLFW_KEY_LEFT_SHIFT    };
	const Key KEY_LEFT_CONTROL    = { "left_control"   , GLFW_KEY_LEFT_CONTROL  };
	const Key KEY_LEFT_ALT        = { "left_alt"       , GLFW_KEY_LEFT_ALT      };
	const Key KEY_LEFT_SUPER      = { "left_super"     , GLFW_KEY_LEFT_SUPER    };
	const Key KEY_RIGHT_SHIFT     = { "rigth_shift"    , GLFW_KEY_RIGHT_SHIFT   };
	const Key KEY_RIGHT_CONTROL   = { "right_control"  , GLFW_KEY_RIGHT_CONTROL };
	const Key KEY_RIGHT_ALT       = { "right_alt"      , GLFW_KEY_RIGHT_ALT     };
	const Key KEY_RIGHT_SUPER     = { "right_super"    , GLFW_KEY_RIGHT_SUPER   };
	const Key KEY_MENU            = { "menu"           , GLFW_KEY_MENU          };

	const int KEY_FIRST       = GLFW_KEY_SPACE;
	const int KEY_LAST        = GLFW_KEY_LAST;

	const int KEY_PRESS       = GLFW_PRESS;
	const int KEY_RELEASE     = GLFW_RELEASE;
	const int KEY_REPEAT      = GLFW_REPEAT;

	std::map<int, Key> keymap = {
		{ KEY_UNKNOWN.getCode()        , KEY_UNKNOWN         },
		{ KEY_SPACE.getCode()          , KEY_SPACE           },
		{ KEY_APOSTROPHE.getCode()     , KEY_APOSTROPHE      },
		{ KEY_COMMA.getCode()          , KEY_COMMA           },
		{ KEY_MINUS.getCode()          , KEY_MINUS           },
		{ KEY_PERIOD.getCode()         , KEY_PERIOD          },
		{ KEY_SLASH.getCode()          , KEY_SLASH           },
		{ KEY_NUMBER_0.getCode()       , KEY_NUMBER_0        },
		{ KEY_NUMBER_1.getCode()       , KEY_NUMBER_1        },
		{ KEY_NUMBER_2.getCode()       , KEY_NUMBER_2        },
		{ KEY_NUMBER_3.getCode()       , KEY_NUMBER_3        },
		{ KEY_NUMBER_4.getCode()       , KEY_NUMBER_4        },
		{ KEY_NUMBER_5.getCode()       , KEY_NUMBER_5        },
		{ KEY_NUMBER_6.getCode()       , KEY_NUMBER_6        },
		{ KEY_NUMBER_7.getCode()       , KEY_NUMBER_7        },
		{ KEY_NUMBER_8.getCode()       , KEY_NUMBER_8        },
		{ KEY_NUMBER_9.getCode()       , KEY_NUMBER_9        },
		{ KEY_SEMICOLON.getCode()      , KEY_SEMICOLON       },
		{ KEY_A.getCode()              , KEY_A               },
		{ KEY_B.getCode()              , KEY_B               },
		{ KEY_C.getCode()              , KEY_C               },
		{ KEY_D.getCode()              , KEY_D               },
		{ KEY_E.getCode()              , KEY_E               },
		{ KEY_F.getCode()              , KEY_F               },
		{ KEY_G.getCode()              , KEY_G               },
		{ KEY_H.getCode()              , KEY_H               },
		{ KEY_I.getCode()              , KEY_I               },
		{ KEY_J.getCode()              , KEY_J               },
		{ KEY_K.getCode()              , KEY_K               },
		{ KEY_L.getCode()              , KEY_L               },
		{ KEY_M.getCode()              , KEY_M               },
		{ KEY_N.getCode()              , KEY_N               },
		{ KEY_O.getCode()              , KEY_O               },
		{ KEY_P.getCode()              , KEY_P               },
		{ KEY_Q.getCode()              , KEY_Q               },
		{ KEY_R.getCode()              , KEY_R               },
		{ KEY_S.getCode()              , KEY_S               },
		{ KEY_T.getCode()              , KEY_T               },
		{ KEY_U.getCode()              , KEY_U               },
		{ KEY_V.getCode()              , KEY_V               },
		{ KEY_W.getCode()              , KEY_W               },
		{ KEY_X.getCode()              , KEY_X               },
		{ KEY_Y.getCode()              , KEY_Y               },
		{ KEY_Z.getCode()              , KEY_Z               },
		{ KEY_LEFT_BRACKET.getCode()   , KEY_LEFT_BRACKET    },
		{ KEY_BACKSLASH.getCode()      , KEY_BACKSLASH       },
		{ KEY_RIGHT_BRACKET.getCode()  , KEY_RIGHT_BRACKET   },
		{ KEY_GRAVE_ACCENT.getCode()   , KEY_GRAVE_ACCENT    },
		{ KEY_WORLD_1.getCode()        , KEY_WORLD_1         },
		{ KEY_WORLD_2.getCode()        , KEY_WORLD_2         },
		{ KEY_ESCAPE.getCode()         , KEY_ESCAPE          },
		{ KEY_ENTER.getCode()          , KEY_ENTER           },
		{ KEY_TAB.getCode()            , KEY_TAB             },
		{ KEY_BACKSPACE.getCode()      , KEY_BACKSPACE       },
		{ KEY_INSERT.getCode()         , KEY_INSERT          },
		{ KEY_DELETE.getCode()         , KEY_DELETE          },
		{ KEY_RIGHT.getCode()          , KEY_RIGHT           },
		{ KEY_LEFT.getCode()           , KEY_LEFT            },
		{ KEY_DOWN.getCode()           , KEY_DOWN            },
		{ KEY_UP.getCode()             , KEY_UP              },
		{ KEY_PAGE_UP.getCode()        , KEY_PAGE_UP         },
		{ KEY_PAGE_DOWN.getCode()      , KEY_PAGE_DOWN       },
		{ KEY_HOME.getCode()           , KEY_HOME            },
		{ KEY_END.getCode()            , KEY_END             },
		{ KEY_CAPS_LOCK.getCode()      , KEY_CAPS_LOCK       },
		{ KEY_SCROLL_LOCK.getCode()    , KEY_SCROLL_LOCK     },
		{ KEY_NUM_LOCK.getCode()       , KEY_NUM_LOCK        },
		{ KEY_PRINT_SCREEN.getCode()   , KEY_PRINT_SCREEN    },
		{ KEY_PAUSE.getCode()          , KEY_PAUSE           },
		{ KEY_F1.getCode()             , KEY_F1              },
		{ KEY_F2.getCode()             , KEY_F2              },
		{ KEY_F3.getCode()             , KEY_F3              },
		{ KEY_F4.getCode()             , KEY_F4              },
		{ KEY_F5.getCode()             , KEY_F5              },
		{ KEY_F6.getCode()             , KEY_F6              },
		{ KEY_F7.getCode()             , KEY_F7              },
		{ KEY_F8.getCode()             , KEY_F8              },
		{ KEY_F9.getCode()             , KEY_F9              },
		{ KEY_F10.getCode()            , KEY_F10             },
		{ KEY_F11.getCode()            , KEY_F11             },
		{ KEY_F12.getCode()            , KEY_F12             },
		{ KEY_F13.getCode()            , KEY_F13             },
		{ KEY_F14.getCode()            , KEY_F14             },
		{ KEY_F15.getCode()            , KEY_F15             },
		{ KEY_F16.getCode()            , KEY_F16             },
		{ KEY_F17.getCode()            , KEY_F17             },
		{ KEY_F18.getCode()            , KEY_F18             },
		{ KEY_F19.getCode()            , KEY_F19             },
		{ KEY_F20.getCode()            , KEY_F20             },
		{ KEY_F21.getCode()            , KEY_F21             },
		{ KEY_F22.getCode()            , KEY_F22             },
		{ KEY_F23.getCode()            , KEY_F23             },
		{ KEY_F24.getCode()            , KEY_F24             },
		{ KEY_F25.getCode()            , KEY_F25             },
		{ KEY_NUMPAD_0.getCode()       , KEY_NUMPAD_0        },
		{ KEY_NUMPAD_1.getCode()       , KEY_NUMPAD_1        },
		{ KEY_NUMPAD_2.getCode()       , KEY_NUMPAD_2        },
		{ KEY_NUMPAD_3.getCode()       , KEY_NUMPAD_3        },
		{ KEY_NUMPAD_4.getCode()       , KEY_NUMPAD_4        },
		{ KEY_NUMPAD_5.getCode()       , KEY_NUMPAD_5        },
		{ KEY_NUMPAD_6.getCode()       , KEY_NUMPAD_6        },
		{ KEY_NUMPAD_7.getCode()       , KEY_NUMPAD_7        },
		{ KEY_NUMPAD_8.getCode()       , KEY_NUMPAD_8        },
		{ KEY_NUMPAD_9.getCode()       , KEY_NUMPAD_9        },
		{ KEY_NUMPAD_DECIMAL.getCode() , KEY_NUMPAD_DECIMAL  },
		{ KEY_NUMPAD_DIVIDE.getCode()  , KEY_NUMPAD_DIVIDE   },
		{ KEY_NUMPAD_MULTIPLY.getCode(), KEY_NUMPAD_MULTIPLY },
		{ KEY_NUMPAD_SUBTRACT.getCode(), KEY_NUMPAD_SUBTRACT },
		{ KEY_NUMPAD_ADD.getCode()     , KEY_NUMPAD_ADD      },
		{ KEY_NUMPAD_ENTER.getCode()   , KEY_NUMPAD_ENTER    },
		{ KEY_NUMPAD_EQUAL.getCode()   , KEY_NUMPAD_EQUAL    },
		{ KEY_LEFT_SHIFT.getCode()     , KEY_LEFT_SHIFT      },
		{ KEY_LEFT_CONTROL.getCode()   , KEY_LEFT_CONTROL    },
		{ KEY_LEFT_ALT.getCode()       , KEY_NUMPAD_SUBTRACT },
		{ KEY_LEFT_SUPER.getCode()     , KEY_NUMPAD_SUBTRACT },
		{ KEY_RIGHT_SHIFT.getCode()    , KEY_NUMPAD_SUBTRACT },
		{ KEY_RIGHT_CONTROL.getCode()  , KEY_NUMPAD_SUBTRACT },
		{ KEY_RIGHT_ALT.getCode()      , KEY_NUMPAD_SUBTRACT },
		{ KEY_RIGHT_SUPER.getCode()    , KEY_NUMPAD_SUBTRACT },
		{ KEY_MENU.getCode()           , KEY_MENU            },
	};

	Key getKey(const std::string& name) {
		for (auto key : keymap) {
			if (key.second.getName() == name)
				return key.second;
		}

		return KEY_UNKNOWN;
	}

	Key getKey(int code) {
		try {
			Key& key = keymap.at(code);
			return key;
		} catch (const std::out_of_range& e) {
			return KEY_UNKNOWN;
		}
	}

}

};
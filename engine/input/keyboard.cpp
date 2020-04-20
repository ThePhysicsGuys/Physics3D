#include "core.h"

#include "keyboard.h"

#include <stdexcept>

#include "GLFW\glfw3.h"

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

	const Key UNKNOWN         = { "none"           , GLFW_KEY_UNKNOWN       };
	const Key SPACE           = { "space"          , GLFW_KEY_SPACE         };
	const Key APOSTROPHE      = { "apostrophe"     , GLFW_KEY_APOSTROPHE    };
	const Key COMMA           = { "comma"          , GLFW_KEY_COMMA         };
	const Key MINUS           = { "minus"          , GLFW_KEY_MINUS         };
	const Key PERIOD          = { "period"         , GLFW_KEY_PERIOD        };
	const Key SLASH           = { "slash"          , GLFW_KEY_SLASH         };
	const Key NUMBER_0        = { "number_0"       , GLFW_KEY_0             };
	const Key NUMBER_1        = { "number_1"       , GLFW_KEY_1             };
	const Key NUMBER_2        = { "number_2"       , GLFW_KEY_2             };
	const Key NUMBER_3        = { "number_3"       , GLFW_KEY_3             };
	const Key NUMBER_4        = { "number_4"       , GLFW_KEY_4             };
	const Key NUMBER_5        = { "number_5"       , GLFW_KEY_5             };
	const Key NUMBER_6        = { "number_6"       , GLFW_KEY_6             };
	const Key NUMBER_7        = { "number_7"       , GLFW_KEY_7             };
	const Key NUMBER_8        = { "number_8"       , GLFW_KEY_8             };
	const Key NUMBER_9        = { "number_9"       , GLFW_KEY_9             };
	const Key SEMICOLON       = { "semicolon"      , GLFW_KEY_SEMICOLON     };
	const Key EQUAL           = { "equal"          , GLFW_KEY_EQUAL         };
	const Key A               = { "a"              , GLFW_KEY_A             };
	const Key B               = { "b"              , GLFW_KEY_B             };
	const Key C               = { "c"              , GLFW_KEY_C             };
	const Key D               = { "d"              , GLFW_KEY_D             };
	const Key E               = { "e"              , GLFW_KEY_E             };
	const Key F               = { "f"              , GLFW_KEY_F             };
	const Key G               = { "g"              , GLFW_KEY_G             };
	const Key H               = { "h"              , GLFW_KEY_H             };
	const Key I               = { "i"              , GLFW_KEY_I             };
	const Key J               = { "j"              , GLFW_KEY_J             };
	const Key K               = { "k"              , GLFW_KEY_K             };
	const Key L               = { "l"              , GLFW_KEY_L             };
	const Key M               = { "m"              , GLFW_KEY_M             };
	const Key N               = { "n"              , GLFW_KEY_N             };
	const Key O               = { "o"              , GLFW_KEY_O             };
	const Key P               = { "p"              , GLFW_KEY_P             };
	const Key Q               = { "q"              , GLFW_KEY_Q             };
	const Key R               = { "r"              , GLFW_KEY_R             };
	const Key S               = { "s"              , GLFW_KEY_S             };
	const Key T               = { "t"              , GLFW_KEY_T             };
	const Key U               = { "u"              , GLFW_KEY_U             };
	const Key V               = { "v"              , GLFW_KEY_V             };
	const Key W               = { "w"              , GLFW_KEY_W             };
	const Key X               = { "x"              , GLFW_KEY_X             };
	const Key Y               = { "y"              , GLFW_KEY_Y             };
	const Key Z               = { "z"              , GLFW_KEY_Z             };
	const Key LEFT_BRACKET    = { "left_bracket"   , GLFW_KEY_LEFT_BRACKET  };
	const Key BACKSLASH       = { "backslash"      , GLFW_KEY_BACKSLASH     };
	const Key RIGHT_BRACKET   = { "right_bracket"  , GLFW_KEY_RIGHT_BRACKET };
	const Key GRAVE_ACCENT    = { "grave_accent"   , GLFW_KEY_GRAVE_ACCENT  };
	const Key WORLD_1         = { "world_1"        , GLFW_KEY_WORLD_1       };
	const Key WORLD_2         = { "world_2"        , GLFW_KEY_WORLD_2       };
	const Key ESCAPE          = { "escape"         , GLFW_KEY_ESCAPE        };
	const Key ENTER           = { "enter"          , GLFW_KEY_ENTER         };
	const Key TAB             = { "tab"            , GLFW_KEY_TAB           };
	const Key BACKSPACE       = { "backspace"      , GLFW_KEY_BACKSPACE     };
	const Key INSERT          = { "insert"         , GLFW_KEY_INSERT        };
	const Key DELETE          = { "delete"         , GLFW_KEY_DELETE        };
	const Key RIGHT           = { "right"          , GLFW_KEY_RIGHT         };
	const Key LEFT            = { "left"           , GLFW_KEY_LEFT          };
	const Key DOWN            = { "down"           , GLFW_KEY_DOWN          };
	const Key UP              = { "up"             , GLFW_KEY_UP            };
	const Key PAGE_UP         = { "page_up"        , GLFW_KEY_PAGE_UP       };
	const Key PAGE_DOWN       = { "page_down"      , GLFW_KEY_PAGE_DOWN     };
	const Key HOME            = { "home"           , GLFW_KEY_HOME          };
	const Key END             = { "end"            , GLFW_KEY_END           };
	const Key CAPS_LOCK       = { "caps_lock"      , GLFW_KEY_CAPS_LOCK     };
	const Key SCROLL_LOCK     = { "scroll_lock"    , GLFW_KEY_SCROLL_LOCK   };
	const Key NUM_LOCK        = { "num_lock"       , GLFW_KEY_NUM_LOCK      };
	const Key PRINT_SCREEN    = { "print_screen"   , GLFW_KEY_PRINT_SCREEN  };
	const Key PAUSE           = { "pause"          , GLFW_KEY_PAUSE         };
	const Key F1              = { "f1"             , GLFW_KEY_F1            };
	const Key F2              = { "f2"             , GLFW_KEY_F2            };
	const Key F3              = { "f3"             , GLFW_KEY_F3            };
	const Key F4              = { "f4"             , GLFW_KEY_F4            };
	const Key F5              = { "f5"             , GLFW_KEY_F5            };
	const Key F6              = { "f6"             , GLFW_KEY_F6            };
	const Key F7              = { "f7"             , GLFW_KEY_F7            };
	const Key F8              = { "f8"             , GLFW_KEY_F8            };
	const Key F9              = { "f9"             , GLFW_KEY_F9            };
	const Key F10             = { "f10"            , GLFW_KEY_F10           };
	const Key F11             = { "f11"            , GLFW_KEY_F11           };
	const Key F12             = { "f12"            , GLFW_KEY_F12           };
	const Key F13             = { "f13"            , GLFW_KEY_F13           };
	const Key F14             = { "f14"            , GLFW_KEY_F14           };
	const Key F15             = { "f15"            , GLFW_KEY_F15           };
	const Key F16             = { "f16"            , GLFW_KEY_F16           };
	const Key F17             = { "f17"            , GLFW_KEY_F17           };
	const Key F18             = { "f18"            , GLFW_KEY_F18           };
	const Key F19             = { "f19"            , GLFW_KEY_F19           };
	const Key F20             = { "f20"            , GLFW_KEY_F20           };
	const Key F21             = { "f21"            , GLFW_KEY_F21           };
	const Key F22             = { "f22"            , GLFW_KEY_F22           };
	const Key F23             = { "f23"            , GLFW_KEY_F23           };
	const Key F24             = { "f24"            , GLFW_KEY_F24           };
	const Key F25             = { "f25"            , GLFW_KEY_F25           };
	const Key NUMPAD_0        = { "numpad_0"       , GLFW_KEY_KP_0          };
	const Key NUMPAD_1        = { "numpad_1"       , GLFW_KEY_KP_1          };
	const Key NUMPAD_2        = { "numpad_2"       , GLFW_KEY_KP_2          };
	const Key NUMPAD_3        = { "numpad_3"       , GLFW_KEY_KP_3          };
	const Key NUMPAD_4        = { "numpad_4"       , GLFW_KEY_KP_4          };
	const Key NUMPAD_5        = { "numpad_5"       , GLFW_KEY_KP_5          };
	const Key NUMPAD_6        = { "numpad_6"       , GLFW_KEY_KP_6          };
	const Key NUMPAD_7        = { "numpad_7"       , GLFW_KEY_KP_7          };
	const Key NUMPAD_8        = { "numpad_8"       , GLFW_KEY_KP_8          };
	const Key NUMPAD_9        = { "numpad_9"       , GLFW_KEY_KP_9          };
	const Key NUMPAD_DECIMAL  = { "numpad_decimal" , GLFW_KEY_KP_DECIMAL    };
	const Key NUMPAD_DIVIDE   = { "numpad_divide"  , GLFW_KEY_KP_DIVIDE     };
	const Key NUMPAD_MULTIPLY = { "numpad_multiply", GLFW_KEY_KP_MULTIPLY   };
	const Key NUMPAD_SUBTRACT = { "numpad_subtract", GLFW_KEY_KP_SUBTRACT   };
	const Key NUMPAD_ADD      = { "numpad_add"     , GLFW_KEY_KP_ADD        };
	const Key NUMPAD_ENTER    = { "numpad_enter"   , GLFW_KEY_KP_ENTER      };
	const Key NUMPAD_EQUAL    = { "numpad_equal"   , GLFW_KEY_KP_EQUAL      };
	const Key LEFT_SHIFT      = { "left_shift"     , GLFW_KEY_LEFT_SHIFT    };
	const Key LEFT_CONTROL    = { "left_control"   , GLFW_KEY_LEFT_CONTROL  };
	const Key LEFT_ALT        = { "left_alt"       , GLFW_KEY_LEFT_ALT      };
	const Key LEFT_SUPER      = { "left_super"     , GLFW_KEY_LEFT_SUPER    };
	const Key RIGHT_SHIFT     = { "rigth_shift"    , GLFW_KEY_RIGHT_SHIFT   };
	const Key RIGHT_CONTROL   = { "right_control"  , GLFW_KEY_RIGHT_CONTROL };
	const Key RIGHT_ALT       = { "right_alt"      , GLFW_KEY_RIGHT_ALT     };
	const Key RIGHT_SUPER     = { "right_super"    , GLFW_KEY_RIGHT_SUPER   };
	const Key MENU            = { "menu"           , GLFW_KEY_MENU          };

	const int KEY_FIRST       = GLFW_KEY_SPACE;
	const int KEY_LAST        = GLFW_KEY_LAST;

	const int KEY_PRESS       = GLFW_PRESS;
	const int KEY_RELEASE     = GLFW_RELEASE;
	const int KEY_REPEAT      = GLFW_REPEAT;

	std::map<int, Key> keymap = {
		{ UNKNOWN.getCode()        , UNKNOWN         },
		{ SPACE.getCode()          , SPACE           },
		{ APOSTROPHE.getCode()     , APOSTROPHE      },
		{ COMMA.getCode()          , COMMA           },
		{ MINUS.getCode()          , MINUS           },
		{ PERIOD.getCode()         , PERIOD          },
		{ SLASH.getCode()          , SLASH           },
		{ NUMBER_0.getCode()       , NUMBER_0        },
		{ NUMBER_1.getCode()       , NUMBER_1        },
		{ NUMBER_2.getCode()       , NUMBER_2        },
		{ NUMBER_3.getCode()       , NUMBER_3        },
		{ NUMBER_4.getCode()       , NUMBER_4        },
		{ NUMBER_5.getCode()       , NUMBER_5        },
		{ NUMBER_6.getCode()       , NUMBER_6        },
		{ NUMBER_7.getCode()       , NUMBER_7        },
		{ NUMBER_8.getCode()       , NUMBER_8        },
		{ NUMBER_9.getCode()       , NUMBER_9        },
		{ SEMICOLON.getCode()      , SEMICOLON       },
		{ A.getCode()              , A               },
		{ B.getCode()              , B               },
		{ C.getCode()              , C               },
		{ D.getCode()              , D               },
		{ E.getCode()              , E               },
		{ F.getCode()              , F               },
		{ G.getCode()              , G               },
		{ H.getCode()              , H               },
		{ I.getCode()              , I               },
		{ J.getCode()              , J               },
		{ K.getCode()              , K               },
		{ L.getCode()              , L               },
		{ M.getCode()              , M               },
		{ N.getCode()              , N               },
		{ O.getCode()              , O               },
		{ P.getCode()              , P               },
		{ Q.getCode()              , Q               },
		{ R.getCode()              , R               },
		{ S.getCode()              , S               },
		{ T.getCode()              , T               },
		{ U.getCode()              , U               },
		{ V.getCode()              , V               },
		{ W.getCode()              , W               },
		{ X.getCode()              , X               },
		{ Y.getCode()              , Y               },
		{ Z.getCode()              , Z               },
		{ LEFT_BRACKET.getCode()   , LEFT_BRACKET    },
		{ BACKSLASH.getCode()      , BACKSLASH       },
		{ RIGHT_BRACKET.getCode()  , RIGHT_BRACKET   },
		{ GRAVE_ACCENT.getCode()   , GRAVE_ACCENT    },
		{ WORLD_1.getCode()        , WORLD_1         },
		{ WORLD_2.getCode()        , WORLD_2         },
		{ ESCAPE.getCode()         , ESCAPE          },
		{ ENTER.getCode()          , ENTER           },
		{ TAB.getCode()            , TAB             },
		{ BACKSPACE.getCode()      , BACKSPACE       },
		{ INSERT.getCode()         , INSERT          },
		{ DELETE.getCode()         , DELETE          },
		{ RIGHT.getCode()          , RIGHT           },
		{ LEFT.getCode()           , LEFT            },
		{ DOWN.getCode()           , DOWN            },
		{ UP.getCode()             , UP              },
		{ PAGE_UP.getCode()        , PAGE_UP         },
		{ PAGE_DOWN.getCode()      , PAGE_DOWN       },
		{ HOME.getCode()           , HOME            },
		{ END.getCode()            , END             },
		{ CAPS_LOCK.getCode()      , CAPS_LOCK       },
		{ SCROLL_LOCK.getCode()    , SCROLL_LOCK     },
		{ NUM_LOCK.getCode()       , NUM_LOCK        },
		{ PRINT_SCREEN.getCode()   , PRINT_SCREEN    },
		{ PAUSE.getCode()          , PAUSE           },
		{ F1.getCode()             , F1              },
		{ F2.getCode()             , F2              },
		{ F3.getCode()             , F3              },
		{ F4.getCode()             , F4              },
		{ F5.getCode()             , F5              },
		{ F6.getCode()             , F6              },
		{ F7.getCode()             , F7              },
		{ F8.getCode()             , F8              },
		{ F9.getCode()             , F9              },
		{ F10.getCode()            , F10             },
		{ F11.getCode()            , F11             },
		{ F12.getCode()            , F12             },
		{ F13.getCode()            , F13             },
		{ F14.getCode()            , F14             },
		{ F15.getCode()            , F15             },
		{ F16.getCode()            , F16             },
		{ F17.getCode()            , F17             },
		{ F18.getCode()            , F18             },
		{ F19.getCode()            , F19             },
		{ F20.getCode()            , F20             },
		{ F21.getCode()            , F21             },
		{ F22.getCode()            , F22             },
		{ F23.getCode()            , F23             },
		{ F24.getCode()            , F24             },
		{ F25.getCode()            , F25             },
		{ NUMPAD_0.getCode()       , NUMPAD_0        },
		{ NUMPAD_1.getCode()       , NUMPAD_1        },
		{ NUMPAD_2.getCode()       , NUMPAD_2        },
		{ NUMPAD_3.getCode()       , NUMPAD_3        },
		{ NUMPAD_4.getCode()       , NUMPAD_4        },
		{ NUMPAD_5.getCode()       , NUMPAD_5        },
		{ NUMPAD_6.getCode()       , NUMPAD_6        },
		{ NUMPAD_7.getCode()       , NUMPAD_7        },
		{ NUMPAD_8.getCode()       , NUMPAD_8        },
		{ NUMPAD_9.getCode()       , NUMPAD_9        },
		{ NUMPAD_DECIMAL.getCode() , NUMPAD_DECIMAL  },
		{ NUMPAD_DIVIDE.getCode()  , NUMPAD_DIVIDE   },
		{ NUMPAD_MULTIPLY.getCode(), NUMPAD_MULTIPLY },
		{ NUMPAD_SUBTRACT.getCode(), NUMPAD_SUBTRACT },
		{ NUMPAD_ADD.getCode()     , NUMPAD_ADD      },
		{ NUMPAD_ENTER.getCode()   , NUMPAD_ENTER    },
		{ NUMPAD_EQUAL.getCode()   , NUMPAD_EQUAL    },
		{ LEFT_SHIFT.getCode()     , LEFT_SHIFT      },
		{ LEFT_CONTROL.getCode()   , LEFT_CONTROL    },
		{ LEFT_ALT.getCode()       , NUMPAD_SUBTRACT },
		{ LEFT_SUPER.getCode()     , NUMPAD_SUBTRACT },
		{ RIGHT_SHIFT.getCode()    , NUMPAD_SUBTRACT },
		{ RIGHT_CONTROL.getCode()  , NUMPAD_SUBTRACT },
		{ RIGHT_ALT.getCode()      , NUMPAD_SUBTRACT },
		{ RIGHT_SUPER.getCode()    , NUMPAD_SUBTRACT },
		{ MENU.getCode()           , MENU            },
	};

	Key getKey(const std::string& name) {
		for (auto key : keymap) {
			if (key.second.getName() == name)
				return key.second;
		}

		return UNKNOWN;
	}

	Key getKey(int code) {
		try {
			Key& key = keymap.at(code);
			return key;
		} catch (std::out_of_range& const e) {
			return UNKNOWN;
		}
	}

}

};
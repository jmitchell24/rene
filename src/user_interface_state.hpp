//
// Created by james on 13/06/25.
//

#pragma once

#define EXPAND_STATES(_state) \
    _var(StateEditing, EDITING) \
    _var(StateArming, ARMING) \
    _var(StateError, ERROR) \
    _var(VarFuzz, FUZZ)

#define STATE_ENUM(_x, _y) ,_y
#define STATE_VARIANT(_x, _y) ,_x

namespace rene
{

}
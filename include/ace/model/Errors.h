/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef ACE_MODEL_ERRORS_H_
#define ACE_MODEL_ERRORS_H_

// Arity

#define ERR_ARITY_INVALID_TYPE                "Arity attribute must be a string"

// Attribute

#define ERR_UNSUPPORTED_VALUE(_v)             "Unsupported value \"", _v, "\"" // NOLINT
#define ERR_INVALID_VALUE(_v)                 "Invalid value for \"", _v, "\"" // NOLINT
#define ERR_MULTIPLE_VALUES_NOT_ALLOWED(_v)   "Multiple values for \"", _v, "\" not allowed" // NOLINT

// Attribute set

#define ERR_WRONG_ATTRIBUTE_SET_TYPE          "attribute set must be an object"

// Basic type

#define ERR_EMPTY_DOC                         "Documentation one-liner cannot be empty" // NOLINT
#define ERR_TYPE_NOT_KNOWN(_t)                "Unknown type ", "\"", _t, "\"" // NOLINT
#define ERR_UNEXPECTED_TYPE(_t)               "Unexpected type ", "\"", _t, "\"" // NOLINT
#define ERR_MISSING_ARITY                     "Missing arity" // NOLINT
#define ERR_INVALID_ARITY(_a)                 "Invalid arity ", "\"", _a, "\"" // NOLINT
#define ERR_INVALID_ARITY_FOR_TYPE(_a, _t)    "Invalid arity \"", _a, "\" for type \"", _t, "\"" // NOLINT
#define ERR_WRONG_INHERIT_TYPE                "Attribute \"inherit\" must be a boolean" // NOLINT
#define ERR_WRONG_ARITY_COUNT                 "Too many values for specified arity" // NOLINT
#define ERR_CONFLICTING_EXT_KIND              "Conflicting kind in extended type" // NOLINT
#define ERR_CONFLICTING_EXT_ARITY             "Conflicting arity in extended type" // NOLINT
#define ERR_CONFLICTING_EXT_DEFAULT           "Conflicting default in extended type" // NOLINT
#define ERR_CONFLICTING_EXT_INHERIT           "Conflicting inheritance in extended type" // NOLINT
#define ERR_CONFLICTING_EXT_ATTR              "Conflicting attributes in extended type" // NOLINT
#define ERR_INVALID_ATTRIBUTE(_a)             "Invalid attribute \"", _a, "\"" // NOLINT
#define ERR_INVALID_ATTRIBUTE_VALUE(_a)       "Invalid value for attribute \"", _a, "\"" // NOLINT
#define ERR_MISSING_ATTRIBUTE(_a)             "Missing attribute \"", _a, "\"" // NOLINT
#define ERR_CONFLICTING_ATTRIBUTE(_a, _b)     "Conflicting attributes \"", _a, "\" and \"", _b, "\"" // NOLINT
#define ERR_CHECK_ATTR_FAILED(_a)             "Check failed for attribute \"", _a, "\"" // NOLINT
#define ERR_INVALID_HOOK_FORMAT(_h)           "Invalid hook format \"", _h, "\""  // NOLINT
#define ERR_INVALID_HOOK_SOURCE(_h)           "Invalid hook source \"", _h, "\""  // NOLINT
#define ERR_NO_HOOKED_VALUE_IN_INSTANCE(_h)   "Hook source \"", _h, "\" has no value"  // NOLINT
#define ERR_NO_HOOKED_VALUE_MATCH(_e)         "No hooked value match for value \"", _e, "\"" // NOLINT
#define ERR_NO_HOOKED_VALUE_MATCH(_e)         "No hooked value match for value \"", _e, "\"" // NOLINT
#define ERR_HOOKED_VALUE_ARITY_MISMATCH(_h)   "Required type cannot be hooked on optional type \"", _h, "\""  // NOLINT
#define ERR_HOOK_GENERATIVE_PATH              "Hook path cannot be generative"  // NOLINT
#define ERR_HOOKED_VALUE_NOT_ENUMERATED       "Hook source must be enumerated or mapped"
#define ERR_CANNOT_CONSTRAIN                  "Type cannot be constrained"

// Body

#define ERR_BODY_NOT_AN_OBJECT                "Element \"body\" must be an object" // NOLINT
#define ERR_INCOMPATIBLE_OPTION(_o, _a, _b)   "Incompatible option \"", _o, "\" in models \"", _a, "\" and \"", _b, "\"" // NOLINT
#define ERR_MISSING_REQUIRED(_o)              "Missing required option \"", _o, "\"" // NOLINT
#define ERR_PATH_ELEM_NOT_FOUND(_o)           "Path element \"", _o, "\" not found in model" // NOLINT
#define ERR_PATH_ELEM_NOT_CLASS(_o)           "Path element \"", _o, "\" is not a class" // NOLINT
#define ERR_FAILED_VALIDATING_TYPE_MODEL(_t)  "Model validation failed for type \"", _t, "\"" // NOLINT
#define ERR_EMPTY_HOOK_MATCH(_e, _v)          "Attribute \"hook\" \"", _e, "\" produces an empty match for value \"", _v, "\"" // NOLINT
#define ERR_EMPTY_HOOK_MATCHSET(_e)           "Attribute \"hook\" \"", _e, "\" produces an empty match set"  // NOLINT
#define ERR_INVALID_HOOK_EXP_COUNT(_e)        "Invalid expansion count in \"hook\" type \"", _e, "\""  // NOLINT
#define ERR_FAILED_CHECKING_INSTANCE(_t)      "Check instance failed for type \"", _t, "\"" // NOLINT
#define ERR_FAILED_RESOLVING_INSTANCE(_t)     "Resolve instance failed for type \"", _t, "\"" // NOLINT
#define ERR_FAILED_FLATTENING_INSTANCE(_t)    "Flatten instance failed for type \"", _t, "\"" // NOLINT
#define ERR_FAILED_DISABLE_TYPE(_t)           "Failed to disable option \"", _t, "\" (possible dependency conflict)" // NOLINT
#define ERR_USING_DISABLED_TYPE(_t)           "Option \"", _t, "\" is disabled in the configuration and has been removed" // NOLINT

// Class

#define ERR_MODEL_NOT_FOUND(_m)               "Model \"", _m, "\" not found" // NOLINT
#define ERR_INVALID_MODEL(_m)                 "Invalid model \"", _m, "\"" // NOLINT
#define ERR_CLASS_NOT_AN_OBJECT               "Attribute \"class\" must be an object" // NOLINT
#define ERR_NOT_A_BASE_CLASS_OF(_a, _b)       "Model \"", _a, "\" is not a base class of \"", _b, "\", or vice-versa" // NOLINT

// CPUID

#define ERR_UNAVAILABLE_CPUID(_d)             "CPU id \"", _d, "\" not available on this platform"  // NOLINT
#define ERR_INVALID_DEFAULT_CPUID(_d)         "Default CPU id \"", _d, "\" not supported" // NOLINT
#define ERR_INVALID_EITHER_CPUID(_d)          "Allowed CPU id \"", _d, "\" not supported" // NOLINT


// Dependency attribute

#define ERR_DEPS_NOT_AN_ARRAY                 "Deps attribute must be an array" // NOLINT
#define ERR_DEPS_PATH_GLOBAL(_d)              "Deps path \"", _d, "\" is not local" // NOLINT
#define ERR_EMPTY_DEPS                        "Dependency list cannot be empty" // NOLINT

// Dependency

#define ERR_EMPTY_REQS                        "Attribute \"require\" list cannot be empty" // NOLINT
#define ERR_WRONG_REQ_TYPE                    "Attribute \"require\" element must be of type String" // NOLINT
#define ERR_WRONG_IN_TYPE                     "Attribute \"in\" must be of type String" // NOLINT
#define ERR_EMPTY_IN_VALUE                    "Attribute \"in\" cannot be empty" // NOLINT
#define ERR_UNDEFINED_IN_OPTION(_o, _m)       "Attribute \"in\" option \"", _o, "\" not found in model \"", _m, "\"" // NOLINT
#define ERR_IN_OPTION_NOT_IN_INSTANCE(_o)     "Attribute \"in\" option \"", _o, "\" not found in instance" // NOLINT
#define ERR_UNBOUND_IN_OPTION(_o)             "Attribute \"in\" option \"", _o, "\" must be bounded with an \"either\" clause" // NOLINT
#define ERR_UNDEFINED_MAY_DEP(_v, _o, _m)     "Allowed value \"", _v, "\" for \"in\" option \"", _o, "\" not found in model \"", _m, "\"" // NOLINT
#define ERR_WRONG_IN_OPT_TYPE(_o)             "Attribute \"in\" option \"", _o, "\" must be of type Selector or String" // NOLINT
#define ERR_WRONG_TYPE_FOR_VALUE_EXP          "Value expansion can only be used with \"string\"" // NOLINT
#define ERR_UNBOUND_VALUE_EXP                 "Value expansion \"%\" must be bounded with an \"either\" clause" // NOLINT
#define ERR_MODEL_EXP_PATH_NOT_FOUND          "Value expansion may lead to an invalid model path" // NOLINT
#define ERR_MODEL_PATH_NOT_FOUND(_p, _m)      "Invalid path \"", _p, "\" within model \"", _m, "\"" // NOLINT
#define ERR_INSTANCE_PATH_NOT_FOUND(_p)       "Option \"", _p, "\" not found in instance" // NOLINT
#define ERR_WRONG_WHEN_FORMAT                 "Attribute \"when\" entry must be either a Primitive or a String" // NOLINT
#define ERR_WHEN_TYPE_MISMATCH                "Attribute \"when\" entry type mismatch"
#define ERR_NO_GLOBAL_PATH_IN_DEPS            "Path in dependencies must be local"

// Constrain dependency

#define ERR_CONSTRAIN_RANGE_OR_EITHER         "Constrained dependency must specify either \"either\" or \"range\""  // NOLINT
#define ERR_CONSTRAIN_EMPTY_LIST              "Constrained dependency either list cannot be empty" // NOLINT
#define ERR_CONSTRAIN_EMPTY_RANGE             "Constrained dependency range cannot be empty" // NOLINT
#define ERR_CONSTRAIN_INVALID_TYPE            "Invalid constraint type" // NOLINT
#define ERR_CONSTRAIN_TGT_NOT_ENUM(_p)        "Constrained dependency target \"", _p, "\" type is not enumerated"  // NOLINT
#define ERR_CONSTRAIN_TGT_NOT_RANG(_p)        "Constrained dependency target \"", _p, "\" type is not ranged"  // NOLINT

// File

#define ERR_INVALID_DEFAULT_FILE_TYPE(_s)     "Default file type \"", _s, "\" not supported" // NOLINT
#define ERR_INVALID_EITHER_FILE_TYPE(_s)      "Allowed file type \"", _s, "\" not supported" // NOLINT
#define ERR_FILE_NO_SUCH_FILE(_p)             "No such file or directory: \"", _p, "\""  // NOLINT
#define ERR_FILE_CANNOT_OPEN_MODE(_p, _m)     "Cannot open file \"", _p, "\" with mode \"", _m, "\""  // NOLINT
#define ERR_FILE_CANNOT_BE_CREATED(_p)        "Path \"", _p, "\" cannot be created"  // NOLINT

// Format

#define ERR_EMPTY_VALUE                       "Value is empty (not authorized)" // NOLINT
#define ERR_WRONG_VALUE_TYPE(_t)              "Wrong type (expected \"", _t, "\")"  // NOLINT
#define ERR_VALUE_NOT_PRIMITIVE               "Value must be a primitive"  // NOLINT

// Header

#define ERR_WRONG_INCLUDE_TYPE                "Attribute \"include\" element must be of type String" // NOLINT
#define ERR_WRONG_INCLUDE_VALUE               "Attribute \"include\" element cannot be empty" // NOLINT
#define ERR_WRONG_TRIGGER_TYPE                "Attribute \"trigger\" element must be of type String" // NOLINT
#define ERR_WRONG_TRIGGER_FORMAT              "Invalid format for attribute \"trigger\"" // NOLINT
#define ERR_WRONG_TRIGGER_SCOPE               "Path for attribute \"trigger\" must be global" // NOLINT
#define ERR_EMPTY_TRIGGER                     "Attribute \"trigger\" cannot be empty" // NOLINT
#define ERR_WRONG_NS_TYPE                     "Attribute \"namespace\" element must be of type String" // NOLINT
#define ERR_WRONG_PACKAGE_TYPE                "Attribute \"package\" element must be of type String" // NOLINT

// IPv4

#define ERR_IPv4_BAD_ADDRESS(_s)              "IPv4 address \"", _s, "\" is not valid" // NOLINT

// Link type

#define ERR_INVALID_DEFAULT_LINK_TYPE(_s)     "default link type \"", _s, "\" not supported" // NOLINT
#define ERR_INVALID_EITHER_LINK_TYPE(_s)      "allowed link type \"", _s, "\" not supported" // NOLINT
#define ERR_VALUE_NOT_A_LINK_TYPE(_s)         "value \"", _s, "\" is not a valid link type" // NOLINT
#define ERR_UNSUPPORTED_LINK_TYPE(_s)         "link type \"", _s, "\" not supported" // NOLINT
#define ERR_LINK_TYPE_NOT_AVAILABLE(_s)       "link type \"", _s, "\" not available on this platform" // NOLINT


// List attribute

#define ERR_LIST_NOT_AN_ARRAY                 "List attribute must be an array" // NOLINT
#define ERR_LIST_INSTANCE_NOT_A_PRIMITIVE     "Instance for list attribute must be a primitive" // NOLINT
#define ERR_LIST_ITEM_TYPE_MISMATCH           "Mismatched list item type" // NOLINT

// MAC

#define ERR_MAC_BAD_ADDRESS(_s)               "MAC address \"", _s, "\" is not valid" // NOLINT

// Map attribute

#define ERR_MAP_NOT_AN_OBJECT                 "Map attribute must be an object" // NOLINT
#define ERR_MAP_INSTANCE_NOT_AN_OBJECT        "Instance for map attribute must be an object" // NOLINT
#define ERR_MAP_ITEM_TYPE_MISMATCH            "Mismatched map item type" // NOLINT
#define ERR_CONFLICTING_MAP_KEY(_k)           "Conflicting map key \"", _k, "\"" // NOLINT

// Model

#define ERR_FAILED_MERGING_TYPE(_t)           "Merge failed for option \"", _t, "\"" // NOLINT
#define ERR_FAILED_OVERRIDING_TYPE(_t)        "Override failed for option \"", _t, "\"" // NOLINT
#define ERR_FAILED_TEMPLATE_MERGE(_m)         "Merge failed for \"templates\" from \"", _m, "\"" // NOLINT
#define ERR_FAILED_TEMPLATE_OVERRIDE          "Override failed for \"templates\"" // NOLINT
#define ERR_FAILED_BODY_MERGE(_m)             "Merge failed for \"body\" from \"", _m, "\"" // NOLINT
#define ERR_FAILED_BODY_OVERRIDE              "Override failed for \"body\"" // NOLINT
#define ERR_MODEL_LOOP_DETECTED(_m)           "Circular reference of model: \"", _m, "\"" // NOLINT
#define ERR_INVALID_PACKAGE_PATH(_p)          "Package path \"", _p, "\" not found in the search path"  // NOLINT

// Path

#define ERR_INVALID_PATH_FORMAT(_p)           "Not a valid path format: \"", _p, "\""

// Plugin

#define ERR_MODEL_NOT_AN_ANCESTOR(_p,_m)      "Model \"", _p, "\" is not an ancestor of mapped model \"", _m, "\"" // NOLINT
#define ERR_BASE_MODEL_MISMATCH               "Base model mismatch" // NOLINT
#define ERR_NO_PLUGIN_TRIGGERS                "No plugin triggers found" // NOLINT
#define ERR_PLUGIN_NOT_LOADED(_n,_p)          "Plugin \"", _p, "\" for value \"", _n, "\" not found (check your path)" // NOLINT
#define ERR_PLUGIN_NO_MATCH_FOUND(_n)         "No trigger match for value \"", _n, "\"" // NOLINT

// Range attribute

#define ERR_RANGE_INVALID_TYPE                "Range attribute must be an array or a string" // NOLINT
#define ERR_RANGE_INSTANCE_NOT_A_PRIMITIVE    "Instance for range attribute must be a primitive" // NOLINT
#define ERR_RANGE_WRONG_COUNT                 "Range attribute must only have two elements" // NOLINT
#define ERR_RANGE_LOW_TYPE_MISMATCH           "Mismatched range attribute lower bound type" // NOLINT
#define ERR_RANGE_HIGH_TYPE_MISMATCH          "Mismatched range attribute higher bound type" // NOLINT
#define ERR_RANGE_NOT_ORDERED                 "Range attribute bounds must be ordered" // NOLINT
#define ERR_RANGE_FORMAT(_f)                  "Invalid range format \"", _f, "\"" // NOLINT

// Section

#define ERR_ILLEGAL_OPTION_NAME(_n)           "Illegal option name \"", _n, "\"" // NOLINT
#define ERR_FAILED_CHECKING_TYPE_MODEL(_t)    "Check model failed for type \"", _t, "\"" // NOLINT
#define ERR_FAILED_FLATTENING_MODEL(_t)       "Flatten model failed for type \"", _t, "\"" // NOLINT

// Selector

#define ERR_TEMPLATE_NOT_FOUND(_t)            "Template \"", _t, "\" not found" // NOLINT
#define ERR_RUNTIME_TEMPLATE_NOT_BOUND        "Instance-defined template must be bound with an either clause" // NOLINT
#define ERR_CONFLICTING_SELECT_NAME(_n)       "Conflicting select name \"", _n, "\"" // NOLINT
#define ERR_INVALID_TARGET_ARITY(_a)          "Invalid target arity ", "\"", _a, "\"" // NOLINT
#define ERR_INVALID_SELECT_SIZE(_a)           "Size ", "\"", _a, "\" not in defined range" // NOLINT
#define ERR_SELECT_UNEXPECTED_EITHER(_a)      "Unexpected \"either\" value ", _a, " in select" // NOLINT

// String

#define ERR_STR_LEN_OUTSIDE_OF_CONSTRAINT     "Length of string is outside of specified constraint" // NOLINT

// Type

#define ERR_WRONG_DEFAULT_FORMAT              "Wrong default value format" // NOLINT
#define ERR_RANGE_INVALID_DEFAULT             "Default value not within range" // NOLINT
#define ERR_EITHER_INVALID_DEFAULT            "Default value not in the allowed list" // NOLINT
#define ERR_MAP_INVALID_DEFAULT               "Default value not in the allowed map" // NOLINT
#define ERR_EITHER_EMPTY                      "Bound list cannot be empty" // NOLINT
#define ERR_MAP_EMPTY                         "Bound map cannot be empty" // NOLINT
#define ERR_INSTANCE_NOT_AN_OBJECT            "Instance value must be an object"  // NOLINT

// URI

#define ERR_INVALID_DEFAULT_SCHEME(_s)        "Default scheme \"", _s, "\" not supported" // NOLINT
#define ERR_INVALID_EITHER_SCHEME(_s)         "Allowed scheme \"", _s, "\" not supported" // NOLINT
#define ERR_VALUE_NOT_AN_URI(_s)              "Value \"", _s, "\" is not a valid URI" // NOLINT
#define ERR_UNSUPPORTED_SCHEME(_s)            "Scheme \"", _s, "://\" not suppported" // NOLINT
#define ERR_FILE_URI_PATH_NOT_A_FILE(_s)      "Path \"", _s, "\" is not a file path (extra / at the end)" // NOLINT
#define ERR_FILE_URI_PATH_NOT_A_DIR(_s)       "Path \"", _s, "\" does not point to a directory" // NOLINT
#define ERR_FILE_URI_PATH_NOT_A_SMB(_s)       "Path \"", _s, "\" does not point to a SMBCast object" // NOLINT
#define ERR_FILE_URI_FILE_NOT_FOUND(_s)       "File \"", _s, "\" does not exist or is not accessible" // NOLINT
#define ERR_FILE_URI_NOT_POSSIBLE(_s)         "Path \"", _s, "\" does not exist and cannot be created" // NOLINT
#define ERR_IPv4_URI_BAD_ADDRESS(_s)          "IPv4 address \"", _s, "\" is not valid" // NOLINT
#define ERR_IPv4_URI_INVALID_PORT(_s)         "Invalid port for IPv4 URI \"", _s, "\"" // NOLINT

#endif  // ACE_MODEL_ERRORS_H_

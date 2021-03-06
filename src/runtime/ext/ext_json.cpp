/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010- Facebook, Inc. (http://www.facebook.com)         |
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include <runtime/ext/ext_json.h>
#include <runtime/ext/JSON_parser.h>
#include <runtime/base/zend/utf8_decode.h>
#include <runtime/base/variable_serializer.h>

namespace HPHP {
IMPLEMENT_DEFAULT_EXTENSION(json);
///////////////////////////////////////////////////////////////////////////////

String f_json_encode(CVarRef value, bool loose /* = false */) {
  VariableSerializer vs(VariableSerializer::JSON, loose ? 1 : 0);
  return vs.serialize(value, true);
}

Variant f_json_decode(CStrRef json, bool assoc /* = false */,
                      bool loose /* = false */) {
  if (json.empty()) {
    return null;
  }

  Variant z;
  if (JSON_parser(z, json.data(), json.size(), assoc, loose)) {
    return z;
  }

  if (json.size() == 4) {
    if (!strcasecmp(json.data(), "null")) return null;
    if (!strcasecmp(json.data(), "true")) return true;
  } else if (json.size() == 5 && !strcasecmp(json.data(), "false")) {
    return false;
  }

  int64 p;
  double d;
  DataType type = json->isNumericWithVal(p, d, 0);
  if (type == KindOfInt64) {
    return p;
  } else if (type == KindOfDouble) {
    return d;
  }

  char ch0 = json.charAt(0);
  if (json.size() > 1 && ch0 == '"' && json.charAt(json.size() - 1) == '"') {
    return json.substr(1, json.size() - 2);
  }

  if (loose && json.size() > 1 &&
      ch0 == '\'' && json.charAt(json.size() - 1) == '\'') {
    return json.substr(1, json.size() - 2);
  }

  if (ch0 == '{' || ch0 == '[') { /* invalid JSON string */
    return null;
  }

  return json;
}

///////////////////////////////////////////////////////////////////////////////
}

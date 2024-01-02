// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("JSON");

Console.writeLn(JSON.encodeWithIndentation(JSON.decode("{\"item\":\"Some Item!\",\"work\":\"Some Work!\"}")));

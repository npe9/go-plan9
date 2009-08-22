// errchk $G $D/$F.go

// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

type I1 interface { I2 }	// ERROR "interface"
type I2 int

type I3 interface { int }	// ERROR "interface"
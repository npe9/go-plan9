// errchk $G $D/$F.go

// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

func f9(a int) (i int, f float) {
	i := 9;  // ERROR "redecl|no new"
	f := float(9);  // ERROR "redecl|no new"
	return i, f;
}

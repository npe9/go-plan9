// errchk $G -e $D/$F.go

// Copyright 2010 The Go Authors.  All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

func main() {
	if x; y		// ERROR "unexpected semicolon or newline before .?{.?|undefined"
	{
		z	// GCCGO_ERROR "undefined"



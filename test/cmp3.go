// $G $D/$F.go && $L $F.$A && ! ./$A.out

// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

func use(bool) { }

func main() {
	var b []int;
	var ib interface{} = b;
	use(ib == ib);
}
